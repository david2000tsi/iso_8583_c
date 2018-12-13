#include <stdio.h>

#include "debug.h"

#define MAX_MSG 1024

static int is_enabled = 0;

void debug_enable()
{
	is_enabled = 1;
}

void debug_disable()
{
	is_enabled = 0;
}

int debug_print(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	char msg[MAX_MSG];

	if(is_enabled)
	{
		vsnprintf(msg, sizeof(msg), format, ap);
		return printf("%s", msg);
	}

	return 0;
}
