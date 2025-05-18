#include <stdbool.h>
#include "screen_driver_internal.h"

esp_err_t screen_driver_execute_script(const screen_transport_t *transport, const uint8_t *script, const uint8_t *parameters, size_t parameters_length)
{
    esp_err_t result = ESP_OK;
    uint8_t length = script[0];
    size_t script_index = 1;
    size_t parameters_index = 0;
    uint8_t header;
    uint8_t command;
    uint8_t data_length;
    bool is_delay;
    uint16_t delay;

    while (length > 0 && result == ESP_OK)
    {
        header = script[script_index++];
        command = script[script_index++]; // Index will always be at the data or the start of the next block

        if (IS_COMMAND(header))
        {
            // ESP_LOGD(TAG, "Command #%d at index %d is 0x%X with header 0x%X", length, index, command, header);
            // ESP_LOGD(TAG, "Sending command 0x%x", command);

            data_length = GET_DATA_LENGTH(header);

            // Send command
            result = transport->send_command(transport, command);

            // Send data
            if (data_length > 0 && result == ESP_OK)
            {
                // ESP_LOGD(TAG, "Sending data of length %d", data_length);

                result = transport->send_data(transport, &script[script_index], data_length);

                script_index += data_length;
            }
        }
        else if (IS_PARAMETERS(header))
        {
            // ESP_LOGD(TAG, "Parameters #%d at index %d is 0x%X with header 0x%X", length, index, command, header);
            // ESP_LOGD(TAG, "Sending command 0x%x", command);

            data_length = GET_DATA_LENGTH(header);

            // Send command
            result = transport->send_command(transport, command);

            // Send data
            if (data_length > 0 && result == ESP_OK)
            {
                // ESP_LOGD(TAG, "Sending data of length %d", data_length);

                result = transport->send_data(transport, &parameters[parameters_index], data_length);

                parameters_index += data_length;
            }
        }
        else if (IS_DELAY(header))
        {
            delay = GET_DELAY_VALUE(header, command);

            // ESP_LOGD(TAG, "Delaying for %d", delay);

            vTaskDelay(pdMS_TO_TICKS(delay));
        }

        length--;
    }

    return result;
}