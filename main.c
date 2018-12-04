/**
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                     *
 *                  Projekt IFJ                        *
 *                                                     *
 * Implementace překladače imperativního jazyka IFJ18. *
 *                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  
                       Súbor:
                       main.c
                      
                       Popis:
       Hlavne telo programu, spusti parser

                      Autori:
               Marek Vaško (xvasko16)
              Handzuš Daniel (xhandz01)
                Alexaj Adam (xalexa07)
*/

#include <stdio.h>
#include "token.h"
#include "debug.h"
#include "dstr.h"
#include "fsm.h"
#include "return.h"

FILE *input;
FILE *output;

int main(int argc, char *argv[]){

    input = stdin;
    output = stdout;
    DStr_t *dynamic_string;
    Token_t token;
    TokenType_t last_token_type;
    int return_value = 0;

    fputs(".IFJcode18", stdout);

    DStrInit(&dynamic_string, 100);

    if(dynamic_string == NULL)
        return INTERNAL_ERROR;

    while(1)
    {
        last_token_type = GetToken(&dynamic_string, &token);

        if(last_token_type != -1 && last_token_type != -2)
            DebugFPrintToken(stdout, &token, dynamic_string);
        
        if(last_token_type == T_EOF)
        {
            break;
        }
        else if(last_token_type == -1)
        {
            return_value = LEXICAL_ERROR;
            break;
        }
        else if(last_token_type == -2)
        {
            return_value = INTERNAL_ERROR;
            break;
        }
    }

    DStrFree(&dynamic_string);

    return return_value;    
}