#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iso_8583.h"
#include "fields_info.h"
#include "debug.h"

#define MASK (unsigned char) 128 // 1000 0000
#define BITS (unsigned char)   8

static char glb_mti[MTI_LEN_BYTES];
static char glb_first_bitmap[BITMAP_LEN_BYTES];
static char glb_second_bitmap[BITMAP_LEN_BYTES];
static char glb_iso_pack[LEN_MAX_ISO];
static char *glb_fields[NUM_FIELD_MAX];

static int add_in_bitmap(int field)
{
	unsigned char position = 0;
	unsigned char swift = 0;
	char *bitmap = NULL;

	if(is_valid_field(field))
	{
		if(field < BITMAP_LEN_BITS)
		{
			bitmap = glb_first_bitmap;
		}
		else
		{
			bitmap = glb_second_bitmap;
			field -= BITMAP_LEN_BITS;
		}

		field--;
		position = field / BITS;
		swift = field % BITS;

		bitmap[position] |= ( MASK >> swift);
		return 0;
	}

	return -1;
}

static int remove_from_bitmap(int field)
{
	unsigned char position = 0;
	unsigned char swift = 0;
	char *bitmap = NULL;

	if(is_valid_field(field))
	{
		if(field < BITMAP_LEN_BITS)
		{
			bitmap = glb_first_bitmap;
		}
		else
		{
			bitmap = glb_second_bitmap;
			field -= BITMAP_LEN_BITS;
		}

		field--;
		position = field / BITS;
		swift = field % BITS;

		bitmap[position] &= ~(MASK >> swift);
		return 0;
	}

	return -1;
}

static int has_second_bitmap()
{
	int i;
	for(i = 0; i < BITMAP_LEN_BYTES; i++)
	{
		if(glb_second_bitmap[i])
		{
			return 1;
		}
	}
	return 0;
}

void clear()
{
	int i = 0;

	memset(glb_mti, 0, sizeof(glb_mti));
	memset(glb_first_bitmap, 0, sizeof(glb_first_bitmap));
	memset(glb_second_bitmap, 0, sizeof(glb_second_bitmap));
	memset(glb_iso_pack, 0, sizeof(glb_iso_pack));

	for(i = 0; i < NUM_FIELD_MAX; i++)
	{
		glb_fields[i] = NULL;
	}

	debug_print("Internal variables cleared\n", __FUNCTION__);
}

int init(int iso_version)
{
	clear();

	init_field_info(iso_version);

	return -1;
}

int set_mti(const char *mti)
{
	if(is_valid_mti(mti))
	{
		memcpy(glb_mti, mti, MTI_LEN_BYTES);
		return 0;
	}

	debug_print("Error: [%s]: Invalid mti\n", __FUNCTION__);

	return -1;
}

int add_field(int field, const char *data, int length)
{
	char *field_value = NULL;

	if(is_valid_field_value(field, data))
	{
		field_value = (char *) malloc(length);
		if(field_value)
		{
			memcpy(field_value, data, length);
			glb_fields[field - 1] = field_value;

			add_in_bitmap(field);

			return 0;
		}
	}

	debug_print("Error: [%s]: Invalid field number (%d)\n", __FUNCTION__, field);

	return -1;
}

int remove_field(int field)
{
	if(is_valid_field(field) && glb_fields[field - 1] != NULL)
	{
		free(glb_fields[field - 1]);
		glb_fields[field - 1] = NULL;

		remove_from_bitmap(field);

		return 0;
	}

	debug_print("Error: [%s]: Invalid field number (%d)\n", __FUNCTION__, field);

	return -1;
}

int generateMessage(char *message)
{
	int i = 0;
	int real_i = 0;
	int size_of_length = 0;
	char format[16];

	if(message == NULL || strlen(glb_mti) != MTI_LEN_BYTES)
	{
		return -1;
	}

	// Add mti to iso message.
	sprintf(glb_iso_pack, "%s", glb_mti);

	// Add first bitmap to iso message.
	for(i = 0; i < BITMAP_LEN_BYTES; i++)
	{
		sprintf(glb_iso_pack + strlen(glb_iso_pack), "%02x", glb_first_bitmap[i]);
	}

	// Add second bitmap to iso message (case there is one).
	if(has_second_bitmap())
	{
		for(i = 0; i < BITMAP_LEN_BYTES; i++)
		{
			sprintf(glb_iso_pack + strlen(glb_iso_pack), "%02x", glb_second_bitmap[i]);
		}
	}

	// Add fields.
	for(i = 0; i < NUM_FIELD_MAX; i++)
	{
		real_i = i + 1;

		if(glb_fields[i] != NULL)
		{
			if(is_variable_field_length(real_i) == 0)
			{
				size_of_length = get_size_length_of_variable_field(real_i);
				sprintf(format, "%%0%dd", size_of_length);
				sprintf(glb_iso_pack + strlen(glb_iso_pack), format, strlen(glb_fields[i]));
			}

			sprintf(glb_iso_pack + strlen(glb_iso_pack), "%s", glb_fields[i]);
		}
	}

	memcpy(message, glb_iso_pack, strlen(glb_iso_pack) + 1);

	debug_print("Message generated!\n", __FUNCTION__);

	return 0;
}
