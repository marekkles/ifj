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

void DebugFPrintToken(FILE *output, const Token_t * token, const DStr_t * Dstr)
{
    fputs(TokenTypesNames[token->type], output);
    fputc('\n', output);
    if(token->type == T_KEYWORD)
    {
        fputs(TokenTypesNames[token->keywordType], output);
        fputc('\n', output);
    }
    else if(token->type == T_OPERATION)
    {
        fputs(TokenOperationTypesNames[token->operationType], output);
        fputc('\n', output);
    }
    else if(token->type == T_INTEGER || token->type == T_SRING)
    {
        fputs(DStrStr(DStr), output);
        fputc('\n', output);
    }
    else if(token->type = T_INTEGER)
    {
        fprintf(output, "%d\n", token->intValue);
    }
    else if(token->type = T_DOUBLE)
    {
        fprintf(output, "%lf\n", token->doubleValue);
    }
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

void DebugFPrintToken(FILE *output, const Token_t * token, const DStr_t * Dstr)
{
    return;
}

#endif