#ifndef ISO8583_H_
#define ISO8583_H_

/**
 * @brief Initialize iso 8583 message.
 * @param[in] iso_version The iso version to be used.
 * @return Returns 0 if was initized with success or -1 case error.
 */
int init(int iso_version);

/**
 * @brief Set message mti.
 * @param[in] mti The message mti.
 * @return Returns 0 to success or -1 case error.
 */
int set_mti(const char *mti);

/**
 * @brief Add field in iso message.
 * @param[in] field The field number.
 * @param[in] data The field data.
 * @param[in] length The field data length.
 * @return Returns 0 if field was added or -1 case error.
 */
int add_field(int field, const char *data, int length);

/**
 * @brief Remove field from iso message.
 * @param[in] field The field number.
 * @return Returns 0 if field was removed or -1 case there is no field number set.
 */
int remove_field(int field);

/**
 * @brief Cleans all internal variables.
 */
void clear();

/**
 * @brief Generate iso message according added fields.
 * @param[out] message The buffer where the message will be stored.
 * @return Returns 0 to success or -1 case error.
 */
int generate_message(char *message);

#endif
