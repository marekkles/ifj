#include "parser.h"

extern FILE *input;
extern FILE *output;

int in_function = 0;
static int function_definition = 0;
static int parameter_count = 0;

static void *symtable;
static void *symtable_local;


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
static int Term(Token_t *token);
static int TermList(DStr_t **dstr, Token_t *token);
static int TermOther(DStr_t **dstr, Token_t *token);
static int Command(DStr_t **dstr, Token_t *token);
static int Expression(DStr_t **dstr, Token_t *token, Token_t *next_token);



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
        //Add identifier to symtable

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
        in_function = 1;

        if((return_value = Command(dstr, token)) != PARSE_OK)
            return return_value;
        DebugFPuts("In: <Program> -> def id ( <ParameterList> ) EOL <Command>\n", output);
        if((return_value = TokenExpectKeyword(token, TK_END)) != PARSE_OK)
            return return_value;
        DebugFPuts("In: <Program> -> def id ( <ParameterList> ) EOL <Command> end\n", output);
        in_function = 0;
        function_definition = 0;

        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        
        return Program(dstr, token);
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
    else if(
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
        DebugFPuts("    In: <ParameterOther> -> , ", output);
        if((return_value = GetTokenExpect(dstr, token, T_IDENTIFIER)) != PARSE_OK)
            return return_value;
        DebugFPuts("    In: <ParameterOther> -> , id", output);
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
        DebugFPuts("  In: <ParameterList> -> id\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        return ParameterOther(dstr, token);
    }
    else
        return PARSE_OK;
}

static int Term(Token_t *token)
{
    int return_value = PARSE_OK;
    DebugFPuts("      In: <Term>\n", output);
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
        if((return_value = Term(token)) != PARSE_OK)
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
    if((return_value = Term(token)) == PARSE_OK)
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
        
        if((return_value = Expression(dstr, token, NULL)) != PARSE_OK)
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
        return Command(dstr, token);
    }
    else if((return_value = TokenExpectKeyword(token, TK_WHILE)) == PARSE_OK)
    {//<Command> -> while <Expression> do EOL <Command> end <Command>
        DebugFPuts("  In: <Command> -> while\n", output);
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        if((return_value = Expression(dstr, token, NULL)) != PARSE_OK)
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
        return Command(dstr, token);
    }
    else if((return_value = TokenExpect(token, T_EOL)) == PARSE_OK)
    {//<Command> -> EOL <Command>
        if((return_value = GetTokenParser(dstr, token)) != PARSE_OK)
            return return_value;
        return Command(dstr, token);
    }
    else if((return_value = TokenExpect(token, T_IDENTIFIER)) == PARSE_OK)
    {//<Command> -> id
        if(in_function)
        {
            return PARSE_OK;
            //Can be assignment or Expression
            //Could be function call or Expression

            //If id is in symtable var -> expression or assignment
            //If next_token -> = -> Assignment
            //Else Expression

            //If id is in symtable function -> function call

            //Function call -> TermList -> number_of_arguments -> add to symtable as undefined function -> call Command

            //If id is not in symtable -> function call or error (depends)
            //If next_token -> TermList -> number_of_arguments -> add to symtable as undefined function -> call Command
            DStr_t *next_dstr;
            Token_t next_token;
            DStrInit(&next_dstr, 25);
            if(next_dstr == NULL)
                return PARSE_INT_ERR;
            if((return_value = GetTokenParser(&next_dstr, &next_token)) != PARSE_OK)
                return return_value;
            
            if((return_value = TokenExpectOperation(&next_token, TO_ASSIGNMENT)))
            //When assignment
            //Could be function call or Expression
            //If id is in symtable var -> expression
            //If id is in symtable function -> function call -> TermList -> chech if number of argument is right -> call Command
            //If id is not in symtable -> function call or undef var (depends)
            

            DStrFree(&next_dstr);
        }
        else
        {
            return PARSE_OK;
            //In symtable decide function -> function call, id -> expression or assignment
        }
    }
    else
    {
        return PARSE_OK;
    }
}

static int Expression(DStr_t **dstr, Token_t *token, Token_t *next_token)
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
        if(next_token != NULL)
        {
            token->type = next_token->type;
            token->intValue = next_token->intValue;
        }
        return Expression(dstr, token, NULL);
    }
    else
        return PARSE_OK;
}

int Parse(void)
{
    int return_value = PARSE_OK;
    int get_token_return;
    //INIT Symtable (local, global)
    void * symtable = NULL;

    DStr_t *dstr;
    DStrInit(&dstr, 50);

    if(dstr == NULL) //Symtable error -> return_value = PARSE_INT_ERR
        return_value = PARSE_INT_ERR;
    else
    {
        Token_t token;
        get_token_return = GetToken(&dstr, &token);
        return_value = Program(&dstr, &token);
    }

    //Check all functions are defined

    DStrFree(&dstr);
    //Free Symtable (local, global)
    return return_value;
}