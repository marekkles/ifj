#include "error.h"

/**
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                     *
 *                  Projekt IFJ                        *
 *                                                     *
 * Implementace překladače imperativního jazyka IFJ18. *
 *                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  
                       Súbor:
                      error.c
                      
                       Popis:
     Implementacia funcii pre vystup na chybovy vystup,
     

                      Autori:
               Marek Vaško (xvasko16)
              Handzuš Daniel (xhandz01)
                Alexaj Adam (xalexa07)
*/

ErrorCode_t ErrorCodeFromReturn(int returnValue)
{
    switch(returnValue)
    {
        case PARSE_LEX_ERR:
            return ERR_LEX;
        case PARSE_SYN_ERR:
            return ERR_SYN;
        case PARSE_UNDEF_VAR:
            return ERR_SEM;
        case PARSE_TYPE_COMP:
            return ERR_COMP;
        case PARSE_PARAM_COUNT:
            return ERR_PAR;
        case PARSE_OTHER:
            return ERR_OTH;
        case PARSE_ZERO_DIV:
            return ERR_ZDIV;
        case PARSE_INT_ERR:
            return ERR_INT;
    }
}

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