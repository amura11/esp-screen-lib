#pragma once

#include "screen_driver.h"

#define RENDER_COMMAND_BUFFER_SIZE 1024

typedef struct {
    render_command_t commands[RENDER_COMMAND_BUFFER_SIZE];
    size_t command_count;

    // Driver reference for sending pixels (abstract transport layer)
    screen_driver_t *driver;

    uint8_t slice_size;

    uint16_t buffer_width;
    uint16_t buffer_height;
    uint16_t buffer_size;
    void *buffer;
} screen_renderer_t;

esp_err_t screen_renderer_draw_filled_rect(screen_renderer_t *renderer, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
esp_err_t screen_renderer_draw_rect(screen_renderer_t *renderer, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
esp_err_t screen_renderer_draw_filled_circle(screen_renderer_t *renderer, uint16_t x, uint16_t y, uint16_t r, uint16_t color);
esp_err_t screen_renderer_draw_circle(screen_renderer_t *renderer, uint16_t x, uint16_t y, uint16_t r, uint16_t color);