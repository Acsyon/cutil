/* cutil/log.h
 *
 * Header for logging stuff
 *
 */

#ifndef CUTIL_LOG_H_INCLUDED
#define CUTIL_LOG_H_INCLUDED

#include <stdarg.h>

#include <cutil/std/stdio.h>

/**
 * Log level enumerator
 */
enum cutil_LogLevel {
    CUTIL_LOG_TRACE,
    CUTIL_LOG_DEBUG,
    CUTIL_LOG_INFO,
    CUTIL_LOG_WARN,
    CUTIL_LOG_ERROR,
    CUTIL_LOG_FATAL,
};

/**
 * Log level prefix strings
 */
extern const char *const CUTIL_LOG_LEVEL_PREFIXES[];

/**
 * Gets the current minimum log level to display.
 *
 * @return current minimum log level
 */
enum cutil_LogLevel
cutil_log_get_level(void);

/**
 * Sets the minimum log level to display.
 *
 * @param[in] level new minimum log level
 *
 * @return old minimum log level
 */
enum cutil_LogLevel
cutil_log_set_level(enum cutil_LogLevel level);

/**
 * Gets the current log stream.
 *
 * @return current log stream
 */
FILE *
cutil_log_get_stream(void);

/**
 * Sets the log stream.
 *
 * @param[in] stream new log stream
 *
 * @return old log stream
 */
FILE *
cutil_log_set_stream(FILE *stream);

/**
 * Returns the prefix of the the specified level or an empty string if the level
 * is invalid.
 *
 * @param[in] level level to return prefix of
 *
 * @return the prefix of the the specified level or an empty string if the level
 * is invalid
 */
const char *
cutil_log_get_prefix(enum cutil_LogLevel level);

/**
 * Prints formatted message `format` to log stream prepended with level token if
 * `level` is greater than the minimum log level.
 *
 * @param[in] level log level of the message
 * @param[in] format printf-like format string
 * @param[in] args va_list of variadic arguments
 */
void
cutil_log_vmessage(enum cutil_LogLevel level, const char *format, va_list arg);

/**
 * Prints formatted message `format` to log stream prepended with level token if
 * `level` is greater than the minimum log level.
 *
 * @param[in] level log level of the message
 * @param[in] format printf-like format string
 */
void
cutil_log_message(enum cutil_LogLevel level, const char *format, ...);

/**
 * Prints formatted message `format` to log stream for level CUTIL_LOG_TRACE.
 *
 * @param[in] format printf-like format string
 */
void
cutil_log_trace(const char *format, ...);

/**
 * Prints formatted message `format` to log stream for level CUTIL_LOG_DEBUG.
 *
 * @param[in] format printf-like format string
 */
void
cutil_log_debug(const char *format, ...);

/**
 * Prints formatted message `format` to log stream for level CUTIL_LOG_INFO.
 *
 * @param[in] format printf-like format string
 */
void
cutil_log_info(const char *format, ...);

/**
 * Prints formatted message `format` to log stream for level CUTIL_LOG_WARN.
 *
 * @param[in] format printf-like format string
 */
void
cutil_log_warn(const char *format, ...);

/**
 * Prints formatted message `format` to log stream for level CUTIL_LOG_ERROR.
 *
 * @param[in] format printf-like format string
 */
void
cutil_log_error(const char *format, ...);

/**
 * Prints formatted message `format` to log stream for level CUTIL_LOG_FATAL.
 *
 * @param[in] format printf-like format string
 */
void
cutil_log_fatal(const char *format, ...);

#endif /* CUTIL_LOG_H_INCLUDED */
