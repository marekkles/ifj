/**
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                     *
 *                  Projekt IFJ                        *
 *                                                     *
 * Implementace překladače imperativního jazyka IFJ18. *
 *                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  
                       Súbor:
                      error.h
                      
                       Popis:
        Hlavickovy subor pre error.c Obsahuje
        definiciu typu ErrorCode_t, sluziaci
            na urcenie toho ktora chybova 
                sprava sa ma vypisat
     
     

                      Autori:
               Marek Vaško (xvasko16)
              Handzuš Daniel (xhandz01)
                Alexaj Adam (xalexa07)
*/
#include <stdio.h>
#include <stdarg.h>
#ifndef ERROR_H

#define ERROR_H

typedef enum {
    ERR_LEX,
    ERR_SYN,
    ERR_SEM,
    ERR_COMP,
    ERR_PAR,
    ERR_OTH,
    ERR_ZDIV,
    ERR_INT,
} ErrorCode_t;

void ErrorPrintErrorCode(ErrorCode_t errCode, const char *msg);
void ErrorPrintf(const char *fmt, ...);
void ErrorPuts(const char *str);
void ErrorPutChar(char c);

#endif