// src/drivers/vesa.c - VESA graphics mode driver

#define DRIVER_VERSION "1.0.0"
#define DRIVER_TYPE "Hardware Driver"
#define DRIVER_NAME "VESA Graphics Driver"

#include "vesa.h"
#include <stdint.h>
#include <stddef.h>

static vesa_mode_info_t current_mode;
static uint8_t* framebuffer = NULL;
static int vesa_available = 0;

// Initialize VESA driver
void vesa_init(void) {
    // VESA initialization requires real mode calls or multiboot2
    // This is a placeholder that checks if VESA is available
    // In a real implementation, you'd parse multiboot VBE info
    
    // For now, we'll just mark it as unavailable
    vesa_available = 0;
    framebuffer = NULL;
}

// Check if VESA is available
int vesa_is_available(void) {
    return vesa_available;
}

// Set video mode (simplified)
int vesa_set_mode(uint16_t width, uint16_t height, uint8_t bpp) {
    if (!vesa_available) {
        return -1;
    }
    
    // In a real implementation, you would:
    // 1. Search for matching mode in VBE mode list
    // 2. Call VBE function to set mode
    // 3. Update framebuffer pointer
    
    current_mode.width = width;
    current_mode.height = height;
    current_mode.bpp = bpp;
    current_mode.pitch = width * (bpp / 8);
    
    return 0;
}

// Get current mode info
vesa_mode_info_t* vesa_get_mode_info(void) {
    if (!vesa_available) {
        return NULL;
    }
    return &current_mode;
}

// Put pixel at coordinates
void vesa_putpixel(uint16_t x, uint16_t y, uint32_t color) {
    if (!vesa_available || framebuffer == NULL) {
        return;
    }
    
    if (x >= current_mode.width || y >= current_mode.height) {
        return;
    }
    
    uint32_t offset = y * current_mode.pitch + x * (current_mode.bpp / 8);
    
    switch (current_mode.bpp) {
        case 32:
            *((uint32_t*)(framebuffer + offset)) = color;
            break;
        case 24:
            framebuffer[offset] = color & 0xFF;
            framebuffer[offset + 1] = (color >> 8) & 0xFF;
            framebuffer[offset + 2] = (color >> 16) & 0xFF;
            break;
        case 16:
            *((uint16_t*)(framebuffer + offset)) = (uint16_t)color;
            break;
    }
}

// Draw rectangle
void vesa_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    for (uint16_t dy = 0; dy < height; dy++) {
        for (uint16_t dx = 0; dx < width; dx++) {
            vesa_putpixel(x + dx, y + dy, color);
        }
    }
}

// Clear screen
void vesa_clear(uint32_t color) {
    if (!vesa_available) {
        return;
    }
    vesa_draw_rect(0, 0, current_mode.width, current_mode.height, color);
}