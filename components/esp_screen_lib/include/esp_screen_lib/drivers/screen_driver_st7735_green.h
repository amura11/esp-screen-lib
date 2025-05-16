#pragma once

#include <stdint.h>
#include "driver/gpio.h"
#include "screen_driver.h"

typedef enum
{
    DEFAULT = 0,
    PIXEL_FIX = 1,        // Use if you get random pixels on two edge(s) of the screen
    PIXEL_FIX_128 = 2,    // Use if you get random pixels on edge(s) of the 128x128 screen
    ROTATION_FIX_128 = 3, // Use if you only get part of 128x128 screen in rotation 0 & 1
    ROTATION_FIX_160 = 4  // Use if you only get part of 128x80 screen in rotation 0 & 1
} screen_st7735_green_variant_t;

typedef struct screen_driver_st7735_green_config_t
{
    uint16_t width;
    uint16_t height;
    uint8_t rotation;
    screen_color_order_t color_order;
    screen_color_format_t color_format;
    gpio_num_t reset_pin;
    screen_st7735_green_variant_t variant;
} screen_driver_st7735_green_config_t;

esp_err_t screen_driver_st7735_green_create(const screen_driver_st7735_green_config_t *config, const screen_driver_t *driver);