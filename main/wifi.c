#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "WIFI";

esp_err_t wifi_start(void) {
    ESP_LOGI(TAG, "wifi_start (skeleton)");
    // TODO: Implement Wi-Fi init (esp_netif, esp_wifi, station/AP, etc.)
    return ESP_OK;
}