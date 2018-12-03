#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "dstr.h"
#include "return.h"
#ifndef SYMTABLE_H

#define SYMTABLE_H

typedef enum {SYM_FUNCTION, SYM_VARIABLE} SymTableSymbolType_t;
typedef enum {
    VAR_INT,
    VAR_DOUBLE,
    VAR_STRING,
    VAR_NIL,
    VAR_UNDEFINED
}SymTableVariableType_t;

typedef struct SymTableItem_s {
    bool local;
    bool def;
    int parameterCount;
    SymTableVariableType_t variableType;

    SymTableSymbolType_t type;
    struct SymTableItem_s *NextPtr;
    char key[];
} SymTableItem_t;


typedef struct {
    size_t size;
    bool localMode;
    SymTableItem_t *table[];
} SymTable_t;

extern SymTable_t *symtable;

/*SymTableItem_t *SymTableAllocateItem(SymTableSymbolType_t type, char *key);

void SymTableItemCopy(SymTableItem_t *Destination, SymTableItem_t *Source); */
unsigned int HashCode(const char *str);

SymTableItem_t *SymtableAllocateItem(size_t key_len);
void SymTableItemFree(SymTableItem_t *Item);

void SymTableInit(SymTable_t **SymTable, size_t size);
void SymTableSetLocalMode(SymTable_t *SymTable);
void SymTableUnSetLocalMode(SymTable_t *SymTable);
unsigned int SymTableIndex(SymTable_t *SymTable, char *key);
SymTableItem_t *SymTableFindItem(SymTable_t *SymTable, char *key);
SymTableItem_t *SymTableAddItem(SymTable_t *SymTable, SymTableItem_t *Item);
SymTableItem_t *SymTableAddFunction(SymTable_t *SymTable, char *str, int parameterCount, bool defined);
SymTableItem_t *SymTableAddVariable(SymTable_t *SymTable, char *str);
bool SymTableAreUndefinedFunctions(SymTable_t *SymTable);
void SymTableRemoveItem(SymTable_t *SymTable, char *key);
void SymTableDispose(SymTable_t **SymTable);

#endif