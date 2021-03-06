#ifndef ISO8583_H_
#define ISO8583_H_

/**
 * @brief Generates hex string from binary data.
 * @param[in] bin The binary data to be converted.
 * @param[in] length The binary data length.
 * @param[out] bin The converted hex string, will be double length of the binary data.
 */
void iso_bin_to_hex_str(const unsigned char *bin, unsigned int length, char *hex_str);

/**
 * @brief Generates binary data from hex string.
 * @param[in] hex_str The hex string to be converted.
 * @param[in] length The hex string length.
 * @param[out] bin The binary converted data, will be half length of the hex string.
 */
void iso_hex_str_to_bin(const char *hex_str, unsigned int length, unsigned char *bin);

/**
 * @brief Initialize iso 8583 message.
 * @param[in] iso_version The iso version to be used.
 * @return Returns 0 if was initized with success or -1 case error.
 */
int iso_init(int iso_version);

/**
 * @brief Release all internal memory allocated by other functions.
 */
void iso_release();

/**
 * @brief Enable auto padding of fields with fixed length (according fields data type).
 * This following fields types will be padding left with '0' character: FI_TYPE__N, FI_TYPE__AN, FI_TYPE__NS, FI_TYPE__ANP, FI_TYPE__ANS.
 * The other fields will be padding right with ' ' character.
 */
void iso_enable_auto_padding();

/**
 * @brief Disable auto padding of fields with fixed length.
 */
void iso_disable_auto_padding();

/**
 * @brief Set message mti.
 * @param[in] mti The message mti.
 * @return Returns 0 to success or -1 case error.
 */
int iso_set_mti(const char *mti);

/**
 * @brief Get message mti.
 * @param[out] mti The buffer to store the message mti.
 * @return Returns 0 to success or -1 case error.
 */
int iso_get_mti(char *mti);

/**
 * @brief Add field in iso message.
 * @param[in] field The field number.
 * @param[in] data The field data.
 * @param[in] length The field data length.
 * @return Returns 0 if field was added or -1 case error.
 */
int iso_add_field(int field, const char *data, int length);

/**
 * @brief Retrieve a value of field.
 * @param[in] field The field number.
 * @param[out] data The buffer to store field value.
 * @return Returns 0 if field was recovered or -1 case error.
 */
int iso_get_field(int field, char *data);

/**
 * @brief Remove field from iso message.
 * @param[in] field The field number.
 * @return Returns 0 if field was removed or -1 case there is no field number set.
 */
int iso_remove_field(int field);

/**
 * @brief Check if there is data in field.
 * @param[in] field The field number.
 * @return Returns 1 case there is data in the field or returns 0 if field is empty or invalid.
 */
int iso_is_set_field(int field);

/**
 * @brief Generate iso message according added fields.
 * @param[out] message The buffer where the message will be stored.
 * @return Returns 0 to success or -1 case error.
 */
int iso_generate_message(char *message);

/**
 * @brief Decode iso message and fill internal fields.
 * @param[in] message The message to be decoded.
 * @return Returns 0 to success or -1 case error.
 */
int iso_decode_message(const char *message);

#endif
