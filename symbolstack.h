#ifndef SYMBOLSTACK_H
#define SYMBOLSTACK_H

#include "symtable.h"
#include "token.h"

#define MIN_REALLOC 50

typedef enum {
	STACK_ITEM_VAR,
	STACK_ITEM_INTEGER,
	STACK_ITEM_DOUBLE,
	STACK_ITEM_LESSER_THAN, 
    STACK_ITEM_GREATER_THAN, 
    STACK_ITEM_EQUAL_TO, 
    STACK_ITEM_NOT_EQUAL_TO,
    STACK_ITEM_GREATER_EQUAL_THAN, 
    STACK_ITEM_LESSER_EQUAL_THAN,
    STACK_ITEM_MULTIPLY, 
    STACK_ITEM_DIVIDE, 
    STACK_ITEM_ADD, 
    STACK_ITEM_SUBTRACT, 
    STACK_ITEM_LBRACKET,
    STACK_ITEM_RBRACKET,
	STACK_ITEM_END
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