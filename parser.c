#include "parser.h"

extern FILE *input;
extern FILE *output;

static int in_function = 0;
static int function_definition = 0;
static int parameter_count = 0;

static void *symtable;
static void *symtable_local;

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


    if((return_value = TokenExpectKeyword(token, TK_DEF)) == PARSE_OK)
    {
        parameter_count = 0;
        function_definition = 1;

        if((return_value = GetTokenExpect(dstr, token, T_IDENTIFIER)) != PARSE_OK)
            return return_value;

        //Add identifier to symtable

        if((return_value = GetTokenExpectOperation(dstr, token, TO_LBRACKET)) != PARSE_OK)
            return return_value;
        if((return_value = Parameters(dstr, token)) != PARSE_OK)
            return return_value;

        //Update number of arguments in symtable

        if((return_value = TokenExpectOperation(token, TO_RBRACKET)) != PARSE_OK)
            return return_value;
        if((return_value = GetTokenExpect(dstr, token, T_EOL)) != PARSE_OK)
            return return_value;

        in_function = 1;

        if((return_value = Command(dstr, token)) != PARSE_OK)
            return return_value;
        if((return_value = TokenExpectKeyword(token, TK_END)) != PARSE_OK)
            return return_value;
        in_function = 0;
        function_definition = 0;

        int GetToken_return = GetToken(dstr, token);

        if(GetToken_return == GET_TOKEN_INT_ERR)
            return PARSE_INT_ERR;
        else if(GetToken_return == GET_TOKEN_LEX_ERR)
            return PARSE_LEX_ERR;
        
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
    else if((return_value = Command(dstr, token)) == PARSE_OK)
    {
        return Program(dstr, token);
    }
    else
        return return_value;
    
}

static int ParameterList(DStr_t **dstr, Token_t *token)
{
    int return_value = PARSE_OK;
    if((return_value = TokenExpect(token, T_IDENTIFIER)) == PARSE_OK)
    {
        parameter_count++;

        int GetToken_return = GetToken(dstr, token);

        if(GetToken_return == GET_TOKEN_INT_ERR)
            return PARSE_INT_ERR;
        else if(GetToken_return == GET_TOKEN_LEX_ERR)
            return PARSE_LEX_ERR;
        return ParameterOther(dstr, token);
    }
    else
        return PARSE_OK;
}

static int ParameterOther(DStr_t **dstr, Token_t *token)
{
    int return_value = PARSE_OK;
    if((return_value = TokenExpectOperation(token, TO_COMMA)) == PARSE_OK)
    {
        parameter_count++;

        if((return_value = GetTokenExpect(dstr, token, T_IDENTIFIER)) != PARSE_OK)
            return return_value;
        
        int GetToken_return = GetToken(dstr, token);

        if(GetToken_return == GET_TOKEN_INT_ERR)
            return PARSE_INT_ERR;
        else if(GetToken_return == GET_TOKEN_LEX_ERR)
            return PARSE_LEX_ERR;
        return ParameterOther(dstr, token);
    }
    else
        return PARSE_OK;
}

static int TermList(DStr_t **dstr, Token_t *token)
{
    int return_value = PARSE_OK;
    parameter_count = 0;
    if((return_value = Term(token)) == PARSE_OK)
    {
        parameter_count++;

        int GetToken_return = GetToken(dstr, token);

        if(GetToken_return == GET_TOKEN_INT_ERR)
            return PARSE_INT_ERR;
        else if(GetToken_return == GET_TOKEN_LEX_ERR)
            return PARSE_LEX_ERR;
        return TermOther(dstr, token);
    }
    else
        return PARSE_OK;
}
static int Term(Token_t *token)
{
    int return_value = PARSE_OK;
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
    if((return_value = TokenExpectOperation(token, TO_COMMA)) == PARSE_OK)
    {
        parameter_count++;

        if((return_value = Term(token)) != PARSE_OK)
            return return_value;
        
        int GetToken_return = GetToken(dstr, token);

        if(GetToken_return == GET_TOKEN_INT_ERR)
            return PARSE_INT_ERR;
        else if(GetToken_return == GET_TOKEN_LEX_ERR)
            return PARSE_LEX_ERR;
        
        return TermOther(dstr, token);
    }
    else
        return PARSE_OK;
}

static int Command(DStr_t **dstr, Token_t *token);

static int Expression(DStr_t **dstr, Token_t *token, Token_t *next_token);

int Parse(FILE *input)
{
    int return_value = PARSE_OK;
    int get_token_return;
    //INIT Symtable (local, global)
    void * symtable = NULL;

    DStr_t *dstr;
    DStrInit(&dstr, 25);

    if(dstr == NULL) //Symtable error -> return_value = PARSE_INT_ERR
        return_value = PARSE_INT_ERR;
    else
    {
        Token_t token;
        get_token_return = GetToken(&dstr, &token);
        return_value = Program(&dstr, &token);
    }


    DStrFree(&dstr);
    //Free Symtable (local, global)
    return return_value;
}