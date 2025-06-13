#include <cutil/io/log.h>

#include <cutil/debug/null.h>
#include <cutil/std/stdlib.h>
#include <cutil/util/macro.h>

/**
 * @brief Default output stream for new logger instances.
 *
 * Used by `cutil_Logger_create_default()` as the initial output destination.
 * Typically points to `stdout` unless modified during compilation.
 */
#define DEFAULT_LOG_STREAM stdout

/**
 * @brief Default minimum log level threshold for new loggers.
 *
 * Used by:
 * - `cutil_Logger_create_default()`
 * - Global logger initialization
 *
 * @value CUTIL_LOG_WARN (recommended for production use)
 */
#define DEFAULT_LOG_LEVEL CUTIL_LOG_WARN

/**
 * @brief Initial capacity for logger handler storage.
 *
 * Defines how many log handlers (`_cutil_LogHandler`) a new logger can hold
 * before requiring reallocation.
 *
 * @note Current implementation uses a fixed-size array that warns when full.
 *       Future versions may implement dynamic expansion.
 */
#define DEFAULT_CAPACITY 8

/**
 * @brief Maps log level enumerations to their string representations.
 *
 * Array index corresponds to `cutil_LogLevel` values, providing human-readable
 * prefixes for log messages (e.g., "[TRACE]", "[ERROR]").
 *
 * @note Must stay synchronized with the `cutil_LogLevel` enum definition.
 */
static const char *const CUTIL_LOG_LEVEL_PREFIXES[] = {
  [CUTIL_LOG_TRACE] = "TRACE", [CUTIL_LOG_DEBUG] = "DEBUG",
  [CUTIL_LOG_INFO] = "INFO",   [CUTIL_LOG_WARN] = "WARN",
  [CUTIL_LOG_ERROR] = "ERROR", [CUTIL_LOG_FATAL] = "FATAL",
};

/**
 * @brief Singleton global logger instance.
 *
 * @note Access via `cutil_get_global_logger()`/`cutil_set_global_logger()`.
 * @warning Never modify directly - use designated functions.
 */
static cutil_Logger *_cutil_global_logger = NULL;

/**
 * @brief Tracks whether `atexit()` cleanup has been registered.
 *
 * Prevents duplicate registration of `_cutil_free_global_logger`.
 */
static cutil_Bool _free_global_logger_at_exit = false;

/**
 * Validates if a log level value is within the defined range.
 *
 * @param[in] level The log level to validate
 *
 * @return true if the level is valid (CUTIL_LOG_TRACE to CUTIL_LOG_FATAL),
 *         false otherwise
 */
static inline cutil_Bool
_cutil_LogLevel_is_valid(enum cutil_LogLevel level)
{
    return !(level < CUTIL_LOG_TRACE || level > CUTIL_LOG_FATAL);
}

/**
 * @brief Internal representation of a log output handler.
 *
 * Each handler defines:
 * - An output stream (FILE*)
 * - A minimum log level filter
 * - A flag controlling stream lifecycle management
 */
struct _cutil_LogHandler {
    FILE *stream;              /**< Output stream (e.g., stdout, file) */
    enum cutil_LogLevel level; /**< Minimum log level to process */
    cutil_Bool close; /**< Should stream be closed when handler is destroyed? */
};

/**
 * Initializes a log handler with the given parameters.
 *
 * @param[in] handler Pointer to the handler to initialize
 * @param[in] stream Output stream for the handler
 * @param[in] level Minimum log level for the handler
 * @param[in] close Whether to close the stream when the handler is destroyed
 */
static void
_cutil_LogHandler_init(
  struct _cutil_LogHandler *handler,
  FILE *stream,
  enum cutil_LogLevel level,
  cutil_Bool close
)
{
    CUTIL_NULL_CHECK(handler);
    CUTIL_NULL_CHECK(stream);

    handler->stream = stream;
    handler->level = level;
    handler->close = close;
}

/**
 * Closes a log handler's stream if configured to do so.
 *
 * @param[in] handler Pointer to the handler to close
 */
static void
_cutil_LogHandler_close(struct _cutil_LogHandler *handler)
{
    CUTIL_NULL_CHECK(handler);
    if (handler->close) {
        cutil_sfclose(handler->stream);
    }
}

/**
 * Formats and writes a log message to a handler's stream if the level passes
 * filtering.
 *
 * @param[in] handler Pointer to the handler to write to
 * @param[in] level log level of the message
 * @param[in] format printf-style format string
 * @param[in] args Variable arguments for the format string
 *
 * @note Performs:
 * 1. Level filtering (skips if message level < handler's minimum level)
 * 2. Prefixing (e.g., "[ERROR]")
 * 3. Newline-terminated output
 */
static void
_cutil_LogHandler_vmessage(
  struct _cutil_LogHandler *handler,
  enum cutil_LogLevel level,
  const char *format,
  va_list args
)
{
    CUTIL_NULL_CHECK(handler);
    CUTIL_NULL_CHECK(format);
    if (level < handler->level) {
        return;
    }
    FILE *const stream = handler->stream;
    const char *const prefix = CUTIL_LOG_LEVEL_PREFIXES[level];
    fprintf(stream, "[%5s] ", prefix);
    vfprintf(stream, format, args);
    fputs("\n", stream);
}

/**
 * @brief Internal representation of a logger instance.
 *
 * Manages:
 * - A minimum log level threshold (messages below this level are filtered out)
 * - A dynamic array of log handlers (output destinations)
 * - Capacity tracking for handler storage
 *
 * @note All instances should be created via `cutil_Logger_create()` or
 *       `cutil_Logger_create_default()`.
 */
struct _cutil_Logger {
    enum cutil_LogLevel level; /**< Minimum log level for message processing */
    int num_handlers;          /**< Current number of active handlers */
    struct _cutil_LogHandler *handlers; /**< Array of log output handlers */
    int capacity;                       /**< Allocated size of handlers array */
};

cutil_Logger *
cutil_Logger_create_default(void)
{
    return cutil_Logger_create_with_handler(
      DEFAULT_LOG_STREAM, DEFAULT_LOG_LEVEL
    );
}

cutil_Logger *
cutil_Logger_create_stdout(enum cutil_LogLevel level)
{
    return cutil_Logger_create_with_handler(stdout, level);
}

cutil_Logger *
cutil_Logger_create_with_handler(FILE *stream, enum cutil_LogLevel level)
{
    CUTIL_NULL_CHECK(stream);
    cutil_Logger *const log = cutil_Logger_create(level);
    cutil_Logger_add_handler_full(log, stream, level, false);
    return log;
}

cutil_Logger *
cutil_Logger_create(enum cutil_LogLevel level)
{
    cutil_Logger *const log = malloc(sizeof *log);

    log->level = level;
    log->capacity = DEFAULT_CAPACITY;
    log->handlers = malloc(log->capacity * sizeof *log->handlers);
    log->num_handlers = 0;

    return log;
}

void
cutil_Logger_free(cutil_Logger *log)
{
    CUTIL_RETURN_IF_NULL(log);

    for (int i = 0; i < log->num_handlers; ++i) {
        _cutil_LogHandler_close(&log->handlers[i]);
    }
    free(log->handlers);

    free(log);
}

enum cutil_LogLevel
cutil_Logger_get_level(const cutil_Logger *log)
{
    CUTIL_NULL_CHECK(log);
    return log->level;
}

enum cutil_LogLevel
cutil_Logger_set_level(cutil_Logger *log, enum cutil_LogLevel level)
{
    CUTIL_NULL_CHECK(log);
    if (!_cutil_LogLevel_is_valid(level)) {
        cutil_Logger_warn(log, "Cannot change log level: invalid value!");
        return log->level;
    }
    enum cutil_LogLevel old_level = log->level;
    log->level = level;
    return old_level;
}

void
cutil_Logger_add_handler(
  cutil_Logger *log, FILE *stream, enum cutil_LogLevel level
)
{
    CUTIL_NULL_CHECK(log);
    CUTIL_NULL_CHECK(stream);
    cutil_Logger_add_handler_full(log, stream, level, true);
}

void
cutil_Logger_add_handler_full(
  cutil_Logger *log, FILE *stream, enum cutil_LogLevel level, cutil_Bool close
)
{
    CUTIL_NULL_CHECK(log);
    CUTIL_NULL_CHECK(stream);
    if (log->num_handlers == log->capacity) {
        cutil_Logger_warn(
          log, "Cannot add another handler: maximum number reached!"
        );
        return;
    }
    struct _cutil_LogHandler *const handler = &log->handlers[log->num_handlers];
    _cutil_LogHandler_init(handler, stream, level, close);
    ++log->num_handlers;
}

/**
 * Prints formatted message `format` prepended with the level token to all
 * handlers in `log` if `level` is greater than both the minimum log level of
 * the logger and the respective handler.
 *
 * @param[in] log cutil_Logger to log message with (NULL for no-op)
 * @param[in] level log level of the message
 * @param[in] format printf-like format string
 * @param[in] args va_list of variadic arguments
 */
static void
_cutil_Logger_vmessage(
  cutil_Logger *log, enum cutil_LogLevel level, const char *format, va_list args
)
{
    CUTIL_RETURN_IF_NULL(log);
    if (level < log->level) {
        return;
    }
    for (int i = 0; i < log->num_handlers; ++i) {
        _cutil_LogHandler_vmessage(&log->handlers[i], level, format, args);
    }
}

void
cutil_Logger_vtrace(cutil_Logger *log, const char *format, va_list args)
{
    _cutil_Logger_vmessage(log, CUTIL_LOG_TRACE, format, args);
}

void
cutil_Logger_trace(cutil_Logger *log, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cutil_Logger_vtrace(log, format, args);
    va_end(args);
}

void
cutil_Logger_vdebug(cutil_Logger *log, const char *format, va_list args)
{
    _cutil_Logger_vmessage(log, CUTIL_LOG_DEBUG, format, args);
}

void
cutil_Logger_debug(cutil_Logger *log, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cutil_Logger_vdebug(log, format, args);
    va_end(args);
}

void
cutil_Logger_vinfo(cutil_Logger *log, const char *format, va_list args)
{
    _cutil_Logger_vmessage(log, CUTIL_LOG_INFO, format, args);
}

void
cutil_Logger_info(cutil_Logger *log, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cutil_Logger_vinfo(log, format, args);
    va_end(args);
}

void
cutil_Logger_vwarn(cutil_Logger *log, const char *format, va_list args)
{
    _cutil_Logger_vmessage(log, CUTIL_LOG_WARN, format, args);
}

void
cutil_Logger_warn(cutil_Logger *log, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cutil_Logger_vwarn(log, format, args);
    va_end(args);
}

void
cutil_Logger_verror(cutil_Logger *log, const char *format, va_list args)
{
    _cutil_Logger_vmessage(log, CUTIL_LOG_ERROR, format, args);
}

void
cutil_Logger_error(cutil_Logger *log, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cutil_Logger_verror(log, format, args);
    va_end(args);
}

void
cutil_Logger_vfatal(cutil_Logger *log, const char *format, va_list args)
{
    _cutil_Logger_vmessage(log, CUTIL_LOG_FATAL, format, args);
}

void
cutil_Logger_fatal(cutil_Logger *log, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cutil_Logger_vfatal(log, format, args);
    va_end(args);
}

/**
 * Frees the global logger instance during program termination.
 *
 * @note Registered via atexit() when the global logger is first set.
 */
static void
_cutil_free_global_logger(void)
{
    cutil_Logger_free(_cutil_global_logger);
    _cutil_global_logger = NULL;
}

cutil_Logger *
cutil_get_global_logger(void)
{
    return _cutil_global_logger;
}

cutil_Logger *
cutil_set_global_logger(cutil_Logger *log)
{
    if (!_free_global_logger_at_exit) {
        atexit(&_cutil_free_global_logger);
        _free_global_logger_at_exit = true;
    }
    cutil_Logger *const old_logger = _cutil_global_logger;
    _cutil_global_logger = log;
    return old_logger;
}

/**
 * Internal function to log a message using the global logger.
 *
 * @param[in] level log level of the message
 * @param[in] format printf-style format string
 * @param[in] args Variable arguments for the format string
 *
 * @note If no global logger is set, the message is silently dropped.
 */
static void
_cutil_log_vmessage(enum cutil_LogLevel level, const char *format, va_list args)
{
    cutil_Logger *const log = cutil_get_global_logger();
    _cutil_Logger_vmessage(log, level, format, args);
}

void
cutil_log_vtrace(const char *format, va_list args)
{
    _cutil_log_vmessage(CUTIL_LOG_TRACE, format, args);
}

void
cutil_log_trace(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cutil_log_vtrace(format, args);
    va_end(args);
}

void
cutil_log_vdebug(const char *format, va_list args)
{
    _cutil_log_vmessage(CUTIL_LOG_DEBUG, format, args);
}

void
cutil_log_debug(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cutil_log_vdebug(format, args);
    va_end(args);
}

void
cutil_log_vinfo(const char *format, va_list args)
{
    _cutil_log_vmessage(CUTIL_LOG_INFO, format, args);
}

void
cutil_log_info(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cutil_log_vinfo(format, args);
    va_end(args);
}

void
cutil_log_vwarn(const char *format, va_list args)
{
    _cutil_log_vmessage(CUTIL_LOG_WARN, format, args);
}

void
cutil_log_warn(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cutil_log_vwarn(format, args);
    va_end(args);
}

void
cutil_log_verror(const char *format, va_list args)
{
    _cutil_log_vmessage(CUTIL_LOG_ERROR, format, args);
}

void
cutil_log_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cutil_log_verror(format, args);
    va_end(args);
}

void
cutil_log_vfatal(const char *format, va_list args)
{
    _cutil_log_vmessage(CUTIL_LOG_FATAL, format, args);
}

void
cutil_log_fatal(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cutil_log_vfatal(format, args);
    va_end(args);
}
