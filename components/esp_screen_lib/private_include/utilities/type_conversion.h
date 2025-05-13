#include <stdint.h>

#define U16_U8_HI(i) ((uint8_t)((uint16_t)i >> 8))
#define U16_U8_LO(i) ((uint8_t)i)