#pragma once

#include <stdint.h>
#include "esp_err.h"

typedef struct screen_transport_t
{
    esp_err_t (*send_command_script)(const screen_transport_t *transport, const uint8_t *script, const uint8_t *parameters, size_t parameters_length);
    esp_err_t (*send_command)(const screen_transport_t *transport, uint8_t command);
    esp_err_t (*send_data)(const screen_transport_t *transport, const void *data, size_t data_length);
    esp_err_t (*send_dma_data)(const screen_transport_t *transport, const void *data, size_t data_length);
    esp_err_t (*flush_dma_data)(const screen_transport_t *transport);

    void *context;
} screen_transport_t;
