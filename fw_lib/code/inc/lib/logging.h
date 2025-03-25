/**************************************************************************************************
 * 
 * \file logging.h
 * 
 * \brief Logging module implementation
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#ifndef _INC_LIB_LOGGING_H
#define _INC_LIB_LOGGING_H

#include <stdarg.h>

enum logging_level {
    LOGGING_NOTSET   = 0,
    LOGGING_DEBUG    = 1,
    LOGGING_INFO     = 2,
    LOGGING_WARNING  = 3,
    LOGGING_ERROR    = 4,
    LOGGING_CRITICAL = 5,
};

static inline const char *logging_level_str(enum logging_level level)
{
    static const char *levels[] = {"NOTSET", "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"};

    return levels[level];
}

#ifdef LOGGING
#define logging(level, fmt, ...)                                                             \
            _logging(level, "[%s] %s()@%d: " fmt, logging_level_str(level), __func__, __LINE__,    \
            __VA_ARGS__);
#else
#define logging(level, fmt, ...)
#endif




struct logging_db;

void logging_init(const struct logging_db *log_db);
void _logging(enum logging_level level, const char *fmt, ...);

#endif /* _INC_LIB_LOGGING_H */
