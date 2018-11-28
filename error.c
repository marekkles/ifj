#include "error.h"


void ErrorPrintErrorCode(ErrorCode_t errCode, const char *msg)
{
    
    const char *fmt_strings[] = {
        "Lexical analysis error ",
        "Syntactic analysis error ",
        "Semantics error: undifined/redefinition of variable/function ",
        "Semantics error: incompatible data types ",
        "Semantics error: bad number of arguments in function call ",
        "Run error division by zero ",
        "Internal error "
    };
    ErrorPuts("\e[1m\e[31m");
    ErrorPuts("ERROR: ");
    ErrorPuts("\e[34m");
    ErrorPuts(fmt_strings[errCode]);
    ErrorPuts("\e[0m");
    ErrorPuts(msg);
    ErrorPutChar('\n');
}
void ErrorPrintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}
void ErrorPuts(const char *str)
{
    fputs(str, stderr);
}
void ErrorPutChar(char c)
{
    fputc(c, stderr);
}