#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C"
{
#endif

    void display_init(void);
    void display_clear(uint16_t color565);
    void display_draw_rgb565(int x, int y, int w, int h, const uint16_t *img);
    void display_draw_rect(int x, int y, int w, int h, uint16_t color);
    void display_draw_banner(const char *text);
    void display_draw_red_alert_border(void);

#ifdef __cplusplus
}
#endif