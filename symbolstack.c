#include "symbolstack.h"

char *SStackAlocateCopyString(char *originalString)
{
	size_t originalStringSize = strlen(originalString)+1;
	char *returnString;
	returnString = malloc(originalStringSize);
	if(returnString == NULL)
		return NULL;
	strcpy(returnString, originalString);
	return returnString;
}
void SStackFreeString(char *string)
{
	free(string);
}
int SStackProcessTokenToItem(DStr_t *dstr, Token_t *token, SStackItem_t *resultingItem)
{
	resultingItem->isLessThan = false;
	resultingItem->isNonterminal = false;
	switch(token->type)
	{
		case T_DOUBLE:
		{
			resultingItem->terminal = TERMINAL_TERM;
			resultingItem->dataType = STACK_DOUBLE;
			resultingItem->data.doubleValue = token->doubleValue;
			break;
		}
		case T_INTEGER:
		{
			resultingItem->terminal = TERMINAL_TERM;
			resultingItem->dataType = STACK_INT;
			resultingItem->data.intValue = token->intValue;
			break;
		}
		case T_STRING:
		{
			resultingItem->terminal = TERMINAL_TERM;
			resultingItem->dataType = STACK_STRING;
			resultingItem->data.string = SStackAlocateCopyString(DStrStr(dstr));
			if(resultingItem->data.string == NULL)
				return PARSE_INT_ERR;
			break;
		}
		case T_IDENTIFIER:
		{
			SymTableItem_t *foundSymtableItem = SymTableFindItem(symtable, DStrStr(dstr));
			if(foundSymtableItem == NULL)
				return PARSE_UNDEF_VAR;
			if(foundSymtableItem->type != SYM_VARIABLE)
				return PARSE_OTHER;

			resultingItem->terminal = TERMINAL_TERM;
			resultingItem->dataType = STACK_SYMBOL;
			resultingItem->data.symbol = foundSymtableItem;
			break;
		}
		case T_OPERATION:
		{
			resultingItem->dataType = STACK_OPERATION;
			resultingItem->data.operation = token->operationType;
			switch(token->operationType)
			{
				case TO_ADD:
				case TO_SUBTRACT:
				{
					resultingItem->terminal = TERMINAL_PLUS_MINUS;
					break;
				}
				case TO_MULTIPLY:
				case TO_DIVIDE:
				{
					resultingItem->terminal = TERMINAL_MULT_DIVIDE;
					break;
				}
				case TO_LESSER_THAN:
				case TO_GREATER_THAN:
				case TO_LESSER_EQUAL_THAN:
				case TO_GREATER_EQUAL_THAN:
				{
					resultingItem->terminal = TERMINAL_COMPARISON;
					break;
				}
				case TO_EQUAL_TO:
				case TO_NOT_EQUAL_TO:
				{
					resultingItem->terminal = TERMINAL_NOT_EQUAL;
					break;
				}
				case TO_LBRACKET:
				{
					resultingItem->terminal = TERMINAL_LEFT_BRACKET;
					break;
				}
				case TO_RBRACKET:
				{
					resultingItem->terminal = TERMINAL_RIGHT_BRACKET;
					break;
				}
				default:
				{
					resultingItem->terminal = TERMINAL_END;
					resultingItem->dataType = STACK_NIL;
				}
			}
			break;
		}
		case T_KEYWORD:
		{
			if(token->keywordType == TK_NIL)
			{
				resultingItem->terminal = TERMINAL_TERM;
				resultingItem->dataType = STACK_NIL;
			}
			break;
		}
		default:
		{
			resultingItem->terminal = TERMINAL_END;
			resultingItem->dataType = STACK_NIL;
			break;
		}
	}
	return PARSE_OK;
}

int SStackTopTerminal(SStack_t *Stack)
{
	for(int i = Stack->top; i >= 0; i--)
	{
		if(Stack->stack[i].isNonterminal == false)
			return i;
	}
	return -1;
}
int SStackTopLT(SStack_t *Stack)
{
	for(int i = Stack->top; i >= 0; i--)
	{
		if(Stack->stack[i].isLessThan == true)
			return i;
	}
	return -1;
}
int SStackTopTerminalAddLT(SStack_t *Stack)
{
	int topTerminalIndex = SStackTopTerminal(Stack);
	if(topTerminalIndex == -1)
		return -1;
	Stack->stack[topTerminalIndex].isLessThan = true;
	return topTerminalIndex;
}
int SStackReduceByRule(SStack_t** Stack, int ltIndex)
{

	
}
int SStackPushEnd(SStack_t** Stack)
{
	SStackItem_t currentStackItem;
    currentStackItem.terminal = TERMINAL_END;
    currentStackItem.isLessThan = false;
    currentStackItem.isNonterminal = false;
    currentStackItem.dataType = STACK_NIL;

    if(!Push_SStack(Stack, &currentStackItem))
    {
        return PARSE_INT_ERR;
    }
	return PARSE_OK;
}

ExpressionPrecedence_t SStackGetExpessionPrecedence(SStack_t *stack, SStackItem_t *stackItem)
{
	const ExpressionPrecedence_t precedenceTable[8][8] = {
	//	 _______________________________________________________________
	//	|       |       |       |       |       |       | id int|       |
	//	|  * /  |  + -  | <= >= | != == |   (   |   )   | float |   $   |
	//	|       |       |  < >  |       |       |       | string|       |
	//	|       |       |       |       |       |       |  nil  |       |
		{ PRE_GT, PRE_GT, PRE_GT, PRE_GT, PRE_LT, PRE_GT, PRE_LT, PRE_GT},  // * /
		{ PRE_LT, PRE_GT, PRE_GT, PRE_GT, PRE_LT, PRE_GT, PRE_LT, PRE_GT},  // + -
		{ PRE_LT, PRE_LT, PRE_GT, PRE_GT, PRE_LT, PRE_GT, PRE_LT, PRE_GT},  // <= >= < >
		{ PRE_LT, PRE_LT, PRE_LT, PRE_GT, PRE_LT, PRE_GT, PRE_LT, PRE_GT},  // != ==
		{ PRE_LT, PRE_LT, PRE_LT, PRE_LT, PRE_LT, PRE_EQ, PRE_LT, PRE_NT},  // (
		{ PRE_GT, PRE_GT, PRE_GT, PRE_GT, PRE_NT, PRE_GT, PRE_NT, PRE_GT},  // )
		{ PRE_GT, PRE_GT, PRE_GT, PRE_GT, PRE_NT, PRE_GT, PRE_NT, PRE_GT},  // id int float sting nil
		{ PRE_LT, PRE_LT, PRE_LT, PRE_LT, PRE_LT, PRE_NT, PRE_LT, PRE_LT},  // $
	};
	int topTerminal = SStackTopTerminal(stack);

	int tableIndex = stack->stack[topTerminal].terminal;

	return precedenceTable[stackItem->terminal][tableIndex];
}



void Init_SStack (SStack_t** Stack, size_t size) 
{
	*Stack = malloc(sizeof(SStack_t)+(sizeof(SStackItem_t)*size));

	if (*Stack == NULL) {
		return;
	}

	(*Stack)->size = size;
	(*Stack)->top = -1;
}

int Push_SStack(SStack_t **Stack, SStackItem_t *Item)
{
	if ((*Stack)->size <= (*Stack)->top + 1)
	{ //overflow
		SStack_t *TmpStack;
		size_t newSize = ((*Stack)->size + MIN_REALLOC)*sizeof(SStackItem_t) + sizeof(SStack_t);
		TmpStack = realloc(*Stack, newSize);
		if (TmpStack == NULL)
		{
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
	if(PStack->stack[PStack->top].dataType == STACK_STRING)
		SStackFreeString(PStack->stack[PStack->top].data.string);
	PStack->top--;
}

SStackItem_t* Top_SStack (SStack_t* Stack) 
{
	return &(Stack->stack[Stack->top]);
}

void Dispose_SStack (SStack_t** Stack)
{
	for(int i = 0; i <= (*Stack)->top; i++)
		if((*Stack)->stack[i].dataType == STACK_STRING)
			SStackFreeString((*Stack)->stack[i].data.string);
	free(*Stack);
	*Stack = NULL;
}
