#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "fields_info.h"
#include "iso_8583.h"

#define ISO_MSG "0200D00000035000400001100042000000001601234567890123451234567890121111111107777777724947654652576423" \
                "5348753451044444175849751472914297587452847592435672497654295247589734254732852438783945729455330348" \
                "6409624354354444222777709876543218999999999999999999999999999999999999999987774372948636547656894769" \
                "84987564264363567"

int main(int argc, char *argv[])
{
	char iso_msg[1024];
	char field_str[128];

	printf("This is a basic implementation of ISO8583 for C language\n");

	debug_enable();

	printf("\nGenerate ISO message test:\n");

	init(FI_ISO8583_1987);
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

	if(generate_message(iso_msg) == 0 && strlen(iso_msg) > 0)
	{
		printf("ISO_MSG: [%s]\n", iso_msg);

		printf("Recovering fields:\n");

		if(get_field(2, field_str) == 0)
		{
			printf("Field %03d: [%s]\n", 2, field_str);
		}

		if(get_field(4, field_str) == 0)
		{
			printf("Field %03d: [%s]\n", 4, field_str);
		}

		if(get_field(31, field_str) == 0)
		{
			printf("Field %03d: [%s]\n", 31, field_str);
		}

		if(get_field(34, field_str) == 0)
		{
			printf("Field %03d: [%s]\n", 34, field_str);
		}

		if(get_field(76, field_str) == 0)
		{
			printf("Field %03d: [%s]\n", 76, field_str);
		}
	}

	release();

	printf("\nDecode ISO message test:\n");

	if(decode_message(ISO_MSG) == 0)
	{
		printf("ISO decoded successfully, fields:\n");

		if(get_field(2, field_str) == 0)
		{
			printf("Field %03d: [%s]\n", 2, field_str);
		}

		if(get_field(4, field_str) == 0)
		{
			printf("Field %03d: [%s]\n", 4, field_str);
		}

		if(get_field(31, field_str) == 0)
		{
			printf("Field %03d: [%s]\n", 31, field_str);
		}

		if(get_field(34, field_str) == 0)
		{
			printf("Field %03d: [%s]\n", 34, field_str);
		}

		if(get_field(76, field_str) == 0)
		{
			printf("Field %03d: [%s]\n", 76, field_str);
		}
	}

	return 0;
}
