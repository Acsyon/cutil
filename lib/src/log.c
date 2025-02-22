#include <cutil/log.h>

#include <stdarg.h>
#include <stdbool.h>

#include <cutil/std/stdio.h>

#define DEFAULT_LOG_STREAM stderr

static FILE *_log_stream = NULL;
static enum cutil_LogLevel _log_level = CUTIL_LOG_WARN;

const char *const CUTIL_LOG_LEVEL_PREFIXES[] = {
  [CUTIL_LOG_TRACE] = "TRACE", [CUTIL_LOG_DEBUG] = "DEBUG",
  [CUTIL_LOG_INFO] = "INFO",   [CUTIL_LOG_WARN] = "WARN",
  [CUTIL_LOG_ERROR] = "ERROR", [CUTIL_LOG_FATAL] = "FATAL",
};

static inline bool
_is_level_valid(enum cutil_LogLevel level)
{
    return !(level < CUTIL_LOG_TRACE || level > CUTIL_LOG_FATAL);
}

enum cutil_LogLevel
cutil_log_get_level(void)
{
    return _log_level;
}

enum cutil_LogLevel
cutil_log_set_level(enum cutil_LogLevel level)
{
    if (!_is_level_valid(level)) {
        cutil_log_warn("Cannot change log level: invalid value!");
        return _log_level;
    }
    enum cutil_LogLevel old_level = _log_level;
    _log_level = level;
    return old_level;
}

FILE *
cutil_log_get_stream(void)
{
    return _log_stream;
}

FILE *
cutil_log_set_stream(FILE *stream)
{
    FILE *old_stream = _log_stream;
    _log_stream = stream;
    return old_stream;
}

const char *
cutil_log_get_prefix(enum cutil_LogLevel level)
{
    if (!_is_level_valid(level)) {
        cutil_log_warn("Cannot return prefix: invalid log level!");
        return "";
    }
    return CUTIL_LOG_LEVEL_PREFIXES[level];
}

void
cutil_log_vmessage(enum cutil_LogLevel level, const char *format, va_list args)
{
    if (_log_stream == NULL) {
        _log_stream = DEFAULT_LOG_STREAM;
    }
    if (!_is_level_valid(level)) {
        return;
    }
    if (level < _log_level) {
        return;
    }

    const char *const prefix = CUTIL_LOG_LEVEL_PREFIXES[level];
    fprintf(_log_stream, "[%s] ", prefix);
    vfprintf(_log_stream, format, args);
    fputs("\n", _log_stream);
}

void
cutil_log_message(enum cutil_LogLevel level, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cutil_log_vmessage(level, format, args);
    va_end(args);
}

void
cutil_log_trace(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cutil_log_vmessage(CUTIL_LOG_TRACE, format, args);
    va_end(args);
}

void
cutil_log_debug(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cutil_log_vmessage(CUTIL_LOG_DEBUG, format, args);
    va_end(args);
}

void
cutil_log_info(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cutil_log_vmessage(CUTIL_LOG_INFO, format, args);
    va_end(args);
}

void
cutil_log_warn(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cutil_log_vmessage(CUTIL_LOG_WARN, format, args);
    va_end(args);
}

void
cutil_log_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cutil_log_vmessage(CUTIL_LOG_ERROR, format, args);
    va_end(args);
}

void
cutil_log_fatal(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cutil_log_vmessage(CUTIL_LOG_FATAL, format, args);
    va_end(args);
}
