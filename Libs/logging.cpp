#include "logging.h"

FILE *Logstream = stdout;

FILE* GetLogStream() {
    return Logstream;
}

void SetLogStream(FILE *stream) {
    Logstream = stream;
}
