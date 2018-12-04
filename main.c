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

#include "parser.h"
#include "error.h"
#include "return.h"
/**
 * Program will get input from stdin and print program to output to stdout
*/
int main(int argc, char *argv[]){
    int return_value = Parse();
    if(return_value != PARSE_OK)
        ErrorPrintErrorCode(ErrorCodeFromReturn(return_value), "");
    return return_value;
}