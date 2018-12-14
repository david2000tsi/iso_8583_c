#include <stdio.h>
#include <string.h>

#include "fields_info.h"
#include "debug.h"

static void load_iso_1987();
static void load_iso_1993();
static void load_iso_2003();

static struct field_info fields_info[NUM_FIELD_MAX];

#define MTI_1 "012"      // {"0", "1", "2"}                          First position;
#define MTI_2 "12345678" // {"1", "2", "3", "4", "5", "6", "7", "8"} Second position;
#define MTI_3 "01234567" // {"0", "1", "2", "3", "4", "5", "6", "7"} Third position;
#define MTI_4 "012345"   // {"0", "1", "2", "3", "4", "5"}           Fourth position.

int is_valid_mti(const char *mti)
{
	char mti_1 = 0;
	char mti_2 = 0;
	char mti_3 = 0;
	char mti_4 = 0;

	if((strlen(mti) == MTI_LEN_BYTES))
	{
		mti_1 = *(mti);
		mti_2 = *(mti + 1);
		mti_3 = *(mti + 2);
		mti_4 = *(mti + 3);

		if(strchr(MTI_1, mti_1) != NULL && strchr(MTI_2, mti_2) != NULL && strchr(MTI_3, mti_3) != NULL && strchr(MTI_4, mti_4) != NULL)
		{
			return 1;
		}
	}

	return 0;
}

int is_valid_field(int field)
{
	return (field >= NUM_FIELD_MIN && field <= NUM_FIELD_MAX);
}

int is_valid_field_value(int field, const char *data)
{
	struct field_info fi_field;
	int value_len = 0;

	if(data != NULL && is_valid_field(field))
	{
		get_field_info(field, &fi_field);
		value_len = strlen(data);

		if(value_len > 0)
		{
			if(fi_field.is_variable_field && value_len <= fi_field.length)
			{
				return 1;
			}
			else if(value_len == fi_field.length)
			{
				return 1;
			}
		}
	}

	return 0;
}

int is_variable_field_length(int field)
{
	struct field_info fi_field;

	if(is_valid_field(field))
	{
		get_field_info(field, &fi_field);
		return fi_field.is_variable_field;
	}

	return -1;
}

int get_field_info(int field, struct field_info *fi_field)
{
	if(is_valid_field(field))
	{
		*fi_field = fields_info[field - 1];
		return 0;
	}

	return -1;
}

int get_field_length(int field)
{
	struct field_info fi_field;

	if(is_valid_field(field))
	{
		get_field_info(field, &fi_field);
		return fi_field.length;
	}

	return -1;
}

int get_size_length_of_variable_field(int field)
{
	struct field_info fi_field;
	char int_str[8];

	if(is_variable_field_length(field) > -1 && get_field_info(field, &fi_field) == 0)
	{
		sprintf(int_str, "%d", fi_field.length);
		return strlen(int_str);
	}

	return -1;
}

struct field_info mount_field_info(const char *type, int is_variable_field, int length, const char *description, const char *format)
{
	struct field_info field;

	memset(&field, 0, sizeof(field));

	memcpy(field.type, type, strlen(type));
	field.is_variable_field = is_variable_field;
	field.length = length;
	memcpy(field.description, description, strlen(description));
	memcpy(field.format, format, strlen(format));

	return field;
}

int init_field_info(int iso_version)
{
	int ret = -1;

	switch(iso_version)
	{
		case ISO8583_1987:
			load_iso_1987();
			debug_print("ISO 1987 loaded successfully\n");
			ret = 0;
			break;
		case ISO8583_1993:
			load_iso_1993();
			debug_print("ISO 1993 loaded successfully\n");
			ret = 0;
			break;
		case ISO8583_2003:
			load_iso_2003();
			debug_print("Error: ISO 1993 not implemented yet\n");
			break;
		default:
			break;
	}

	return ret;
}

// Load fields info for ISO8583:1987.
static void load_iso_1987()
{
	fields_info[0]   = mount_field_info(TYPE__B,   VARIABLE_FIELD_FALSE,  64, "secondary bitmap", "");
	fields_info[1]   = mount_field_info(TYPE__N,   VARIABLE_FIELD_TRUE,   19, "primary account number", TYPE__LLVAR);
	fields_info[2]   = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   6, "processing code", "");
	fields_info[3]   = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  12, "amount, transaction", "");
	fields_info[4]   = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  12, "amount, reconciliation", "");
	fields_info[5]   = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  12, "amount, cardholder biling", "");
	fields_info[6]   = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "date and time, transmission", TYPE__MMDDYYHHMMSS);
	fields_info[7]   = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   8, "amount, cardholder biling fee", "");
	fields_info[8]   = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   8, "conversion rate, settlement", "");
	fields_info[9]   = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   8, "conversion rate, cardholder biling", "");
	fields_info[10]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   6, "system trace audit number", "");
	fields_info[11]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   6, "date and time, local transaction", TYPE__HHMINSS);
	fields_info[12]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   4, "date, local transaction", TYPE__MMDD);
	fields_info[13]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   4, "date, expiration", "");
	fields_info[14]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   4, "date, settlement", "");
	fields_info[15]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   4, "date, conversion", "");
	fields_info[16]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   4, "date, capture", "");
	fields_info[17]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   4, "merchant type", "");
	fields_info[18]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   3, "country code, acquiring institution", "");
	fields_info[19]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   3, "country code, primary account number", "");
	fields_info[20]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   3, "country code, forwarding institution", "");
	fields_info[21]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_FALSE,   3, "point of service data code", "");
	fields_info[22]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   3, "card sequence number", "");
	fields_info[23]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   3, "function code", "");
	fields_info[24]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   2, "point of sale condition code", "");
	fields_info[25]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   2, "point of sale capture code", "");
	fields_info[26]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   1, "authorization identification response length", "");
	fields_info[27]  = mount_field_info(TYPE__XN,  VARIABLE_FIELD_FALSE,   8, "amount, transaction fee", "");
	fields_info[28]  = mount_field_info(TYPE__XN,  VARIABLE_FIELD_FALSE,   8, "amount, settlement fee", "");
	fields_info[29]  = mount_field_info(TYPE__XN,  VARIABLE_FIELD_FALSE,   8, "amount, transaction processing fee", "");
	fields_info[30]  = mount_field_info(TYPE__XN,  VARIABLE_FIELD_FALSE,   8, "amount, settlement processing fee", "");
	fields_info[31]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_TRUE,   11, "acquirer institution identification code", TYPE__LLVAR);
	fields_info[32]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_TRUE,   11, "fowarding institution identification code", TYPE__LLVAR);
	fields_info[33]  = mount_field_info(TYPE__NS,  VARIABLE_FIELD_TRUE,   28, "primary account number, extended", TYPE__LLVAR);
	fields_info[34]  = mount_field_info(TYPE__Z,   VARIABLE_FIELD_TRUE,   37, "track 2 data", TYPE__LLVAR);
	fields_info[35]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_TRUE,  104, "track 3 data", TYPE__LLLVAR);
	fields_info[36]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_FALSE,  12, "retrieval reference number", "");
	fields_info[37]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_FALSE,   6, "authorization identificarion response", "");
	fields_info[38]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_FALSE,   2, "response code", "");
	fields_info[39]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_FALSE,   3, "service restriction code", "");
	fields_info[40]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_FALSE,   8, "card acceptor terminal idetification", "");
	fields_info[41]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_FALSE,  15, "card acceptor identification code", "");
	fields_info[42]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_FALSE,  40, "card acceptor name/location", TYPE__LLVAR);
	fields_info[43]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_TRUE,   25, "aditional response data", TYPE__LLVAR);
	fields_info[44]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_TRUE,   76, "track 1 data", TYPE__LLVAR);
	fields_info[45]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_TRUE,  999, "addicional data (iso)", TYPE__LLLVAR);
	fields_info[46]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_TRUE,  999, "additional data, national", TYPE__LLLVAR);
	fields_info[47]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_TRUE,  999, "additional data, private", TYPE__LLLVAR);
	fields_info[48]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_FALSE,   3, "currency code, transaction", "");
	fields_info[49]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_FALSE,   3, "currency code, settlement", "");
	fields_info[50]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_FALSE,   3, "currency code, cardholder biling", "");
	fields_info[51]  = mount_field_info(TYPE__B,   VARIABLE_FIELD_FALSE,   8, "personal identification number (PIN) data", "");
	fields_info[52]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  16, "security related control information", TYPE__LLVAR);
	fields_info[53]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_TRUE,  120, "amounts, additional", TYPE__LLLVAR);
	fields_info[54]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "integrated circuit card system related data", TYPE__LLLVAR);
	fields_info[55]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reserved (iso)", TYPE__LLLVAR);
	fields_info[56]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reserved for national use", TYPE__LLLVAR);
	fields_info[57]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reserved for national use", TYPE__LLLVAR);
	fields_info[58]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reserved for national use", TYPE__LLLVAR);
	fields_info[59]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reserved for national use", TYPE__LLLVAR);
	fields_info[60]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reserved for private use", TYPE__LLLVAR);
	fields_info[61]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reserved for private use", TYPE__LLLVAR);
	fields_info[62]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reserved for private use", TYPE__LLLVAR);
	fields_info[63]  = mount_field_info(TYPE__B,   VARIABLE_FIELD_FALSE,  16, "message authentication code (mac)", "");
	fields_info[64]  = mount_field_info(TYPE__B,   VARIABLE_FIELD_FALSE,   1, "extended bitmap indicator", "");
	fields_info[65]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   1, "settlement code", "");
	fields_info[66]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   2, "extended payment code", "");
	fields_info[67]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   3, "country code, receiving institution", "");
	fields_info[68]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   3, "country code, settlement institution", "");
	fields_info[69]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   3, "network management institution code", "");
	fields_info[70]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   4, "message number", "");
	fields_info[71]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   4, "last message number", "");
	fields_info[72]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   6, "date, action", TYPE__YYMMDD);
	fields_info[73]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "credits, number", "");
	fields_info[74]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "credits, reversal number", "");
	fields_info[75]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "debits, number", "");
	fields_info[76]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "debits, reversal number", "");
	fields_info[77]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "transfer number", "");
	fields_info[78]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "transfer, reversal number", "");
	fields_info[79]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "inquiries, number", "");
	fields_info[80]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "authorizations, number", "");
	fields_info[81]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  12, "credits, processing fee amount", "");
	fields_info[82]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  12, "credits, transaction fee amount", "");
	fields_info[83]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  12, "debits, processing fee amount", "");
	fields_info[84]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  12, "debits, transaction fee amount", "");
	fields_info[85]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  16, "credits, total amount", "");
	fields_info[86]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  16, "credits, reversal amount", "");
	fields_info[87]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  16, "debits, total amount", "");
	fields_info[88]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  16, "debits, reversal amount", "");
	fields_info[89]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  42, "original data elements", "");
	fields_info[90]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_FALSE,   1, "file update code", "");
	fields_info[91]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_FALSE,   2, "file securiry code", "");
	fields_info[92]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_FALSE,   5, "response indicator", "");
	fields_info[93]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_FALSE,   7, "service indicator", "");
	fields_info[94]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_FALSE,  42, "replacement amounts", "");
	fields_info[95]  = mount_field_info(TYPE__B,   VARIABLE_FIELD_FALSE,  64, "message securiry code", "");
	fields_info[96]  = mount_field_info(TYPE__XN,  VARIABLE_FIELD_FALSE,  16, "amount, net settlement", "");
	fields_info[97]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_FALSE,  25, "payee", "");
	fields_info[98]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_TRUE,   11, "settlement institution identification code", TYPE__LLVAR);
	fields_info[99]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_TRUE,   11, "receiving institution identification code", TYPE__LLVAR);
	fields_info[100] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,   17, "file name", TYPE__LLVAR);
	fields_info[101] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,   28, "account identification 1", TYPE__LLVAR);
	fields_info[102] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,   28, "account identification 2", TYPE__LLVAR);
	fields_info[103] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  100, "transaction description", TYPE__LLLVAR);
	fields_info[104] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for ISO use", TYPE__LLLVAR);
	fields_info[105] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for ISO use", TYPE__LLLVAR);
	fields_info[106] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for ISO use", TYPE__LLLVAR);
	fields_info[107] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for ISO use", TYPE__LLLVAR);
	fields_info[108] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for ISO use", TYPE__LLLVAR);
	fields_info[109] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for ISO use", TYPE__LLLVAR);
	fields_info[110] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for ISO use", TYPE__LLLVAR);
	fields_info[111] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for national use", TYPE__LLLVAR);
	fields_info[112] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for national use", TYPE__LLLVAR);
	fields_info[113] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for national use", TYPE__LLLVAR);
	fields_info[114] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for national use", TYPE__LLLVAR);
	fields_info[115] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for national use", TYPE__LLLVAR);
	fields_info[116] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for national use", TYPE__LLLVAR);
	fields_info[117] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for national use", TYPE__LLLVAR);
	fields_info[118] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for national use", TYPE__LLLVAR);
	fields_info[119] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for private use", TYPE__LLLVAR);
	fields_info[120] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for private use", TYPE__LLLVAR);
	fields_info[121] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for private use", TYPE__LLLVAR);
	fields_info[122] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for private use", TYPE__LLLVAR);
	fields_info[123] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for private use", TYPE__LLLVAR);
	fields_info[124] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for private use", TYPE__LLLVAR);
	fields_info[125] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for private use", TYPE__LLLVAR);
	fields_info[126] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for private use", TYPE__LLLVAR);
	fields_info[127] = mount_field_info(TYPE__B,   VARIABLE_FIELD_FALSE,  64, "message authentication code", "");
}

// Load fields info for ISO8583:1993.
static void load_iso_1993()
{
	fields_info[0]   = mount_field_info(TYPE__B,   VARIABLE_FIELD_FALSE,   8, "secondary bitmap (optional)", "");
	fields_info[1]   = mount_field_info(TYPE__N,   VARIABLE_FIELD_TRUE,   19, "primary account number", TYPE__LLVAR);
	fields_info[2]   = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   6, "processing code", "");
	fields_info[3]   = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  12, "amount, transaction", "");
	fields_info[4]   = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  12, "amount, reconciliation", "");
	fields_info[5]   = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  12, "amount, cardholder biling", "");
	fields_info[6]   = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "date and time, transmission", TYPE__MMDDYYHHMMSS);
	fields_info[7]   = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   8, "amount, cardholder biling fee", "");
	fields_info[8]   = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   8, "conversion rate, reconciliation", "");
	fields_info[9]   = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   8, "conversion rate, cardholder biling", "");
	fields_info[10]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   6, "system trace audit number", "");
	fields_info[11]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  12, "date and time, local transaction", TYPE__MMDDYYHHMMSS);
	fields_info[12]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   4, "date, effective", TYPE__YYMM);
	fields_info[13]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   4, "date, expiration", TYPE__YYMM);
	fields_info[14]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   6, "date, settlement", TYPE__YYMMDD);
	fields_info[15]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   4, "date, conversion", TYPE__MMDD);
	fields_info[16]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   4, "date, capture", TYPE__MMDD);
	fields_info[17]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   4, "merchant type", "");
	fields_info[18]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   3, "country code, acquiring institution", "");
	fields_info[19]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   3, "country code, primary account number", "");
	fields_info[20]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   3, "country code, forwarding institution", "");
	fields_info[21]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_FALSE,  12, "point of service data code", "");
	fields_info[22]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   3, "card sequence number", "");
	fields_info[23]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   3, "function code", "");
	fields_info[24]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   4, "message reason code", "");
	fields_info[25]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   4, "card receptor business code", "");
	fields_info[26]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   1, "approval code length", "");
	fields_info[27]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   6, "date, reconciliation", TYPE__YYMMDD);
	fields_info[28]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   3, "reconciliation indicator", "");
	fields_info[29]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  24, "amount original", "");
	fields_info[30]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,   99, "acquirer reference data", TYPE__LLVAR);
	fields_info[31]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_TRUE,   11, "acquirer institution identification code", TYPE__LLVAR);
	fields_info[32]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_TRUE,   11, "fowarding institution identification code", TYPE__LLVAR);
	fields_info[33]  = mount_field_info(TYPE__NS,  VARIABLE_FIELD_TRUE,   28, "primary account number, extended", TYPE__LLVAR);
	fields_info[34]  = mount_field_info(TYPE__Z,   VARIABLE_FIELD_FALSE,  37, "track 2 data", TYPE__LLVAR);
	fields_info[35]  = mount_field_info(TYPE__Z,   VARIABLE_FIELD_FALSE, 104, "track 3 data", TYPE__LLLVAR);
	fields_info[36]  = mount_field_info(TYPE__ANP, VARIABLE_FIELD_FALSE,  12, "retrieval reference number", "");
	fields_info[37]  = mount_field_info(TYPE__ANP, VARIABLE_FIELD_FALSE,   6, "approval code", "");
	fields_info[38]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   3, "action code", "");
	fields_info[39]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   3, "service code", "");
	fields_info[40]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_FALSE,   8, "card acceptor terminal idetification", "");
	fields_info[41]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_FALSE,  15, "card acceptor identification code", "");
	fields_info[42]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,   99, "card acceptor name/location", TYPE__LLVAR);
	fields_info[43]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,   99, "aditional response data", TYPE__LLVAR);
	fields_info[44]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,   76, "track 1 data", TYPE__LLVAR);
	fields_info[45]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  204, "amounts, fees", TYPE__LLLVAR);
	fields_info[46]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "additional data, national", TYPE__LLLVAR);
	fields_info[47]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "additional data, private", TYPE__LLLVAR);
	fields_info[48]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_FALSE,   3, "currency code, transaction", "");
	fields_info[49]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_FALSE,   3, "currency code, reconciliation", "");
	fields_info[50]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_FALSE,   3, "currency code, cardholder biling", "");
	fields_info[51]  = mount_field_info(TYPE__B,   VARIABLE_FIELD_FALSE,   8, "personal identification number (PIN) data", "");
	fields_info[52]  = mount_field_info(TYPE__B,   VARIABLE_FIELD_TRUE,   48, "security related control information", TYPE__LLVAR);
	fields_info[53]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  120, "amounts, additional", TYPE__LLLVAR);
	fields_info[54]  = mount_field_info(TYPE__B,   VARIABLE_FIELD_TRUE,  255, "integrated circuit card system related data", TYPE__LLLVAR);
	fields_info[55]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_TRUE,   35, "original data elements", TYPE__LLVAR);
	fields_info[56]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   3, "authorization life cycle code", "");
	fields_info[57]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_TRUE,   11, "authorizing agent institution identification code", TYPE__LLVAR);
	fields_info[58]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "transport data", TYPE__LLVAR);
	fields_info[59]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reserved for national use", TYPE__LLLVAR);
	fields_info[60]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reserved for national use", TYPE__LLLVAR);
	fields_info[61]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reserved for national use", TYPE__LLLVAR);
	fields_info[62]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reserved for national use", TYPE__LLLVAR);
	fields_info[63]  = mount_field_info(TYPE__B,   VARIABLE_FIELD_FALSE,   8, "message authentication code field", "");
	fields_info[64]  = mount_field_info(TYPE__B,   VARIABLE_FIELD_FALSE,   8, "reserved for ISO use", "");
	fields_info[65]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  204, "amounts, origial fees", TYPE__LLLVAR);
	fields_info[66]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   2, "extended payment data", "");
	fields_info[67]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   3, "country code, receiving institution", "");
	fields_info[68]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   3, "country code, settlement institution", "");
	fields_info[69]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   3, "country code, authorizing agent institution", "");
	fields_info[70]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   8, "message number", "");
	fields_info[71]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "data record", TYPE__LLLVAR);
	fields_info[72]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   6, "date, action", TYPE__YYMMDD);
	fields_info[73]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "credits, number", "");
	fields_info[74]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "credits, reversal number", "");
	fields_info[75]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "debits, number", "");
	fields_info[76]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "debits, reversal number", "");
	fields_info[77]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "transfer number", "");
	fields_info[78]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "transfer, reversal number", "");
	fields_info[79]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "inquiries, number", "");
	fields_info[80]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "authorizations, number", "");
	fields_info[81]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "inquiries, reversal number", "");
	fields_info[82]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "payments, number", "");
	fields_info[83]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "payments, reversal number", "");
	fields_info[84]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "fee collections, number", "");
	fields_info[85]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  16, "credits, amount", "");
	fields_info[86]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  16, "credits, reversal amount", "");
	fields_info[87]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  16, "debits, amount", "");
	fields_info[88]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  16, "debits, reversal amount", "");
	fields_info[89]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "authorizations, reversal number", "");
	fields_info[90]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   3, "country code, transaction destination institution", "");
	fields_info[91]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,   3, "country code, transaction originator institution", "");
	fields_info[92]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_TRUE,   11, "transaction destination institution identification code", TYPE__LLVAR);
	fields_info[93]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_TRUE,   11, "transaction originator institution identification code", TYPE__LLVAR);
	fields_info[94]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,   99, "card issuer reference data", TYPE__LLVAR);
	fields_info[95]  = mount_field_info(TYPE__B,   VARIABLE_FIELD_TRUE,  999, "key management data", TYPE__LLLVAR);
	fields_info[96]  = mount_field_info(TYPE__XN,  VARIABLE_FIELD_FALSE,  16, "amount, net reconciliation", "");
	fields_info[97]  = mount_field_info(TYPE__ANS, VARIABLE_FIELD_FALSE,  25, "payee", "");
	fields_info[98]  = mount_field_info(TYPE__AN,  VARIABLE_FIELD_TRUE,   11, "settlement institution identification code", TYPE__LLVAR);
	fields_info[99]  = mount_field_info(TYPE__N,   VARIABLE_FIELD_TRUE,   11, "receiving institution identification code", TYPE__LLVAR);
	fields_info[100] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,   17, "file name", TYPE__LLVAR);
	fields_info[101] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,   28, "account identification 1", TYPE__LLVAR);
	fields_info[102] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,   28, "account identification 2", TYPE__LLVAR);
	fields_info[103] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  100, "transaction description", TYPE__LLLVAR);
	fields_info[104] = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  16, "credits, chargeback amount", "");
	fields_info[105] = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  16, "debits, chargeback amount", "");
	fields_info[106] = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "credits, chargeback number", "");
	fields_info[107] = mount_field_info(TYPE__N,   VARIABLE_FIELD_FALSE,  10, "debits, chargeback number", "");
	fields_info[108] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,   84, "credits, fee amounts", TYPE__LLVAR);
	fields_info[109] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,   84, "debits, fee amounts", TYPE__LLVAR);
	fields_info[110] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for ISO use", TYPE__LLLVAR);
	fields_info[111] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for ISO use", TYPE__LLLVAR);
	fields_info[112] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for ISO use", TYPE__LLLVAR);
	fields_info[113] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for ISO use", TYPE__LLLVAR);
	fields_info[114] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for ISO use", TYPE__LLLVAR);
	fields_info[115] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for national use", TYPE__LLLVAR);
	fields_info[116] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for national use", TYPE__LLLVAR);
	fields_info[117] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for national use", TYPE__LLLVAR);
	fields_info[118] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for national use", TYPE__LLLVAR);
	fields_info[119] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for national use", TYPE__LLLVAR);
	fields_info[120] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for national use", TYPE__LLLVAR);
	fields_info[121] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for national use", TYPE__LLLVAR);
	fields_info[122] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for private use", TYPE__LLLVAR);
	fields_info[123] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for private use", TYPE__LLLVAR);
	fields_info[124] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for private use", TYPE__LLLVAR);
	fields_info[125] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for private use", TYPE__LLLVAR);
	fields_info[126] = mount_field_info(TYPE__ANS, VARIABLE_FIELD_TRUE,  999, "reversed for private use", TYPE__LLLVAR);
	fields_info[127] = mount_field_info(TYPE__B,   VARIABLE_FIELD_FALSE,   8, "message authentication code field", "");
}

// Load fields info for ISO8583:2003.
static void load_iso_2003()
{
}
