#pragma once

#include "screen_renderer.h"

typedef enum
{
    RENDER_CMD_FILLED_RECTANGLE,
    RENDER_CMD_LINED_RECTANGLE,
    RENDER_CMD_FILLED_CIRCLE,
    RENDER_CMD_LINED_CIRCLE,
    // Future: bitmaps, text, lines, etc.
} render_command_type_t;

typedef struct
{
    render_command_type_t type;
    union
    {
        struct
        {
            uint16_t x, y, width, height, color;
        } filled_rectangle;
        struct
        {
            uint16_t x, y, width, height, color;
        } lined_rectangle;
        struct
        {
            uint16_t x, y, radius, color;
        } filled_circle;
        struct
        {
            uint16_t x, y, radius, color;
        } lined_circle;
    };
} render_command_t;

typedef void (*render_command_handler_t)(screen_renderer_t *renderer, const render_command_t *command, void *buffer, uint16_t slice_y);