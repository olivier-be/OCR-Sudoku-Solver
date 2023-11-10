#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

static FILE* log_file = NULL;

bool log_init(void) {
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    char file_name[512];

    sprintf(file_name, "log_%d-%d-%d_%d-%d-%d.log", tm->tm_year + 1900,
        tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

    log_file = fopen(file_name, "w");
    return log_file != NULL;
}

void log_terminate(void) { fclose(log_file); }

void log_message(LogLevel level, const char* format, ...) {
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);

    switch (level) {
    case LogInfo:
#if LOG_CONSOLE
        fprintf(stdout, "[%d/%d][%d:%d:%d][INFO] ", tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec);
#endif
        fprintf(log_file, "[%d/%d][%d:%d:%d][INFO] ", tm->tm_mon + 1,
            tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
        break;
    case LogWarning:
#if LOG_CONSOLE
        fprintf(stdout, "[%d/%d][%d:%d:%d][WARN] ", tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec);
#endif
        fprintf(log_file, "[%d/%d][%d:%d:%d][WARN] ", tm->tm_mon + 1,
            tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
        break;
    case LogError:
#if LOG_CONSOLE
        fprintf(stdout, "[%d/%d][%d:%d:%d][ERROR] ", tm->tm_mon + 1,
            tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
#endif
        fprintf(log_file, "[%d/%d][%d:%d:%d][ERROR] ", tm->tm_mon + 1,
            tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
        break;
    case LogFatal:
#if LOG_CONSOLE
        fprintf(stdout, "[%d/%d][%d:%d:%d][FATAL] ", tm->tm_mon + 1,
            tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
#endif
        fprintf(log_file, "[%d/%d][%d:%d:%d][FATAL] ", tm->tm_mon + 1,
            tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
        break;
    }

    va_list args;
#if LOG_CONSOLE
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    fprintf(stdout, "\n");
#endif

    va_start(args, format);
    vfprintf(log_file, format, args);
    va_end(args);
    fprintf(log_file, "\n");
}
