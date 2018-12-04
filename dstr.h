/**
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                     *
 *                  Projekt IFJ                        *
 *                                                     *
 * Implementace překladače imperativního jazyka IFJ18. *
 *                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  
                       Súbor:
                       dstr.h
                      
                       Popis:
     Hlavickovy subor pre dstr.c, definicia typu DStr_t

                      Autori:
               Marek Vaško (xvasko16)
              Handzuš Daniel (xhandz01)
                Alexaj Adam (xalexa07)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef DSTR_H

#define DSTR_H


#define DSTR_DEFAULT_SIZE 50
#define DSTR_REALOCATION_SIZE 50

typedef struct {
    size_t size;
    size_t strlen;
    char str[];
} DStr_t;

void DStrInit(DStr_t **DStr, size_t size);
void DStrFree(DStr_t **DStr);
int DStrAddChar(DStr_t **DStr, char c);
int DStrCat(DStr_t **DStr, const char *str);
int DStrReplace(DStr_t **DStr, const char *str);
char DStrLast(DStr_t *DStr);
void DStrDeleteLast(DStr_t *DStr);
int DStrClear(DStr_t *DStr);
size_t DStrLen(DStr_t *DStr);
char *DStrStr(DStr_t *DStr);

#endif