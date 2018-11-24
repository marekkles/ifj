#include "parser.h"

static int Program(FILE *input, void * symtable, DStr_t **dstr, Token_t *token)
{
    int return_value = PARSE_OK;
    int get_token_return;
    if(token->type == T_KEYWORD && token->keywordType == TK_DEF)
    {
        get_token_return = GetToken(input, dstr, token);
        if(get_token_return == T_IDENTIFIER)
        {
            //add token to symtable
        }
    }
    
}

static int Parameters(FILE *input, void * symtable, DStr_t **dstr, Token_t *token);

static int Argument(FILE *input, void * symtable, DStr_t **dstr, Token_t *token);

static int ArgumentOther(FILE *input, void * symtable, DStr_t **dstr, Token_t *token);

static int Statement(FILE *input, void * symtable, DStr_t **dstr, Token_t *token);

static int Expression(FILE *input, void * symtable, DStr_t **dstr, Token_t *token);

int Parse(FILE *input)
{
    int return_value = PARSE_OK;
    int get_token_return;
    //INIT Symtable (local, global)
    void * symtable = NULL;

    DStr_t *dstr;
    DStrInit(&dstr, 25);

    Token_t *token;
    token = malloc(sizeof(Token_t));

    if(dstr == NULL || token == NULL) //Symtable error -> return_value = PARSE_INT_ERR
    {
        return_value = PARSE_INT_ERR;
    }
    else
    {
        get_token_return = GetToken(input, &dstr, token);
        
        if(get_token_return == GET_TOKEN_LEX_ERR)
        {
            return_value = PARSE_LEX_ERR;
        }
        else if(get_token_return == GET_TOKEN_INT_ERR)
        {
            return_value = PARSE_INT_ERR;
        }
        else
        {
            return_value = Program(input, symtable, &dstr, token);
        }
    }

    DStrFree(&dstr);
    free(token);
    //Free Symtable (local, global)
    return return_value;
}