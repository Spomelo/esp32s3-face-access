#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "LCD";

esp_err_t display_init(void) {
    ESP_LOGI(TAG, "display_init (skeleton)");
    // TODO: Initialize SPI bus and LCD panel (ST7789) using esp_lcd APIs
    return ESP_OK;
}