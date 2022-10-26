#include "logging.h"

FILE *Logstream = stdout;

FILE* GetLogStream() {
    return Logstream;
}

void SetLogStream(FILE *stream) {
    Logstream = stream;
}

void log (const char *zv)
{
    fprintf (Logstream, zv);
}