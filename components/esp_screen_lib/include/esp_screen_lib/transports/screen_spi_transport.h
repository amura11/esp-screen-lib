#pragma once

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "screen_transport.h"

typedef struct
{
    gpio_num_t miso_pin;
    gpio_num_t mosi_pin;
    gpio_num_t clock_pin;
    gpio_num_t select_pin;
    gpio_num_t mode_pin;
    gpio_num_t reset_pin;
    spi_host_device_t spi_host;
} screen_spi_transport_config_t;

esp_err_t screen_spi_transport_create(const screen_spi_transport_config_t *config, screen_transport_t *transport);

void screen_spi_transport_destroy(screen_transport_t *transport);