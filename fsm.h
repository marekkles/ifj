/**
 * Subor fsm.h
 * Testovaci subor pre implementaciu zakladnej syntaktickej analizy
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "dstr.h"
#include "token.h"
#include "debug.h"


#ifndef FSM_H

#define FSM_H

extern int CurrentlyReading[2];

int GetToken(FILE * Input, DStr_t **DStr, Token_t *token);

#endif