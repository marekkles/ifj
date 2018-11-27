#include <stdlib.h>

#include "symbolstack.h"

void Init_SStack (SStack_t** Stack, size_t size) {
	
	*Stack = malloc(sizeof(SStack_t)+(sizeof(StackItem_t)*size));

	if (*Stack == NULL) {
		return;
	}

	(*Stack)->size = size;
	(*Stack)->top = -1;
}

void Push_SStack (SStack_t** Stack, StackItem_t* Item) {
	
	if ((*Stack)->size <= (*Stack)->top + 1) { //overflow
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

void Pop_SStack (SStack* Stack) {

	if ((*Stack)->top = -1;) {
		return NULL; //underflow
	}
	
	StackItem_t* Item = (*Stack)->Stack[(*Stack)->Top];
	(*Stack)->Top--;
}

StackItem_t* Top_SStack (SStack_t* Stack) {

	return (*Stack)->Stack[(*Stack)->Top];
}

void Dispose_SStack (SStack_t** Stack);

	free(Stack);



