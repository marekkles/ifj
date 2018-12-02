#include <stdio.h>
#include "symbolstack.h"
#include "debug.h"


SymTable_t *symtable;

int main(int argc, char const *argv[])
{


    SStack_t *sstack;
    SStackItem_t sstackItem, *topItem;
    Init_SStack(&sstack, 10);
    SymTableInit(&symtable, 10);
    SStackPushEnd(&sstack);

    Token_t token;
    DStr_t *dstr;
    DStrInit(&dstr, 20);
    
    token.type = T_INTEGER;
    token.intValue = 10;
    DStrClear(dstr);
    DStrCat(&dstr, "");
    SStackProcessTokenToItem(dstr, &token, &sstackItem);
    Push_SStack(&sstack, &sstackItem);

    DebugFPrintSStack(stdout, sstack);

    token.type = T_DOUBLE;
    token.doubleValue = 3.141592654359;
    DStrClear(dstr);
    DStrCat(&dstr, "");
    SStackProcessTokenToItem(dstr, &token, &sstackItem);
    Push_SStack(&sstack, &sstackItem);

    DebugFPrintSStack(stdout, sstack);

    token.type = T_STRING;
    DStrClear(dstr);
    DStrCat(&dstr, "String1234567890");
    SStackProcessTokenToItem(dstr, &token, &sstackItem);
    Push_SStack(&sstack, &sstackItem);

    DebugFPrintSStack(stdout, sstack);

    token.type = T_IDENTIFIER;
    DStrClear(dstr);
    DStrCat(&dstr, "XXX");
    SymTableAddVariable(symtable, DStrStr(dstr));
    SStackProcessTokenToItem(dstr, &token, &sstackItem);
    Push_SStack(&sstack, &sstackItem);

    DebugFPrintSStack(stdout, sstack);

    token.type = T_OPERATION;
    token.operationType = TO_ADD;
    DStrClear(dstr);
    SStackProcessTokenToItem(dstr, &token, &sstackItem);
    Push_SStack(&sstack, &sstackItem);

    DebugFPrintSStack(stdout, sstack);

    token.type = T_OPERATION;
    token.operationType = TO_SUBTRACT;
    DStrClear(dstr);
    SStackProcessTokenToItem(dstr, &token, &sstackItem);
    Push_SStack(&sstack, &sstackItem);

    DebugFPrintSStack(stdout, sstack);

    token.type = T_OPERATION;
    token.operationType = TO_MULTIPLY;
    DStrClear(dstr);
    SStackProcessTokenToItem(dstr, &token, &sstackItem);
    Push_SStack(&sstack, &sstackItem);

    DebugFPrintSStack(stdout, sstack);

    token.type = T_OPERATION;
    token.operationType = TO_DIVIDE;
    DStrClear(dstr);
    SStackProcessTokenToItem(dstr, &token, &sstackItem);
    Push_SStack(&sstack, &sstackItem);

    DebugFPrintSStack(stdout, sstack);

    token.type = T_OPERATION;
    token.operationType = TO_LESSER_THAN;
    DStrClear(dstr);
    SStackProcessTokenToItem(dstr, &token, &sstackItem);
    Push_SStack(&sstack, &sstackItem);

    DebugFPrintSStack(stdout, sstack);

    token.type = T_OPERATION;
    token.operationType = TO_GREATER_THAN;
    DStrClear(dstr);
    SStackProcessTokenToItem(dstr, &token, &sstackItem);
    Push_SStack(&sstack, &sstackItem);


    token.type = T_OPERATION;
    token.operationType = TO_LESSER_EQUAL_THAN;
    DStrClear(dstr);
    SStackProcessTokenToItem(dstr, &token, &sstackItem);
    Push_SStack(&sstack, &sstackItem);

    token.type = T_OPERATION;
    token.operationType = TO_GREATER_EQUAL_THAN;
    DStrClear(dstr);
    SStackProcessTokenToItem(dstr, &token, &sstackItem);
    Push_SStack(&sstack, &sstackItem);

    token.type = T_OPERATION;
    token.operationType = TO_LBRACKET;
    DStrClear(dstr);
    SStackProcessTokenToItem(dstr, &token, &sstackItem);
    Push_SStack(&sstack, &sstackItem);


    token.type = T_OPERATION;
    token.operationType = TO_RBRACKET;
    DStrClear(dstr);
    SStackProcessTokenToItem(dstr, &token, &sstackItem);
    Push_SStack(&sstack, &sstackItem);

    DStrFree(&dstr);
    //DebugFPrintSStack(stdout, sstack);

    Dispose_SStack(&sstack);
    return 0;
}
