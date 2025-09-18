#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "config.h"

static const char *TAG = "APP";

// Forward declarations for module init/start functions
esp_err_t wifi_start(void);
esp_err_t mqtt_start(void);
esp_err_t camera_task_start(void);
esp_err_t display_init(void);
esp_err_t mpu6050_init(void);
esp_err_t door_init(void);

void app_main(void) {
    ESP_LOGI(TAG, "Booting esp32s3-face-access (skeleton)");

    // Basic IDF init (safe even if modules are stubs)
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_netif_init());

    // Initialize peripherals/subsystems (best-effort order)
    if (display_init() != ESP_OK) {
        ESP_LOGW(TAG, "display_init failed (stub)");
    }
    if (camera_task_start() != ESP_OK) {
        ESP_LOGW(TAG, "camera_task_start failed (stub or camera not present)");
    }
    if (wifi_start() != ESP_OK) {
        ESP_LOGW(TAG, "wifi_start failed (stub)");
    }
    if (mqtt_start() != ESP_OK) {
        ESP_LOGW(TAG, "mqtt_start failed (stub)");
    }
    if (mpu6050_init() != ESP_OK) {
        ESP_LOGW(TAG, "mpu6050_init failed (stub)");
    }
    if (door_init() != ESP_OK) {
        ESP_LOGW(TAG, "door_init failed (stub)");
    }

    // Idle loop
    while (1) {
        ESP_LOGI(TAG, "System alive");
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}