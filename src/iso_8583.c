#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "iso_8583.h"
#include "fields_info.h"
#include "debug.h"

#define ISO_MASK (unsigned char) 128 // 1000 0000
#define ISO_BITS (unsigned char)   8

// String: Stores the mti.
static char glb_mti[FI_MTI_LEN_BYTES + 1];

// Byte Vector: Store the first bitmap;
static char glb_first_bitmap[FI_BITMAP_LEN_BYTES];

// Byte Vector: Store the second bitmap;
static char glb_second_bitmap[FI_BITMAP_LEN_BYTES];

// String: Store the iso message;
static char glb_iso_pack[FI_LEN_MAX_ISO + 1];

// Pointer Vector: Store the fields data.
static char *glb_fields[FI_NUM_FIELD_MAX];

// Function prototype.
static int _iso_has_second_bitmap();

// Appends new_str to end of original original_str.
static void _iso_append_str_data(char *original_str, const char *new_str)
{
	char msg[FI_LEN_MAX_ISO];

	// Create new buffer with original_str and new_str.
	sprintf(msg, "%s%s", original_str, new_str);

	sprintf(original_str, "%s", msg);
}

// Appends new_str in hex format to end of original original_str.
static void _iso_append_hex_data(char *original_str, const unsigned char new_str)
{
	char msg[FI_LEN_MAX_ISO];

	// Create new buffer with original_str and new_str.
	sprintf(msg, "%s%02X", original_str, new_str);

	sprintf(original_str, "%s", msg);
}

// Extract 'length' bytes from original_str and store it in the output.
static void _iso_extract_str_data(char *original_str, unsigned int length, char *output)
{
	char msg[FI_LEN_MAX_ISO];

	// Extract 'length' bytes and put into msg buffer
	memcpy(msg, original_str, length);
	msg[length] = '\0';
	sprintf(output, "%s", msg);

	// Clean extracted data from original string.
	memcpy(msg, original_str, strlen(original_str));
	msg[strlen(original_str)] = '\0';
	sprintf(original_str, "%s", msg + length);
}

// Update the bit one of first bitmap.
static void _iso_update_bit_one()
{
	if(_iso_has_second_bitmap())
	{
		glb_first_bitmap[0] |= ISO_MASK;
	}
}

// Check if bit one is up.
static int _iso_is_up_bit_one()
{
	return (glb_first_bitmap[0] & ISO_MASK);
}

// Check if field is up in the bitmap.
static int _iso_is_up_field(int field)
{
	unsigned char position = 0;
	unsigned char shift = 0;
	char *bitmap = NULL;

	if(fi_is_valid_field(field))
	{
		if(field <= FI_BITMAP_LEN_BITS)
		{
			bitmap = glb_first_bitmap;
		}
		else
		{
			bitmap = glb_second_bitmap;
			field -= FI_BITMAP_LEN_BITS;
		}

		field--;
		position = field / ISO_BITS;
		shift = field % ISO_BITS;

		return (bitmap[position] & (ISO_MASK >> shift));
	}

	return -1;
}

// Add field in the bitmap.
static int _iso_add_in_bitmap(int field)
{
	unsigned char position = 0;
	unsigned char shift = 0;
	char *bitmap = NULL;

	if(fi_is_valid_field(field))
	{
		if(field < FI_BITMAP_LEN_BITS)
		{
			bitmap = glb_first_bitmap;
		}
		else
		{
			bitmap = glb_second_bitmap;
			field -= FI_BITMAP_LEN_BITS;
		}

		field--;
		position = field / ISO_BITS;
		shift = field % ISO_BITS;

		bitmap[position] |= (ISO_MASK >> shift);

		_iso_update_bit_one();

		return 0;
	}

	return -1;
}

// Remove field from the bitmap.
static int _iso_remove_from_bitmap(int field)
{
	unsigned char position = 0;
	unsigned char shift = 0;
	char *bitmap = NULL;

	if(fi_is_valid_field(field))
	{
		if(field < FI_BITMAP_LEN_BITS)
		{
			bitmap = glb_first_bitmap;
		}
		else
		{
			bitmap = glb_second_bitmap;
			field -= FI_BITMAP_LEN_BITS;
		}

		field--;
		position = field / ISO_BITS;
		shift = field % ISO_BITS;

		bitmap[position] &= ~(ISO_MASK >> shift);

		_iso_update_bit_one();

		return 0;
	}

	return -1;
}

// Check if there is a second bitmap.
static int _iso_has_second_bitmap()
{
	int i;
	for(i = 0; i < FI_BITMAP_LEN_BYTES; i++)
	{
		if(glb_second_bitmap[i])
		{
			return 1;
		}
	}
	return 0;
}

// Check if bitmap is valid.
static int _iso_is_valid_bitmap(const char *bmp_hex_str)
{
	int i = 0;
	char l = 0;

	if(strlen(bmp_hex_str) % 2)
	{
		return 0;
	}

	for(i = 0; i < strlen(bmp_hex_str); i++)
	{
		l = *(bmp_hex_str + i);
		if(isxdigit(l) == 0)
		{
			return 0;
		}
	}

	return 1;
}

// Decode bitmap from hex string to binary.
static int _iso_decode_bitmap(const char *bmp_hex_str, char *output)
{
	int i = 0;
	char tmp[8];
	char bmp[16];

	if(_iso_is_valid_bitmap(bmp_hex_str))
	{
		for(i = 0; i < FI_BITMAP_LEN_BYTES; i++)
		{
			tmp[0] = *(bmp_hex_str + (i * 2));
			tmp[1] = *(bmp_hex_str + (i * 2) + 1);
			tmp[2] = '\0';

			bmp[i] = (char) strtol(tmp, NULL, 16);
			bmp[i + 1] = '\0';
		}

		memcpy(output, bmp, FI_BITMAP_LEN_BYTES);

		return 0;
	}

	return -1;
}

// Decode first bitmap from hex string to binary.
static int _iso_decode_first_bitmap(const char *bmp_hex_str)
{
	return _iso_decode_bitmap(bmp_hex_str, glb_first_bitmap);
}

// Decode second bitmap from hex string to binary.
static int _iso_decode_second_bitmap(const char *bmp_hex_str)
{
	return _iso_decode_bitmap(bmp_hex_str, glb_second_bitmap);
}

// Cleans the internal variables, never call this function before release fields memory with free function.
static void _iso_clear_internal_vars()
{
	int i = 0;

	memset(glb_mti, 0, sizeof(glb_mti));
	memset(glb_first_bitmap, 0, sizeof(glb_first_bitmap));
	memset(glb_second_bitmap, 0, sizeof(glb_second_bitmap));
	memset(glb_iso_pack, 0, sizeof(glb_iso_pack));

	for(i = 0; i < FI_NUM_FIELD_MAX; i++)
	{
		glb_fields[i] = NULL;
	}
}

int iso_init(int iso_version)
{
	_iso_clear_internal_vars();

	fi_init_field_info(iso_version);

	return -1;
}

void iso_release()
{
	int i = 0;

	for(i = 0; i < FI_NUM_FIELD_MAX; i++)
	{
		if(glb_fields[i] != NULL)
		{
			free(glb_fields[i]);
			glb_fields[i] = NULL;
		}
	}

	_iso_clear_internal_vars();
}

int iso_set_mti(const char *mti)
{
	if(fi_is_valid_mti(mti))
	{
		memcpy(glb_mti, mti, FI_MTI_LEN_BYTES);
		return 0;
	}

	debug_print("Error: [%s]: Invalid mti\n", __FUNCTION__);

	return -1;
}

int iso_get_mti(char *mti)
{
	if(mti != NULL && strlen(glb_mti) == FI_MTI_LEN_BYTES)
	{
		sprintf(mti, "%s", glb_mti);
		return 0;
	}

	return -1;
}

int iso_add_field(int field, const char *data, int length)
{
	char *field_value = NULL;

	if(field == 1)
	{
		debug_print("Error: [%s]: Reserved use for field (%d)!\n", __FUNCTION__, field);
		return -1;
	}

	if(fi_is_valid_field_value(field, data))
	{
		field_value = (char *) malloc(length + 1);
		if(field_value)
		{
			memcpy(field_value, data, length);
			field_value[length] = '\0';

			glb_fields[field - 1] = field_value;

			_iso_add_in_bitmap(field);

			return 0;
		}
	}

	debug_print("Error: [%s]: Invalid field number (%d)\n", __FUNCTION__, field);

	return -1;
}

int iso_get_field(int field, char *data)
{
	if(field == 1)
	{
		debug_print("Error: [%s]: Reserved use for field (%d)!\n", __FUNCTION__, field);
		return -1;
	}

	if(fi_is_valid_field(field) && glb_fields[field - 1] != NULL)
	{
		sprintf(data,"%s", glb_fields[field - 1]);
		return 0;
	}

	return -1;
}

int iso_remove_field(int field)
{
	if(field == 1)
	{
		debug_print("Error: [%s]: Reserved use for field (%d)!\n", __FUNCTION__, field);
		return -1;
	}

	if(fi_is_valid_field(field) && glb_fields[field - 1] != NULL)
	{
		free(glb_fields[field - 1]);
		glb_fields[field - 1] = NULL;

		_iso_remove_from_bitmap(field);

		return 0;
	}

	debug_print("Error: [%s]: Invalid field number (%d)\n", __FUNCTION__, field);

	return -1;
}

int iso_generate_message(char *message)
{
	int i = 0;
	int real_i = 0;
	int size_of_length = 0;
	char format[16];

	if(message == NULL || strlen(glb_mti) != FI_MTI_LEN_BYTES)
	{
		return -1;
	}

	// Add mti to iso message.
	_iso_append_str_data(glb_iso_pack, glb_mti);

	// Add first bitmap to iso message.
	for(i = 0; i < FI_BITMAP_LEN_BYTES; i++)
	{
		_iso_append_hex_data(glb_iso_pack, (const unsigned char) glb_first_bitmap[i]);
	}

	// Add second bitmap to iso message (case there is one), it will be stored in the field 1.
	if(_iso_has_second_bitmap())
	{
		glb_fields[0] = (char *) malloc(FI_BITMAP_HEX_BYTES + 1);
		if(glb_fields[0] != NULL)
		{
			*glb_fields[0] = '\0';

			for(i = 0; i < FI_BITMAP_LEN_BYTES; i++)
			{
				_iso_append_hex_data(glb_fields[0], (const unsigned char) glb_second_bitmap[i]);
			}

			_iso_add_in_bitmap(1);
		}
	}

	// Add fields.
	for(i = 0; i < FI_NUM_FIELD_MAX; i++)
	{
		real_i = i + 1;

		if(glb_fields[i] != NULL)
		{
			if(fi_is_variable_field_length(real_i))
			{
				size_of_length = fi_get_size_length_of_variable_field(real_i);
				sprintf(format, "%%0%dd", size_of_length);
				sprintf(glb_iso_pack + strlen(glb_iso_pack), format, strlen(glb_fields[i]));
			}

			_iso_append_str_data(glb_iso_pack, (const char *) glb_fields[i]);
		}
	}

	sprintf(message, "%s", glb_iso_pack);

	debug_print("Message generated!\n", __FUNCTION__);

	return 0;
}

int iso_decode_message(const char *message)
{
	int i = 0;
	struct fi_field_info _fi_field;
	int length = 0;
	char msg_to_decode[FI_LEN_MAX_ISO];
	char buffer[2014];

	iso_release();

	// Copy original message to internal buffer.
	sprintf(msg_to_decode, "%s", message);

	// Extract mti.
	_iso_extract_str_data(msg_to_decode, FI_MTI_LEN_BYTES, glb_mti);
	if(!fi_is_valid_mti(glb_mti))
	{
		debug_print("Error: [%s]: Invalid ISO message!\n", __FUNCTION__);
		return -1;
	}

	// Extract first bitmap.
	_iso_extract_str_data(msg_to_decode, FI_BITMAP_HEX_BYTES, buffer);
	_iso_decode_first_bitmap(buffer);

	// If there is second bitmap we will to extract it also (aka field 1).
	if(_iso_is_up_bit_one())
	{
		_iso_extract_str_data(msg_to_decode, FI_BITMAP_HEX_BYTES, buffer);
		_iso_decode_second_bitmap(buffer);
	}

	// Extract fields (skip field 1).
	for(i = 2; i <= FI_NUM_FIELD_MAX; i++)
	{
		if(_iso_is_up_field(i) > 0)
		{
			if(fi_get_field_info(i, &_fi_field) == 0)
			{
				if(_fi_field.is_variable_field)
				{
					length = fi_get_size_length_of_variable_field(i);
					_iso_extract_str_data(msg_to_decode, length, buffer);
					length = strtol(buffer, NULL, 10);
				}
				else
				{
					length = _fi_field.length;
				}

				glb_fields[i - 1] = (char *) malloc(length + 1);
				if(glb_fields[i - 1] != NULL)
				{
					_iso_extract_str_data(msg_to_decode, length, buffer);
					sprintf(glb_fields[i - 1], "%s", buffer);
				}
			}
		}
	}

	return 0;
}
