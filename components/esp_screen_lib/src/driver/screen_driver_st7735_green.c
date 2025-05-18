#include "esp_log.h"
#include "utility/flow_control.h"
#include "utility/type_conversion.h"
#include "driver/screen_driver_internal.h"
#include "driver/screen_st7735_common.h"
#include "driver/screen_driver_st7735_green.h"

typedef struct screen_st7735_green_driver_context_t
{
    const screen_transport_t *transport;
    uint8_t rotation;
    gpio_num_t reset_pin;
    screen_st7735_green_variant_t variant;
    screen_st7735_color_order_t color_order;
} screen_st7735_green_driver_context_t;

static const uint8_t init_script[] = {
    SCRIPT_LENGTH(25),

    COMMAND(ST7735_SWRESET), // Software reset
    DELAY(150),
    COMMAND(ST7735_SLPOUT), // Out of sleep mode
    DELAY(500),
    COMMAND(ST7735_FRMCTR1, 0x01, 0x2C, 0x2D),                   // Frame rate control - normal mode: [Rate = fosc/(1x2+40) * (LINE+2C+2D)]
    COMMAND(ST7735_FRMCTR2, 0x01, 0x2C, 0x2D),                   // Frame rate control - idle mode: [Rate = fosc/(1x2+40) * (LINE+2C+2D)]
    COMMAND(ST7735_FRMCTR3, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D), // Frame rate control - partial mode: [3|Dot inversion mode, 3|Line inversion mode]
    COMMAND(ST7735_INVCTR, 0x07),                                // Inversion control: [None]
    COMMAND(ST7735_PWCTR1, 0xA2, 0x02, 0x84),                    // Power control: [??, -4.6V, Auto Mode]
    COMMAND(ST7735_PWCTR2, 0xC5),                                // Power control: [VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD]
    COMMAND(ST7735_PWCTR3, 0x0A, 0x00),                          // Power control: [Opamp current small, Boost frequency]
    COMMAND(ST7735_PWCTR4, 0x8A, 0x2A),                          // Power control: [BCLK/2, Opamp current small & Medium low]
    COMMAND(ST7735_PWCTR5, 0x8A, 0xEE),                          // Power control: [??, ??]
    COMMAND(ST7735_VMCTR1, 0x0E),                                // Power control: [??]
    COMMAND(ST7735_INVOFF),                                      // Don't invert display
    PARAMETERS(ST7735_MADCTL, 1),                                // Get the MADCTL value from the parameters
    COMMAND(ST7735_COLMOD, 0x05),                                // Color mode: [16-bit Color]
    COMMAND(ST7735_CASET, 0x00, 0x00, 0x00, 0x7F),               // Column address set: [2| x-start = 0, 2|x-end = 127]
    COMMAND(ST7735_RASET, 0x00, 0x00, 0x00, 0x7F),               // Row address set: [2| x-start = 0, 2|x-end = 159]
    COMMAND(ST7735_GMCTRP1, 0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10),
    COMMAND(ST7735_GMCTRN1, 0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10),
    COMMAND(ST7735_NORON),
    DELAY(10),
    COMMAND(ST7735_DISPON),
    DELAY(100),
};

// --- Forward declarations ---
static esp_err_t initialize(const screen_driver_t *driver, const screen_transport_t *transport);
static esp_err_t reset(const screen_driver_t *driver);
static esp_err_t set_rotation(const screen_driver_t *driver, uint8_t rotation);
static esp_err_t set_window(const screen_driver_t *driver, uint16_t column_start, uint16_t row_start, uint16_t column_end, uint16_t row_end);
static esp_err_t write_pixels(const screen_driver_t *driver, const void *pixel_data, size_t data_length_bytes);
static esp_err_t flush(const screen_driver_t *driver);
static esp_err_t get_capabilities(const screen_driver_t *driver, screen_driver_capabilities_t *capabilities);
static esp_err_t map_color_order(screen_color_order_t generic_order, screen_st7735_color_order_t *st7735_order);

esp_err_t screen_driver_st7735_green_create(const screen_driver_st7735_green_config_t *config, screen_driver_t *driver)
{
    assert(driver != NULL);

    memset(driver, 0, sizeof(screen_driver_t));

    screen_st7735_green_driver_context_t *context = calloc(1, sizeof(screen_st7735_green_driver_context_t));
    if (context == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    screen_st7735_color_order_t st7735_color_order;
    esp_err_t result = map_color_order(config->color_order, &st7735_color_order);
    if (result != ESP_OK)
    {
        free(context);
        return result;
    }

    context->reset_pin = config->reset_pin;
    context->rotation = config->rotation;
    context->variant = config->variant;
    context->color_order = st7735_color_order;

    driver->context = context;
    driver->initialize = initialize;
    driver->reset = reset;
    driver->set_rotation = set_rotation;
    driver->set_window = set_window;
    driver->write_pixels = write_pixels;
    driver->flush = flush;
    driver->get_capabilities = get_capabilities;

    driver->screen_width = config->width;
    driver->screen_height = config->height;
    driver->color_order = config->color_order;
    driver->color_format = config->color_format;

    return ESP_OK;
}

static esp_err_t initialize(const screen_driver_t *driver, const screen_transport_t *transport)
{
    esp_err_t result;

    screen_st7735_green_driver_context_t *context = (screen_st7735_green_driver_context_t *)driver->context;
    context->transport = transport;

    uint8_t parameters = {0x0C | context->color_order};

    RETURN_IF((result = reset(driver)) != ESP_OK, result);

    RETURN_IF((result = screen_driver_execute_script(transport, init_script, parameters, 1)) != ESP_OK, result);

    return result;
}

static esp_err_t reset(const screen_driver_t *driver)
{
    esp_err_t result;

    const screen_st7735_green_driver_context_t *context = (const screen_st7735_green_driver_context_t *)driver->context;

    // Reset the controller
    RETURN_IF((result = gpio_set_level(context->reset_pin, 0)) != ESP_OK, result);
    vTaskDelay(pdMS_TO_TICKS(20));

    RETURN_IF((result = gpio_set_level(context->reset_pin, 1)) != ESP_OK, result);
    vTaskDelay(pdMS_TO_TICKS(150));

    return result;
}

static esp_err_t set_window(const screen_driver_t *driver, uint16_t column_start, uint16_t row_start, uint16_t column_end, uint16_t row_end)
{
    const screen_st7735_green_driver_context_t *context = (const screen_st7735_green_driver_context_t *)driver->context;

    uint8_t script[] = {
        COMMAND(ST7735_CASET, U16_U8_HI(column_start), U16_U8_LO(column_start), U16_U8_HI(column_end), U16_U8_LO(column_end)),
        COMMAND(ST7735_RASET, U16_U8_HI(row_start), U16_U8_LO(row_start), U16_U8_HI(row_end), U16_U8_LO(row_end)),
    };

    return screen_driver_execute_script(context->transport, script, NULL, 0);
}

static esp_err_t write_pixels(const screen_driver_t *driver, const void *pixel_data, size_t data_length_bytes)
{
    const screen_st7735_green_driver_context_t *context = (const screen_st7735_green_driver_context_t *)driver->context;

    return context->transport->send_dma_data(context->transport, pixel_data, data_length_bytes);
}

static esp_err_t flush(const screen_driver_t *driver)
{
    const screen_st7735_green_driver_context_t *context = (const screen_st7735_green_driver_context_t *)driver->context;

    return context->transport->flush_dma_data(context->transport);
}

static esp_err_t get_capabilities(const screen_driver_t *driver, screen_driver_capabilities_t *capabilities)
{
    if (capabilities == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    capabilities->dma_alignment_bytes = 4;
    capabilities->maximum_transfer_size_bytes = 32 * 1024;
    capabilities->supports_partial_update = true;
    capabilities->supports_invert_colors = true;
    capabilities->supports_madctl_rotation = true;
    capabilities->color_depth_bits = 16;

    return ESP_OK;
}

static esp_err_t map_color_order(screen_color_order_t generic_order, screen_st7735_color_order_t *st7735_order)
{
    if (st7735_order == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    switch (generic_order)
    {
        case SCREEN_COLOR_ORDER_RGB:
            *st7735_order = RGB;
            return ESP_OK;
        case SCREEN_COLOR_ORDER_BGR:
            *st7735_order = BGR;
            return ESP_OK;
        case SCREEN_COLOR_ORDER_GRB:
        case SCREEN_COLOR_ORDER_GBR:
        case SCREEN_COLOR_ORDER_RBG:
        case SCREEN_COLOR_ORDER_BRG:
            *st7735_order = RGB; // Fallback
            ESP_LOGW("ST7735", "Unsupported color order (%d). Falling back to RGB.", generic_order);
            return ESP_OK;
        default:
            *st7735_order = RGB; // Fallback
            ESP_LOGW("ST7735", "Unknown color order (%d). Falling back to RGB.", generic_order);
            return ESP_ERR_INVALID_ARG;
    }
}
