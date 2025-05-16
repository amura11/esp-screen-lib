#pragma once

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"
#include "screen_transport.h"

typedef enum {
    SCREEN_COLOR_ORDER_RGB,  // Red-Green-Blue
    SCREEN_COLOR_ORDER_BGR,  // Blue-Green-Red
    SCREEN_COLOR_ORDER_GRB,  // Green-Red-Blue
    SCREEN_COLOR_ORDER_GBR,  // Green-Blue-Red
    SCREEN_COLOR_ORDER_RBG,  // Red-Blue-Green
    SCREEN_COLOR_ORDER_BRG   // Blue-Red-Green
} screen_color_order_t;

typedef enum {
    SCREEN_COLOR_FORMAT_RGB565,      // 16-bit: R5-G6-B5
    SCREEN_COLOR_FORMAT_RGB666,      // 18-bit: R6-G6-B6 (packed or padded)
    SCREEN_COLOR_FORMAT_RGB888,      // 24-bit: R8-G8-B8
    SCREEN_COLOR_FORMAT_MONOCHROME,  // 1-bit B/W displays
    SCREEN_COLOR_FORMAT_RGB332,      // 8-bit: R3-G3-B2
    SCREEN_COLOR_FORMAT_RGB444       // 12-bit: R4-G4-B4
} screen_color_format_t;

typedef struct {
    size_t dma_alignment_bytes;         // Required buffer alignment for DMA transfers (e.g., 4 bytes)
    size_t maximum_transfer_size_bytes; // Maximum number of bytes per DMA transfer

    bool supports_partial_update;       // Can the panel update a sub-region efficiently?
    bool supports_invert_colors;        // Does the panel support color inversion commands?
    bool supports_madctl_rotation;      // Can the panel handle rotation via MADCTL register?

    uint16_t color_depth_bits;          // Effective color depth per pixel (16, 18, 24 bpp)
} screen_driver_capabilities_t;

typedef struct screen_driver_t {
    esp_err_t (*initialize)(const struct screen_driver_t *driver, const screen_transport_t *transport);
    esp_err_t (*reset)(const struct screen_driver_t *driver);

    esp_err_t (*set_rotation)(const struct screen_driver_t *driver, uint8_t rotation);
    esp_err_t (*set_window)(const struct screen_driver_t *driver, uint16_t column_start, uint16_t row_start, uint16_t column_end, uint16_t row_end);

    esp_err_t (*write_pixels)(const struct screen_driver_t *driver, const void *pixel_data, size_t data_length);
    esp_err_t (*flush)(const struct screen_driver_t *driver);

    esp_err_t (*get_capabilities)(const struct screen_driver_t *driver, screen_driver_capabilities_t *capabilities);

    uint16_t screen_width;
    uint16_t screen_height;
    screen_color_order_t color_order;
    screen_color_format_t color_format;

    void *context;
} screen_driver_t;
