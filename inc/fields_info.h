#ifndef FIELDS_INFO_H_
#define FIELDS_INFO_H_

// Definitions and legend for abbreviations:
#define FI_TYPE__A                  "a"   // Alhpabetical characters (A-Z, a-z);
#define FI_TYPE__N                  "n"   // Numeric digits, (0-9);
#define FI_TYPE__P                  "p"   // Pad character (space);
#define FI_TYPE__S                  "s"   // Special characters;
#define FI_TYPE__AN                 "an"  // Alphabetical and numeric characters;
#define FI_TYPE__AS                 "as"  // Alphabetical and special characters;
#define FI_TYPE__NS                 "ns"  // Numeric and special characters;
#define FI_TYPE__ANP                "anp" // Alphabetical, numeric and space (pad) characters;
#define FI_TYPE__ANS                "ans" // Alphabetical, numeric and special characters;

#define FI_TYPE__B                  "b"   // Binary representation of data;
#define FI_TYPE__Z                  "z"   // Tracks 2 and 3 code set as defined in ISO4909 and ISO7813;

#define FI_TYPE__X                  "x"   // 'C' for Credit and 'D' for Debit and shall always be associated wih a numeric amount data element.
//I.e., x+n16 in amount, net reconciliation means prefix 'C' or 'D' and 16 digits of amount, net reconciliation.
#define FI_TYPE__XN                 FI_TYPE__X FI_TYPE__N

#define FI_TYPE__MM                 "MM"  // Month  (01-12);
#define FI_TYPE__DD                 "DD"  // Day    (01-31);
#define FI_TYPE__YY                 "YY"  // Year   (00-99);
#define FI_TYPE__HH                 "hh"  // Hour   (00-23);
#define FI_TYPE__MIN                "mm"  // Minute (01-59);
#define FI_TYPE__SS                 "ss"  // Second (01-59);

#define FI_TYPE__LL                 "LL"  // Length of variable data element (01-99);
#define FI_TYPE__LLL                "LLL" // Length of variable data element (001-999);
#define FI_TYPE__VAR                "VAR" // Variable length data element;

#define FI_TYPE__YYMM               FI_TYPE__YY FI_TYPE__MM
#define FI_TYPE__YYMMDD             FI_TYPE__YY FI_TYPE__MM FI_TYPE__DD
#define FI_TYPE__MMDD               FI_TYPE__MM FI_TYPE__DD
#define FI_TYPE__HHMINSS            FI_TYPE__HH FI_TYPE__MIN FI_TYPE__SS
#define FI_TYPE__MMDDYYHHMMSS       FI_TYPE__MM FI_TYPE__DD FI_TYPE__YY FI_TYPE__HH FI_TYPE__MIN FI_TYPE__SS

#define FI_TYPE__LLVAR              FI_TYPE__LL FI_TYPE__VAR
#define FI_TYPE__LLLVAR             FI_TYPE__LLL FI_TYPE__VAR

#define FI_VARIABLE_FIELD_FALSE     0  // Fixed length;
#define FI_VARIABLE_FIELD_TRUE      1  // Variable length up to maximun $fieldsInfo[$fieldNum]['length'] characters.

// NOTE:
// All vaiable length fields shall in addition contain two or three positions at the beginning of the data element
// to identity the number of positions following to the end of that data element.

// All fixed length 'n' (FI_TYPE__N) data elements are assumed to be right justified with leadinf zeroes.
// All other fixed length data elements are left justified with trailing spaces.
// In all 'b' data elements, blocks of 8 bits are assumed to be left justified with trailing zeros.
// All data elements are counted from left to right.

#define FI_MTI_LEN_BYTES        4
#define FI_BITMAP_LEN_BITS      64
#define FI_BITMAP_LEN_BYTES     (FI_BITMAP_LEN_BITS / 8)
#define FI_BITMAP_HEX_BYTES     (FI_BITMAP_LEN_BYTES * 2)
#define FI_NUM_FIELD_MIN        1
#define FI_NUM_FIELD_MAX        128
#define FI_LEN_MAX_ISO          (1024 * 10)

// ISO Versions:
#define FI_ISO8583_1987         0
#define FI_ISO8583_1993         1
#define FI_ISO8583_2003         2

// MTI (Message Type Identifier) Structure:
// First position:
//                  0 -> ISO8583:1987;
//                  1 -> ISO8583:1993;
//                  2 -> ISO8583:2003;
//                  3-7 -> Reserved for ISO use;
//                  8 -> Reserved for national use;
//                  9 -> Reserved for private use;
// Second position:
//                  0 -> Reserved for ISO use;
//                  1 -> Authorization;
//                  2 -> Financial;
//                  3 -> File action;
//                  4 -> Reserval/chargeback;
//                  5 -> Reconciliation;
//                  6 -> Administrative;
//                  7 -> Fee collection;
//                  8 -> Network management;
//                  9 -> Reserved for ISO use;
// Third position:
//                  0 -> Request;
//                  1 -> Request response;
//                  2 -> Advice;
//                  3 -> Advice response;
//                  4 -> Notification;
//                  5 -> Notification acknowledgement;
//                  6 -> Instruction (ISO8583:2003 only);
//                  7 -> Instruction acknowledgement (ISO8583:2003 only);
//                  8-9 -> Reserved for ISO use;
// Fourth position:
//                  0 -> Acquirer;
//                  1 -> Acquirer repeat;
//                  2 -> Card issuer;
//                  3 -> Card issuer repeat;
//                  4 -> Other
//                  5 -> Other repeat;
//                  6-9 -> Reserved for ISO use.

/**
 * Struct to be store fields info.
 */
struct fi_field_info
{
	unsigned char type[32];
	int is_variable_field;
	int length;
	unsigned char description[64];
	unsigned char format[32];
};

/**
 * Initialize fields info.
 * @param[in] mode The operation mode of fields info, you should use the following defines:
 *                 FI_ISO8583_1987 for iso 1987;
 *                 FI_ISO8583_1993 for iso 1993;
 *                 FI_ISO8583_2003 for iso 2003;
 * @return Returns 0 to success or -1 case error.
 */
int fi_init_field_info(int iso_version);

/**
 * @brief Mount struct fi_field_info using received info.
 * @param[in] type Type of field data.
 * @param[in] is_variable_field Field is variable, 0 to false or 1 to true.
 * @param[in] length Length of field.
 * @param[in] description Description of field.
 * @param[in] format Field data format.
 * @return Returns struct fi_field_info with informed data.
 */
struct fi_field_info fi_mount_field_info(const char *type, int is_variable_field, int length, const char *description, const char *format);

/**
 * @brief Validate mti length.
 * @param[in] mti The mti to be validated.
 * @return Returns 1 if valid or 0 if invalid.
 */
int fi_is_valid_mti(const char *mti);

/**
 * @brief Validate field number.
 * @param[in] field The field number to be validated.
 * @return Returns 1 if valid or 0 if invalid.
 */
int fi_is_valid_field(int field);

/**
 * @brief Validate field value.
 * @param[in] field The field number to be validated.
 * @param[in] data The data of field to be validated.
 * @return Returns 1 if data is valid or 0 if invalid.
 */
int fi_is_valid_field_value(int field, const char *data);

/**
 * @brief Validate if field has variable length.
 * @param[in] field The field number to be validated.
 * @return Returns 1 id field has variable length, 0 if is not variable length or -1 if field is invalid.
 */
int fi_is_variable_field_length(int field);

/**
 * @brief Gets info from field.
 * @param[in] field The field number to be recovered.
 * @param[out] fi_field The struct fi_field_info when info will be stored.
 * @return Returns 0 to success or -1 case error.
 */
int fi_get_field_info(int field, struct fi_field_info *fi_field);

/**
 * @brief Gets field length.
 * @param[in] field The field number to be recovered.
 * @return Returns the field length or -1 case error.
 */
int fi_get_field_length(int field);

/**
 * @brief Gets field size of length, case the field length is 999 the size of length will be 3 (strlen(999)).
 * @param[in] field The field number to be recovered.
 * @return Returns the field size of length or -1 case error.
 */
int fi_get_size_length_of_variable_field(int field);

#endif
