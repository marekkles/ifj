#include <stdio.h>
#include <stdarg.h>
#ifndef ERROR_H

#define ERROR_H

typedef enum {
    ERR_LEX,
    ERR_SYN,
    ERR_SEM,
    ERR_COMP,
    ERR_PAR,
    ERR_OTH,
    ERR_ZDIV,
    ERR_INT,
} ErrorCode_t;

void ErrorPrintErrorCode(ErrorCode_t errCode, const char *msg);
void ErrorPrintf(const char *fmt, ...);
void ErrorPuts(const char *str);
void ErrorPutChar(char c);

#endif