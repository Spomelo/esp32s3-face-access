#include "esp_log.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "config.h"

static const char *TAG = "DOOR";

esp_err_t door_init(void) {
    ESP_LOGI(TAG, "door_init (skeleton)");
    gpio_config_t io = {
        .pin_bit_mask = 1ULL << RELAY_GPIO,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE,
    };
    esp_err_t err = gpio_config(&io);
    if (err != ESP_OK) return err;
    gpio_set_level(RELAY_GPIO, 0);
    return ESP_OK;
}

void door_unlock_ms(uint32_t ms) {
    ESP_LOGI(TAG, "Unlock door for %u ms", (unsigned)ms);
    gpio_set_level(RELAY_GPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(ms));
    gpio_set_level(RELAY_GPIO, 0);
}