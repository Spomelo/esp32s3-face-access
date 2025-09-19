#include "camera_task.h"
#include "config.h"
#include "esp_camera.h"
#include "esp_log.h"
#include "display.h"
#include "door.h"
#include "mqtt.h"
#include "utils.h"
#include "mpu6050.h"

// 来自 esp-who / esp-face
#include "img_converters.h"
#include "fd_forward.h"

static const char *TAG = "camera_task";

// 人脸检测配置（MTCNN）
static mtmn_config_t mtmn_config;

static void camera_init(void)
{
    camera_config_t config = {
        .pin_pwdn = CAM_PIN_PWDN,
        .pin_reset = CAM_PIN_RESET,
        .pin_xclk = CAM_PIN_XCLK,
        .pin_sccb_sda = CAM_PIN_SIOD,
        .pin_sccb_scl = CAM_PIN_SIOC,

        .pin_d7 = CAM_PIN_D7,
        .pin_d6 = CAM_PIN_D6,
        .pin_d5 = CAM_PIN_D5,
        .pin_d4 = CAM_PIN_D4,
        .pin_d3 = CAM_PIN_D3,
        .pin_d2 = CAM_PIN_D2,
        .pin_d1 = CAM_PIN_D1,
        .pin_d0 = CAM_PIN_D0,
        .pin_vsync = CAM_PIN_VSYNC,
        .pin_href = CAM_PIN_HREF,
        .pin_pclk = CAM_PIN_PCLK,

        .xclk_freq_hz = 20000000,
        .ledc_timer = LEDC_TIMER_0,
        .ledc_channel = LEDC_CHANNEL_0,

        .pixel_format = PIXFORMAT_RGB565, // 用于显示与检测
        .frame_size = CAM_FRAME_SIZE,
        .jpeg_quality = CAM_JPEG_QUALITY,
        .fb_count = 2,
        .grab_mode = CAMERA_GRAB_WHEN_EMPTY};

    ESP_ERROR_CHECK(esp_camera_init(&config));

    sensor_t *s = esp_camera_sensor_get();
    s->set_framesize(s, CAM_FRAME_SIZE);
    s->set_brightness(s, 0);
    s->set_saturation(s, 0);
    s->set_contrast(s, 0);
}

static void draw_frame_to_lcd(camera_fb_t *fb)
{
    // 将 320x240 的 RGB565 原样居中/或裁剪至 240x240
    int src_w = fb->width;
    int src_h = fb->height;
    int dst_w = LCD_HOR_RES;
    int dst_h = LCD_VER_RES;

    const uint16_t *src = (const uint16_t *)fb->buf;

    // 简单裁剪适配：取中间 240x240
    int x_off = (src_w > dst_w) ? (src_w - dst_w) / 2 : 0;
    int y_off = (src_h > dst_h) ? (src_h - dst_h) / 2 : 0;

    // 按行输出，避免大缓冲
    for (int y = 0; y < dst_h; ++y)
    {
        const uint16_t *line = &src[(y + y_off) * src_w + x_off];
        display_draw_rgb565(0, y, dst_w, 1, line);
    }
}

static void draw_boxes_on_lcd(box_array_t *boxes, int src_w, int src_h)
{
    if (!boxes)
        return;
    // 映射至 LCD 坐标（已裁剪中间 240x240）
    int x_off = (src_w > LCD_HOR_RES) ? (src_w - LCD_HOR_RES) / 2 : 0;
    int y_off = (src_h > LCD_VER_RES) ? (src_h - LCD_VER_RES) / 2 : 0;

    for (int i = 0; i < boxes->len; i++)
    {
        box_t b = boxes->box[i];
        // 裁剪后的坐标
        int x = b.box_p[0] - x_off;
        int y = b.box_p[1] - y_off;
        int w = b.box_p[2] - b.box_p[0];
        int h = b.box_p[3] - b.box_p[1];
        // 保证在屏幕内
        int x1 = x < 0 ? 0 : x;
        int y1 = y < 0 ? 0 : y;
        int x2 = (x + w) > LCD_HOR_RES ? LCD_HOR_RES : (x + w);
        int y2 = (y + h) > LCD_VER_RES ? LCD_VER_RES : (y + h);
        if (x2 > x1 && y2 > y1)
        {
            display_draw_rect(x1, y1, x2 - x1, y2 - y1, 0xFFE0); // 黄框
        }
    }
}

// 将 RGB565 -> JPEG（使用 esp32-camera 的 fmt2jpg）
static bool rgb565_to_jpeg(const uint8_t *rgb565, int w, int h, int quality, uint8_t **out, size_t *out_len)
{
    return fmt2jpg(rgb565, w * h * 2, w, h, PIXFORMAT_RGB565, quality, out, out_len);
}

static void camera_task(void *arg)
{
    mtmn_config = mtmn_init_config();

    uint64_t last_face_sent = 0;

    while (1)
    {
        // 姿态检测
        float ax, ay, az, pitch, roll;
        if (mpu6050_read_accel_g(&ax, &ay, &az) == ESP_OK)
        {
            mpu6050_compute_tilt_deg(ax, ay, az, &pitch, &roll);
            if (fabsf(pitch) > TILT_DEG_THRESH || fabsf(roll) > TILT_DEG_THRESH)
            {
                display_draw_red_alert_border();
                mqtt_publish_alert("TILT_ALERT");
            }
        }

        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb)
        {
            ESP_LOGW(TAG, "Camera fb get failed");
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        // 显示原始帧
        draw_frame_to_lcd(fb);

        // 人脸检测：需 RGB888
        dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
        if (image_matrix)
        {
            bool ok = fmt2rgb888(fb->buf, fb->len, PIXFORMAT_RGB565, image_matrix->item);
            box_array_t *net_boxes = NULL;
            if (ok)
            {
                net_boxes = face_detect(image_matrix, &mtmn_config);
            }

            // 叠加框线
            if (net_boxes && net_boxes->len > 0)
            {
                draw_boxes_on_lcd(net_boxes, fb->width, fb->height);

                uint64_t now = now_ms();
                if (now - last_face_sent > FACE_MQTT_ONCE_INTERVAL_MS)
                {
                    // 转 JPEG 发送
                    uint8_t *jpg = NULL;
                    size_t jlen = 0;
                    if (rgb565_to_jpeg(fb->buf, fb->width, fb->height, CAM_JPEG_QUALITY, &jpg, &jlen))
                    {
                        mqtt_publish_image_base64(jpg, jlen);
                        free(jpg);
                        last_face_sent = now;
                    }
                    // 开门（继电器）
                    door_unlock_ms(DOOR_UNLOCK_MS);
                }
            }

            if (net_boxes)
            {
                free(net_boxes->score);
                free(net_boxes->box);
                free(net_boxes->landmark);
                free(net_boxes);
            }
            dl_matrix3du_free(image_matrix);
        }

        esp_camera_fb_return(fb);
        taskYIELD();
    }
}

void start_camera_pipeline(void)
{
    camera_init();
    xTaskCreatePinnedToCore(camera_task, "camera_task", 6 * 1024, NULL, 5, NULL, 1);
}