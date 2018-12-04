/**
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                     *
 *                  Projekt IFJ                        *
 *                                                     *
 * Implementace překladače imperativního jazyka IFJ18. *
 *                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  
                       Súbor:
                      dstr.c
                      
                       Popis:
     Praca so strukturov DStr_t (dynamicky string), 
     inicializacia, pridavanie znakov, cistenie stringu,
     odstrnenie stringu, konkateacia, prepis stringu ...

                      Autori:
               Marek Vaško (xvasko16)
              Handzuš Daniel (xhandz01)
                Alexaj Adam (xalexa07)
*/

#include "dstr.h"
#include "debug.h"

const static int min_chunk = 50;
/**
 * Function initializes dynamic string, it will store pointer to alllocated
 * memory in *DStr, and memory will be allocated to size size
 * @param DStr Pointer to where pointer to dynamic string will be saved
 * @param size Initial size of dynamic string
*/
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
            (*DStr)->str[i] = '\0';
        }
    }
    return;
}
/**
 * Function will free dynamic string and st pointer to it to NULL
 * @param DStr Dynamic string to be freed
 */
void DStrFree(DStr_t **DStr)
{
    free(*DStr);
    *DStr = NULL;
}
/**
 * Function will add char to the end of dynamic string. It will
 * return 1 if char was succesfully added and 0 if not.
 * @param DStr Dynamic string where new char will be added
 * @param c Char to be added
 */
int DStrAddChar(DStr_t **DStr, char c)
{
    if((*DStr)->size <= (*DStr)->strlen+2)
    {
        DStr_t *DStrNew;
        size_t new_size = sizeof(DStr_t) + (*DStr)->size + sizeof(char)*min_chunk;
        //DebugFPrintf(stdout, "Dynamic string reallocation from: %dB to %dB\n", sizeof(DStr_t) + (*DStr)->size, new_size);
        DStrNew = realloc(*DStr, new_size);
        if(DStrNew == NULL)
        {
            //DebugFPuts("Dynamic string reallocation FAILED\n", stdout);
            return 0;
        }    
        *DStr = DStrNew;
        (*DStr)->size += min_chunk;
    }
    (*DStr)->str[(*DStr)->strlen] = c;
    (*DStr)->strlen++; 
    (*DStr)->str[(*DStr)->strlen] = 0;
    return 1;
}

/**
 * Function will concatenate DStr wit input str
 * @param DStr Target dynamic string
 * @param str String to be concatenated
*/
int DStrCat(DStr_t **DStr, const char *str)
{
    int str_len = strlen(str);
    if((*DStr)->size <= (*DStr)->strlen + str_len + 1)
    {
        DStr_t *DStrNew;
        size_t min_expansion =  (*DStr)->strlen + str_len + 1 - (*DStr)->size;
        size_t expansion = min_expansion + (50 - min_expansion%50);
        size_t new_size = sizeof(DStr_t) + (*DStr)->size + sizeof(char)*expansion;

        //DebugFPrintf(stdout, "Dynamic string reallocation from: %dB to %dB\n", sizeof(DStr_t) + (*DStr)->size, new_size);

        DStrNew = realloc(*DStr, new_size);
        if(DStrNew == NULL)
        {
            //DebugFPuts("Dynamic string reallocation FAILED\n", stdout);
            return 0;
        }    
        *DStr = DStrNew;
        (*DStr)->size += sizeof(char)*expansion;
    }
    strcat(DStrStr(*DStr), str);
    (*DStr)->strlen += str_len; 
    return 1;
}
/**
 * Function will replace content of dynamic string with str,
 * it will return 1 if repacement went as planed, 0 if not
 * @param DStr Dynamic string to replace
 * @param str String to replace dstr with
 */
int DStrReplace(DStr_t **DStr, const char *str)
{
    DStrClear(*DStr);
    return DStrCat(DStr, str);
}
/**
 * Function will return las char in dynamic string
 * @param DStr input string to get char from
*/

char DStrLast(DStr_t *DStr)
{
    if(DStr->strlen == 0)
        return '\0';
    return DStr->str[DStr->strlen - 1];
}

/**
 * Function will deete last character and set it to zero
 * it will also decremet length of string
 * @param DStr Input dynamic string
 */
void DStrDeleteLast(DStr_t *DStr)
{
    if(DStr->strlen <= 0)
        return;
    (DStr->strlen)--;
    DStr->str[DStr->strlen] = 0;
}
/**
 * Function will clear dynamic string
 * @param DStr Dynamic strin to be cleaned
*/
int DStrClear(DStr_t *DStr)
{
    DStr->str[0] = 0;
    DStr->strlen = 0;
}
/**
 * Function will retur length of string currently stored in
 * dynamic string.
 * @param DStr Input dynamic string
*/
size_t DStrLen(DStr_t *DStr)
{
    return DStr->strlen;
}
/**
 * Function will return pointer to string stored in dynamic string
 * @param DStr Input dynamic string
*/
char *DStrStr(DStr_t *DStr)
{
    return DStr->str;
}
