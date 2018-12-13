#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "fields_info.h"
#include "iso_8583.h"

int main(int argc, char *argv[])
{
	char iso_msg[1024];

	printf("This is a basic implementation of ISO8583 for C language\n");

	debug_enable();

	init(ISO8583_1987);
	set_mti("0200");

	add_field(2, "0123456789012345", 16);
	add_field(90, "899999999999999999999999999999999999999998", 42);

	generateMessage(iso_msg);

	if(strlen(iso_msg) > 0)
	{
		printf("ISO_MSG: [%s]\n", iso_msg);
	}

	return 0;
}
