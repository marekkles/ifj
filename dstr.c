#include "dstr.h"
#include "debug.h"

const static int min_chunk = 50;

void DStrInit(DStr_t **DStr, size_t size)
{
    if((*DStr = malloc(sizeof(DStr_t) + sizeof(char)*size)) == NULL)
        return;
    else
    {
        (*DStr)->size = size;
        (*DStr)->strlen = 0;
        for(int i = 0; i < size; i++)
        {
            (*DStr)->str[i] = 0;
        }
    }
    return;
}
void DStrFree(DStr_t **DStr)
{
    free(*DStr);
    *DStr = NULL;
}
int DStrAddChar(DStr_t **DStr, char c)
{
    if((*DStr)->size <= (*DStr)->strlen+2)
    {
        DStr_t *DStrNew;
        size_t new_size = sizeof(DStr_t) + (*DStr)->size + sizeof(char)*min_chunk;
        DebugFPrintf(stdout, "Dynamic string reallocation from: %dB to %dB\n", sizeof(DStr_t) + (*DStr)->size, new_size);
        DStrNew = realloc(*DStr, new_size);
        if(DStrNew == NULL)
        {
            DebugFPuts("Dynamic string reallocation FAILED\n", stdout);
            return 0;
        }    
        *DStr = DStrNew;
        (*DStr)->size += sizeof(char)*min_chunk;
    }
    (*DStr)->str[(*DStr)->strlen] = c;
    (*DStr)->strlen++; 
    (*DStr)->str[(*DStr)->strlen] = 0;
    return 1;
}
void DStrDeleteLast(DStr_t *DStr)
{
    DStr->strlen = DStr->strlen-1;
    DStr->str[DStr->strlen] = 0;
}
int DStrClear(DStr_t *DStr)
{
    DStr->str[0] = 0;
    DStr->strlen = 0;
}
size_t DStrLen(DStr_t *DStr)
{
    return DStr->strlen;
}
char *DStrStr(DStr_t *DStr)
{
    return DStr->str;
}
