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

int SStackTopTerminalIndex(SStack_t *Stack)
{
	for(int i = Stack->top; i >= 0; i--)
	{
		if(Stack->stack[i].isNonterminal == false)
			return i;
	}
	return -1;
}

SStackItem_t *SStackTopTerminal(SStack_t *Stack)
{
	int topTerminalIndex = SStackTopTerminalIndex(Stack);
	if(topTerminalIndex != -1)
		return &(Stack->stack[topTerminalIndex]); 
	return NULL;
}

int SStackTopLTIndex(SStack_t *Stack)
{
	for(int i = Stack->top; i >= 0; i--)
	{
		if(Stack->stack[i].isLessThan == true)
			return i;
	}
	return -1;
}

SStackItem_t *SStackTopLT(SStack_t *Stack)
{
	int topLTIndex = SStackTopLTIndex(Stack);
	if(topLTIndex != -1)
		return &(Stack->stack[topLTIndex]); 
	return NULL;
}

int SStackTopTerminalAddLT(SStack_t *Stack)
{
	SStackItem_t *topTerminal = SStackTopTerminal(Stack);
	if(topTerminal == NULL)
		return PARSE_OTHER;
	topTerminal->isLessThan = true;
	return PARSE_OK;
}


typedef enum {
	C_NIL_NIL, C_INT_INT, C_DOUBLE_DOUBLE, C_STRING_STRING, C_VAR_VAR,
	C_NIL_INT, C_NIL_DOUBLE, C_NIL_STRING, C_NIL_VAR,
	C_INT_DOUBLE, C_INT_STRING, C_INT_VAR,
	C_DOUBLE_STRING, C_DOUBLE_VAR,
	C_STRING_VAR,
	C_OTHER
} OperandsCombinations_t;

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
static double SStackGetItemDoubleValue(SStackItem_t *item)
{
	if(item->dataType == STACK_INT)
		return (double)item->data.intValue;
	return item->data.doubleValue;
}

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
			if(item2->dataType == STACK_STRING)
				SStackFreeString(item2->data.string);
			if(item1->dataType == STACK_STRING)
				SStackFreeString(item1->data.string);
			break;
		}
		default:
			return PARSE_TYPE_COMP;
	}
	Stack->top = Stack->top - 2;
	return return_value;
}
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

			CodeAddTextToBody("\nMOVE GF@%operand1 ");
			CodeAddSStackItem(item1);
			CodeAddTextToBody("\nMOVE GF@%operand2 ");
			CodeAddSStackItem(item2);
			CodeAddTextToBody("\nTYPE GF@%operand1type GF@%operand1");
			CodeAddTextToBody("\nTYPE GF@%operand2type GF@%operand2");
			CodeAddTextToBody("\nJUMPIFEQ ");
			CodeAddLabelName("$$operation", uniqueOperationId, "compatibility");
			CodeAddTextToBody(" GF@%operand1type GF@%operand2type");

			CodeAddTextToBody("\nCALL $$operand1ToFloat");
			CodeAddTextToBody("\nCALL $$operand2ToFloat");

			CodeAddTextToBody("\nLABEL ");
			CodeAddLabelName("$$operation", uniqueOperationId, "compatibility");
			CodeAddTextToBody("\nCALL $$operandNumberCompatibility");
			CodeAddTextToBody("\nSUB ");
			CodeAddTempVariable(*temporaryVariableCount);
			CodeAddTextToBody(" GF@%operand1 GF@%operand2");

			if(item1->dataType == STACK_STRING)
				SStackFreeString(item1->data.string);
			item1->dataType = STACK_SYMBOL;
			break;
		}
		default:
			return PARSE_TYPE_COMP;
	}
	Stack->top = Stack->top - 2;
	return PARSE_OK;
}
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

			CodeAddTextToBody("\nMOVE GF@%operand1 ");
			CodeAddSStackItem(item1);
			CodeAddTextToBody("\nMOVE GF@%operand2 ");
			CodeAddSStackItem(item2);
			CodeAddTextToBody("\nTYPE GF@%operand1type GF@%operand1");
			CodeAddTextToBody("\nTYPE GF@%operand2type GF@%operand2");
			CodeAddTextToBody("\nJUMPIFEQ ");
			CodeAddLabelName("$$operation", uniqueOperationId, "compatibility");
			CodeAddTextToBody(" GF@%operand1type GF@%operand2type");

			CodeAddTextToBody("\nCALL $$operand1ToFloat");
			CodeAddTextToBody("\nCALL $$operand2ToFloat");

			CodeAddTextToBody("\nLABEL ");
			CodeAddLabelName("$$operation", uniqueOperationId, "compatibility");
			CodeAddTextToBody("\nCALL $$operandNumberCompatibility");
			CodeAddTextToBody("\nMUL ");
			CodeAddTempVariable(*temporaryVariableCount);
			CodeAddTextToBody(" GF@%operand1 GF@%operand2");

			if(item1->dataType == STACK_STRING)
				SStackFreeString(item1->data.string);
			item1->dataType = STACK_SYMBOL;
			break;
		}
		default:
			return PARSE_TYPE_COMP;
	}
	Stack->top = Stack->top - 2;
	return PARSE_OK;
}
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
			CodeAddTextToBody("\nMOVE GF@%operand1 ");
			CodeAddSStackItem(item1);
			CodeAddTextToBody("\nMOVE GF@%operand2 ");
			CodeAddSStackItem(item2);
			CodeAddTextToBody("\nTYPE GF@%operand1type GF@%operand1");
			CodeAddTextToBody("\nTYPE GF@%operand2type GF@%operand2");
			CodeAddTextToBody("\nJUMPIFNEQ ");
			CodeAddLabelName("$$operation", uniqueOperationId, "conevert");
			CodeAddTextToBody(" GF@%operand1type string@int");
			CodeAddTextToBody("\nJUMPIFNEQ ");
			CodeAddLabelName("$$operation", uniqueOperationId, "conevert");
			CodeAddTextToBody(" GF@%operand2type string@int");
			CodeAddTextToBody("\nIDIV ");
			CodeAddTempVariable(*temporaryVariableCount);
			CodeAddTextToBody(" GF@%operand1 GF@%operand2");
			CodeAddTextToBody("\nJUMP ");
			CodeAddLabelName("$$operation", uniqueOperationId, "end");
			CodeAddTextToBody("\nLABEL ");
			CodeAddLabelName("$$operation", uniqueOperationId, "conevert");
			CodeAddTextToBody("\nCALL $$operand1ToFloat");
			CodeAddTextToBody("\nCALL $$operand2ToFloat");
			CodeAddTextToBody("\nCALL $$operandNumberCompatibility");
			CodeAddTextToBody("\nDIV ");
			CodeAddTempVariable(*temporaryVariableCount);
			CodeAddTextToBody(" GF@%operand1 GF@%operand2");
			CodeAddTextToBody("\nLABEL ");
			CodeAddLabelName("$$operation", uniqueOperationId, "end");
			if(item1->dataType == STACK_STRING)
				SStackFreeString(item1->data.string);
			item1->dataType = STACK_SYMBOL;
			break;
		}
		default:
			return PARSE_TYPE_COMP;
	}
	Stack->top = Stack->top - 2;
	return PARSE_OK;
}
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
		case C_STRING_STRING:
		{
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (strcmp(item1->data.string, item2->data.string) < 0);
			SStackFreeString(item1->data.string);
			SStackFreeString(item2->data.string);
			break;
		}
		case C_DOUBLE_VAR: case C_INT_VAR: case C_STRING_VAR: case C_VAR_VAR:
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
			break;
		}
		default:
			return PARSE_TYPE_COMP;
	}
	Stack->top = Stack->top - 2;
	return PARSE_OK;
}
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
		case C_STRING_STRING:
		{
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (strcmp(item1->data.string, item2->data.string) <= 0);
			SStackFreeString(item1->data.string);
			SStackFreeString(item2->data.string);
			break;
		}
		case C_DOUBLE_VAR: case C_INT_VAR: case C_STRING_VAR: case C_VAR_VAR:
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
			break;
		}
		default:
			return PARSE_TYPE_COMP;
	}
	Stack->top = Stack->top - 2;
	return PARSE_OK;
}
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
		case C_STRING_STRING:
		{
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (strcmp(item1->data.string, item2->data.string) > 0);
			SStackFreeString(item1->data.string);
			SStackFreeString(item2->data.string);
			break;
		}
		case C_DOUBLE_VAR: case C_INT_VAR: case C_STRING_VAR: case C_VAR_VAR:
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
			break;
		}
		default:
			return PARSE_TYPE_COMP;
	}
	Stack->top = Stack->top - 2;
	return PARSE_OK;
}
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
		case C_STRING_STRING:
		{
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (strcmp(item1->data.string, item2->data.string) >= 0);
			SStackFreeString(item1->data.string);
			SStackFreeString(item2->data.string);
			break;
		}
		case C_DOUBLE_VAR: case C_INT_VAR: case C_STRING_VAR: case C_VAR_VAR:
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
			break;
		}
		default:
			return PARSE_TYPE_COMP;
	}
	Stack->top = Stack->top - 2;
	return PARSE_OK;
}
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
		case C_STRING_STRING:
		{
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (strcmp(item1->data.string, item2->data.string) == 0);
			SStackFreeString(item1->data.string);
			SStackFreeString(item2->data.string);
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
			item1->dataType = STACK_BOOL;
			item1->data.boolValue = (strcmp(item1->data.string, item2->data.string) != 0);
			SStackFreeString(item1->data.string);
			SStackFreeString(item2->data.string);
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
static int SStackRuleTerm(SStack_t *Stack, int startingIndex)
{
	Stack->stack[startingIndex].isNonterminal = true;
	return PARSE_OK;
}
static int SStackRuleBrackets(SStack_t *Stack, int startingIndex)
{
	Stack->stack[startingIndex] = Stack->stack[startingIndex+1];
	Stack->top = Stack->top - 2;
	return PARSE_OK;
}
int SStackUseRule(SStack_t *Stack, int startingIndex, int *temporaryVariableCount)
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
