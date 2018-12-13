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
	add_field(4, "123456789012", 12);
	add_field(31, "11111111", 8);
	add_field(32, "7777777", 7);
	add_field(34, "947654652576423534875345", 24);
	add_field(36, "44441758497514729142975874528475924356724976542952475897342547328524387839457294553303486409624354354444", 104);
	add_field(50, "222", 3);
	add_field(72, "7777", 4);
	add_field(76, "0987654321", 10);
	add_field(90, "899999999999999999999999999999999999999998", 42);
	add_field(95, "777437294863654765689476984987564264363567", 42);

	generate_message(iso_msg);

	if(strlen(iso_msg) > 0)
	{
		printf("ISO_MSG: [%s]\n", iso_msg);
	}

	return 0;
}
