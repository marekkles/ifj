#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "symtable.h"

int main(int argc, char const *argv[])
{
    SymTable_t *symtable;
    DebugFPuts("Initializing symtable\n", stdout);
    SymTableInit(&symtable, 10);
    if(symtable == NULL)
    {
        DebugFPuts("SymTable: Allocation error\n", stdout);
        return 0;
    }
    DStr_t *dstr;
    DebugFPuts("Initializing dynamic string\n", stdout);
    DStrInit(&dstr, 50);
    if(dstr == NULL)
    {
        DebugFPuts("DStr: Allocation error\n", stdout);
        SymTableDispose(&symtable);
        return 0;
    }

    DebugFPrintSymTable(stdout ,symtable);

    DStrReplace(&dstr, "abcdFun1");
    DebugFPuts(DStrStr(dstr), stdout);
    SymTableAddFunction(symtable, DStrStr(dstr), 5, 1);
    DebugFPrintSymTable(stdout ,symtable);
    DStrReplace(&dstr, "abYey1");
    DebugFPuts(DStrStr(dstr), stdout);
    SymTableAddFunction(symtable, DStrStr(dstr), 5, 1);
    DebugFPrintSymTable(stdout ,symtable);
    DStrReplace(&dstr, "LoLoLo");
    DebugFPuts(DStrStr(dstr), stdout);
    SymTableAddFunction(symtable, DStrStr(dstr), 5, 1);
    DebugFPrintSymTable(stdout ,symtable);
    DStrReplace(&dstr, "xxxx");
    DebugFPuts(DStrStr(dstr), stdout);
    SymTableAddFunction(symtable, DStrStr(dstr), 5, 1);
    DebugFPrintSymTable(stdout ,symtable);
    DebugFPutc('\n', stdout);

    SymTableAddFunction(symtable, DStrStr(dstr), 5, 1);
    DebugFPrintSymTable(stdout ,symtable);

    SymTableAddFunction(symtable, "www", 0, 0);
    DebugFPrintSymTable(stdout ,symtable);

    SymTableDispose(&symtable);
    return 0;
}
