#pragma once

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "transports/screen_spi_transport.h"

typedef enum
{
    MODE_COMMAND = 0,
    MODE_DATA = 1
} dc_mode_t;

typedef struct
{
    dc_mode_t mode_value;
    gpio_num_t mode_pin;
} transaction_user_data_t;

typedef struct {
    /// @brief The device handle to pass to ESP-IDF SPI methods.
    spi_device_handle_t device;
    
    /// @brief The GPIO pin for D/C line.
    gpio_num_t mode_pin;
    
    /// @brief The GPIO pin for the reset line.
    gpio_num_t reset_pin;

    /// @brief A flag indicating if a DMA transfer is taking place.
    bool dma_in_flight;
} screen_spi_transport_t;

static void pre_transfer_callback(spi_transaction_t *transaction);

// Blocking
esp_err_t screen_spi_transport_send_command(const screen_transport_t *transport, uint8_t command);
esp_err_t screen_spi_transport_send_data(const screen_transport_t *transport, const uint8_t *data, size_t data_length);

//DMA
esp_err_t screen_spi_transport_send_dma_data(const screen_transport_t *transport, const uint8_t *data, size_t data_length);
esp_err_t screen_spi_transport_flush_dma_data(const screen_transport_t *transport);