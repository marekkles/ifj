#ifndef SYMBOLSTACK_H
#define SYMBOLSTACK_H


#include <stdlib.h>
#include "symtable.h"
#include "codegen.h"
#include "token.h"
#include "dstr.h"
#include "return.h"

#define MIN_REALLOC 50
#define SSTACK_DEFAULT_SIZE 25

typedef enum {
	PRE_LT, // <
	PRE_GT, // >
	PRE_EQ, // =
	PRE_NT  // blank 
} ExpressionPrecedence_t;
typedef enum {
	TERMINAL_MULT_DIVIDE, // * /
	TERMINAL_PLUS_MINUS, // + -
	TERMINAL_COMPARISON, // <= >= < >
	TERMINAL_NOT_EQUAL, // == !=
	TERMINAL_LEFT_BRACKET, // (
	TERMINAL_RIGHT_BRACKET, // )
	TERMINAL_TERM, // id int float string nil
	TERMINAL_END, // $
} ExpressionTerminal_t;


typedef enum {
	STACK_SYMBOL,
	STACK_INT,
	STACK_DOUBLE,
	STACK_STRING,
	STACK_BOOL,
	STACK_NIL,
	STACK_OPERATION
} StackItemDataType_t;

typedef struct {
	ExpressionTerminal_t terminal;
	bool isNonterminal;
	bool isLessThan;

	StackItemDataType_t dataType;
	union {
		SymTableItem_t *symbol;
		int intValue;
		double doubleValue;
		bool boolValue;
		char *string;
		TokenOperationType_t operation;
	} data;
} SStackItem_t;

typedef struct {
	int top;
	size_t size;
	SStackItem_t stack[];
} SStack_t;

char *SStackAlocateCopyString(char *originalString);
void SStackFreeString(char *string);
int SStackProcessTokenToItem(DStr_t *dstr, Token_t *token, SStackItem_t *retsultingItem);

SStackItem_t *SStackTopTerminal(SStack_t *Stack);
int SStackTopTerminalIndex(SStack_t *Stack);
SStackItem_t *SStackTopLT(SStack_t *Stack);
int SStackTopLTIndex(SStack_t *Stack);
int SStackTopTerminalAddLT(SStack_t *Stack);

int SStackReduceByRule(SStack_t *Stack);
int SStackPushEnd(SStack_t** Stack);

ExpressionPrecedence_t SStackGetExpessionPrecedence(SStack_t *stack, SStackItem_t *stackItem);

void Init_SStack (SStack_t** Stack, size_t size);
int Push_SStack (SStack_t** Stack, SStackItem_t* Item);
void Pop_SStack (SStack_t* Stack);
void PopN_SStack (SStack_t* Stack, unsigned int number);
SStackItem_t* Top_SStack (SStack_t* Stack);
//add item, add int, add double, add op_type
void Dispose_SStack (SStack_t** Stack);

#endif