#include "parser.h"

extern FILE *input;
extern FILE *output;

int in_function = 0;
static int function_definition = 0;
static int parameter_count = 0;

static SymTable_t *symtable;


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
static int Expression(DStr_t **dstr, Token_t *token, DStr_t **nextDstr,Token_t *nextToken);

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
    
    DebugFPuts("In: <Program> <= ", output);
    DebugFPrintToken(output, token, *dstr);
    if((return_value = TokenExpectKeyword(token, TK_DEF)) == PARSE_OK)
    {
        DebugFPuts("In: <Program> -> def\n", output);
        parameter_count = 0;
        function_definition = 1;

        if((return_value = GetTokenExpect(dstr, token, T_IDENTIFIER)) != PARSE_OK)
            return return_value;
        DebugFPuts("In: <Program> -> def id\n", output);
        

        /*
        Test if there is no symbol with the same name
        */
        
        SymTableItem_t *function;
        function = SymTableFindItem(symtable, DStrStr(*dstr));
        if((function != NULL && function->type != SYM_FUNCTION) || (function != NULL && function->def == 1) )
            return PARSE_UNDEF_VAR;
        if(function == NULL)
        {
            function = SymTableAddFunction(symtable, DStrStr(*dstr), 0, 1);
            if(function == NULL)
                return PARSE_INT_ERR;
        }
            
        
        SymTableSetLocalMode(symtable);

        if((return_value = GetTokenExpectOperation(dstr, token, TO_LBRACKET)) != PARSE_OK)
            return return_value;
        DebugFPuts("In: <Program> -> def id (\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        if((return_value = ParameterList(dstr, token)) != PARSE_OK)
            return return_value;
        DebugFPuts("In: <Program> -> def id ( <ParameterList>\n", output);
        //Update number of arguments in symtable
        if((return_value = TokenExpectOperation(token, TO_RBRACKET)) != PARSE_OK)
            return return_value;
        DebugFPuts("In: <Program> -> def id ( <ParameterList> )\n", output);
        if((return_value = GetTokenExpect(dstr, token, T_EOL)) != PARSE_OK)
            return return_value;
        DebugFPuts("In: <Program> -> def id ( <ParameterList> ) EOL\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;

        if(function->def == 1)
            function->parameterCount = parameter_count;
        else if(function->parameterCount != parameter_count)
            return PARSE_PARAM_COUNT;
        else
            function->def = 1;

        in_function = 1;

        if((return_value = Command(dstr, token)) != PARSE_OK)
            return return_value;
        DebugFPuts("In: <Program> -> def id ( <ParameterList> ) EOL <Command>\n", output);
        if((return_value = TokenExpectKeyword(token, TK_END)) != PARSE_OK)
            return return_value;
        DebugFPuts("In: <Program> -> def id ( <ParameterList> ) EOL <Command> end\n", output);

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
    DebugFPuts("    In: <ParameterOther> <= ", output);
    DebugFPrintToken(output, token, *dstr);
    if((return_value = TokenExpectOperation(token, TO_COMMA)) == PARSE_OK)
    {
        parameter_count++;
        DebugFPuts("    In: <ParameterOther> -> , \n", output);
        if((return_value = GetTokenExpect(dstr, token, T_IDENTIFIER)) != PARSE_OK)
            return return_value;
        
        //ADD variable to symtable
        if(SymTableFindItem(symtable, DStrStr(*dstr)) != NULL)
            return PARSE_UNDEF_VAR;
        if(SymTableAddVariable(symtable, DStrStr(*dstr)) == NULL)
            return PARSE_INT_ERR;

        DebugFPuts("    In: <ParameterOther> -> , id\n", output);
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
    
    DebugFPuts("  In: <ParameterList> <= ", output);
    DebugFPrintToken(output, token, *dstr);
    
    if((return_value = TokenExpect(token, T_IDENTIFIER)) == PARSE_OK)
    {
        parameter_count++;
        
        //Add variable to symtable
        if(SymTableFindItem(symtable, DStrStr(*dstr)) != NULL)
            return PARSE_UNDEF_VAR;
        if(SymTableAddVariable(symtable, DStrStr(*dstr)) == NULL)
            return PARSE_INT_ERR;
        
        DebugFPuts("  In: <ParameterList> -> id\n", output);
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
    DebugFPuts("      In: <Term> <= ", output);
    DebugFPrintToken(output, token, *dstr);
    if(TokenExpect(token, T_IDENTIFIER) == PARSE_OK)
    {
        //Check if is in symtable return error if not
        //Add to function call
        return PARSE_OK; 
    }
    else if(TokenExpect(token, T_INTEGER) == PARSE_OK)
    {
        //Add to function call
        return PARSE_OK;
    }
    else if(TokenExpect(token, T_DOUBLE) == PARSE_OK)
    {
        //Add to function call
        return PARSE_OK;
    }
    else if(TokenExpect(token, T_STRING) == PARSE_OK)
    {
        //Add to function call
        return PARSE_OK;
    }
    else
        return PARSE_SYN_ERR;
}

static int TermOther(DStr_t **dstr, Token_t *token)
{
    int return_value = PARSE_OK;
    DebugFPuts("      In: <TermOther> <= ", output);
    DebugFPrintToken(output, token, *dstr);
    if((return_value = TokenExpectOperation(token, TO_COMMA)) == PARSE_OK)
    {
        parameter_count++;
        DebugFPuts("      In: <TermOther> -> , \n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        if((return_value = Term(dstr, token)) != PARSE_OK)
            return return_value;
        DebugFPuts("      In: <TermOther> -> , <Term>\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        
        return TermOther(dstr, token);
    }
    else
        return PARSE_OK;
}

static int TermList(DStr_t **dstr, Token_t *token)
{
    int return_value = PARSE_OK;
    DebugFPuts("    In: <TermList> <= ", output);
    DebugFPrintToken(output, token, *dstr);
    parameter_count = 0;
    if((return_value = Term(dstr, token)) == PARSE_OK)
    {
        parameter_count++;
        DebugFPuts("    In: <TermList> -> <Term>\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        return TermOther(dstr, token);
    }
    else
        return PARSE_OK;
}

static int CommandEnd(DStr_t **dstr, Token_t *token)
{
    int return_value = PARSE_OK;
    DebugFPuts("    In: <CommandEnd> <=", output);
    DebugFPrintToken(output, token, *dstr);
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
    DebugFPuts("  In: <Command> <= ", output);
    DebugFPrintToken(output, token, *dstr);
    if((return_value = TokenExpectKeyword(token, TK_IF)) == PARSE_OK)
    {//<Command> -> if <Expression> then EOL <Command> else <Command> end  <Command>
        DebugFPuts("  In: <Command> -> if\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        
        if((return_value = Expression(dstr, token, NULL, NULL)) != PARSE_OK)
            return return_value;
        DebugFPuts("  In: <Command> -> if <expression>\n", output);
        if((return_value = TokenExpectKeyword(token, TK_THEN)) != PARSE_OK)
            return return_value;
        DebugFPuts("  In: <Command> -> if <expression> then\n", output);
        if((return_value = GetTokenExpect(dstr, token, T_EOL)) != PARSE_OK)
            return return_value;
        DebugFPuts("  In: <Command> -> if <expression> then EOL\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;

        if((return_value = Command(dstr, token)) != PARSE_OK)
            return return_value;
        DebugFPuts("  In: <Command> -> if <expression> then EOL <Command>\n", output);
        if((return_value = TokenExpectKeyword(token, TK_ELSE)) != PARSE_OK)
            return return_value;
        DebugFPuts("  In: <Command> -> if <expression> then EOL <Command> else\n", output);
        if((return_value = GetTokenExpect(dstr, token, T_EOL)) != PARSE_OK)
            return return_value;
        DebugFPuts("  In: <Command> -> if <expression> then EOL <Command> else EOL\n", output);
        if((return_value = Command(dstr, token)) != PARSE_OK)
            return return_value;
        DebugFPuts("  In: <Command> -> if <expression> then EOL <Command> else EOL <Command>\n", output);
        if((return_value = TokenExpectKeyword(token, TK_END)) != PARSE_OK)
            return return_value;
        DebugFPuts("  In: <Command> -> if <expression> then EOL <Command> else EOL <Command> end\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        return CommandEnd(dstr, token);
    }
    else if((return_value = TokenExpectKeyword(token, TK_WHILE)) == PARSE_OK)
    {//<Command> -> while <Expression> do EOL <Command> end <Command>
        DebugFPuts("  In: <Command> -> while\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        if((return_value = Expression(dstr, token, NULL, NULL)) != PARSE_OK)
            return return_value;
        DebugFPuts("  In: <Command> -> while <Expression>\n", output);
        if((return_value = TokenExpectKeyword(token, TK_DO)) != PARSE_OK)
            return return_value;
        DebugFPuts("  In: <Command> -> while <Expression> do\n", output);
        if((return_value = GetTokenExpect(dstr, token, T_EOL)) != PARSE_OK)
            return return_value;
        DebugFPuts("  In: <Command> -> while <Expression> do EOL\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        if((return_value = Command(dstr, token)) != PARSE_OK)
            return return_value;
        DebugFPuts("  In: <Command> -> while <Expression> do EOL <Command>\n", output);
        if((return_value = TokenExpectKeyword(token, TK_END)) != PARSE_OK)
            return return_value;
        DebugFPuts("  In: <Command> -> while <Expression> do EOL <Command> end\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
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
        DebugFPuts("  In: <Command> -> id\n", output);
        int is_var = 0;
        int is_function = 0;
        
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

        SymTableItem_t *returnVariable = NULL;

        if(TokenExpectOperation(&tempToken, TO_ASSIGNMENT) == PARSE_OK)
        {
            DebugFPuts("  In: <Command> -> id =\n", output);
            SymTableItem_t *variable= SymTableFindItem(symtable, DStrStr(*dstr));
            if(variable == NULL)
            {
                variable = SymTableAddVariable(symtable, DStrStr(*dstr));
                if(variable == NULL)
                {
                    DStrFree(&tempDstr);
                    return PARSE_INT_ERR;
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
            if((return_value = GetTokenParser(&tempDstr, &tempToken)) != PARSE_OK)
            {
                DStrFree(&tempDstr);
                return return_value;
            }
        }

        //Function calls
        if(TokenExpectOperation(&tempToken, TO_LBRACKET) == PARSE_OK || TokenExpect(&tempToken, T_IDENTIFIER) == PARSE_OK)
        {
            DebugFPuts("  In: <Command> -> id  [= id, ] [( , ' ']\n", output);
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

            if(TokenExpectOperation(token, TO_LBRACKET) == PARSE_OK)
            {
                if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
                    return return_value;
                if((return_value = TermList(dstr, token)) != PARSE_OK)
                    return return_value;
                DebugFPuts("  In: <Command> -> id ( <TermList>\n", output);
                if((return_value = TokenExpectOperation(token, TO_RBRACKET)) != PARSE_OK)
                    return return_value;
                DebugFPuts("  In: <Command> -> id ( <TermList> )\n", output);
                if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
                    return return_value;
            }
            else
            {
                if((return_value = TermList(dstr, token)) != PARSE_OK)
                    return return_value;
                DebugFPuts("  In: <Command> -> id <TermList>\n", output);
            }
            
            if(function->def == 0 && function->parameterCount == -1)
                function->parameterCount = parameter_count;
            else if(function->parameterCount != parameter_count)
                return PARSE_PARAM_COUNT;
                    
        }
        else if((return_value = Expression(dstr, token, &tempDstr, &tempToken)) != PARSE_OK)
        {
            DStrFree(&tempDstr);
            return return_value;
        }
        return CommandEnd(dstr, token);
    }
    else
    {
        return PARSE_OK;
    }
}

static int Expression(DStr_t **dstr, Token_t *token, DStr_t **nextDstr, Token_t *nextToken)
{
    DebugFPuts("    In: <Expression> <= ", output);
    DebugFPrintToken(output, token, *dstr);
    if(
        TokenExpect(token, T_IDENTIFIER) == PARSE_OK || 
        TokenExpect(token, T_DOUBLE) == PARSE_OK || 
        TokenExpect(token, T_INTEGER) == PARSE_OK || 
        TokenExpect(token, T_STRING) == PARSE_OK || 
        TokenExpect(token, T_OPERATION) == PARSE_OK
       )
    {
        int return_value;
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        if(nextToken != NULL)
        {
            token->type = nextToken->type;
            token->intValue = nextToken->intValue;
        }
        return Expression(dstr, token, NULL, NULL);
    }
    else
        return PARSE_OK;
}

int Parse(void)
{
    int return_value = PARSE_OK;

    DStr_t *dstr;
    DStrInit(&dstr, 50);
    SymTableInit(&symtable, 10);
    if(dstr == NULL || symtable == NULL)
        return_value = PARSE_INT_ERR;
    else
    {
        Token_t token;
        if((return_value = GetTokenParser(&dstr, &token)) == PARSE_OK)
            return_value = Program(&dstr, &token);
    }

    DStrFree(&dstr);
    SymTableDispose(&symtable);
    return return_value;
}