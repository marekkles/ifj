/**
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                     *
 *                  Projekt IFJ                        *
 *                                                     *
 * Implementace překladače imperativního jazyka IFJ18. *
 *                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  
                       Súbor:
                    symbolstack.c
                      
                       Popis:
     Obsahuje implementacie prese so strukturov SStack_t
	  co obsahuje zaroven aj pravidla pre redukciu
	  vyrazov pomocou predecencnej tabulky, a nasledne
	  	 generovanie kodu podla danych pravidiel

                      Autori:
               Marek Vaško (xvasko16)
              Handzuš Daniel (xhandz01)
                Alexaj Adam (xalexa07)
*/

#include "symbolstack.h"
/**
 * Function will allocate memmory of size of originalString
 * and it will copy its content to newly allocated space.
 * In case of error it will return NULL, else pointer to
 * allocated string.
 * @param string Pointer to string to free
*/
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
/**
 * Function will free alocated string.
 * @param string Pointer to string to free
*/
void SStackFreeString(char *string)
{
	free(string);
}
/**
 * Function will process token and synamic string to stack
 * item. If conversion goes as planed it will return PARSE_INT_ERR
 * else it will return PARSE_OK.
 * @param dstr pointer to dynamic string
 * @param token Pointer to token
 * @param resultingItem Item to save resulting stack item to
*/
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
			else
			{
				resultingItem->terminal = TERMINAL_END;
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
/**
 * Function will return index of first terminal
 * from top of the stack. It will return -1
 * if there is no terminal
 * @param Stack Stack to use
*/
int SStackTopTerminalIndex(SStack_t *Stack)
{
	for(int i = Stack->top; i >= 0; i--)
	{
		if(Stack->stack[i].isNonterminal == false)
			return i;
	}
	return -1;
}
/**
 * Function will return pointer to first terminal
 * from top of the stack. It will return NULL
 * if there is no terminal
 * @param Stack Stack to use
*/
SStackItem_t *SStackTopTerminal(SStack_t *Stack)
{
	int topTerminalIndex = SStackTopTerminalIndex(Stack);
	if(topTerminalIndex != -1)
		return &(Stack->stack[topTerminalIndex]); 
	return NULL;
}
/**
 * Function will return index of first item
 * from top of the stack which has < flag.
 * It will return -1 if there there is no
 * item with < flag.
 * @param Stack Stack to use
*/
int SStackTopLTIndex(SStack_t *Stack)
{
	for(int i = Stack->top; i >= 0; i--)
	{
		if(Stack->stack[i].isLessThan == true)
			return i;
	}
	return -1;
}
/**
 * Function will return pointer to first item
 * from top of the stack which has < flag.
 * It will return NULL if there there is no
 * item with < flag.
 * @param Stack Stack to use
*/
SStackItem_t *SStackTopLT(SStack_t *Stack)
{
	int topLTIndex = SStackTopLTIndex(Stack);
	if(topLTIndex != -1)
		return &(Stack->stack[topLTIndex]); 
	return NULL;
}
/**
 * Function will add < flag to top terminal
 * @param Stack Stack to use
*/
int SStackTopTerminalAddLT(SStack_t *Stack)
{
	SStackItem_t *topTerminal = SStackTopTerminal(Stack);
	if(topTerminal == NULL)
		return PARSE_OTHER;
	topTerminal->isLessThan = true;
	return PARSE_OK;
}

/**
 * Enum used to classify what combination of type are Stack items
*/
typedef enum {
	C_NIL_NIL, C_INT_INT, C_DOUBLE_DOUBLE, C_STRING_STRING, C_VAR_VAR,
	C_NIL_INT, C_NIL_DOUBLE, C_NIL_STRING, C_NIL_VAR,
	C_INT_DOUBLE, C_INT_STRING, C_INT_VAR,
	C_DOUBLE_STRING, C_DOUBLE_VAR,
	C_STRING_VAR,
	C_OTHER
} OperandsCombinations_t;

/**
 * Function will return combination of two items from OperandsCombinations_t
 * @param item1 First item to compare
 * @param item2 Second item to compare
*/
static int SStackWhatAreItems(SStackItem_t *item1, SStackItem_t *item2)
{
	OperandsCombinations_t return_value = C_OTHER;
	if(item1->dataType == STACK_NIL && item2->dataType == STACK_NIL)
		return_value = C_NIL_NIL;
	else if(item1->dataType == STACK_INT && item2->dataType == STACK_INT)
		return_value = C_INT_INT;
	else if(item1->dataType == STACK_DOUBLE && item2->dataType == STACK_DOUBLE)
		return_value = C_DOUBLE_DOUBLE;
	else if(item1->dataType == STACK_STRING && item2->dataType == STACK_STRING)
		return_value = C_STRING_STRING;
	else if(item1->dataType == STACK_SYMBOL && item2->dataType == STACK_SYMBOL)
		return_value = C_VAR_VAR;
	else if((item1->dataType == STACK_INT || item1->dataType == STACK_NIL) &&
	        (item2->dataType == STACK_INT || item2->dataType == STACK_NIL))
		return_value = C_NIL_INT;
	else if((item1->dataType == STACK_DOUBLE || item1->dataType == STACK_NIL) &&
	        (item2->dataType == STACK_DOUBLE || item2->dataType == STACK_NIL))
		return_value = C_NIL_DOUBLE;
	else if((item1->dataType == STACK_STRING || item1->dataType == STACK_NIL) &&
	        (item2->dataType == STACK_STRING || item2->dataType == STACK_NIL))
		return_value = C_NIL_STRING;
	else if((item1->dataType == STACK_SYMBOL || item1->dataType == STACK_NIL) &&
	        (item2->dataType == STACK_SYMBOL || item2->dataType == STACK_NIL))
		return_value = C_NIL_VAR;
	else if((item1->dataType == STACK_DOUBLE || item1->dataType == STACK_INT) &&
	        (item2->dataType == STACK_DOUBLE || item2->dataType == STACK_INT))
		return_value = C_INT_DOUBLE;
	else if((item1->dataType == STACK_STRING || item1->dataType == STACK_INT) &&
	        (item2->dataType == STACK_STRING || item2->dataType == STACK_INT))
		return_value = C_INT_STRING;
	else if((item1->dataType == STACK_SYMBOL || item1->dataType == STACK_INT) &&
	        (item2->dataType == STACK_SYMBOL || item2->dataType == STACK_INT))
		return_value = C_INT_VAR;
	else if((item1->dataType == STACK_STRING || item1->dataType == STACK_DOUBLE) &&
	        (item2->dataType == STACK_STRING || item2->dataType == STACK_DOUBLE))
		return_value = C_DOUBLE_STRING;
	else if((item1->dataType == STACK_SYMBOL || item1->dataType == STACK_DOUBLE) &&
	        (item2->dataType == STACK_SYMBOL || item2->dataType == STACK_DOUBLE))
		return_value = C_DOUBLE_VAR;
	else if((item1->dataType == STACK_SYMBOL || item1->dataType == STACK_STRING) &&
	        (item2->dataType == STACK_SYMBOL || item2->dataType == STACK_STRING))
		return_value = C_STRING_VAR;
	return return_value;
}
/**
 * Function will return double value of item, if it is int it will be converted
 * @param item Stack item to convert
*/
static double SStackGetItemDoubleValue(SStackItem_t *item)
{
	if(item->dataType == STACK_INT)
		return (double)item->data.intValue;
	return item->data.doubleValue;
}
/**
 * Function will add stack item in neccesary form, to code in compatible form
 * In case of error it will return PARSE_INT_ERR else PARSE_OK
 * @param item Stack item to add to code
*/
static int CodeAddSStackItem(SStackItem_t *item)
{
	int return_value = PARSE_OK;
    if(item->dataType == STACK_DOUBLE)
        return_value = CodeAddDouble(item->data.doubleValue);
    else if(item->dataType == STACK_INT)
        return_value = CodeAddInt(item->data.intValue);
    else if(item->dataType == STACK_NIL)
        return_value = CodeAddNil();
    else if(item->dataType == STACK_BOOL)
        return_value = CodeAddBool(item->data.boolValue);
    else if(item->dataType == STACK_STRING)
        return_value = CodeAddString(item->data.string);
    else if(item->dataType == STACK_SYMBOL)
        return_value = CodeAddVariable(item->data.symbol->key);
    else
        return PARSE_INT_ERR;
    return return_value;
} 
/**
 * Function will find efficient way to reduce two items to addition,
 * if they are constant it will not add anything to code,
 * Code is added only if one of them is variable.
 * @param Stack stack to use
 * @param startingIndex Index to start reduction from
 * @param temporaryVariableCount Pointer to counter of temtorary variables
*/
static int SStackRuleAddition(SStack_t *Stack, int startingIndex, int *temporaryVariableCount)
{
	SStackItem_t *item1 = &(Stack->stack[startingIndex]);
	SStackItem_t *item2 = &(Stack->stack[startingIndex+2]);
	int itemsType = SStackWhatAreItems(item1, item2);
	int return_value = PARSE_OK;
	switch(itemsType)
	{
		case C_INT_INT:
		{
			item1->data.intValue += item2->data.intValue;
			break;
		}
		case C_DOUBLE_DOUBLE:
		{
			item1->data.doubleValue += item2->data.doubleValue;
			break;
		}
		case C_INT_DOUBLE:
		{
			double doubleValue1 = SStackGetItemDoubleValue(item1);
			double doubleValue2 = SStackGetItemDoubleValue(item2);
			item1->dataType = STACK_DOUBLE;
			item1->data.doubleValue = doubleValue1 + doubleValue2;
			break;
		}
		case C_STRING_STRING:
		{
			int stringLength = strlen(item1->data.string) + strlen(item2->data.string) + 1;
			char *newString = realloc(item1->data.string, stringLength * sizeof(char));
			if(newString == NULL)
				return PARSE_INT_ERR;
			
			item1->data.string = newString;
			strcat(item1->data.string, item2->data.string);
			SStackFreeString(item2->data.string);
			break;
		}
		case C_STRING_VAR: case C_DOUBLE_VAR: case C_INT_VAR: case C_VAR_VAR:
		{
			(*temporaryVariableCount)++;
			int uniqueOperationId = CodeGetUniqueOperation();

			return_value |= CodeAddTextToBody("\nMOVE GF@%operand1 ");
			return_value |= CodeAddSStackItem(item1);
			return_value |= CodeAddTextToBody("\nMOVE GF@%operand2 ");
			return_value |= CodeAddSStackItem(item2);
			return_value |= CodeAddTextToBody("\nTYPE GF@%operand1type GF@%operand1");
			return_value |= CodeAddTextToBody("\nTYPE GF@%operand2type GF@%operand2");
			return_value |= CodeAddTextToBody("\nJUMPIFNEQ ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "conversion");
			return_value |= CodeAddTextToBody(" GF@%operand1type string@string");
			return_value |= CodeAddTextToBody("\nJUMPIFNEQ ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "conversion");
			return_value |= CodeAddTextToBody(" GF@%operand2type string@string");
			return_value |= CodeAddTextToBody("\nCONCAT ");
			return_value |= CodeAddTempVariable(*temporaryVariableCount);
			return_value |= CodeAddTextToBody(" GF@%operand1 GF@%operand2");
			return_value |= CodeAddTextToBody("\nJUMP ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "end");
			//LABEL $$operation[uniqueOperationId]conversion
			return_value |= CodeAddTextToBody("\nLABEL ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "conversion");
			return_value |= CodeAddTextToBody("\nJUMPIFEQ ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "compatibility");
			return_value |= CodeAddTextToBody(" GF@%operand1type GF@%operand2type");
			return_value |= CodeAddTextToBody("\nCALL $$operand1ToFloat");
			return_value |= CodeAddTextToBody("\nCALL $$operand2ToFloat");
			//LABEL $$operation[uniqueOperationId]compatibility
			return_value |= CodeAddTextToBody("\nLABEL ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "compatibility");
			return_value |= CodeAddTextToBody("\nCALL $$operandAdditionCompatibility");
			return_value |= CodeAddTextToBody("\nADD ");
			return_value |= CodeAddTempVariable(*temporaryVariableCount);
			return_value |= CodeAddTextToBody(" GF@%operand1 GF@%operand2");
			//LABEL $$operation[uniqueOperationId]end
			return_value |= CodeAddTextToBody("\nLABEL ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "end");
			
			if(return_value != PARSE_OK)
				return PARSE_INT_ERR;
			if(item1->dataType == STACK_STRING)
				SStackFreeString(item1->data.string);
			if(item2->dataType == STACK_STRING)
				SStackFreeString(item2->data.string);
			break;
		}
		default:
			return PARSE_TYPE_COMP;
	}
	Stack->top = Stack->top - 2;
	return return_value;
}
/**
 * Function will find efficient way to reduce two items to subtraction,
 * if they are constant it will not add anything to code,
 * Code is added only if one of them is variable. It will return PARSE_OK if everything
 * went ok, PARSE_INT_ERR if code addition went wrong, PARSE_TYPE_COMP if operands
 * are of incompatible type/
 * @param Stack stack to use
 * @param startingIndex Index to start reduction from
 * @param temporaryVariableCount Pointer to counter of temtorary variables
*/
static int SStackRuleSubtraction(SStack_t *Stack, int startingIndex, int *temporaryVariableCount)
{
	SStackItem_t *item1 = &(Stack->stack[startingIndex]);
	SStackItem_t *item2 = &(Stack->stack[startingIndex+2]);
	int itemsType = SStackWhatAreItems(item1, item2);
	int return_value = PARSE_OK;
	switch(itemsType)
	{
		case C_INT_INT:
		{
			item1->data.intValue -= item2->data.intValue;
			break;
		}
		case C_DOUBLE_DOUBLE:
		{
			item1->data.doubleValue -= item2->data.doubleValue;
			break;
		}
		case C_INT_DOUBLE:
		{
			double doubleValue1 = SStackGetItemDoubleValue(item1);
			double doubleValue2 = SStackGetItemDoubleValue(item2);
			item1->dataType = STACK_DOUBLE;
			item1->data.doubleValue = doubleValue1 - doubleValue2;
			break;
		}
		case C_DOUBLE_VAR: case C_INT_VAR: case C_VAR_VAR:
		{
			(*temporaryVariableCount)++;
			int uniqueOperationId = CodeGetUniqueOperation();

			return_value |= CodeAddTextToBody("\nMOVE GF@%operand1 ");
			return_value |= CodeAddSStackItem(item1);
			return_value |= CodeAddTextToBody("\nMOVE GF@%operand2 ");
			return_value |= CodeAddSStackItem(item2);
			return_value |= CodeAddTextToBody("\nTYPE GF@%operand1type GF@%operand1");
			return_value |= CodeAddTextToBody("\nTYPE GF@%operand2type GF@%operand2");
			return_value |= CodeAddTextToBody("\nJUMPIFEQ ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "compatibility");
			return_value |= CodeAddTextToBody(" GF@%operand1type GF@%operand2type");
			return_value |= CodeAddTextToBody("\nCALL $$operand1ToFloat");
			return_value |= CodeAddTextToBody("\nCALL $$operand2ToFloat");
			return_value |= CodeAddTextToBody("\nLABEL ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "compatibility");
			return_value |= CodeAddTextToBody("\nCALL $$operandNumberCompatibility");
			return_value |= CodeAddTextToBody("\nSUB ");
			return_value |= CodeAddTempVariable(*temporaryVariableCount);
			return_value |= CodeAddTextToBody(" GF@%operand1 GF@%operand2");

			if(return_value != PARSE_OK)
				return PARSE_INT_ERR;
			
			break;
		}
		default:
			return PARSE_TYPE_COMP;
	}
	Stack->top = Stack->top - 2;
	return PARSE_OK;
}
/**
 * Function will find efficient way to reduce two items to multiplication,
 * if they are constant it will not add anything to code,
 * Code is added only if one of them is variable. It will return PARSE_OK if everything
 * went ok, PARSE_INT_ERR if code addition went wrong, PARSE_TYPE_COMP if operands
 * are of incompatible type.
 * @param Stack stack to use
 * @param startingIndex Index to start reduction from
 * @param temporaryVariableCount Pointer to counter of temtorary variables
*/
static int SStackRuleMultiplication(SStack_t *Stack, int startingIndex, int *temporaryVariableCount)
{
	SStackItem_t *item1 = &(Stack->stack[startingIndex]);
	SStackItem_t *item2 = &(Stack->stack[startingIndex+2]);
	int itemsType = SStackWhatAreItems(item1, item2);
	int return_value = PARSE_OK;
	switch(itemsType)
	{
		case C_INT_INT:
		{
			item1->data.intValue *= item2->data.intValue;
			break;
		}
		case C_DOUBLE_DOUBLE:
		{
			item1->data.doubleValue *= item2->data.doubleValue;
			break;
		}
		case C_INT_DOUBLE:
		{
			double doubleValue1 = SStackGetItemDoubleValue(item1);
			double doubleValue2 = SStackGetItemDoubleValue(item2);
			item1->dataType = STACK_DOUBLE;
			item1->data.doubleValue = doubleValue1 * doubleValue2;
			break;
		}
		case C_DOUBLE_VAR: case C_INT_VAR: case C_VAR_VAR:
		{
			(*temporaryVariableCount)++;
			int uniqueOperationId = CodeGetUniqueOperation();
			return_value |= CodeAddTextToBody("\nMOVE GF@%operand1 ");
			return_value |= CodeAddSStackItem(item1);
			return_value |= CodeAddTextToBody("\nMOVE GF@%operand2 ");
			return_value |= CodeAddSStackItem(item2);
			return_value |= CodeAddTextToBody("\nTYPE GF@%operand1type GF@%operand1");
			return_value |= CodeAddTextToBody("\nTYPE GF@%operand2type GF@%operand2");
			return_value |= CodeAddTextToBody("\nJUMPIFEQ ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "compatibility");
			return_value |= CodeAddTextToBody(" GF@%operand1type GF@%operand2type");
			return_value |= CodeAddTextToBody("\nCALL $$operand1ToFloat");
			return_value |= CodeAddTextToBody("\nCALL $$operand2ToFloat");
			return_value |= CodeAddTextToBody("\nLABEL ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "compatibility");
			return_value |= CodeAddTextToBody("\nCALL $$operandNumberCompatibility");
			return_value |= CodeAddTextToBody("\nMUL ");
			return_value |= CodeAddTempVariable(*temporaryVariableCount);
			return_value |= CodeAddTextToBody(" GF@%operand1 GF@%operand2");
			if(return_value != PARSE_OK)
				return PARSE_INT_ERR;
			break;
		}
		default:
			return PARSE_TYPE_COMP;
	}
	Stack->top = Stack->top - 2;
	return PARSE_OK;
}
/**
 * Function will find efficient way to reduce two items to division,
 * if they are constant it will not add anything to code,
 * Code is added only if one of them is variable.  It will return PARSE_OK if everything
 * went ok, PARSE_INT_ERR if code addition went wrong, PARSE_TYPE_COMP if operands
 * are of incompatible type.
 * @param Stack stack to use
 * @param startingIndex Index to start reduction from
 * @param temporaryVariableCount Pointer to counter of temtorary variables
*/
static int SStackRuleDivision(SStack_t *Stack, int startingIndex, int *temporaryVariableCount)
{
	SStackItem_t *item1 = &(Stack->stack[startingIndex]);
	SStackItem_t *item2 = &(Stack->stack[startingIndex+2]);
	int itemsType = SStackWhatAreItems(item1, item2);
	int return_value = PARSE_OK;

	if((item2->dataType == STACK_DOUBLE && item2->data.doubleValue == 0) || 
	   (item2->dataType == STACK_INT && item2->data.intValue == 0))
		return PARSE_ZERO_DIV;
	
	switch(itemsType)
	{
		case C_INT_INT:
		{
			item1->data.intValue /= item2->data.intValue;
			break;
		}
		case C_DOUBLE_DOUBLE:
		{
			item1->data.doubleValue /= item2->data.doubleValue;
			break;
		}
		case C_INT_DOUBLE:
		{
			double doubleValue1 = SStackGetItemDoubleValue(item1);
			double doubleValue2 = SStackGetItemDoubleValue(item2);
			item1->dataType = STACK_DOUBLE;
			item1->data.doubleValue = doubleValue1 / doubleValue2;
			break;
		}
		case C_DOUBLE_VAR: case C_INT_VAR: case C_VAR_VAR:
		{
			(*temporaryVariableCount)++;
			int uniqueOperationId = CodeGetUniqueOperation();
			return_value |= CodeAddTextToBody("\nMOVE GF@%operand1 ");
			return_value |= CodeAddSStackItem(item1);
			return_value |= CodeAddTextToBody("\nMOVE GF@%operand2 ");
			return_value |= CodeAddSStackItem(item2);
			return_value |= CodeAddTextToBody("\nTYPE GF@%operand1type GF@%operand1");
			return_value |= CodeAddTextToBody("\nTYPE GF@%operand2type GF@%operand2");
			return_value |= CodeAddTextToBody("\nJUMPIFNEQ ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "conevert");
			return_value |= CodeAddTextToBody(" GF@%operand1type string@int");
			return_value |= CodeAddTextToBody("\nJUMPIFNEQ ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "conevert");
			return_value |= CodeAddTextToBody(" GF@%operand2type string@int");
			return_value |= CodeAddTextToBody("\nIDIV ");
			return_value |= CodeAddTempVariable(*temporaryVariableCount);
			return_value |= CodeAddTextToBody(" GF@%operand1 GF@%operand2");
			return_value |= CodeAddTextToBody("\nJUMP ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "end");
			return_value |= CodeAddTextToBody("\nLABEL ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "conevert");
			return_value |= CodeAddTextToBody("\nCALL $$operand1ToFloat");
			return_value |= CodeAddTextToBody("\nCALL $$operand2ToFloat");
			return_value |= CodeAddTextToBody("\nCALL $$operandNumberCompatibility");
			return_value |= CodeAddTextToBody("\nDIV ");
			return_value |= CodeAddTempVariable(*temporaryVariableCount);
			return_value |= CodeAddTextToBody(" GF@%operand1 GF@%operand2");
			return_value |= CodeAddTextToBody("\nLABEL ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "end");
			if(return_value != PARSE_OK)
				return PARSE_INT_ERR;
			break;
		}
		default:
			return PARSE_TYPE_COMP;
	}
	Stack->top = Stack->top - 2;
	return PARSE_OK;
}
/**
 * Function will find efficient way to reduce two items to lesser than,
 * if they are constant it will not add anything to code (strings are exeption),
 * Code is added only if one of them is variable.  It will return PARSE_OK if everything
 * went ok, PARSE_INT_ERR if code addition went wrong, PARSE_TYPE_COMP if operands
 * are of incompatible type.
 * @param Stack stack to use
 * @param startingIndex Index to start reduction from
 * @param temporaryVariableCount Pointer to counter of temtorary variables
*/
static int SStackRuleIsLesserThan(SStack_t *Stack, int startingIndex, int *temporaryVariableCount)
{
	SStackItem_t *item1 = &(Stack->stack[startingIndex]);
	SStackItem_t *item2 = &(Stack->stack[startingIndex+2]);
	int itemsType = SStackWhatAreItems(item1, item2);
	int return_value = PARSE_OK;
	switch(itemsType)
	{
		case C_INT_INT:
		{
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (item1->data.intValue < item2->data.intValue);
			break;
		}
		case C_DOUBLE_DOUBLE:
		{
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (item1->data.doubleValue < item2->data.doubleValue);
			break;
		}
		case C_INT_DOUBLE:
		{
			double doubleValue1 = SStackGetItemDoubleValue(item1);
			double doubleValue2 = SStackGetItemDoubleValue(item2);
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (doubleValue1 < doubleValue1);
			break;
		}
		case C_STRING_STRING: case C_DOUBLE_VAR: case C_INT_VAR: case C_STRING_VAR: case C_VAR_VAR:
		{
			(*temporaryVariableCount)++;
			int uniqueOperationId = CodeGetUniqueOperation();
			return_value |= CodeAddTextToBody("\nMOVE GF@%operand1 ");
			return_value |= CodeAddSStackItem(item1);
			return_value |= CodeAddTextToBody("\nMOVE GF@%operand2 ");
			return_value |= CodeAddSStackItem(item2);
			return_value |= CodeAddTextToBody("\nTYPE GF@%operand1type GF@%operand1");
			return_value |= CodeAddTextToBody("\nTYPE GF@%operand2type GF@%operand2");
			return_value |= CodeAddTextToBody("\nJUMPIFEQ ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "compatibility");
			return_value |= CodeAddTextToBody(" GF@%operand1type GF@%operand2type");
			return_value |= CodeAddTextToBody("\nCALL $$operand1ToFloat");
			return_value |= CodeAddTextToBody("\nCALL $$operand2ToFloat");
			//LABEL $$operation[uniqueOperationId]compatibility
			return_value |= CodeAddTextToBody("\nLABEL ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "compatibility");
			return_value |= CodeAddTextToBody("\nCALL $$operandCompareCompatibility");
			return_value |= CodeAddTextToBody("\nLT ");
			return_value |= CodeAddTempVariable(*temporaryVariableCount);
			return_value |= CodeAddTextToBody(" GF@%operand1 GF@%operand2");
			if(return_value != PARSE_OK)
				return PARSE_INT_ERR;

			if(item1->dataType == STACK_STRING)
				SStackFreeString(item1->data.string);
			if(item2->dataType == STACK_STRING)
				SStackFreeString(item2->data.string);
			break;
		}
		default:
			return PARSE_TYPE_COMP;
	}
	Stack->top = Stack->top - 2;
	return PARSE_OK;
}
/**
 * Function will find efficient way to reduce two items to lesser or equal to,
 * if they are constant it will not add anything to code (strings are exeption),
 * Code is added only if one of them is variable. It will return PARSE_OK if everything
 * went ok, PARSE_INT_ERR if code addition went wrong, PARSE_TYPE_COMP if operands
 * are of incompatible type.
 * @param Stack stack to use
 * @param startingIndex Index to start reduction from
 * @param temporaryVariableCount Pointer to counter of temtorary variables
*/
static int SStackRuleIsLesserEqual(SStack_t *Stack, int startingIndex, int *temporaryVariableCount)
{
	SStackItem_t *item1 = &(Stack->stack[startingIndex]);
	SStackItem_t *item2 = &(Stack->stack[startingIndex+2]);
	int itemsType = SStackWhatAreItems(item1, item2);
	int return_value = PARSE_OK;
	switch(itemsType)
	{
		case C_INT_INT:
		{
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (item1->data.intValue <= item2->data.intValue);
			break;
		}
		case C_DOUBLE_DOUBLE:
		{
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (item1->data.doubleValue <= item2->data.doubleValue);
			break;
		}
		case C_INT_DOUBLE:
		{
			double doubleValue1 = SStackGetItemDoubleValue(item1);
			double doubleValue2 = SStackGetItemDoubleValue(item2);
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (doubleValue1 <= doubleValue1);
			break;
		}
		case C_STRING_STRING: case C_DOUBLE_VAR: case C_INT_VAR: case C_STRING_VAR: case C_VAR_VAR:
		{
			(*temporaryVariableCount)++;
			int uniqueOperationId = CodeGetUniqueOperation();
			return_value |= CodeAddTextToBody("\nMOVE GF@%operand1 ");
			return_value |= CodeAddSStackItem(item1);
			return_value |= CodeAddTextToBody("\nMOVE GF@%operand2 ");
			return_value |= CodeAddSStackItem(item2);
			return_value |= CodeAddTextToBody("\nTYPE GF@%operand1type GF@%operand1");
			return_value |= CodeAddTextToBody("\nTYPE GF@%operand2type GF@%operand2");
			return_value |= CodeAddTextToBody("\nJUMPIFEQ ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "compatibility");
			return_value |= CodeAddTextToBody(" GF@%operand1type GF@%operand2type");
			return_value |= CodeAddTextToBody("\nCALL $$operand1ToFloat");
			return_value |= CodeAddTextToBody("\nCALL $$operand2ToFloat");
			//LABEL $$operation[uniqueOperationId]compatibility
			return_value |= CodeAddTextToBody("\nLABEL ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "compatibility");
			return_value |= CodeAddTextToBody("\nCALL $$operandCompareCompatibility");
			return_value |= CodeAddTextToBody("\nGT ");
			return_value |= CodeAddTempVariable(*temporaryVariableCount);
			return_value |= CodeAddTextToBody(" GF@%operand1 GF@%operand2");
			return_value |= CodeAddTextToBody("\nNOT ");
			return_value |= CodeAddTempVariable(*temporaryVariableCount);
			return_value |= CodeAddTextToBody(" ");
			return_value |= CodeAddTempVariable(*temporaryVariableCount);

			if(return_value != PARSE_OK)
				return PARSE_INT_ERR;

			if(item1->dataType == STACK_STRING)
				SStackFreeString(item1->data.string);
			if(item2->dataType == STACK_STRING)
				SStackFreeString(item2->data.string);
			break;
		}
		default:
			return PARSE_TYPE_COMP;
	}
	Stack->top = Stack->top - 2;
	return PARSE_OK;
}
/**
 * Function will find efficient way to reduce two items to greater than,
 * if they are constant it will not add anything to code (strings are exeption),
 * Code is added only if one of them is variable.  It will return PARSE_OK if everything
 * went ok, PARSE_INT_ERR if code addition went wrong, PARSE_TYPE_COMP if operands
 * are of incompatible type.
 * @param Stack stack to use
 * @param startingIndex Index to start reduction from
 * @param temporaryVariableCount Pointer to counter of temtorary variables
*/
static int SStackRuleIsGreaterThan(SStack_t *Stack, int startingIndex, int *temporaryVariableCount)
{
	SStackItem_t *item1 = &(Stack->stack[startingIndex]);
	SStackItem_t *item2 = &(Stack->stack[startingIndex+2]);
	int itemsType = SStackWhatAreItems(item1, item2);
	int return_value = PARSE_OK;
	switch(itemsType)
	{
		case C_INT_INT:
		{
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (item1->data.intValue > item2->data.intValue);
			break;
		}
		case C_DOUBLE_DOUBLE:
		{
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (item1->data.doubleValue > item2->data.doubleValue);
			break;
		}
		case C_INT_DOUBLE:
		{
			double doubleValue1 = SStackGetItemDoubleValue(item1);
			double doubleValue2 = SStackGetItemDoubleValue(item2);
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (doubleValue1 > doubleValue1);
			break;
		}
		case C_STRING_STRING: case C_DOUBLE_VAR: case C_INT_VAR: case C_STRING_VAR: case C_VAR_VAR:
		{
			(*temporaryVariableCount)++;
			int uniqueOperationId = CodeGetUniqueOperation();
			return_value |= CodeAddTextToBody("\nMOVE GF@%operand1 ");
			return_value |= CodeAddSStackItem(item1);
			return_value |= CodeAddTextToBody("\nMOVE GF@%operand2 ");
			return_value |= CodeAddSStackItem(item2);
			return_value |= CodeAddTextToBody("\nTYPE GF@%operand1type GF@%operand1");
			return_value |= CodeAddTextToBody("\nTYPE GF@%operand2type GF@%operand2");
			return_value |= CodeAddTextToBody("\nJUMPIFEQ ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "compatibility");
			return_value |= CodeAddTextToBody(" GF@%operand1type GF@%operand2type");
			return_value |= CodeAddTextToBody("\nCALL $$operand1ToFloat");
			return_value |= CodeAddTextToBody("\nCALL $$operand2ToFloat");
			//LABEL $$operation[uniqueOperationId]compatibility
			return_value |= CodeAddTextToBody("\nLABEL ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "compatibility");
			return_value |= CodeAddTextToBody("\nCALL $$operandCompareCompatibility");
			return_value |= CodeAddTextToBody("\nGT ");
			return_value |= CodeAddTempVariable(*temporaryVariableCount);
			return_value |= CodeAddTextToBody(" GF@%operand1 GF@%operand2");
			if(item1->dataType == STACK_STRING)
				SStackFreeString(item1->data.string);
			if(item2->dataType == STACK_STRING)
				SStackFreeString(item2->data.string);
			break;
		}
		default:
			return PARSE_TYPE_COMP;
	}
	Stack->top = Stack->top - 2;
	return PARSE_OK;
}
/**
 * Function will find efficient way to reduce two items to greater or equal to,
 * if they are constant it will not add anything to code (strings are exeption),
 * Code is added only if one of them is variable. It will return PARSE_OK if everything
 * went ok, PARSE_INT_ERR if code addition went wrong, PARSE_TYPE_COMP if operands
 * are of incompatible type.
 * @param Stack stack to use
 * @param startingIndex Index to start reduction from
 * @param temporaryVariableCount Pointer to counter of temtorary variables
*/
static int SStackRuleIsGreaterEqual(SStack_t *Stack, int startingIndex, int *temporaryVariableCount)
{
	SStackItem_t *item1 = &(Stack->stack[startingIndex]);
	SStackItem_t *item2 = &(Stack->stack[startingIndex+2]);
	int itemsType = SStackWhatAreItems(item1, item2);
	int return_value = PARSE_OK;
	switch(itemsType)
	{
		case C_INT_INT:
		{
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (item1->data.intValue >= item2->data.intValue);
			break;
		}
		case C_DOUBLE_DOUBLE:
		{
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (item1->data.doubleValue >= item2->data.doubleValue);
			break;
		}
		case C_INT_DOUBLE:
		{
			double doubleValue1 = SStackGetItemDoubleValue(item1);
			double doubleValue2 = SStackGetItemDoubleValue(item2);
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (doubleValue1 >= doubleValue1);
			break;
		}
		case C_STRING_STRING: case C_DOUBLE_VAR: case C_INT_VAR: case C_STRING_VAR: case C_VAR_VAR:
		{
			(*temporaryVariableCount)++;
			int uniqueOperationId = CodeGetUniqueOperation();
			return_value |= CodeAddTextToBody("\nMOVE GF@%operand1 ");
			return_value |= CodeAddSStackItem(item1);
			return_value |= CodeAddTextToBody("\nMOVE GF@%operand2 ");
			return_value |= CodeAddSStackItem(item2);
			return_value |= CodeAddTextToBody("\nTYPE GF@%operand1type GF@%operand1");
			return_value |= CodeAddTextToBody("\nTYPE GF@%operand2type GF@%operand2");
			return_value |= CodeAddTextToBody("\nJUMPIFEQ ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "compatibility");
			return_value |= CodeAddTextToBody(" GF@%operand1type GF@%operand2type");
			return_value |= CodeAddTextToBody("\nCALL $$operand1ToFloat");
			return_value |= CodeAddTextToBody("\nCALL $$operand2ToFloat");
			//LABEL $$operation[uniqueOperationId]compatibility
			return_value |= CodeAddTextToBody("\nLABEL ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "compatibility");
			return_value |= CodeAddTextToBody("\nCALL $$operandCompareCompatibility");
			return_value |= CodeAddTextToBody("\nLT ");
			return_value |= CodeAddTempVariable(*temporaryVariableCount);
			return_value |= CodeAddTextToBody(" GF@%operand1 GF@%operand2");
			return_value |= CodeAddTextToBody("\nNOT ");
			return_value |= CodeAddTempVariable(*temporaryVariableCount);
			return_value |= CodeAddTextToBody(" ");
			return_value |= CodeAddTempVariable(*temporaryVariableCount);
			
			if(item1->dataType == STACK_STRING)
				SStackFreeString(item1->data.string);
			if(item2->dataType == STACK_STRING)
				SStackFreeString(item2->data.string);
			break;
		}
		default:
			return PARSE_TYPE_COMP;
	}
	Stack->top = Stack->top - 2;
	return PARSE_OK;
}
/**
 * Function will find efficient way to reduce two items to equal to,
 * if they are constant it will not add anything to code (strings are exeption),
 * Code is added only if one of them is variable. It will return PARSE_OK if everything
 * went ok, PARSE_INT_ERR if code addition went wrong, PARSE_TYPE_COMP if operands
 * are of incompatible type.
 * @param Stack stack to use
 * @param startingIndex Index to start reduction from
 * @param temporaryVariableCount Pointer to counter of temtorary variables
*/
static int SStackRuleIsEqual(SStack_t *Stack, int startingIndex, int *temporaryVariableCount)
{
	SStackItem_t *item1 = &(Stack->stack[startingIndex]);
	SStackItem_t *item2 = &(Stack->stack[startingIndex+2]);
	int itemsType = SStackWhatAreItems(item1, item2);
	int return_value = PARSE_OK;
	switch(itemsType)
	{
		case C_INT_INT:
		{
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (item1->data.intValue == item2->data.intValue);
			break;
		}
		case C_DOUBLE_DOUBLE:
		{
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (item1->data.doubleValue == item2->data.doubleValue);
			break;
		}
		case C_INT_DOUBLE:
		{
			double doubleValue1 = SStackGetItemDoubleValue(item1);
			double doubleValue2 = SStackGetItemDoubleValue(item2);
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (doubleValue1 == doubleValue1);
			break;
		}
		case C_NIL_NIL:
		{
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = true;
			break;
		}
		case C_NIL_DOUBLE: case C_NIL_INT: case C_NIL_STRING:
		{
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = false;
			break;
		}
		case C_STRING_STRING: case C_DOUBLE_VAR: case C_STRING_VAR: case C_NIL_VAR: case C_INT_VAR: case C_VAR_VAR:
		{
			(*temporaryVariableCount)++;
			int uniqueOperationId = CodeGetUniqueOperation();
			return_value |= CodeAddTextToBody("\nMOVE GF@%operand1 ");
			return_value |= CodeAddSStackItem(item1);
			return_value |= CodeAddTextToBody("\nMOVE GF@%operand2 ");
			return_value |= CodeAddSStackItem(item2);
			return_value |= CodeAddTextToBody("\nTYPE GF@%operand1type GF@%operand1");
			return_value |= CodeAddTextToBody("\nTYPE GF@%operand2type GF@%operand2");
			return_value |= CodeAddTextToBody("\nJUMPIFEQ ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "");
			return_value |= CodeAddTextToBody(" GF@%operand1type GF@%operand2type");

			return_value |= CodeAddTextToBody("\nMOVE ");
			return_value |= CodeAddTempVariable(*temporaryVariableCount);
			return_value |= CodeAddTextToBody(" ");
			return_value |= CodeAddBool(false);
			return_value |= CodeAddTextToBody("\nJUMP ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "end");

			//LABEL $$operation[uniqueOperationId]compatibility
			return_value |= CodeAddTextToBody("\nLABEL ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "");
			return_value |= CodeAddTextToBody("\nEQ ");
			return_value |= CodeAddTempVariable(*temporaryVariableCount);
			return_value |= CodeAddTextToBody(" GF@%operand1 GF@%operand2");
			return_value |= CodeAddTextToBody("\nLABEL ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "end");
			break;
		}
		default:
			return PARSE_TYPE_COMP;
	}
	Stack->top = Stack->top - 2;
	return PARSE_OK;
}
/**
 * Function will find efficient way to reduce two items to equal,
 * if they are constant it will not add anything to code (strings are exeption),
 * Code is added only if one of them is variable. It will return PARSE_OK if everything
 * went ok, PARSE_INT_ERR if code addition went wrong, PARSE_TYPE_COMP if operands
 * are of incompatible type.
 * @param Stack stack to use
 * @param startingIndex Index to start reduction from
 * @param temporaryVariableCount Pointer to counter of temtorary variables
*/
static int SStackRuleIsNotEqual(SStack_t *Stack, int startingIndex, int *temporaryVariableCount)
{
	SStackItem_t *item1 = &(Stack->stack[startingIndex]);
	SStackItem_t *item2 = &(Stack->stack[startingIndex+2]);
	int itemsType = SStackWhatAreItems(item1, item2);
	int return_value = PARSE_OK;
	switch(itemsType)
	{
		case C_INT_INT:
		{
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (item1->data.intValue != item2->data.intValue);
			break;
		}
		case C_DOUBLE_DOUBLE:
		{
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (item1->data.doubleValue != item2->data.doubleValue);
			break;
		}
		case C_INT_DOUBLE:
		{
			double doubleValue1 = SStackGetItemDoubleValue(item1);
			double doubleValue2 = SStackGetItemDoubleValue(item2);
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (doubleValue1 != doubleValue1);
			break;
		}
		case C_STRING_STRING:
		{
			char *str1 = item1->data.string;
			char *str2 = item1->data.string;
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (strcmp(item1->data.string, item2->data.string) != 0);
			SStackFreeString(str1);
			SStackFreeString(str2);
			break;
		}
		case C_NIL_NIL:
		{
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = false;
			break;
		}
		case C_NIL_DOUBLE: case C_NIL_INT: case C_NIL_STRING:
		{
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = true;
			break;
		}
		case C_DOUBLE_VAR: case C_STRING_VAR: case C_NIL_VAR: case C_INT_VAR: case C_VAR_VAR:
		{
			(*temporaryVariableCount)++;
			int uniqueOperationId = CodeGetUniqueOperation();
			return_value |= CodeAddTextToBody("\nMOVE GF@%operand1 ");
			return_value |= CodeAddSStackItem(item1);
			return_value |= CodeAddTextToBody("\nMOVE GF@%operand2 ");
			return_value |= CodeAddSStackItem(item2);
			return_value |= CodeAddTextToBody("\nTYPE GF@%operand1type GF@%operand1");
			return_value |= CodeAddTextToBody("\nTYPE GF@%operand2type GF@%operand2");
			return_value |= CodeAddTextToBody("\nJUMPIFEQ ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "");
			return_value |= CodeAddTextToBody(" GF@%operand1type GF@%operand2type");

			return_value |= CodeAddTextToBody("\nMOVE ");
			return_value |= CodeAddTempVariable(*temporaryVariableCount);
			return_value |= CodeAddTextToBody(" ");
			return_value |= CodeAddBool(true);
			return_value |= CodeAddTextToBody("\nJUMP ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "end");

			//LABEL $$operation[uniqueOperationId]compatibility
			return_value |= CodeAddTextToBody("\nLABEL ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "");
			return_value |= CodeAddTextToBody("\nEQ ");
			return_value |= CodeAddTempVariable(*temporaryVariableCount);
			return_value |= CodeAddTextToBody(" GF@%operand1 GF@%operand2");
			return_value |= CodeAddTextToBody("\nNOT ");
			return_value |= CodeAddTempVariable(*temporaryVariableCount);
			return_value |= CodeAddTextToBody(" ");
			return_value |= CodeAddTempVariable(*temporaryVariableCount);
			return_value |= CodeAddTextToBody("\nLABEL ");
			return_value |= CodeAddLabelName("$$operation", uniqueOperationId, "end");
			break;
		}
		default:
			return PARSE_TYPE_COMP;
	}
	Stack->top = Stack->top - 2;
	return PARSE_OK;
}
/**
 * Function will reduce terminal term to nonterminal
 * It will always return PARSE_OK
 * @param Stack stack to use
 * @param startingIndex Index to start reduction from
*/
static int SStackRuleTerm(SStack_t *Stack, int startingIndex)
{
	Stack->stack[startingIndex].isNonterminal = true;
	return PARSE_OK;
}
/**
 * Function will reduce brackets
 * It will always return PARSE_OK
 * @param Stack stack to use
 * @param startingIndex Index to start reduction from
*/
static int SStackRuleBrackets(SStack_t *Stack, int startingIndex)
{
	Stack->stack[startingIndex] = Stack->stack[startingIndex+1];
	Stack->top = Stack->top - 2;
	return PARSE_OK;
}
/**
 * Function will find correct rule to use for reduction depending on operation
 * it will return PARSE_TYPE_COMP or PARSE_INT_ERR or PARSE_OTHER in case of
 * error. If rule for reduction exists function will call one of the *Rule*
 * function and it will inharite its return_value.
 * @param Stack stack to use
 * @param startingIndex Index to start reduction from
 * @param temporaryVariableCount pointer to temporary variable counter
*/
static int SStackUseRule(SStack_t *Stack, int startingIndex, int *temporaryVariableCount)
{
	if(startingIndex+2 == Stack->top && 
	   Stack->stack[startingIndex].isNonterminal && 
	   Stack->stack[startingIndex+1].isNonterminal == false && 
	   Stack->stack[startingIndex+2].isNonterminal)
	{//RULE E -> E [+, -, *, /, <=, >=, <, >, !=, ==] E
		switch(Stack->stack[startingIndex+1].terminal)
		{
			case TERMINAL_COMPARISON:
			case TERMINAL_MULT_DIVIDE:
			case TERMINAL_PLUS_MINUS:
			case TERMINAL_NOT_EQUAL:
			{
				//Create return variable to reduce to

				switch(Stack->stack[startingIndex+1].data.operation)
				{
					case TO_ADD:
						return SStackRuleAddition(Stack, startingIndex, temporaryVariableCount);
					case TO_SUBTRACT:
						return SStackRuleSubtraction(Stack, startingIndex, temporaryVariableCount);
					case TO_MULTIPLY:
						return SStackRuleMultiplication(Stack, startingIndex, temporaryVariableCount);
					case TO_DIVIDE:
						return SStackRuleDivision(Stack, startingIndex, temporaryVariableCount);
					case TO_LESSER_THAN:
						return SStackRuleIsLesserThan(Stack, startingIndex, temporaryVariableCount);
					case TO_LESSER_EQUAL_THAN:
						return SStackRuleIsLesserEqual(Stack, startingIndex, temporaryVariableCount);
					case TO_GREATER_THAN:
						return SStackRuleIsGreaterThan(Stack, startingIndex, temporaryVariableCount);
					case TO_GREATER_EQUAL_THAN:
						return SStackRuleIsGreaterEqual(Stack, startingIndex, temporaryVariableCount);
					case TO_EQUAL_TO:
						return SStackRuleIsEqual(Stack, startingIndex, temporaryVariableCount);
					case TO_NOT_EQUAL_TO:
						return SStackRuleIsNotEqual(Stack, startingIndex, temporaryVariableCount);
					default:
						return PARSE_INT_ERR;
				}
				break;
			}
			default:
			{
				return PARSE_SYN_ERR;
				break;
			}
		}
	}
	else if (
	   startingIndex+2 == Stack->top &&
	   Stack->stack[startingIndex].isNonterminal == false &&
	   Stack->stack[startingIndex].terminal == TERMINAL_LEFT_BRACKET &&
	   Stack->stack[startingIndex+1].isNonterminal == true &&
	   Stack->stack[startingIndex+2].isNonterminal == false &&
	   Stack->stack[startingIndex+2].terminal == TERMINAL_RIGHT_BRACKET)
	{//Rule E -> ( E )
		return SStackRuleBrackets(Stack, startingIndex);
	}
	else if (
	   startingIndex == Stack->top &&
	   Stack->stack[startingIndex].isNonterminal == false &&
	   Stack->stack[startingIndex].terminal == TERMINAL_TERM)
	{//Rule E -> [id, int, float, string, nil, bool]
		return SStackRuleTerm(Stack, startingIndex);
	}
	else
		return PARSE_SYN_ERR;
}
int SStackReduceByRule(SStack_t *Stack, int *temporaryVariableCount)
{
	int topLTIndex = SStackTopLTIndex(Stack);
	SStackItem_t *topLT = SStackTopLT(Stack);
	int startingIndex = topLTIndex + 1;
	topLT->isLessThan = 0;
	return SStackUseRule(Stack, startingIndex, temporaryVariableCount);
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
		{ PRE_LT, PRE_LT, PRE_NT, PRE_NT, PRE_LT, PRE_GT, PRE_LT, PRE_GT},  // <= >= < >
		{ PRE_LT, PRE_LT, PRE_NT, PRE_NT, PRE_LT, PRE_GT, PRE_LT, PRE_GT},  // != ==
		{ PRE_LT, PRE_LT, PRE_LT, PRE_LT, PRE_LT, PRE_EQ, PRE_LT, PRE_NT},  // (
		{ PRE_GT, PRE_GT, PRE_GT, PRE_GT, PRE_NT, PRE_GT, PRE_NT, PRE_GT},  // )
		{ PRE_GT, PRE_GT, PRE_GT, PRE_GT, PRE_NT, PRE_GT, PRE_NT, PRE_GT},  // id int float sting nil
		{ PRE_LT, PRE_LT, PRE_LT, PRE_LT, PRE_LT, PRE_NT, PRE_LT, PRE_GT},  // $
	};
	SStackItem_t *topTerminal= SStackTopTerminal(stack);

	int stackTopTableIndex = topTerminal->terminal;
	int readItemTableIndex = stackItem->terminal;
	ExpressionPrecedence_t predecence = precedenceTable[stackTopTableIndex][readItemTableIndex];

	return predecence;
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
