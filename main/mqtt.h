#pragma once
#include <stdbool.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C"
{
#endif

    void mqtt_start(void);
    bool mqtt_is_ready(void);
    void mqtt_publish_alert(const char *msg);
    void mqtt_publish_image_base64(const uint8_t *jpeg, size_t jpeg_len);

#ifdef __cplusplus
}
#endif