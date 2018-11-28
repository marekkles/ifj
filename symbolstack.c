#include <stdlib.h>

#include "symbolstack.h"

void Init_SStack (SStack_t** Stack, size_t size) 
{
	*Stack = malloc(sizeof(SStack_t)+(sizeof(SStackItem_t)*size));

	if (*Stack == NULL) {
		return;
	}

	(*Stack)->size = size;
	(*Stack)->top = -1;
}

int Push_SStack (SStack_t** Stack, SStackItem_t* Item) 
{
	if ((*Stack)->size <= (*Stack)->top + 1) { //overflow
		SStack_t* TmpStack = realloc(TmpStack, ((*Stack)->size + MIN_REALLOC)*sizeof(SStackItem_t) + sizeof(SStack_t));
			if (TmpStack == NULL){
				return 0;
			}
		*Stack = TmpStack;
		(*Stack)->size += MIN_REALLOC;
	}

	(*Stack)->top++;
	(*Stack)->stack[(*Stack)->top] = *Item;
	return 1;
}

void Pop_SStack (SStack_t* PStack) 
{

	if (PStack->top == -1) {
		return; //underflow
	}
	PStack->top--;
}

SStackItem_t* Top_SStack (SStack_t* Stack) 
{
	return &(Stack->stack[Stack->top]);
}

void Dispose_SStack (SStack_t** Stack)
{
	free(*Stack);
}
