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
    fflush(output);
    return;
}
void DebugFPutc(char c, FILE *output)
{
    fputc(c, output);
    fflush(output);
    return;
}
void DebugFPrintf(FILE *output ,const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(output, fmt, args);
    va_end(args);
    fflush(output);
    return;
}

void DebugFPrintToken(FILE *output, Token_t * token, DStr_t * DStr)
{
    fputs(TokenTypesNames[token->type], output);
    if(token->type == T_KEYWORD)
    {
        fputs(", ", output);
        fputs(TokenKeywordTypesNames[token->keywordType], output);
    }
    else if(token->type == T_OPERATION)
    {
        fputs(", ", output);
        fputs(TokenOperationTypesNames[token->operationType], output);
    }
    else if(token->type == T_STRING || token->type == T_IDENTIFIER)
    {
        fputs(", ", output);
        fputs((const char *)DStrStr(DStr), output);
    }
    else if(token->type == T_INTEGER)
    {
        fputs(", ", output);
        fprintf(output, "%d", token->intValue);
    }
    else if(token->type == T_DOUBLE)
    {
        fputs(", ", output);
        fprintf(output, "%lf", token->doubleValue);
    }
    fputc('\n', output);
    fflush(output);
    return;
}

void DebugFPrintSymTableItem(FILE *output, SymTableItem_t *symtableItem)
{
    if(symtableItem->type == SYM_VARIABLE)
    {
        fputs("SYM_VARIABLE, \"", output);
        fputs(symtableItem->key, output);
        fprintf(output, "\", local = %d", symtableItem->local);
    }
    else
    {
        fputs("SYM_FUNCTION, \"", output);
        fputs(symtableItem->key, output);
        fprintf(output, "\", parameterCount = %d, def = %d", symtableItem->parameterCount, symtableItem->def);
    }
    fflush(output);
}

void DebugFPrintSymTable(FILE *output, SymTable_t *symtable)
{
    fputs("\n[Symtable]\n", output);
    fputs("-+--------\n", output);
    for(int i = 0; i < symtable->size; i++)
    {
        fprintf(output, " +-> Idx: [%d]\n", i);
        if(i == symtable->size - 1)
            fputs(          "     -+-------\n", output);
        else
            fputs(          " |   -+-------\n", output);
        SymTableItem_t *currentItem = symtable->table[i];
        while(currentItem != NULL)
        {
            if(symtable->table[i] == currentItem)
            {
                fputs(" |    |\n", output);
                fputs(" |    +-> ", output);
                
            }
            else
            {
                fputs(" |        |\n", output);
                fputs(" |        +-> ", output);
            }
            DebugFPrintSymTableItem(output, currentItem);
            fputc('\n', output);
            currentItem = currentItem->NextPtr;
            if(i == symtable->size - 1)
                    fputs("  ", output);
            else
                fputs(" |", output);
            if(currentItem != NULL)
                fputs("        |", output);
            fputc('\n', output);
        }
    }
    fflush(output);
}

void DebugFPrintSStackItem(FILE *output, SStackItem_t *sstackItem)
{
    const char *stackItemTypesNames[] = {
        "STACK_ITEM_SYMBOL",
        "STACK_ITEM_INTEGER",
        "STACK_ITEM_DOUBLE",
        "STACK_ITEM_OPERATION"
    };
    int intValue;
		double doubleValue;
		TokenOperationType_t operationType;

    fputs(stackItemTypesNames[sstackItem->type], output);

    if(sstackItem->type == STACK_ITEM_INTEGER)
        fprintf(output, ", %d", sstackItem->intValue);
    else if(sstackItem->type == STACK_ITEM_DOUBLE)
        fprintf(output, ", %lf", sstackItem->doubleValue);
    else if(sstackItem->type == STACK_ITEM_OPERATION)
    {
        fputs(", ", output);
        fputs(TokenOperationTypesNames[sstackItem->operationType], output);
    }
    fputc('\n', output);
    return;
}

void DebugFPrintSStack(FILE *output, SStack_t *sstack)
{
    fputs("+--------+\n", output);
    fputs("| SStack |\n", output);
    fputs("+--------+\n", output);
    fputs(" top ->", output);
    for(int i = sstack->top; i >= 0; i--)
    {
        if(i != sstack->top)
            fputs("       ", output);    
        fputs("  ", output);
        DebugFPrintSStackItem(output, &(sstack->stack[i]));
    }
    fputc('\n', output);
    return;
}

#else

void DebugFPuts(const char * str, FILE *output)
{
    return;
}
void DebugFPutc(char c, FILE *output)
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
void DebugFPrintSymTableItem(FILE *output, SymTableItem_t *symtableItem)
{
    return;
}
void DebugFPrintSymTable(FILE *output, SymTable_t *symtable)
{
    return;
}

void DebugFPrintSStackItem(FILE *output, SStackItem_t *sstackItem)
{
    return;
}
void DebugFPrintSStack(FILE *output, SStack_t *sstack)
{
    return;
}
#endif