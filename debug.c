#include "debug.h"

#ifdef DEBUG

const char * TokenTypesNames[] = {
    "T_IDENTIFIER",
    "T_INTEGER",
    "T_DOUBLE",
    "T_STRING",
    "T_KEYWORD",
    "T_OPERATION",
    "T_EOL",
    "T_EOF"
};

const char * TokenKeywordTypesNames[] = {
    "TK_DEF",
    "TK_DO",
    "TK_ELSE",
    "TK_END",
    "TK_IF",
    "TK_NOT",
    "TK_NIL",
    "TK_THEN",
    "TK_WHILE"
};

const char * TokenOperationTypesNames[] = {
    "TO_LESSER_THAN", 
    "TO_GREATER_THAN", 
    "TO_ASSIGNMENT", 
    "TO_EQUAL_TO", 
    "TO_NOT_EQUAL_TO", 
    "TO_MULTIPLY", 
    "TO_DIVIDE", 
    "TO_ADD", 
    "TO_SUBTRACT", 
    "TO_GREATER_EQUAL_THAN", 
    "TO_LESSER_EQUAL_THAN",
    "TO_LBRACKET",
    "TO_RBRACKET",
    "TO_COMMA"
};

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

void DebugFPrintToken(FILE *output, Token_t * token, DStr_t * DStr)
{
    fputs(TokenTypesNames[token->type], output);
    fputc('\n', output);
    if(token->type == T_KEYWORD)
    {
        fputs(TokenKeywordTypesNames[token->keywordType], output);
        fputc('\n', output);
    }
    else if(token->type == T_OPERATION)
    {
        fputs(TokenOperationTypesNames[token->operationType], output);
        fputc('\n', output);
    }
    else if(token->type == T_STRING || token->type == T_IDENTIFIER)
    {
        fputs((const char *)DStrStr(DStr), output);
        fputc('\n', output);
    }
    else if(token->type == T_INTEGER)
    {
        fprintf(output, "%d\n", token->intValue);
    }
    else if(token->type == T_DOUBLE)
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

void DebugFPrintToken(FILE *output, Token_t * token, DStr_t * Dstr)
{
    return;
}

#endif