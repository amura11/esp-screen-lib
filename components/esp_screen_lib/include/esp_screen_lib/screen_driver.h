#pragma once

#include <stdint.h>

typedef struct screen_driver_t
{
    void (*set_window)(screen_driver_t *driver, uint16_t column_start, uint16_t row_start, uint16_t column_end, uint16_t row_end);
} screen_driver_t;