/**
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                     *
 *                  Projekt IFJ                        *
 *                                                     *
 * Implementace překladače imperativního jazyka IFJ18. *
 *                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  
                       Súbor:
                      debug.h
                      
                       Popis:
            Hlavickovy subor pre debug.c

                      Autori:
               Marek Vaško (xvasko16)
              Handzuš Daniel (xhandz01)
                Alexaj Adam (xalexa07)
*/

#include <stdio.h>
#include <stdarg.h>
#include "token.h"
#include "dstr.h"
#include "symtable.h"
#include "symbolstack.h"

#ifndef DEBUG_H

#define DEBUG_H

void DebugFPuts(const char * str, FILE *output);
void DebugFPutc(char c, FILE *output);
void DebugFPrintf(FILE *output, const char * fmt, ...);
void DebugFPrintToken(FILE *output, Token_t * token, DStr_t * DStr);
void DebugFPrintSymTableItem(FILE *output, SymTableItem_t *symtableItem);
void DebugFPrintSymTable(FILE *output, SymTable_t *symtable);
void DebugFPrintSStackItem(FILE *output, SStackItem_t *sstackItem);
void DebugFPrintSStack(FILE *output, SStack_t *sstack);

#endif