#include <stdio.h>
#include <stdlib.h>
#include "dstr.h"
#include "debug.h"

int main(int argc, char const *argv[])
{
    if(argc != 2)
    {
        DebugFPuts("Unsupported number of arguments\n", stdout);
        return 1;
    }
    
    DebugFPuts("\nInitializing dynamic string\n", stdout);
    DStr_t *DStr;
    DStrInit(&DStr, 50);
    fputs(DStrStr(DStr), stdout);
    
    DebugFPuts("\nClearing dynamic string\n", stdout);
    DStrClear(DStr);
    fputs(DStrStr(DStr), stdout);
    
    DebugFPuts("\nAdding sequence of 200 alphabet letters\n", stdout);
    for(int i = 0; i <200; i++)
    {
        DStrAddChar(&DStr, 'a' + i%26);
    }
    fputs(DStrStr(DStr), stdout);
    
    DebugFPuts("\nAdding new line\n", stdout);
    DStrAddChar(&DStr, '\n');
    fputs(DStrStr(DStr), stdout);
    
    DebugFPuts("\nDeleting new line\n", stdout);
    DStrDeleteLast(DStr);
    fputs(DStrStr(DStr), stdout);

    DebugFPuts("\nClearing dynamic string\n", stdout);
    DStrClear(DStr);
    fputs(DStrStr(DStr), stdout);

    DebugFPuts("\nAdding sequence of 20 numbers\n", stdout);
    for(int i = 0; i <20; i++)
    {
        DStrAddChar(&DStr, '0' + i%10);
    }
    fputs(DStrStr(DStr), stdout);

    DebugFPuts("\nFree dynamic string\n", stdout);
    DStrFree(&DStr);
    return 0;
}
