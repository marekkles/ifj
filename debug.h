#include <stdio.h>
#include <stdarg.h>
#include "token.h"
#include "dstr.h"

#ifndef DEBUG_H

#define DEBUG_H

void DebugFPuts(const char * str, FILE *output);
void DebugFPrintf(FILE *output, const char * fmt, ...);
void DebugFPrintToken(FILE *output, const Token_t * token, const DStr_t * Dstr);

#endif