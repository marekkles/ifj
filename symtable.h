#include <stdlib.h>
#include <string.h>
#include "hash.h"

#ifndef SYMTABLE_H

#define SYMTABLE_H

typedef enum {S_FUNCTION, S_INTEGER, S_DOUBLE, S_UNDEF} SymTableSymbolType_t;

typedef struct {
    char *key;
    SymTableSymbolType_t type;
    int global;
    SymTableItem_t *NextPtr;  
} SymTableItem_t;


typedef struct {
    size_t size;
    SymTableItem_t *table[];
} SymTable_t;

/*SymTableItem_t *SymTableAllocateItem(SymTableSymbolType_t type, char *key);

void SymTableItemCopy(SymTableItem_t *Destination, SymTableItem_t *Source); */

void SymTableItemFree(SymTableItem_t *Item);
void SymTableInit(SymTable_t **SymTable, size_t size);
unsigned int SymTableIndex(SymTable_t *SymTable, char *key);
SymTableItem_t *SymTableFindItem(SymTable_t *SymTable, char *key);
SymTableItem_t *SymTableAddItem(SymTable_t *SymTable, SymTableItem_t *Item);
void SymTableRemoveItem(SymTable_t *SymTable, char *key);
void SymTableDispose(SymTable_t **SymTable);

#endif