#pragma once

// 摄像头引脚（OV2640 DVP 并口）
#define CAM_PIN_PWDN    -1
#define CAM_PIN_RESET   -1
#define CAM_PIN_XCLK    15
#define CAM_PIN_SIOD    4
#define CAM_PIN_SIOC    5
#define CAM_PIN_VSYNC   6
#define CAM_PIN_HREF    7
#define CAM_PIN_PCLK    13
#define CAM_PIN_D0      11
#define CAM_PIN_D1      9
#define CAM_PIN_D2      8
#define CAM_PIN_D3      10
#define CAM_PIN_D4      12
#define CAM_PIN_D5      14
#define CAM_PIN_D6      47
#define CAM_PIN_D7      48

// LCD ST7789 SPI 引脚和参数
#define LCD_PIN_SCLK    40
#define LCD_PIN_MOSI    41
#define LCD_PIN_DC      42
#define LCD_PIN_CS      38
#define LCD_PIN_RST     3
#define LCD_PIN_BL      2
#define LCD_HOR_RES     240
#define LCD_VER_RES     240
#define LCD_COLOR_BITS  16

// I2C (MPU6050)
#define I2C_SDA_GPIO    16
#define I2C_SCL_GPIO    17
#define I2C_PORT_NUM    0
#define I2C_FREQ_HZ     400000

// 门锁继电器 GPIO
#define RELAY_GPIO      1

// 其它参数
#define FACE_MQTT_ONCE_INTERVAL_MS 5000   // 同一人脸事件发送间隔
#define DOOR_UNLOCK_MS  CONFIG_DOOR_UNLOCK_MS
#define TILT_DEG_THRESH CONFIG_TILT_THRESHOLD_DEG

// MQTT 主题
#define MQTT_TOPIC_IMAGE CONFIG_MQTT_TOPIC_IMAGE
#define MQTT_TOPIC_ALERT CONFIG_MQTT_TOPIC_ALERT

// 摄像头分辨率与 JPEG 质量
#define CAM_FRAME_SIZE  FRAMESIZE_QVGA  // 320x240，兼顾检测与显示
#define CAM_JPEG_QUALITY 12
