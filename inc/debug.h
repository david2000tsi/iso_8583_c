#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdarg.h>

void debug_enable();
void debug_disable();
int debug_print(const char *format, ...);

#endif
