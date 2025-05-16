#pragma once

#include <stdint.h>
#include "screen_transport.h"

#define PARAMETERS_FLAG 0b01000000
#define DELAY_FLAG 0b10000000

#define GET_DELAY_VALUE(b1, b2) (uint16_t)(((((uint32_t)b1) & 0x0F) << 8) | (((uint32_t)b2) & 0xFF))
#define GET_DATA_LENGTH(x) (x & 0b00111111)

#define IS_DELAY(x) ((x & DELAY_FLAG) == DELAY_FLAG)
#define IS_COMMAND(x) ((x & 0b11000000) == 0)
#define IS_PARAMETERS(x) ((x & PARAMETERS_FLAG) == PARAMETERS_FLAG)

// --- Step 1: Define all COMMAND_N macros ---
#define COMMAND_0(cmd)       0x00, cmd
#define COMMAND_1(cmd, ...)  0x01, cmd, __VA_ARGS__
#define COMMAND_2(cmd, ...)  0x02, cmd, __VA_ARGS__
#define COMMAND_3(cmd, ...)  0x03, cmd, __VA_ARGS__
#define COMMAND_4(cmd, ...)  0x04, cmd, __VA_ARGS__
#define COMMAND_5(cmd, ...)  0x05, cmd, __VA_ARGS__
#define COMMAND_6(cmd, ...)  0x06, cmd, __VA_ARGS__
#define COMMAND_7(cmd, ...)  0x07, cmd, __VA_ARGS__
#define COMMAND_8(cmd, ...)  0x08, cmd, __VA_ARGS__
#define COMMAND_9(cmd, ...)  0x09, cmd, __VA_ARGS__
#define COMMAND_10(cmd, ...) 0x0A, cmd, __VA_ARGS__
#define COMMAND_11(cmd, ...) 0x0B, cmd, __VA_ARGS__
#define COMMAND_12(cmd, ...) 0x0C, cmd, __VA_ARGS__
#define COMMAND_13(cmd, ...) 0x0D, cmd, __VA_ARGS__
#define COMMAND_14(cmd, ...) 0x0E, cmd, __VA_ARGS__
#define COMMAND_15(cmd, ...) 0x0F, cmd, __VA_ARGS__
#define COMMAND_16(cmd, ...) 0x10, cmd, __VA_ARGS__
#define COMMAND_17(cmd, ...) 0x11, cmd, __VA_ARGS__
#define COMMAND_18(cmd, ...) 0x12, cmd, __VA_ARGS__
#define COMMAND_19(cmd, ...) 0x13, cmd, __VA_ARGS__
#define COMMAND_20(cmd, ...) 0x14, cmd, __VA_ARGS__
#define COMMAND_21(cmd, ...) 0x15, cmd, __VA_ARGS__
#define COMMAND_22(cmd, ...) 0x16, cmd, __VA_ARGS__
#define COMMAND_23(cmd, ...) 0x17, cmd, __VA_ARGS__
#define COMMAND_24(cmd, ...) 0x18, cmd, __VA_ARGS__
#define COMMAND_25(cmd, ...) 0x19, cmd, __VA_ARGS__
#define COMMAND_26(cmd, ...) 0x1A, cmd, __VA_ARGS__
#define COMMAND_27(cmd, ...) 0x1B, cmd, __VA_ARGS__
#define COMMAND_28(cmd, ...) 0x1C, cmd, __VA_ARGS__
#define COMMAND_29(cmd, ...) 0x1D, cmd, __VA_ARGS__
#define COMMAND_30(cmd, ...) 0x1E, cmd, __VA_ARGS__
#define COMMAND_31(cmd, ...) 0x1F, cmd, __VA_ARGS__
#define COMMAND_32(cmd, ...) 0x20, cmd, __VA_ARGS__

// --- Step 2: Count number of arguments ---
#define GET_MACRO(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,NAME,...) NAME

// --- Step 3: Dispatch to the correct macro ---
#define COMMAND_DISPATCH(...) \
    GET_MACRO(__VA_ARGS__, \
        COMMAND_32, COMMAND_31, COMMAND_30, COMMAND_29, COMMAND_28, COMMAND_27, \
        COMMAND_26, COMMAND_25, COMMAND_24, COMMAND_23, COMMAND_22, COMMAND_21, \
        COMMAND_20, COMMAND_19, COMMAND_18, COMMAND_17, COMMAND_16, COMMAND_15, \
        COMMAND_14, COMMAND_13, COMMAND_12, COMMAND_11, COMMAND_10, COMMAND_9, \
        COMMAND_8, COMMAND_7, COMMAND_6, COMMAND_5, COMMAND_4, COMMAND_3, \
        COMMAND_2, COMMAND_1, COMMAND_0)(__VA_ARGS__)

#define SCRIPT_LENGTH(x) (x)
#define COMMAND(...) COMMAND_DISPATCH(__VA_ARGS__)
#define PARAMETERS(command, length) (PARAMETERS_FLAG | (length & 0b00111111)), command
#define DELAY(length) (DELAY_FLAG | ((((uint16_t)length) & 0x0F00) >> 8)), (((uint16_t)length) & 0x00FF)

esp_err_t screen_driver_execute_script(const screen_transport_t *transport, const uint8_t *script, const uint8_t *parameters, size_t parameters_length);