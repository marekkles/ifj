/**
 * Subor fsm.h
 * Testovaci subor pre implementaciu zakladnej syntaktickej analizy
 */

#include <stdio.h>
#include <stdlib.h>

#ifndef DSTR_H

#define DSTR_H

typedef struct {
    size_t size;
    size_t strlen;
    char str[];
} DStr_t;

void DStrInit(DStr_t **DStr, size_t size);
void DStrFree(DStr_t **DStr);
int DStrAddChar(DStr_t **DStr, char c);
void DStrDeleteLast(DStr_t *DStr);
int DStrClear(DStr_t *DStr);
size_t DStrLen(DStr_t *DStr);
char *DStrStr(DStr_t *DStr);

#endif