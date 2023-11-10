#pragma once

#include <stdbool.h>

#include "config.h"

typedef enum LogLevel {
    LogInfo = 0,
    LogWarning,
    LogError,
    LogFatal,
} LogLevel;

/**
 * Initialise the log system
 * \return true if the initialisation is a success
 */
bool log_init(void);
/**
 * Terminate the log system
 */
void log_terminate(void);

/**
 * Print a formated log message (same rules as printf)
 * \param level the importance of the log
 * \param format the message format
 */
void log_message(LogLevel level, const char* format, ...);

#if LOG_LEVEL <= LogInfo
#define INFO(...) log_message(LogInfo, __VA_ARGS__)
#else
#define INFO(...)
#endif

#if LOG_LEVEL <= LogWarning
#define WARN(...) log_message(LogWarning, __VA_ARGS__)
#else
#define WARN(...)
#endif

#if LOG_LEVEL <= LogError
#define ERROR(...) log_message(LogError, __VA_ARGS__)
#else
#define ERROR(...)
#endif

#if LOG_LEVEL <= LogFatal
#define FATAL(...) log_message(LogFatal, __VA_ARGS__)
#else
#define FATAL(...)
#endif
