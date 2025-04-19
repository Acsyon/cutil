#include <cutil/log.h>

#include <stdlib.h>

#define DEFAULT_LOG_STREAM stdout
#define DEFAULT_LOG_LEVEL CUTIL_LOG_WARN
#define DEFAULT_CAPACITY 8

static const char *const CUTIL_LOG_LEVEL_PREFIXES[] = {
  [CUTIL_LOG_TRACE] = "TRACE", [CUTIL_LOG_DEBUG] = "DEBUG",
  [CUTIL_LOG_INFO] = "INFO",   [CUTIL_LOG_WARN] = "WARN",
  [CUTIL_LOG_ERROR] = "ERROR", [CUTIL_LOG_FATAL] = "FATAL",
};

static inline bool
_cutil_LogLevel_is_valid(enum cutil_LogLevel level)
{
    return !(level < CUTIL_LOG_TRACE || level > CUTIL_LOG_FATAL);
}

struct _cutil_LogHandler {
    FILE *stream;
    enum cutil_LogLevel level;
    bool close;
};

static void
_cutil_LogHandler_init(
  struct _cutil_LogHandler *handler,
  FILE *stream,
  enum cutil_LogLevel level,
  bool close
)
{
    handler->stream = stream;
    handler->level = level;
    handler->close = close;
}

static void
_cutil_LogHandler_close(struct _cutil_LogHandler *handler)
{
    if (handler->close) {
        cutil_sfclose(handler->stream);
    }
}

void
_cutil_LogHandler_vmessage(
  struct _cutil_LogHandler *handler,
  enum cutil_LogLevel level,
  const char *format,
  va_list args
)
{
    if (level < handler->level) {
        return;
    }
    FILE *const stream = handler->stream;
    const char *const prefix = CUTIL_LOG_LEVEL_PREFIXES[level];
    fprintf(stream, "[%5s] ", prefix);
    vfprintf(stream, format, args);
    fputs("\n", stream);
}

struct _cutil_Logger {
    enum cutil_LogLevel level;
    int num_handlers;
    struct _cutil_LogHandler *handlers;
    int capacity;
};

cutil_Logger *
cutil_Logger_create_default(void)
{
    cutil_Logger *const log = cutil_Logger_create(DEFAULT_LOG_LEVEL);
    cutil_Logger_add_handler_full(
      log, DEFAULT_LOG_STREAM, DEFAULT_LOG_LEVEL, false
    );
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
    if (log == NULL) {
        return;
    }

    for (int i = 0; i < log->num_handlers; ++i) {
        _cutil_LogHandler_close(&log->handlers[i]);
    }
    free(log->handlers);

    free(log);
}

enum cutil_LogLevel
cutil_Logger_get_level(const cutil_Logger *log)
{
    return log->level;
}

enum cutil_LogLevel
cutil_Logger_set_level(cutil_Logger *log, enum cutil_LogLevel level)
{
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
    cutil_Logger_add_handler_full(log, stream, level, true);
}

void
cutil_Logger_add_handler_full(
  cutil_Logger *log, FILE *stream, enum cutil_LogLevel level, bool close
)
{
    if (log->num_handlers == log->capacity) {
        cutil_Logger_warn(
          log, "Cannot change add another handler: maximum number reached!"
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
 * @param[in] log cutil_Logger to log message with
 * @param[in] level log level of the message
 * @param[in] format printf-like format string
 * @param[in] args va_list of variadic arguments
 */
void
_cutil_Logger_vmessage(
  cutil_Logger *log, enum cutil_LogLevel level, const char *format, va_list args
)
{
    if (level < log->level) {
        return;
    }
    for (int i = 0; i < log->num_handlers; ++i) {
        _cutil_LogHandler_vmessage(&log->handlers[i], level, format, args);
    }
}

void
cutil_Logger_trace(cutil_Logger *log, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    _cutil_Logger_vmessage(log, CUTIL_LOG_TRACE, format, args);
    va_end(args);
}

void
cutil_Logger_debug(cutil_Logger *log, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    _cutil_Logger_vmessage(log, CUTIL_LOG_DEBUG, format, args);
    va_end(args);
}

void
cutil_Logger_info(cutil_Logger *log, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    _cutil_Logger_vmessage(log, CUTIL_LOG_INFO, format, args);
    va_end(args);
}

void
cutil_Logger_warn(cutil_Logger *log, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    _cutil_Logger_vmessage(log, CUTIL_LOG_WARN, format, args);
    va_end(args);
}

void
cutil_Logger_error(cutil_Logger *log, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    _cutil_Logger_vmessage(log, CUTIL_LOG_ERROR, format, args);
    va_end(args);
}

void
cutil_Logger_fatal(cutil_Logger *log, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    _cutil_Logger_vmessage(log, CUTIL_LOG_FATAL, format, args);
    va_end(args);
}
