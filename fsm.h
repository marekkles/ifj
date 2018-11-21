/**
 * Subor fsm.h
 * Testovaci subor pre implementaciu zakladnej syntaktickej analizy
 */

#include <stdio.h>
#include <ctype.h>
#include <limits.h>

#include "dstr.h"
#include "debug.h"
#ifndef FSM_H

#define FSM_H

extern int CurrentlyReading[2];

int GetToken(FILE * Input, DStr_t **DStr);

#endif