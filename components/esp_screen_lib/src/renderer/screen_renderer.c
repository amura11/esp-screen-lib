#include <stdint.h>
#include "screen_driver.h"
#include "renderer/screen_renderer_internal.h"

#include "render_commands_rectangle.c"

static const render_command_handler_t render_command_handlers[] = {
    [RENDER_CMD_FILLED_RECTANGLE] = render_filled_rectangle_to_slice,
    [RENDER_CMD_LINED_RECTANGLE] = render_lined_rectangle_to_slice,
    [RENDER_CMD_FILLED_CIRCLE] = NULL,
    [RENDER_CMD_LINED_CIRCLE] = NULL,
    // future entries...
};

esp_err_t screen_renderer_draw_filled_rect(screen_renderer_t *renderer, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if (renderer->command_count >= RENDER_COMMAND_BUFFER_SIZE)
    {
        return ESP_ERR_NO_MEM;
    }

    renderer->commands[renderer->command_count++] = (render_command_t){
        .type = RENDER_CMD_FILLED_RECTANGLE,
        .filled_rectangle = {x, y, w, h, color}};

    return ESP_OK;
}

esp_err_t screen_renderer_draw_rect(screen_renderer_t *renderer, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if (renderer->command_count >= RENDER_COMMAND_BUFFER_SIZE)
    {
        return ESP_ERR_NO_MEM;
    }

    renderer->commands[renderer->command_count++] = (render_command_t){
        .type = RENDER_CMD_LINED_RECTANGLE,
        .lined_rectangle = {x, y, w, h, color}};

    return ESP_OK;
}

esp_err_t screen_renderer_draw_filled_circle(screen_renderer_t *renderer, uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
    if (renderer->command_count >= RENDER_COMMAND_BUFFER_SIZE)
        return ESP_ERR_NO_MEM;
    renderer->commands[renderer->command_count++] = (render_command_t){
        .type = RENDER_CMD_FILLED_CIRCLE,
        .filled_circle = {x, y, r, color}};
    return ESP_OK;
}

esp_err_t screen_renderer_draw_circle(screen_renderer_t *renderer, uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
    if (renderer->command_count >= RENDER_COMMAND_BUFFER_SIZE)
    {
        return ESP_ERR_NO_MEM;
    }

    renderer->commands[renderer->command_count++] = (render_command_t){
        .type = RENDER_CMD_LINED_CIRCLE,
        .lined_circle = {x, y, r, color}};

    return ESP_OK;
}

void screen_renderer_flush(screen_renderer_t *renderer) {
    for (uint16_t slice_y = 0; slice_y < renderer->driver->screen_height; slice_y += renderer->slice_size) {
        memset(renderer->buffer, 0, renderer->buffer_size);

        for (size_t i = 0; i < renderer->command_count; i++) {
            const render_command_t *cmd = &renderer->commands[i];

            if (cmd->type >= sizeof(render_command_handlers) / sizeof(render_command_handlers[0])) continue;
            render_command_handler_t handler = render_command_handlers[cmd->type];
            if (handler) {
                handler(renderer, cmd, slice_y);
            }
        }

        //renderer->driver->set_window(renderer->driver, 0, slice_y, SCREEN_WIDTH, SLICE_HEIGHT);
        //renderer->driver->write_pixels(renderer->driver, renderer->slice_buffer, SCREEN_WIDTH * SLICE_HEIGHT);
    }

    renderer->command_count = 0;
}
