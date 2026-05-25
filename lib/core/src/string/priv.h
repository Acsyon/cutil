/** cutilsrc/core/string/priv.h
 *
 * Header for private functions of the string type.
 */

#ifndef CUTILSRC_CORE_STRING_PRIV_H_INCLUDED
#define CUTILSRC_CORE_STRING_PRIV_H_INCLUDED

/**
 * Initializes `s` to default state (str == NULL, length == 0).
 *
 * @param[in] vs pointer to cutil_String to initialize
 */
void
priv_cutil_String_init_default(void *vs);

/**
 * Initializes `sv` to default state (str == NULL, length == 0).
 *
 * @param[in] vsv pointer to cutil_StringView to initialize
 */
void
priv_cutil_StringView_init_default(void *vsv);

/**
 * Initializes `sb` to default state (capacity == STRING_DEFAULT_SIZE, empty
 * string, bufsiz == STRING_DEFAULT_SIZE).
 *
 * @param[in] vsb pointer to cutil_StringBuilder to initialize
 */
void
priv_cutil_StringBuilder_init_default(void *vsb);

#endif /* CUTILSRC_CORE_STRING_PRIV_H_INCLUDED */
