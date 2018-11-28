#ifndef SYMBOLSTACK_H
#define SYMBOLSTACK_H

#include "symtable.h"
#include "token.h"

#define MIN_REALLOC 50

typedef enum {
	T_ITEM,
	INTEGER,
	DOUBLE,
	OP_TYPE,
} StackItemType_enum;

typedef struct {
	
	union{
		SymTableItem_t * TItem;
		int intValue;
		double doubleValue;
		TokenOperationType_t operationType;
	} item;
	
	StackItemType_enum Type;
} StackItem_t;

typedef struct {
	int Top;
	size_t size;
	StackItem_t Stack[];
} SStack_t;

void Init_SStack (SStack_t** Stack, size_t size);
void Push_SStack (SStack_t** Stack, StackItem_t* Item);
void Pop_SStack (SStack_t* Stack);
StackItem_t* Top_SStack (SStack_t* Stack);
//add item, add int, add double, add op_type
void Dispose_SStack (SStack_t** Stack);

#endif