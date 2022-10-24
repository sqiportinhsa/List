#ifndef LOGGING
#define LOGGING

#include <stdio.h>

typedef struct {
    int          line_of_creation;
    const char*  file_of_creation;
    const char*  func_of_creation;
} Creation_logs;

void  SetLogStream(FILE *stream);
FILE* GetLogStream();

#endif