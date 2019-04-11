#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "fields_info.h"
#include "iso_8583.h"

int main(int argc, char *argv[])
{
	char iso_msg[1024];
	char field_str[128];

	printf("This is a basic implementation of ISO8583 for C language\n");

	debug_enable();

	printf("\nGenerate ISO message test:\n");

	iso_init(FI_ISO8583_1987);
	iso_enable_auto_padding();
	iso_set_mti("0200");

	iso_add_field(2, "0123456789012345", 16);
	iso_add_field(4, "123456789012", 12);
	iso_add_field(31, "11111111", 8);
	iso_add_field(32, "7777777", 7);
	iso_add_field(34, "947654652576423534875345", 24);
	iso_add_field(36, "44441758497514729142975874528475924356724976542952475897342547328524387839457294553303486409624354354444", 104);
	iso_add_field(37, "373737", 0); // Auto padding to length 12 (using '0')
	iso_add_field(43, "434343", 0); // Auto padding to length 40 (using '0')
	iso_add_field(50, "222", 3);
	iso_add_field(52, "52", 0); // Auto padding to length 8 (using ' ')
	iso_add_field(72, "7777", 4);
	iso_add_field(76, "0987654321", 10);
	iso_add_field(90, "899999999999999999999999999999999999999998", 42);
	iso_add_field(95, "777437294863654765689476984987564264363567", 42);

	if(iso_generate_message(iso_msg) == 0 && strlen(iso_msg) > 0)
	{
		printf("ISO_MSG: [%s]\n", iso_msg);

		printf("Recovering fields:\n");

		for(int i = 2; i <= 128; i++)
		{
			if(iso_is_set_field(i) && iso_get_field(i, field_str) == 0)
			{
				printf("Field %03d: [%s]\n", i, field_str);
			}
		}
	}

	iso_release();

	printf("\nDecode ISO message test:\n");

	if(iso_decode_message(iso_msg) == 0)
	{
		printf("ISO decoded successfully, fields:\n");

		for(int i = 2; i <= 128; i++)
		{
			if(iso_is_set_field(i) && iso_get_field(i, field_str) == 0)
			{
				printf("Field %03d: [%s]\n", i, field_str);
			}
		}
	}

	return 0;
}
