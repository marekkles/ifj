/**
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                     *
 *                  Projekt IFJ                        *
 *                                                     *
 * Implementace překladače imperativního jazyka IFJ18  *
 *                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  
                       Súbor:
                      parser.c

                       Popis:
        Implementacia analyzy vyrazov, tela programu,
                 generovanie kodu

                      Autori:
               Marek Vaško (xvasko16)
              Handzuš Daniel (xhandz01)
                Alexaj Adam (xalexa07)
*/

#include "parser.h"

//extern FILE *input;
//extern FILE *output;

int in_function = 0;
int in_while = 0;
static int function_definition = 0;
static int parameter_count = 0;

enum {
    FUN_INPUTS,
    FUN_INPUTI,
    FUN_INPUTF,
    FUN_PRINT
};

SymTable_t *symtable;


static int TokenExpect(Token_t *token, TokenType_t type);
static int TokenExpectOperation(Token_t *token, TokenOperationType_t operation);
static int TokenExpectKeyword(Token_t *token, TokenKewordType_t keyword);
static int GetTokenParser(DStr_t **dstr, Token_t *token);
static int GetTokenExpect(DStr_t **dstr, Token_t *token, TokenType_t type);
static int GetTokenExpectOperation(DStr_t **dstr, Token_t *token, TokenOperationType_t operation);
static int GetTokenExpectKeyword(DStr_t **dstr, Token_t *token, TokenKewordType_t keyword);
static int Program(DStr_t **dstr, Token_t *token);
static int ParameterList(DStr_t **dstr, Token_t *token);
static int ParameterOther(DStr_t **dstr, Token_t *token);
static int Term(DStr_t **dstr, Token_t *token);
static int TermList(DStr_t **dstr, Token_t *token);
static int TermOther(DStr_t **dstr, Token_t *token);
static int CommandEnd(DStr_t **dstr, Token_t *token);
static int Command(DStr_t **dstr, Token_t *token);
static int Expression(DStr_t **dstr, Token_t *token, DStr_t **nextDstr,Token_t *nextToken, SymTableItem_t *returnVariable);

static int SymTableAddInternalFunctions(SymTable_t *symtable)
{
    SymTableItem_t *function;
    function = SymTableAddFunction(symtable, "substr", 3, true);
    if(function == NULL)
        return PARSE_INT_ERR;
    function = SymTableAddFunction(symtable, "ord", 2, true);
    if(function == NULL)
        return PARSE_INT_ERR;
    function = SymTableAddFunction(symtable, "chr", 1, true);
    if(function == NULL)
        return PARSE_INT_ERR;
    function = SymTableAddFunction(symtable, "length", 1, true);
    if(function == NULL)
        return PARSE_INT_ERR;
}

static int ExpectTerm(DStr_t **dstr, Token_t *token)
{
    int return_value = PARSE_OK;
    //DebugFPuts("      In: <ExpectTerm> <= ", output);
    //DebugFPrintToken(output, token, *dstr);
    if(TokenExpect(token, T_IDENTIFIER) == PARSE_OK)
        return PARSE_OK;
    else if(TokenExpect(token, T_INTEGER) == PARSE_OK)
        return PARSE_OK;
    else if(TokenExpect(token, T_DOUBLE) == PARSE_OK)
        return PARSE_OK;
    else if(TokenExpect(token, T_STRING) == PARSE_OK)
        return PARSE_OK;
    else if(TokenExpectKeyword(token, TK_NIL) == PARSE_OK)
        return PARSE_OK;
    else
        return PARSE_PARAM_COUNT;
}

static int IsInternalInlineFunction(DStr_t **dstr, Token_t *token)
{
    if(token->type != T_IDENTIFIER)
        return -1;
    if(strcmp("inputs", DStrStr(*dstr)) == 0)
        return FUN_INPUTS;
    else if (strcmp("inputi", DStrStr(*dstr)) == 0)
        return FUN_INPUTI;
    else if (strcmp("inputf", DStrStr(*dstr)) == 0)
        return FUN_INPUTF;
    else if (strcmp("print", DStrStr(*dstr)) == 0)
        return FUN_PRINT;
    else
        return -1;
}

static int PrintTerm(DStr_t **dstr, Token_t *token)
{
    int return_value = PARSE_OK;
    //DebugFPuts("      In: <PrintTerm> <= ", output);
    //DebugFPrintToken(output, token, *dstr);
    if((return_value = CodeAddInstruction(WRITE)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;

    if(TokenExpect(token, T_IDENTIFIER) == PARSE_OK)
    {
        SymTableItem_t *foundItem = SymTableFindItem(symtable, DStrStr(*dstr));
        if(foundItem == NULL)
            return PARSE_UNDEF_VAR;
        if((return_value = CodeAddVariable(DStrStr(*dstr))) != PARSE_OK)
            return return_value;
        return PARSE_OK; 
    }
    else if(TokenExpect(token, T_INTEGER) == PARSE_OK)
    {
        if((return_value = CodeAddInt(token->intValue)) != PARSE_OK)
            return return_value;
        return PARSE_OK;
    }
    else if(TokenExpect(token, T_DOUBLE) == PARSE_OK)
    {
        if((return_value = CodeAddDouble(token->doubleValue)) != PARSE_OK)
            return return_value;
        return PARSE_OK;
    }
    else if(TokenExpect(token, T_STRING) == PARSE_OK)
    {
        if((return_value = CodeAddString(DStrStr(*dstr))) != PARSE_OK)
            return return_value;
        return PARSE_OK;
    }
    else if(TokenExpectKeyword(token, TK_NIL) == PARSE_OK)
    {
        if((return_value = CodeAddNil()) != PARSE_OK)
            return return_value;
        return PARSE_OK;
    }
    else
        return PARSE_SYN_ERR;
}

static int PrintTermOther(DStr_t **dstr, Token_t *token)
{
    int return_value = PARSE_OK;
    //DebugFPuts("      In: <PrintTermOther> <= ", output);
    //DebugFPrintToken(output, token, *dstr);
    if((return_value = TokenExpectOperation(token, TO_COMMA)) == PARSE_OK)
    {
        parameter_count++;
        //DebugFPuts("      In: <PrintTermOther> -> , \n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        if((return_value = PrintTerm(dstr, token)) != PARSE_OK)
            return return_value;
        //DebugFPuts("      In: <PrintTermOther> -> , <PrintTerm>\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        
        return PrintTermOther(dstr, token);
    }
    else
        return PARSE_OK;
}

static int PrintTermList(DStr_t **dstr, Token_t *token)
{
    int return_value = PARSE_OK;
    //DebugFPuts("    In: <PrintTermList> <= ", output);
    //DebugFPrintToken(output, token, *dstr);
    parameter_count = 0;
    if((return_value = PrintTerm(dstr, token)) == PARSE_OK)
    {
        parameter_count++;
        //DebugFPuts("    In: <PrintTermList> -> <PrintTerm>\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        return PrintTermOther(dstr, token);
    }
    else
        return PARSE_OK;
} 

static int ParseInternalInlineFunction(int iternalInlineFunctionNumber, SymTableItem_t *returnVariable, DStr_t ** dstr, Token_t * token)
{
    int return_value = PARSE_OK;
    
    int uses_brackets = 0;
    if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
        return return_value;
    if(TokenExpectOperation(token, TO_LBRACKET) == PARSE_OK)
    {
        uses_brackets = 1;
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
    }

    switch(iternalInlineFunctionNumber) {
        case FUN_INPUTS:
        {
            if((return_value = ExpectTerm(dstr, token)) == PARSE_OK)
                return PARSE_PARAM_COUNT;
            if((return_value = CodeAddInstruction(READ)) != PARSE_OK)
                return return_value;
            if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
                return return_value;
            if(returnVariable != NULL)
            {
                if((return_value = CodeAddVariable(returnVariable->key)) != PARSE_OK)
                    return return_value;
            }
            else
            {
                if((return_value = CodeAddTextToBody("GF@%void")) != PARSE_OK)
                    return return_value;
            }
            if((return_value = CodeAddTextToBody(" string")) != PARSE_OK)
                return return_value;
            break;
        }
        case FUN_INPUTI:
        {
            if((return_value = ExpectTerm(dstr, token)) == PARSE_OK)
                return PARSE_PARAM_COUNT;
            if((return_value = CodeAddInstruction(READ)) != PARSE_OK)
                return return_value;
            if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
                return return_value;
            if(returnVariable != NULL)
            {
                if((return_value = CodeAddVariable(returnVariable->key)) != PARSE_OK)
                    return return_value;
            }
            else
            {
                if((return_value = CodeAddTextToBody("GF@%void")) != PARSE_OK)
                    return return_value;
            }
            if((return_value = CodeAddTextToBody(" int")) != PARSE_OK)
                return return_value;
            break;
        }
        case FUN_INPUTF:
        {
            if((return_value = ExpectTerm(dstr, token)) == PARSE_OK)
                return PARSE_PARAM_COUNT;
            if((return_value = CodeAddInstruction(READ)) != PARSE_OK)
                return return_value;
            if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
                return return_value;
            if(returnVariable != NULL)
            {
                if((return_value = CodeAddVariable(returnVariable->key)) != PARSE_OK)
                    return return_value;
            }
            else
            {
                if((return_value = CodeAddTextToBody("GF@%void")) != PARSE_OK)
                    return return_value;
            }
            if((return_value = CodeAddTextToBody(" float")) != PARSE_OK)
                return return_value;
            break;
        }
        case FUN_PRINT:
        {
            if((return_value = PrintTermList(dstr, token)) != PARSE_OK)
                return return_value;
            if(returnVariable != NULL)
                if((return_value = CodeMoveNil(returnVariable->key)) != PARSE_OK)
                    return return_value;
            
            break;
        }
    }
    if(uses_brackets == 1)
    {
        if((return_value = TokenExpectOperation(token, TO_RBRACKET)) != PARSE_OK)
            return return_value;
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
    }
    
    return CommandEnd(dstr, token);
}

static SymTableItem_t * IsFunctionInSymtable(char *key)
{
    SymTableItem_t *foundItem;
    if((foundItem = SymTableFindItem(symtable, key)) != NULL)
        if(foundItem->type == SYM_FUNCTION)
            return foundItem;
    return NULL;
}

static SymTableItem_t * IsVarInSymtable(char *key)
{
    SymTableItem_t *foundItem;
    if((foundItem = SymTableFindItem(symtable, key)) != NULL)
        if(foundItem->type == SYM_VARIABLE)
            return foundItem;
    return NULL;
}

static int TokenExpect(Token_t *token, TokenType_t type)
{
    if(token->type == type)
        return PARSE_OK;
    else
        return PARSE_SYN_ERR;
}

static int TokenExpectOperation(Token_t *token, TokenOperationType_t operation)
{
    if(TokenExpect(token, T_OPERATION) == PARSE_OK && token->operationType == operation)
        return PARSE_OK;
    return PARSE_SYN_ERR;
}

static int TokenExpectKeyword(Token_t *token, TokenKewordType_t keyword)
{
    if(TokenExpect(token, T_KEYWORD) == PARSE_OK && token->keywordType == keyword)
        return PARSE_OK;
    return PARSE_SYN_ERR;
}

static int GetTokenParser(DStr_t **dstr, Token_t *token)
{
    int GetToken_return = GetToken(dstr, token);
    if(GetToken_return == GET_TOKEN_INT_ERR)
        return PARSE_INT_ERR;
    else if(GetToken_return == GET_TOKEN_LEX_ERR)
        return PARSE_LEX_ERR;
    return PARSE_OK;
}

static int GetTokenExpect(DStr_t **dstr, Token_t *token, TokenType_t type)
{
    int GetToken_return = GetToken(dstr, token);

    if(GetToken_return == type)
        return PARSE_OK;
    else if(GetToken_return == GET_TOKEN_INT_ERR)
        return PARSE_INT_ERR;
    else if(GetToken_return == GET_TOKEN_LEX_ERR)
        return PARSE_LEX_ERR;
    else
        return PARSE_SYN_ERR;
}

static int GetTokenExpectOperation(DStr_t **dstr, Token_t *token, TokenOperationType_t operation)
{
    int return_value = GetTokenExpect(dstr, token, T_OPERATION);
    if(return_value == PARSE_OK && token->operationType != operation)
            return PARSE_SYN_ERR;
    return return_value;
}

static int GetTokenExpectKeyword(DStr_t **dstr, Token_t *token, TokenKewordType_t keyword)
{
    int return_value = GetTokenExpect(dstr, token, T_KEYWORD);
    if(return_value == PARSE_OK && token->keywordType != keyword)
            return PARSE_SYN_ERR;
    return return_value;
}

static int Program(DStr_t **dstr, Token_t *token)
{
    int return_value = PARSE_OK;
    
    //DebugFPuts("In: <Program> <= ", output);
    //DebugFPrintToken(output, token, *dstr);
    if((return_value = TokenExpectKeyword(token, TK_DEF)) == PARSE_OK)
    {
        //DebugFPrintSymTable(output, symtable);
        //DebugFPuts("In: <Program> -> def\n", output);
        parameter_count = 0;
        function_definition = 1;

        if((return_value = GetTokenExpect(dstr, token, T_IDENTIFIER)) != PARSE_OK)
            return return_value;
        //DebugFPuts("In: <Program> -> def id\n", output);
        

        /*
        Test if there is no symbol with the same name
        */
        SymTableItem_t *function;
        function = SymTableFindItem(symtable, DStrStr(*dstr));
        if((function != NULL && function->type != SYM_FUNCTION) || (function != NULL && function->def == 1) )
            return PARSE_UNDEF_VAR;
        if(function != NULL)
        {
            //DebugFPuts("In: <Program> -> def: Defining declared function\n", output);
        }
        else if(function == NULL)
        {
            //DebugFPuts("In: <Program> -> def: Adding new function\n", output);
            function = SymTableAddFunction(symtable, DStrStr(*dstr), 0, 1);
            if(function == NULL)
                return PARSE_INT_ERR;
        }

    
        //Add start of function instruction
        if((return_value = CodeAddFunctionStart(function->key)) != PARSE_OK)
            return return_value;
        SymTableSetLocalMode(symtable);
        SymTableAddVariable(symtable, "%return");

        if((return_value = GetTokenExpectOperation(dstr, token, TO_LBRACKET)) != PARSE_OK)
            return return_value;
        //DebugFPuts("In: <Program> -> def id (\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        if((return_value = ParameterList(dstr, token)) != PARSE_OK)
            return return_value;
        //DebugFPuts("In: <Program> -> def id ( <ParameterList>\n", output);
        //Update number of arguments in symtable
        if((return_value = TokenExpectOperation(token, TO_RBRACKET)) != PARSE_OK)
            return return_value;
        //DebugFPuts("In: <Program> -> def id ( <ParameterList> )\n", output);
        if((return_value = GetTokenExpect(dstr, token, T_EOL)) != PARSE_OK)
            return return_value;
        //DebugFPuts("In: <Program> -> def id ( <ParameterList> ) EOL\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;



        if(function->def == 1)
            function->parameterCount = parameter_count;
        else if(function->parameterCount != parameter_count)
        {
            //DebugFPuts("In: <Program> -> def: parameter count does not match\n", output);
            return PARSE_PARAM_COUNT;
        }
        else
            function->def = 1;

        in_function = 1;

        if((return_value = Command(dstr, token)) != PARSE_OK)
            return return_value;
        //DebugFPuts("In: <Program> -> def id ( <ParameterList> ) EOL <Command>\n", output);
        if((return_value = TokenExpectKeyword(token, TK_END)) != PARSE_OK)
            return return_value;
        //DebugFPuts("In: <Program> -> def id ( <ParameterList> ) EOL <Command> end\n", output);

        //Add end of function instruction
        if((return_value = CodeAddFunctionEnd()) != PARSE_OK)
            return return_value;
        
        SymTableUnSetLocalMode(symtable);
        in_function = 0;
        function_definition = 0;

        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        if(TokenExpect(token, T_EOL) == PARSE_OK || TokenExpect(token, T_EOF) == PARSE_OK)
            return Program(dstr, token);
        else
            return PARSE_SYN_ERR;
    }
    else if((return_value = TokenExpect(token, T_EOL)) == PARSE_OK)
    {
        int GetToken_return = GetToken(dstr, token);

        if(GetToken_return == GET_TOKEN_INT_ERR)
            return PARSE_INT_ERR;
        else if(GetToken_return == GET_TOKEN_LEX_ERR)
            return PARSE_LEX_ERR;
        
        return Program(dstr, token);
    }
    else if((return_value = TokenExpect(token, T_EOF)) == PARSE_OK)
    {
        return PARSE_OK;
    }
    else if(//Error handeling
        TokenExpectKeyword(token, TK_DO) == PARSE_OK ||
        TokenExpectKeyword(token, TK_ELSE) == PARSE_OK ||
        TokenExpectKeyword(token, TK_END) == PARSE_OK ||
        TokenExpectKeyword(token, TK_NOT) == PARSE_OK ||
        TokenExpectKeyword(token, TK_THEN) == PARSE_OK ||
        TokenExpectOperation(token, TO_ASSIGNMENT) == PARSE_OK ||
        TokenExpectOperation(token, TO_COMMA) == PARSE_OK
        )
    {
        return PARSE_SYN_ERR;
    }
    else if((return_value = Command(dstr, token)) == PARSE_OK)
    {
        return Program(dstr, token);
    }
    else
        return return_value;
    
}

static int ParameterOther(DStr_t **dstr, Token_t *token)
{
    int return_value = PARSE_OK;
    //DebugFPuts("    In: <ParameterOther> <= ", output);
    //DebugFPrintToken(output, token, *dstr);
    if((return_value = TokenExpectOperation(token, TO_COMMA)) == PARSE_OK)
    {
        parameter_count++;
        //DebugFPuts("    In: <ParameterOther> -> , \n", output);
        if((return_value = GetTokenExpect(dstr, token, T_IDENTIFIER)) != PARSE_OK)
            return return_value;
        
        if(DStrLast(*dstr) == '!' || DStrLast(*dstr) == '?')
            return PARSE_SYN_ERR;
        //ADD variable to symtable
        if(SymTableFindItem(symtable, DStrStr(*dstr)) != NULL)
            return PARSE_UNDEF_VAR;
        if(SymTableAddVariable(symtable, DStrStr(*dstr)) == NULL)
            return PARSE_INT_ERR;

        //Generate code for new parameter
        if((return_value = CodeAddFunctionParameter(DStrStr(*dstr))) != PARSE_OK)
            return return_value;

        //DebugFPuts("    In: <ParameterOther> -> , id\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        return ParameterOther(dstr, token);
    }
    else
        return PARSE_OK;
}

static int ParameterList(DStr_t **dstr, Token_t *token)
{
    int return_value = PARSE_OK;
    
    //DebugFPuts("  In: <ParameterList> <= ", output);
    //DebugFPrintToken(output, token, *dstr);
    
    if((return_value = TokenExpect(token, T_IDENTIFIER)) == PARSE_OK)
    {
        parameter_count++;
        if(DStrLast(*dstr) == '!' || DStrLast(*dstr) == '?')
            return PARSE_SYN_ERR;
        
        //Add variable to symtable
        if(SymTableFindItem(symtable, DStrStr(*dstr)) != NULL)
            return PARSE_UNDEF_VAR;
        if(SymTableAddVariable(symtable, DStrStr(*dstr)) == NULL)
            return PARSE_INT_ERR;
        
        //Generate code for new parameter
        if((return_value = CodeAddFunctionParameter(DStrStr(*dstr))) != PARSE_OK)
            return return_value;

        //DebugFPuts("  In: <ParameterList> -> id\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        return ParameterOther(dstr, token);
    }
    else
        return PARSE_OK;
}

static int Term(DStr_t **dstr, Token_t *token)
{
    int return_value = PARSE_OK;
    //DebugFPuts("      In: <Term> <= ", output);
    //DebugFPrintToken(output, token, *dstr);
    if(TokenExpect(token, T_IDENTIFIER) == PARSE_OK)
    {
        SymTableItem_t *foundItem = SymTableFindItem(symtable, DStrStr(*dstr));
        if(foundItem == NULL)
            return SEMANTIC_ERROR_UNDEFINED_VAR;
        if((return_value = CodeAddFunctionCallVariable(DStrStr(*dstr))) != PARSE_OK)
            return return_value;
        //DebugFPuts("      In: <Term> -> id\n", output);
        return PARSE_OK; 
    }
    else if(TokenExpect(token, T_INTEGER) == PARSE_OK)
    {
        if((return_value = CodeAddFunctionCallInt(token->intValue)) != PARSE_OK)
            return return_value;
        //DebugFPuts("      In: <Term> -> int\n", output);
        return PARSE_OK;
    }
    else if(TokenExpect(token, T_DOUBLE) == PARSE_OK)
    {
        if((return_value = CodeAddFunctionCallInt(token->doubleValue)) != PARSE_OK)
            return return_value;
        //DebugFPuts("      In: <Term> -> double\n", output);
        return PARSE_OK;
    }
    else if(TokenExpect(token, T_STRING) == PARSE_OK)
    {
        if((return_value = CodeAddFunctionCallString(DStrStr(*dstr))) != PARSE_OK)
            return return_value;
        //DebugFPuts("      In: <Term> -> string\n", output);
        return PARSE_OK;
    }
    else if(TokenExpectKeyword(token, TK_NIL) == PARSE_OK)
    {
        if((return_value = CodeAddFunctionCallNil()) != PARSE_OK)
            return return_value;
        //DebugFPuts("      In: <Term> -> nil\n", output);
        return PARSE_OK;
    }
    else
    {
        //DebugFPuts("      In: <Term> -> PARSE_SYN_ERR\n", output);
        return PARSE_SYN_ERR;
    }
        
}

static int TermOther(DStr_t **dstr, Token_t *token)
{
    int return_value = PARSE_OK;
    //DebugFPuts("      In: <TermOther> <= ", output);
    //DebugFPrintToken(output, token, *dstr);
    if((return_value = TokenExpectOperation(token, TO_COMMA)) == PARSE_OK)
    {
        parameter_count++;
        //DebugFPuts("      In: <TermOther> -> , \n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        if((return_value = Term(dstr, token)) != PARSE_OK)
            return return_value;
        //DebugFPuts("      In: <TermOther> -> , <Term>\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        
        return TermOther(dstr, token);
    }
    else
    {
        //DebugFPuts("      In: <TermOther> -> PARSE_OK\n", output);
        return PARSE_OK;
    }
}

static int TermList(DStr_t **dstr, Token_t *token)
{
    int return_value = PARSE_OK;
    //DebugFPuts("    In: <TermList> <= ", output);
    //DebugFPrintToken(output, token, *dstr);
    parameter_count = 0;
    if((return_value = Term(dstr, token)) == PARSE_OK)
    {
        parameter_count++;
        //DebugFPuts("    In: <TermList> -> <Term>\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        return TermOther(dstr, token);
    }
    else if(return_value == SEMANTIC_ERROR_UNDEFINED_VAR)
        return return_value;
    else
        return PARSE_OK;
}

static int CommandEnd(DStr_t **dstr, Token_t *token)
{
    int return_value = PARSE_OK;
    //DebugFPuts("    In: <CommandEnd> <=", output);
    //DebugFPrintToken(output, token, *dstr);
    if((return_value = TokenExpect(token, T_EOF)) == PARSE_OK)
    {
        return PARSE_OK;
    }
    else if((return_value = TokenExpect(token, T_EOL)) == PARSE_OK)
    {
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        return Command(dstr, token);
    }
    else
        return PARSE_SYN_ERR;
}

static int Command(DStr_t **dstr, Token_t *token)
{
    int return_value = PARSE_OK;
    SymTableItem_t *returnVariable = NULL;
    if(in_while == 0)
        returnVariable = SymTableFindItem(symtable, "%return");

    //DebugFPuts("  In: <Command> <= ", output);
    //DebugFPrintToken(output, token, *dstr);
    if((return_value = TokenExpectKeyword(token, TK_IF)) == PARSE_OK)
    {//<Command> -> if <Expression> then EOL <Command> else <Command> end  <Command>
        //DebugFPuts("  In: <Command> -> if\n", output);

        SymTableItem_t *conditionVar = SymTableFindItem(symtable, "%condition");
        SymTableItem_t *conditionVarType = SymTableFindItem(symtable, "%conditionType");
        if(conditionVar == NULL)
        {
            conditionVar = SymTableAddVariable(symtable, "%condition");
            if(conditionVar == NULL)
                return PARSE_INT_ERR;
            if((return_value = CodeDeclareVariable( "%condition")) != PARSE_OK)
                return return_value;
        }
        if(conditionVarType == NULL)
        {
            conditionVarType = SymTableAddVariable(symtable, "%conditionType");
            if(conditionVarType == NULL)
                return PARSE_INT_ERR;
            if((return_value = CodeDeclareVariable( "%conditionType")) != PARSE_OK)
                return return_value;
        }

        if(returnVariable != NULL)
            if((return_value = CodeMoveNil(returnVariable->key)));
        int uniqueIfNumber = CodeGetUniqueIf();
        if((return_value = CodeAddIfStart(uniqueIfNumber)) != PARSE_OK)
            return return_value;

        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        
        if((return_value = Expression(dstr, token, NULL, NULL, conditionVar)) != PARSE_OK)
            return return_value;

        if((return_value = CodeAddIfCondition(uniqueIfNumber)) != PARSE_OK)
            return return_value;

        //DebugFPuts("  In: <Command> -> if <expression>\n", output);
        if((return_value = TokenExpectKeyword(token, TK_THEN)) != PARSE_OK)
            return return_value;
        //DebugFPuts("  In: <Command> -> if <expression> then\n", output);
        if((return_value = GetTokenExpect(dstr, token, T_EOL)) != PARSE_OK)
            return return_value;
        //DebugFPuts("  In: <Command> -> if <expression> then EOL\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;

        if((return_value = Command(dstr, token)) != PARSE_OK)
            return return_value;
        //DebugFPuts("  In: <Command> -> if <expression> then EOL <Command>\n", output);
        if((return_value = TokenExpectKeyword(token, TK_ELSE)) != PARSE_OK)
            return return_value;
        //DebugFPuts("  In: <Command> -> if <expression> then EOL <Command> else\n", output);
        if((return_value = GetTokenExpect(dstr, token, T_EOL)) != PARSE_OK)
            return return_value;

        if((return_value = CodeAddIfElse(uniqueIfNumber)) != PARSE_OK)
            return return_value;


        //DebugFPuts("  In: <Command> -> if <expression> then EOL <Command> else EOL\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        if((return_value = Command(dstr, token)) != PARSE_OK)
            return return_value;
        //DebugFPuts("  In: <Command> -> if <expression> then EOL <Command> else EOL <Command>\n", output);
        if((return_value = TokenExpectKeyword(token, TK_END)) != PARSE_OK)
            return return_value;
        //DebugFPuts("  In: <Command> -> if <expression> then EOL <Command> else EOL <Command> end\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        
        if((return_value = CodeAddIfEnd(uniqueIfNumber)) != PARSE_OK)
            return return_value;

        return CommandEnd(dstr, token);
    }
    else if((return_value = TokenExpectKeyword(token, TK_WHILE)) == PARSE_OK)
    {//<Command> -> while <Expression> do EOL <Command> end <Command>
        //DebugFPuts("  In: <Command> -> while\n", output);
        SymTableItem_t *conditionVar = SymTableFindItem(symtable, "%condition");
        SymTableItem_t *conditionVarType = SymTableFindItem(symtable, "%conditionType");
        if(conditionVar == NULL)
        {
            conditionVar = SymTableAddVariable(symtable, "%condition");
            if(conditionVar == NULL)
                return PARSE_INT_ERR;
            if((return_value = CodeDeclareVariable( "%condition")) != PARSE_OK)
                return return_value;
        }
        if(conditionVarType == NULL)
        {
            conditionVarType = SymTableAddVariable(symtable, "%conditionType");
            if(conditionVarType == NULL)
                return PARSE_INT_ERR;
            if((return_value = CodeDeclareVariable( "%conditionType")) != PARSE_OK)
                return return_value;
        }

        int uniqueWhileNumber = CodeGetUniqueWhile();
        if((return_value = CodeAddWhileStart(uniqueWhileNumber)) != PARSE_OK)
            return return_value;
        
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        if((return_value = Expression(dstr, token, NULL, NULL, conditionVar)) != PARSE_OK)
            return return_value;
        //DebugFPuts("  In: <Command> -> while <Expression>\n", output);
        if((return_value = TokenExpectKeyword(token, TK_DO)) != PARSE_OK)
            return return_value;
        //DebugFPuts("  In: <Command> -> while <Expression> do\n", output);
        if((return_value = GetTokenExpect(dstr, token, T_EOL)) != PARSE_OK)
            return return_value;
        
        //DebugFPuts("  In: <Command> -> while <Expression> do EOL\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;

        in_while = 1;
        if((return_value = CodeAddWhileBody(uniqueWhileNumber)) != PARSE_OK)
            return return_value;
        
        if((return_value = Command(dstr, token)) != PARSE_OK)
            return return_value;
        //DebugFPuts("  In: <Command> -> while <Expression> do EOL <Command>\n", output);
        if((return_value = TokenExpectKeyword(token, TK_END)) != PARSE_OK)
            return return_value;
        //DebugFPuts("  In: <Command> -> while <Expression> do EOL <Command> end\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;

        in_while = 0;
        if((return_value = CodeAddWhileEnd(uniqueWhileNumber)) != PARSE_OK)
            return return_value;
        if(returnVariable != NULL)
            if((return_value = CodeMoveNil(returnVariable->key)));
        



        return CommandEnd(dstr, token);
    }
    else if((return_value = TokenExpect(token, T_EOL)) == PARSE_OK)
    {//<Command> -> EOL <Command>
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        return Command(dstr, token);
    }
    else if((return_value = TokenExpect(token, T_IDENTIFIER)) == PARSE_OK)
    {//<Command> -> <Expression> <CommandEnd>
     //<Command> -> id = <Expression> <CommandEnd>
     //<Command> -> id = id <TermList> <CommandEnd>
     //<Command> -> id = id ( <TermList> ) <CommandEnd>
     //<Command> -> id <TermList> <CommandEnd>
     //<Command> -> id ( <TermList> ) <CommandEnd>
        //DebugFPuts("  In: <Command> -> id\n", output);

        //Test for internal functions
        int iternal_function_number = -1;
        if((iternal_function_number = IsInternalInlineFunction(dstr, token)) != -1)
            return ParseInternalInlineFunction(iternal_function_number, returnVariable, dstr, token);

        //Lookup one
        Token_t tempToken;
        DStr_t *tempDstr;
        DStrInit(&tempDstr, 50);
        if(tempDstr == NULL)
            return PARSE_INT_ERR;
        if((return_value = GetTokenParser(&tempDstr, &tempToken)) != PARSE_OK)
        {
            DStrFree(&tempDstr);
            return return_value;
        }

        if(TokenExpectOperation(&tempToken, TO_ASSIGNMENT) == PARSE_OK)
        {
            //DebugFPuts("  In: <Command> -> id =\n", output);
            SymTableItem_t *variable= SymTableFindItem(symtable, DStrStr(*dstr));
            if(variable == NULL)
            {
                if(DStrLast(*dstr) == '!' || DStrLast(*dstr) == '?')
                    return PARSE_SYN_ERR;
                variable = SymTableAddVariable(symtable, DStrStr(*dstr));
                if(variable == NULL)
                {
                    DStrFree(&tempDstr);
                    return PARSE_INT_ERR;
                }
                if((return_value = CodeDeclareVariable(variable->key)))
                {
                    DStrFree(&tempDstr);
                    return return_value;
                }

            }
            else if(variable->type != SYM_VARIABLE)
            {
                DStrFree(&tempDstr);
                return PARSE_UNDEF_VAR;
            }

            returnVariable = variable;
            //Lookup another two
            if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            {
                DStrFree(&tempDstr);
                return return_value;
            }

            //Test for internal function call
            if((iternal_function_number = IsInternalInlineFunction(dstr, token)) != -1)
            {
                DStrFree(&tempDstr);
                return ParseInternalInlineFunction(iternal_function_number, returnVariable, dstr, token);
            }

            //Lookup other one
            if((return_value = GetTokenParser(&tempDstr, &tempToken)) != PARSE_OK)
            {
                DStrFree(&tempDstr);
                return return_value;
            }
        }

        //Function calls, zero (in brackets) or one or more parameters
        if(TokenExpect(token, T_IDENTIFIER) == PARSE_OK && (TokenExpectOperation(&tempToken, TO_LBRACKET) == PARSE_OK || ExpectTerm(&tempDstr, &tempToken) == PARSE_OK))
        {
            //DebugFPuts("  In: <Command> -> potential function call\n", output);
            SymTableItem_t *function = SymTableFindItem(symtable, DStrStr(*dstr)); 

            if(function == NULL && !in_function)
            {
                DStrFree(&tempDstr);
                return PARSE_UNDEF_VAR;
            }
            else if(function == NULL && in_function)
            {   
                function = SymTableAddFunction(symtable, DStrStr(*dstr), -1, 0);
                if(function == NULL)
                {
                    DStrFree(&tempDstr);
                    return PARSE_INT_ERR;
                }
            }

            DStrReplace(dstr, DStrStr(tempDstr));
            *token = tempToken;
            DStrFree(&tempDstr);

            int uses_brackets = 0;
            //Add create frame start of function call
            if((return_value = CodeAddFunctionCallStart()) != PARSE_OK)
                return return_value;

            if(TokenExpectOperation(token, TO_LBRACKET) == PARSE_OK)
            {
                //DebugFPuts("  In: <Command> -> id ( \n", output);
                if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
                    return return_value;
                uses_brackets = 1;
            }
            if((return_value = TermList(dstr, token)) != PARSE_OK)
                return return_value;
            
            //DebugFPuts("  In: <Command> -> id [(, ] <TermList> [), ]\n", output);
            if(uses_brackets == 1)
            {
                if((return_value = TokenExpectOperation(token, TO_RBRACKET)) != PARSE_OK)
                    return return_value;
                if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
                    return return_value;
            }
            if((return_value = CodeAddFunctionCall(function->key)) != PARSE_OK)
                return return_value;
            
            if(function->def == 0 && function->parameterCount == -1)
                function->parameterCount = parameter_count;
            else if(function->parameterCount != parameter_count)
                return PARSE_PARAM_COUNT;
            //Assign return if needed
            if(returnVariable != NULL)
                if((return_value = CodeMoveReturn(returnVariable->key)) != PARSE_OK)
                    return return_value;
                    
        }
        //Zero variable function call or expression
        else if(TokenExpect(token, T_IDENTIFIER) == PARSE_OK && (TokenExpect(&tempToken, T_EOL) || TokenExpect(&tempToken, T_EOF)))
        {
            SymTableItem_t *function = SymTableFindItem(symtable, DStrStr(*dstr)); 
            if(function == NULL && !in_function)
            {
                DStrFree(&tempDstr);
                return PARSE_UNDEF_VAR;
            }
            else if(function == NULL && in_function)
            {   
                function = SymTableAddFunction(symtable, DStrStr(*dstr), 0, 0);
                //DebugFPrintSymTable(output, symtable);
                if(function == NULL)
                {
                    DStrFree(&tempDstr);
                    return PARSE_INT_ERR;
                }
            }
            if(function->type == SYM_VARIABLE)
            {
                if((return_value = Expression(dstr, token, &tempDstr, &tempToken, returnVariable)) != PARSE_OK)
                {
                    DStrFree(&tempDstr);
                    return return_value;
                }
            }
            if(function->type == SYM_FUNCTION)
            {
                //DebugFPuts("  In: <Command> -> id = id (zero parameters)\n", output);
                if(function->parameterCount != 0)
                    return PARSE_PARAM_COUNT;
                if((return_value = CodeAddFunctionCallStart()) != PARSE_OK)
                    return return_value;
                if((return_value = CodeAddFunctionCall(function->key)) != PARSE_OK)
                    return return_value;
                if(returnVariable != NULL)
                    if((return_value = CodeMoveReturn(returnVariable->key)) != PARSE_OK)
                        return return_value;
                DStrReplace(dstr, DStrStr(tempDstr));
                *token = tempToken;
                DStrFree(&tempDstr);
            }
        }
        //Expressions
        else if((return_value = Expression(dstr, token, &tempDstr, &tempToken, returnVariable)) != PARSE_OK)
        {
            DStrFree(&tempDstr);
            return return_value;
        }
        DStrFree(&tempDstr);
        return CommandEnd(dstr, token);
    }
    else if(ExpectTerm(dstr, token) == PARSE_OK || TokenExpect(token, T_OPERATION) == PARSE_OK)
    {
        if((return_value = Expression(dstr, token, NULL, NULL, returnVariable)) != PARSE_OK)
            return return_value;
        return CommandEnd(dstr, token);
    }
    else
    {
        return PARSE_OK;
    }
}

static int SStackPushReadNext(SStack_t *stack, SStackItem_t *currentStackItem, DStr_t **dstr, Token_t *token, DStr_t **nextDstr, Token_t *nextToken)
{
    int return_value = PARSE_OK;
    if(!Push_SStack(&stack, currentStackItem))
        return PARSE_INT_ERR;
    
    //Read next token if nextDstr and nextToken are not null coppy them else read another one
    if(nextDstr != NULL && nextToken != NULL)
    {
        if(!DStrReplace(dstr, DStrStr(*nextDstr)))
            return PARSE_INT_ERR;
        *token = *nextToken;
        
    }
    else if(nextDstr == NULL && nextToken == NULL)
    {
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
    }
    else
        return PARSE_INT_ERR;
    //Process new token to be compatible with SStack
    if((return_value = SStackProcessTokenToItem(*dstr, token, currentStackItem)) != PARSE_OK)
        return return_value;
    return return_value;
}

static int CodeMoveSStackItem(SymTableItem_t *returnVariable, SStackItem_t *item)
{
    int return_value = PARSE_OK;
    if(returnVariable == NULL)
        return return_value;
    if(item->dataType == STACK_DOUBLE)
        return_value = CodeMoveFloat(returnVariable->key, item->data.doubleValue);
    else if(item->dataType == STACK_INT)
        return_value = CodeMoveInt(returnVariable->key, item->data.intValue);
    else if(item->dataType == STACK_NIL)
        return_value = CodeMoveNil(returnVariable->key);
    else if(item->dataType == STACK_BOOL)
        return_value = CodeMoveBool(returnVariable->key, item->data.boolValue);
    else if(item->dataType == STACK_STRING)
        return_value = CodeMoveString(returnVariable->key, item->data.string);
    else if(item->dataType == STACK_SYMBOL)
        return_value = CodeMoveVar(returnVariable->key, item->data.symbol->key);
    else
        return PARSE_INT_ERR;
    return return_value;
}

SymTableItem_t * SymtableCodeAddTemporaryVariable(SymTable_t *symtable, int tempVariableNumber)
{
    char temporaryVariableName[40] = {'\0',};
    sprintf(temporaryVariableName, "%%temp%d", tempVariableNumber);
    SymTableItem_t *tempVariable = SymTableFindItem(symtable, temporaryVariableName);
    if(tempVariable == NULL)
    {
        if((tempVariable = SymTableAddVariable(symtable, temporaryVariableName)) == NULL)
            return NULL;
        else if(CodeDeclareVariable(temporaryVariableName) != PARSE_OK)
            return NULL;
    }
    return tempVariable;
}

static int Expression(DStr_t **dstr, Token_t *token, DStr_t **nextDstr, Token_t *nextToken, SymTableItem_t *returnVariable)
{
    int return_value = PARSE_OK;
    SStack_t *stack;
    int temporaryVariableCount = 0;
    Init_SStack(&stack, SSTACK_DEFAULT_SIZE);
    if(stack == NULL)
        return PARSE_INT_ERR;
    
    if((return_value =  SStackPushEnd(&stack)) != PARSE_OK)
    {
        Dispose_SStack(&stack);
        return return_value;
    }

    SStackItem_t currentStackItem;
    if((return_value = SStackProcessTokenToItem(*dstr, token, &currentStackItem)) != PARSE_OK)
    {
        Dispose_SStack(&stack);
        return return_value;
    }
    while(SStackTopTerminal(stack) != NULL && !(SStackTopTerminal(stack)->terminal == TERMINAL_END && currentStackItem.terminal == TERMINAL_END))
    {
        ExpressionPrecedence_t precedenceRule = SStackGetExpessionPrecedence(stack, &currentStackItem);
        if(precedenceRule == PRE_EQ)
        {
            if((return_value = SStackPushReadNext(stack, &currentStackItem, dstr, token, nextDstr, nextToken)) != PARSE_OK)
            {
                Dispose_SStack(&stack);
            }
            nextDstr = NULL;
            nextToken = NULL;
            //DebugFPrintSStack(output, stack);
        }
        else if(precedenceRule == PRE_LT)
        {
            SStackTopTerminalAddLT(stack);
            if((return_value = SStackPushReadNext(stack, &currentStackItem, dstr, token, nextDstr, nextToken)) != PARSE_OK)
            {
                Dispose_SStack(&stack);
            }
            nextDstr = NULL;
            nextToken = NULL;
            //DebugFPrintSStack(output, stack);
        }
        else if(precedenceRule == PRE_GT)
        {
            int oldTemporaryVatiableCount = temporaryVariableCount;
            if((return_value = SStackReduceByRule(stack, &temporaryVariableCount)) != PARSE_OK)
            {
                Dispose_SStack(&stack);
                return return_value;
            }

            if(oldTemporaryVatiableCount != temporaryVariableCount)
            {
                SymTableItem_t *tempItem = SymtableCodeAddTemporaryVariable(symtable, temporaryVariableCount);
                if(tempItem == NULL)
                {
                    Dispose_SStack(&stack);
                    return PARSE_INT_ERR;
                }        
                SStackItem_t *top_item = Top_SStack(stack);

                top_item->dataType = STACK_SYMBOL;
                top_item->data.symbol = tempItem;
            }
            //DebugFPrintSStack(output, stack);
        }
        else
        {
            Dispose_SStack(&stack);
            return PARSE_SYN_ERR;
        }
    }

    SStackItem_t *topItem = Top_SStack(stack);
    
    return_value = CodeMoveSStackItem(returnVariable, topItem);
    
    //DebugFPrintf(output, "%d, %lf\n", topItem->data.intValue, topItem->data.doubleValue);

    Dispose_SStack(&stack);
    return return_value;
}

int Parse(void)
{
    int return_value = PARSE_OK;

    DStr_t *dstr;
    DStrInit(&dstr, 50);
    SymTableInit(&symtable, 10);
    SymTableAddInternalFunctions(symtable);
    
    if(dstr == NULL || symtable == NULL || CodeInitialize() != PARSE_OK)
        return_value = PARSE_INT_ERR;
    else
    {
        Token_t token;
        if((return_value = GetTokenParser(&dstr, &token)) == PARSE_OK)
            return_value = Program(&dstr, &token);
        //if(return_value != PARSE_OK)
            //DebugFPrintToken(stderr, &token, dstr);
    }
    if(return_value != PARSE_OK)
        if(SymTableAreUndefinedFunctions(symtable))
            return_value = PARSE_UNDEF_VAR;
    
    DStrFree(&dstr);
    SymTableDispose(&symtable);
    CodeFinalize();
    return return_value;
}