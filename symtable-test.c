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

    SymTableAddFunction(symtable, "fun", 0, true);

    SymTableSetLocalMode(symtable);
    SymTableAddVariable(symtable, "a");
    DebugFPuts("Add: a\n", stdout);
    SymTableAddVariable(symtable, "b");
    DebugFPuts("Add: b\n", stdout);
    SymTableAddVariable(symtable, "c");
    DebugFPuts("Add: c\n", stdout);
    SymTableAddVariable(symtable, "d");
    DebugFPuts("Add: d\n", stdout);
    SymTableAddVariable(symtable, "e");
    DebugFPuts("Add: e\n", stdout);
    SymTableAddVariable(symtable, "f");
    DebugFPuts("Add: f\n", stdout);
    SymTableAddVariable(symtable, "g");
    DebugFPuts("Add: g\n", stdout);
    SymTableAddVariable(symtable, "h");
    DebugFPuts("Add: h\n", stdout);
    SymTableAddVariable(symtable, "i");
    DebugFPuts("Add: i\n", stdout);
    SymTableAddVariable(symtable, "j");
    DebugFPuts("Add: j\n", stdout);

    DebugFPrintSymTable(stdout, symtable);

    SymTableUnSetLocalMode(symtable);

    DebugFPrintSymTable(stdout, symtable);

    SymTableDispose(&symtable);
    return 0;
}
