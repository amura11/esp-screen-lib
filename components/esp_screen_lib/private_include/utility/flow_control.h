#include <stdint.h>

#define __SECTION_IF_INTERNAL(test, x) for (uint8_t x = 0; x == 0 && (test); x++)
#define __SECTION_START_INTERNAL(x) for (uint8_t x = 0; x == 0; x++)

/// @brief Starts a section if the provided test is true, otherwise it will be skipped.
/// @param test The statement to check before entering the section.
#define SECTION_START_IF(test) __SECTION_IF_INTERNAL(test, __x##__COUNTER__)

#define SECTION_START() __SECTION_START_INTERNAL(__x##__COUNTER__)

/// @brief Leaves the current section if the test if true.
/// @param test The statement to check before leaving the section.
#define LEAVE_IF(test) \
    if (test)          \
    {                  \
        break;         \
    }

/// @brief Returns the provided value if the test is true.
/// @param test The statement to check before returning the provided value.
/// @param value The value to return when the statement is true.
#define RETURN_IF(test, value) \
    if (test)                  \
    {                          \
        return value;          \
    }
