#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdarg.h>

/**
 * @brief Enable debug messages.
 */
void debug_enable();

/**
 * @brief Disable debug messages.
 */
void debug_disable();

/**
 * @brief Print message to console.
 * @param[in] format The message format as printf function.
 * @return Returns 0 case debug is disabled or returns the number of printed bytes case debug is enabled.
 */
int debug_print(const char *format, ...);

#endif
