// src/drivers/vesa.h - VESA driver header

#ifndef VESA_H
#define VESA_H

#include <stdint.h>

typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t bpp;        // Bits per pixel
    uint32_t pitch;     // Bytes per scanline
    uint8_t* framebuffer;
} vesa_mode_info_t;

void vesa_init(void);
int vesa_is_available(void);
int vesa_set_mode(uint16_t width, uint16_t height, uint8_t bpp);
vesa_mode_info_t* vesa_get_mode_info(void);
void vesa_putpixel(uint16_t x, uint16_t y, uint32_t color);
void vesa_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
void vesa_clear(uint32_t color);

#endif