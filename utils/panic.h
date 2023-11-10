#pragma once

#include <stdlib.h>

#include "logger.h"

typedef enum ErrorType {
    ErrorNone = 0,
    ErrorAllocFailed,
    ErrorPngInvalidSection,
    ErrorPngInvalidPixelFormat,
    ErrorInvalidMatrixSize
} ErrorType;

static const char* const messages[] = { "NO ERROR", "ALLOCATION FAILED",
    "PNG INVALID SECTION", "INVALID PIXEL FORMAT", "INVALID MATRIX SIZE" };

#define PANIC(error, ...)                                                      \
    do {                                                                       \
        FATAL("Error: %s\n", messages[error]);                                 \
        FATAL(__VA_ARGS__);                                                    \
        exit(error);                                                           \
    } while (0);
