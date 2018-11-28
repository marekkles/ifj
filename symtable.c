#include <string.h>
#include "symtable.h"

unsigned int HashCode(const char *str)
{
    unsigned int hash = 5381;
    char c;
    while(c = *str++)
    {
        hash = (hash << 5) + hash + c;
    }
    return hash;
}

SymTableItem_t *SymtableAllocateItem(size_t key_len)
{
    SymTableItem_t *newItem;
    if((newItem = malloc(sizeof(SymTableItem_t) + sizeof(char)*(key_len + 1))) == NULL)
        return NULL;
    return newItem;
}

void SymTableItemFree(SymTableItem_t *Item)
{
    free(Item);
}
/**
 * Function will ty to allocate symtable, if it fails it will return ALLOCATION_ERR
 * and SymTable will be NULL, else ALL_OK.
*/
void SymTableInit(SymTable_t **SymTable, size_t size)
{
    if((*SymTable = malloc(sizeof(SymTable_t) + sizeof(SymTableItem_t *)*size)) == NULL)
    {
        return;
    }
    (*SymTable)->localMode = 0;
    (*SymTable)->size = size;
    for(unsigned int i = 0; i < size; i++)
        (*SymTable)->table[i] = NULL;
    return;
}

void SymTableSetLocalMode(SymTable_t *SymTable)
{
    SymTable->localMode = 1;
}
void SymTableUnSetLocalMode(SymTable_t *SymTable)
{
    SymTable->localMode = 0;
    SymTableItem_t *currentItem, *tempItem;
    bool beginning;
    for(int i = 0; i < SymTable->size; i++)
    {
        currentItem = SymTable->table[i];
        beginning = 1;
        while(currentItem != NULL)
        {
            if(beginning == 1 && currentItem->type == SYM_VARIABLE && currentItem->local == true)
            {
                if(beginning == 1)
                {
                    tempItem = currentItem;
                    SymTable->table[i] = currentItem->NextPtr;
                    currentItem = currentItem->NextPtr;
                    SymTableItemFree(tempItem);
                    beginning = 0;
                }   
                else if(currentItem->NextPtr != NULL)
                {
                    tempItem = currentItem->NextPtr;
                    currentItem = tempItem->NextPtr;
                    SymTableItemFree(tempItem);
                }
                else
                    currentItem = currentItem->NextPtr;
            }
            else
                currentItem = currentItem->NextPtr;
        }
    }
}
unsigned int SymTableIndex(SymTable_t *SymTable, char *key)
{
    return HashCode(key) % SymTable->size;
}

SymTableItem_t *SymTableFindItem(SymTable_t *SymTable, char *key)
{
    unsigned int Index = SymTableIndex(SymTable, key);
    SymTableItem_t * CurrentItem = SymTable->table[Index];
    while(CurrentItem != NULL)
    {
        if(SymTable->localMode == CurrentItem->local || CurrentItem->type == SYM_FUNCTION)
        {
            if(strcmp(key, CurrentItem->key) == 0)
                break;
        }    
        CurrentItem = CurrentItem->NextPtr;
    }
    return CurrentItem;
}

SymTableItem_t *SymTableAddItem(SymTable_t *SymTable, SymTableItem_t *Item)
{
    SymTableItem_t *foundItem = SymTableFindItem(SymTable, Item->key);
    if(foundItem != NULL)
    {
        return NULL;
    }
    
    else
    {
        int Index = SymTableIndex(SymTable, Item->key);
        Item->NextPtr = SymTable->table[Index];
        SymTable->table[Index] = Item;
        return Item;
    }
}
SymTableItem_t *SymTableAddFunction(SymTable_t *SymTable, char *str, int parameterCount, bool defined)
{
    SymTableItem_t *newItem;
    newItem = SymtableAllocateItem(strlen(str));
    if(newItem == NULL)
        return NULL;
    newItem->type = SYM_FUNCTION;
    strcpy(newItem->key, str);
    newItem->def = defined;
    newItem->parameterCount = parameterCount;
    return SymTableAddItem(SymTable, newItem);
}
SymTableItem_t *SymTableAddVariable(SymTable_t *SymTable, char *str)
{
    SymTableItem_t *newItem;
    newItem = SymtableAllocateItem(strlen(str));
    if(newItem == NULL)
        return NULL;
    newItem->type = SYM_VARIABLE;
    strcpy(newItem->key, str);
    newItem->local = SymTable->localMode;
    return SymTableAddItem(SymTable, newItem);
}

void SymTableRemoveItem(SymTable_t *SymTable, char *key)
{
    int Index = SymTableIndex(SymTable, key);//<<potrebujes prepisat aj ukazatel co ukazoval na tento item
    SymTableItem_t *tempItem, **rewritePointer;
    rewritePointer = &(SymTable->table[Index]);
    while(*rewritePointer != NULL)
    {   
        if((*rewritePointer)->type == SYM_FUNCTION || (*rewritePointer)->local == SymTable->localMode)
        {
            if(strcmp((*rewritePointer)->key, key) == 0)
            {
                tempItem = *rewritePointer;
                *rewritePointer = tempItem->NextPtr;
                SymTableItemFree(tempItem);
                return;
            }
        }
        else
        {
            rewritePointer = &((*rewritePointer)->NextPtr);
        }
    }
}
void SymTableDispose(SymTable_t **SymTable)
{
    SymTableItem_t *CurrentItem, *TempItem;
    
    for (int i = 0; i < (*SymTable)->size ; ++i)
    {
        CurrentItem = (*SymTable)->table[i];
        while(CurrentItem != NULL)
        {
            TempItem = CurrentItem;
            CurrentItem = CurrentItem->NextPtr;
            SymTableItemFree(TempItem);
        }
    }
    free(*SymTable);
    *SymTable = NULL;
}