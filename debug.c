#include "debug.h"

#ifdef DEBUG

void DebugFPuts(const char * str, FILE *output)
{
    fputs(str, output);
    return;
}
void DebugFPrintf(FILE *output ,const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(output, fmt, args);
    va_end(args);
    return;
}

#else

void DebugFPuts(const char * str, FILE *output)
{
    return;
}
void DebugFPrintf(FILE *output ,const char * fmt, ...)
{
    return;
}

#endif