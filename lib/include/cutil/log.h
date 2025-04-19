/* cutil/log.h
 *
 * Header for logging stuff
 *
 */

#ifndef CUTIL_LOG_H_INCLUDED
#define CUTIL_LOG_H_INCLUDED

#include <stdarg.h>
#include <stdbool.h>

#include <cutil/std/stdio.h>

/**
 * Enumerates all available log severity levels in ascending order of
 * importance.
 *
 * @value CUTIL_LOG_TRACE   Detailed debugging traces (most verbose)
 * @value CUTIL_LOG_DEBUG   Debugging information
 * @value CUTIL_LOG_INFO    Routine operational messages
 * @value CUTIL_LOG_WARN    Warning conditions
 * @value CUTIL_LOG_ERROR   Error conditions requiring attention
 * @value CUTIL_LOG_FATAL   Critical failures causing termination (least
 * verbose)
 *
 * @note Levels are ordered such that higher values indicate more severe
 * messages
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
 * Opaque handle to a logger instance.
 *
 * The complete structure definition is intentionally hidden to:
 * - Maintain ABI stability across versions
 * - Enforce access through designated functions
 * - Allow internal implementation changes
 *
 * @note All instances must be created via cutil_Logger_create() family
 * @warning Never attempt to allocate or access members directly
 */
typedef struct _cutil_Logger cutil_Logger;

/**
 * Creates a new logger instance with default configuration.
 *
 * @return pointer to newly allocated logger
 *
 * @note Default configuration includes:
 *       - Log level: CUTIL_LOG_WARNING
 *       - Single stderr output handler
 */
cutil_Logger *
cutil_Logger_create_default(void);

/**
 * Creates a new logger instance with specified minimum log level `level`.
 *
 * @param[in] level minimum log level threshold for the new logger
 *
 * @return pointer to newly allocated logger
 *
 * @note Initial configuration includes:
 *       - No output handlers (use cutil_Logger_add_handler() to add them)
 */
cutil_Logger *
cutil_Logger_create(enum cutil_LogLevel level);

/**
 * Properly deallocates logger instance `log` and all associated resources.
 *
 * @param[in, out] log pointer to logger instance to destroy (may be NULL)
 *
 * @note Safe to call with NULL (no-op)
 */
void
cutil_Logger_free(cutil_Logger *log);

/**
 * Retrieves the minimum log level currently configured for `log`.
 *
 * @param[in] log cutil_Logger to retrieve minimum log level of
 *
 * @return currently configured minimum log level
 */
enum cutil_LogLevel
cutil_Logger_get_level(const cutil_Logger *log);

/**
 * Updates the minimum log level threshold and returns the previous value.
 *
 * @param[in] log cutil_Logger to set minimum log level of
 * @param[in] level new minimum log level to set
 *
 * @return previously configured minimum log level before modification
 */
enum cutil_LogLevel
cutil_Logger_set_level(cutil_Logger *log, enum cutil_LogLevel level);

/**
 * Adds another handler to `log` that writes to `stream` with a minimum log
 * level of `level`. The stream in this handler is closed on destruction of
 * `log`.
 *
 * @param[in] log cutil_Logger to add handler to
 * @param[in] level minimum log level of handler
 */
void
cutil_Logger_add_handler(
  cutil_Logger *log, FILE *stream, enum cutil_LogLevel level
);

/**
 * Adds another handler to `log` that writes to `stream` with a minimum log
 * level of `level`. The stream in this handler is closed on destruction of
 * `log` according to `close`.
 *
 * @param[in] log cutil_Logger to add handler to
 * @param[in] level minimum log level of handler
 * @param[in] close should stream be closed together with log?
 */
void
cutil_Logger_add_handler_full(
  cutil_Logger *log, FILE *stream, enum cutil_LogLevel level, bool close
);

/**
 * Prints formatted message `format` to `log` for level CUTIL_LOG_TRACE.
 *
 * @param[in] log cutil_Logger to log message with
 * @param[in] format printf-like format string
 * @param[in] args va_list of variadic arguments
 */
void
cutil_Logger_vtrace(cutil_Logger *log, const char *format, va_list args);

/**
 * Prints formatted message `format` to `log` for level CUTIL_LOG_TRACE.
 *
 * @param[in] log cutil_Logger to log message with
 * @param[in] format printf-like format string
 */
void
cutil_Logger_trace(cutil_Logger *log, const char *format, ...);

/**
 * Prints formatted message `format` to `log` for level CUTIL_LOG_DEBUG.
 *
 * @param[in] log cutil_Logger to log message with
 * @param[in] format printf-like format string
 * @param[in] args va_list of variadic arguments
 */
void
cutil_Logger_vdebug(cutil_Logger *log, const char *format, va_list args);

/**
 * Prints formatted message `format` to `log` for level CUTIL_LOG_DEBUG.
 *
 * @param[in] log cutil_Logger to log message with
 * @param[in] format printf-like format string
 */
void
cutil_Logger_debug(cutil_Logger *log, const char *format, ...);

/**
 * Prints formatted message `format` to `log` for level CUTIL_LOG_INFO.
 *
 * @param[in] log cutil_Logger to log message with
 * @param[in] format printf-like format string
 * @param[in] args va_list of variadic arguments
 */
void
cutil_Logger_vinfo(cutil_Logger *log, const char *format, va_list args);

/**
 * Prints formatted message `format` to `log` for level CUTIL_LOG_INFO.
 *
 * @param[in] log cutil_Logger to log message with
 * @param[in] format printf-like format string
 */
void
cutil_Logger_info(cutil_Logger *log, const char *format, ...);

/**
 * Prints formatted message `format` to `log` for level CUTIL_LOG_WARN.
 *
 * @param[in] log cutil_Logger to log message with
 * @param[in] format printf-like format string
 * @param[in] args va_list of variadic arguments
 */
void
cutil_Logger_vwarn(cutil_Logger *log, const char *format, va_list args);

/**
 * Prints formatted message `format` to `log` for level CUTIL_LOG_WARN.
 *
 * @param[in] log cutil_Logger to log message with
 * @param[in] format printf-like format string
 */
void
cutil_Logger_warn(cutil_Logger *log, const char *format, ...);

/**
 * Prints formatted message `format` to `log` for level CUTIL_LOG_ERROR.
 *
 * @param[in] log cutil_Logger to log message with
 * @param[in] format printf-like format string
 * @param[in] args va_list of variadic arguments
 */
void
cutil_Logger_verror(cutil_Logger *log, const char *format, va_list args);

/**
 * Prints formatted message `format` to `log` for level CUTIL_LOG_ERROR.
 *
 * @param[in] log cutil_Logger to log message with
 * @param[in] format printf-like format string
 */
void
cutil_Logger_error(cutil_Logger *log, const char *format, ...);

/**
 * Prints formatted message `format` to `log` for level CUTIL_LOG_FATAL.
 *
 * @param[in] log cutil_Logger to log message with
 * @param[in] format printf-like format string
 * @param[in] args va_list of variadic arguments
 */
void
cutil_Logger_vfatal(cutil_Logger *log, const char *format, va_list args);

/**
 * Prints formatted message `format` to `log` for level CUTIL_LOG_FATAL.
 *
 * @param[in] log cutil_Logger to log message with
 * @param[in] format printf-like format string
 */
void
cutil_Logger_fatal(cutil_Logger *log, const char *format, ...);

/**
 * Retrieves the current global logger instance.
 *
 * @return The current global logger, or `NULL` if no logger has been set.
 *
 * @note This function does NOT create a default logger. To ensure a logger
 * exists, use `cutil_set_global_logger()` or check for `NULL` and handle it.
 */
cutil_Logger *
cutil_get_global_logger(void);

/**
 * Replaces the global logger instance.
 *
 * @param[in] log The new logger to set (ownership transferred to the global
 * state). Pass `NULL` to clear the global logger without replacement.
 *
 * @return The previous global logger (or `NULL` if none was set).
 *
 * @note The returned logger is *not* freed automatically. The caller must
 *       call `cutil_Logger_free()` on it if no longer needed.
 * @note Registers automatic cleanup via `atexit()` on the first call.
 */
cutil_Logger *
cutil_set_global_logger(cutil_Logger *log);

/**
 * Prints formatted message `format` to global logger for level CUTIL_LOG_TRACE.
 *
 * @param[in] format printf-like format string
 * @param[in] args va_list of variadic arguments
 */
void
cutil_log_vtrace(const char *format, va_list args);

/**
 * Prints formatted message `format` to global logger for level CUTIL_LOG_TRACE.
 *
 * @param[in] format printf-like format string
 */
void
cutil_log_trace(const char *format, ...);

/**
 * Prints formatted message `format` to global logger for level CUTIL_LOG_DEBUG.
 *
 * @param[in] format printf-like format string
 * @param[in] args va_list of variadic arguments
 */
void
cutil_log_vdebug(const char *format, va_list args);

/**
 * Prints formatted message `format` to global logger for level CUTIL_LOG_DEBUG.
 *
 * @param[in] format printf-like format string
 */
void
cutil_log_debug(const char *format, ...);

/**
 * Prints formatted message `format` to global logger for level CUTIL_LOG_INFO.
 *
 * @param[in] format printf-like format string
 * @param[in] args va_list of variadic arguments
 */
void
cutil_log_vinfo(const char *format, va_list args);

/**
 * Prints formatted message `format` to global logger for level CUTIL_LOG_INFO.
 *
 * @param[in] format printf-like format string
 */
void
cutil_log_info(const char *format, ...);

/**
 * Prints formatted message `format` to global logger for level CUTIL_LOG_WARN.
 *
 * @param[in] format printf-like format string
 * @param[in] args va_list of variadic arguments
 */
void
cutil_log_vwarn(const char *format, va_list args);

/**
 * Prints formatted message `format` to global logger for level CUTIL_LOG_WARN.
 *
 * @param[in] format printf-like format string
 */
void
cutil_log_warn(const char *format, ...);

/**
 * Prints formatted message `format` to global logger for level CUTIL_LOG_ERROR.
 *
 * @param[in] format printf-like format string
 * @param[in] args va_list of variadic arguments
 */
void
cutil_log_verror(const char *format, va_list args);

/**
 * Prints formatted message `format` to global logger for level CUTIL_LOG_ERROR.
 *
 * @param[in] format printf-like format string
 */
void
cutil_log_error(const char *format, ...);

/**
 * Prints formatted message `format` to global logger for level CUTIL_LOG_FATAL.
 *
 * @param[in] format printf-like format string
 * @param[in] args va_list of variadic arguments
 */
void
cutil_log_vfatal(const char *format, va_list args);

/**
 * Prints formatted message `format` to global logger for level CUTIL_LOG_FATAL.
 *
 * @param[in] format printf-like format string
 */
void
cutil_log_fatal(const char *format, ...);

#endif /* CUTIL_LOG_H_INCLUDED */
