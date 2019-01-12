#include <stdio.h>
#include <stdarg.h>
#include "../Include/Macros.h"

#define FTOK_ID 'G'

void report_error(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "[ERROR]: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}

key_t getKeyByPath(const char *path) {
    return GENERATE_KEY(path, FTOK_ID);
}