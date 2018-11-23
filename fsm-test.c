#include "fsm.h"
#include "dstr.h"
#include "debug.h"
#include "token.h"

int main(int argc, char const *argv[])
{
    FILE *output = stdout;
    DebugFPuts("------------- DEBUG defined ---------------\n", output);
    if(argc != 2)
    {
        DebugFPuts("------------ Missing FILENAME -------------\n", output);
        return 1;
    }
    DebugFPuts("------- Correct number of arguments -------\n", output);
    FILE *input;
    
    if((input = fopen(argv[1], "r")) == NULL)
        return 1;
    DebugFPuts("--------------- File opened ---------------\n", output);

    DStr_t *DStr;
    Token_t Token;
    DStrInit(&DStr, 50);
    if(DStr != NULL)
    {
        DebugFPuts("-------- Dynamic string Initialized -------\n", output);
        TokenType_t lastType; 
        while((lastType = GetToken(input, &DStr, &Token)) != -1)
        {
            DebugFPrintToken(stdout, &Token, DStr);
            if(lastType == T_EOF)
                break;
        }
        DStrFree(&DStr);
        DebugFPuts("----------- Dynamic string freed ----------\n", output);
    }
    else
    {
        DebugFPuts("----------- Dynamic string error ----------\n", output);
    }
    
    DebugFPuts("--------------- File closed ---------------\n", output);
    fclose(input);
    DebugFPuts("------------- Program finised -------------\n", output);
    return 0;
}
