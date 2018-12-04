/**
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                     *
 *                  Projekt IFJ                        *
 *                                                     *
 * Implementace překladače imperativního jazyka IFJ18. *
 *                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  
                       Súbor:
                       fsm.h
                      
                       Popis:
            Hlavickovy subor pre fsm.c

                      Autori:
               Marek Vaško (xvasko16)
              Handzuš Daniel (xhandz01)
                Alexaj Adam (xalexa07)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "return.h"
#include "dstr.h"
#include "token.h"
#include "debug.h"


#ifndef FSM_H

#define FSM_H

int GetToken(DStr_t **DStr, Token_t *token);

#endif