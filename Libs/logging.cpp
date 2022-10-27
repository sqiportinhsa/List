#include <stdarg.h>

#include "logging.h"

FILE *Logstream = stdout;

FILE* GetLogStream() {
    return Logstream;
}

void SetLogStream(FILE *stream) {
    Logstream = stream;
}

void RealPrintToLogs(FILE *stream, const char *func, const char *file, int line, const char *format, ...) {
    va_list ptr = {};
    va_start(ptr, format);
    fprintf(stream, "Message called at %s(%d) in file %s:", func, line, file);
    fprintf(stream, format, ptr);
    va_end(ptr);
}