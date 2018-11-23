#include "fsm.h"
#include "dstr.h"
#include "debug.h"
#include "token.h"
#include "return.h"

int main(int argc, char const *argv[])
{
    FILE *output = stdout;
    int return_value = 0;
    DebugFPuts("------------- DEBUG defined ---------------\n", output);
    FILE *input;
    input = stdin;
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
        if(lastType == -1)
        {
            return_value = LEXICAL_ERROR;
        }
        else if(lastType == -2)
        {
            return_value = INTERNAL_ERROR;
        }
        DStrFree(&DStr);
        DebugFPuts("----------- Dynamic string freed ----------\n", output);
    }
    else
    {
        DebugFPuts("----------- Dynamic string error ----------\n", output);
        return_value = INTERNAL_ERROR;
    }
    
    DebugFPuts("--------------- File closed ---------------\n", output);
    fclose(input);
    DebugFPuts("------------- Program finised -------------\n", output);
    return return_value;
}
