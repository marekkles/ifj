#include "symtable.h"

unsigned int hashCode(const char *str)
{
    unsigned int hash = 5381;
    char c;
    while(c = *str++)
    {
        hash = (hash << 5) + hash + c;
    }
    return hash;
}
/*
SymTableItem_t *SymTableAllocateItem(SymTableSymbolType_t type, char *key)
{
    SymTableItem_t *Item;
    Item = malloc(sizeof(SymTableItem_t));
    if(Item == NULL)
        return NULL;
    
    char *assigned_key;
    assigned_key = malloc(sizeof(char) * (strlen(key)+1));
    if(assigned_key == NULL)
    {
        free(Item);
        return NULL;
    }
    strcpy(assigned_key, key);
    Item->key = assigned_key;
    return Item;
}
void SymTableItemFree(SymTableItem_t *Item)
{
    free(Item->key);
    if(Item->type == S_FUNCTION)
        SymTableDispose(&(Item->STable));
    free(Item);
}
void SymTableItemCopy(SymTableItem_t *Destination, SymTableItem_t *Source);
//commit
*/
void SymTableInit(SymTable_t **SymTable, size_t size)
{
    if((*SymTable = malloc(sizeof(SymTable_t) + sizeof(SymTableItem_t *)*size)) == NULL)
    {
        //DebugPrint(E_ALLOC, "SymTable", sizeof(SymTable_t) + sizeof(SymTableItem_t)*size))
        return;
    }
    (*SymTable)->size = size;
    for(unsigned int i = 0; i < size; i++)
        (*SymTable)->table[i] = NULL;
}

unsigned int SymTableIndex(SymTable_t *SymTable, char *key)
{
    return hashCode(key) % SymTable->size;
}

SymTableItem_t *SymTableFindItem(SymTable_t *SymTable, char *key)
{
    unsigned int Index = SymTableIndex(SymTable, key);
    SymTableItem_t * CurrentItem = SymTable->table[Index];
    while(CurrentItem != NULL)
    {
        if(strcmp(key, CurrentItem->key) == 0)
            break;
        else
            CurrentItem = CurrentItem->NextPtr;
    }
    return CurrentItem;
}

SymTableItem_t *SymTableAddItem(SymTable_t *SymTable, SymTableItem_t *Item)
{
    SymTableItem_t *foundItem =SymTableFindItem(SymTable, Item->key);
    if(foundItem != NULL)
    {
        return NULL;
    }
    else
    {

    }
}

void SymTableRemoveItem(SymTable_t *SymTable, char *key)
{
    unsigned int Index = SymTableIndex(SymTable, key);
    
    SymTableItemFree(foundItem);
}
void SymTableDispose(SymTable_t **SymTable);