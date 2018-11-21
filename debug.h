#include <stdio.h>
#include <stdarg.h>


#ifndef DEBUG_H

#define DEBUG_H

void DebugFPuts(const char * str, FILE *output);
void DebugFPrintf(FILE *output ,const char * fmt, ...);

#endif