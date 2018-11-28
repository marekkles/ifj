#ifndef SYMBOLSTACK_H
#define SYMBOLSTACK_H

#include "symtable.h"
#include "token.h"

#define MIN_REALLOC 50

typedef enum {
	STACK_ITEM_SYMBOL,
	STACK_ITEM_INTEGER,
	STACK_ITEM_DOUBLE,
	STACK_ITEM_OPERATION,
} StackItemType_t;

typedef struct {
	StackItemType_t type;
	union{
		SymTableItem_t * symTableItem;
		int intValue;
		double doubleValue;
		TokenOperationType_t operationType;
	};
} SStackItem_t;

typedef struct {
	int top;
	size_t size;
	SStackItem_t stack[];
} SStack_t;

void Init_SStack (SStack_t** Stack, size_t size);
int Push_SStack (SStack_t** Stack, SStackItem_t* Item);
void Pop_SStack (SStack_t* Stack);
SStackItem_t* Top_SStack (SStack_t* Stack);
//add item, add int, add double, add op_type
void Dispose_SStack (SStack_t** Stack);

#endif