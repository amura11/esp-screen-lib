#include "transports/screen_spi_transport_internal.h"
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

    /// @brief A flag indicating if a DMA transfer is taking place.
    bool dma_in_flight;
} screen_spi_transport_t;

static void pre_transfer_callback(spi_transaction_t *transaction);

// Blocking
static esp_err_t send_command(const screen_transport_t *transport, uint8_t command);
static esp_err_t send_data(const screen_transport_t *transport, const uint8_t *data, size_t data_length);

//DMA
static esp_err_t send_dma_data(const screen_transport_t *transport, const uint8_t *data, size_t data_length);
static esp_err_t flush_dma_data(const screen_transport_t *transport);

esp_err_t screen_spi_transport_create(const screen_spi_transport_config_t *config, screen_transport_t *transport)
{
    esp_err_t result;

    memset(transport, 0, sizeof(screen_transport_t));
    screen_spi_transport_t *spi_transport = calloc(1, sizeof(screen_spi_transport_t));

    spi_bus_config_t bus_config = {
        .miso_io_num = config->miso_pin,
        .mosi_io_num = config->mosi_pin,
        .sclk_io_num = config->clock_pin,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        // TODO Figure out: .max_transfer_sz = config->max_transfer_length,
    };

    spi_device_interface_config_t device_config = {
        .clock_speed_hz = 40 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = config->select_pin,
        .queue_size = 7,
        .pre_cb = pre_transfer_callback,
    };

    gpio_config_t io_conf = {
        .pin_bit_mask = ((1ULL << config->reset_pin) | (1ULL << config->mode_pin)),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = true,
    };

    // Setup the non-SPI GPIOs
    if ((result = gpio_config(&io_conf)) != ESP_OK)
    {
        return result;
    }

    // Setup the SPI bus
    if ((result = spi_bus_initialize(config->spi_host, &bus_config, SPI_DMA_CH_AUTO)) != ESP_OK)
    {
        return result;
    }

    // Configure the bus to have the device
    if ((result = spi_bus_add_device(config->spi_host, &device_config, spi_transport->device)) != ESP_OK)
    {
        return result;
    }

    // Configure the transport struct
    transport->context = spi_transport;
    transport->send_command = send_command;
    transport->send_data = send_data;
    transport->send_dma_data = send_dma_data;
    transport->flush_dma_data = flush_dma_data;

    return result;
}

static void pre_transfer_callback(spi_transaction_t *transaction)
{
    transaction_user_data_t *data = (transaction_user_data_t *)transaction->user;
    gpio_set_level(data->mode_pin, data->mode_value);
}

static esp_err_t send_command(const screen_transport_t *transport, uint8_t command)
{
    screen_spi_transport_t *spi_transport = (screen_spi_transport_t *)transport->context;

    transaction_user_data_t user_data = {
        .mode_pin = spi_transport->mode_pin,
        .mode_value = MODE_COMMAND,
    };

    spi_transaction_t transaction = {0};
    transaction.user = (void *)&user_data;
    transaction.length = 8;
    transaction.tx_buffer = &command;

    return spi_device_polling_transmit(spi_transport->device, &transaction);
}

static esp_err_t send_data(const screen_transport_t *transport, const uint8_t *data, size_t data_length)
{
    screen_spi_transport_t *spi_transport = (screen_spi_transport_t *)transport->context;

    transaction_user_data_t user_data = {
        .mode_pin = spi_transport->mode_pin,
        .mode_value = MODE_DATA,
    };

    spi_transaction_t transaction = {0};
    transaction.user = (void *)&user_data;
    transaction.length = 8 * data_length;
    transaction.tx_buffer = data;

    return spi_device_polling_transmit(spi_transport->device, &transaction);
}

static esp_err_t send_dma_data(const screen_transport_t *transport, const uint8_t *data, size_t data_length)
{
    screen_spi_transport_t *spi_transport = (screen_spi_transport_t *)transport->context;

    transaction_user_data_t user_data = {
        .mode_pin = spi_transport->mode_pin,
        .mode_value = MODE_COMMAND,
    };

    assert(((uintptr_t)data % 4) == 0);
    assert((data_length % 4) == 0);

    spi_transaction_t transaction = {0};
    transaction.user = (void *)&user_data;
    transaction.length = 8 * data_length;
    transaction.tx_buffer = data;

    esp_err_t result = spi_device_queue_trans(spi_transport->device, &transaction, portMAX_DELAY);
    spi_transport->dma_in_flight = (result == ESP_OK);

    return result;
}

static esp_err_t flush_dma_data(const screen_transport_t *transport)
{
    esp_err_t result = ESP_OK;
    screen_spi_transport_t *spi_transport = (screen_spi_transport_t *)transport->context;

    if (spi_transport->dma_in_flight)
    {
        spi_transaction_t *completed_transaction;
        result = spi_device_get_trans_result(spi_transport->device, &completed_transaction, portMAX_DELAY);
     
        if (result == ESP_OK)
        {
            spi_transport->dma_in_flight = false;
        }
    }

    return result;
}