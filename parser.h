/**
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                     *
 *                  Projekt IFJ                        *
 *                                                     *
 * Implementace překladače imperativního jazyka IFJ18. *
 *                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  
                       Súbor:
                      parser.h
                      
                       Popis:
            Hlavickovy subor pre parser.c

                      Autori:
               Marek Vaško (xvasko16)
              Handzuš Daniel (xhandz01)
                Alexaj Adam (xalexa07)
*/
#include <stdio.h>
#include <stdlib.h>
#include "return.h"
#include "debug.h"
#include "dstr.h"
#include "fsm.h"
#include "symtable.h"
#include "codegen.h"
#include "symbolstack.h"

#ifndef PARSER_H

#define PARSER_H

extern FILE *input;
extern FILE *output;
extern int in_function;

int Parse(void);

#endif