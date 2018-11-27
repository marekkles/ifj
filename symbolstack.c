#include <stdlib.h>

#include "symbolstack.h"

void Init_SStack (SStack_t** Stack, size_t size) {
	
	*Stack = malloc(sizeof(SStack_t)+(sizeof(StackItem_t)*size));

	if (*Stack == NULL) {
		return;
	}

	(*Stack)->size = size;
	(*Stack)->Top = -1;
}

void Push_SStack (SStack_t** Stack, StackItem_t* Item) {
	
	if ((*Stack)->size <= (*Stack)->Top + 1) { //overflow
		SStack_t* TmpStack = realloc(TmpStack, ((*Stack)->size + MIN_REALLOC + sizeof(SStack_t)));
			if (TmpStack == NULL){
				return 0;
			}
		*Stack = TmpStack;
		(*Stack)->size += MIN_REALLOC;
	}

	(*Stack)->Top++;
	(*Stack)->Stack[(*Stack)->Top] = *Item;
	return 1;
}

void Pop_SStack (SStack_t* PStack) {

	if ((*PStack)->top = -1) {
		return NULL; //underflow
	}
	
	StackItem_t* Item = (*PStack)->Stack[PStack->Top];
	PStack->Top--;
}

StackItem_t* Top_SStack (SStack_t* Stack) {

	return (*Stack)->Stack[(*Stack)->Top];
}

void Dispose_SStack (SStack_t** Stack);

	free(Stack);



