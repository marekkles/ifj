#include "fsm.h"
//int CurrentlyReading[2] = {0, 1};


typedef enum {
    //Initial state
    S_INITIAL, 
    //End of line
    S_EOL,
    //State for identifiers and keywords
    S_IDENTIFIER_READ,
    S_POTENTIAL_IDENTIFIER_READ,
    S_FUNCTION,
    //Stases for 0 and other integer values
    S_INT_READ, 
    S_INT_ZERO, 
    //Possible BASE extention states
    S_INT_BIN0, 
    S_INT_OCT, 
    S_INT_HEX0,
    S_INT_BIN1, 
    S_INT_HEX1, 
    // States for reading real values
    // 0. 1.
    S_DOUBLE_POINT,
    // 0.123 
    S_DOUBLE_DEC_READ,
    // 0.123e 0.123E 123e 123E 
    S_DOUBLE_EXP, 
    // 0.123E+ 123e-
    S_DOUBLE_EXP_SIGN,
    // 0.123E+456 123e-456
    S_DOUBLE_EXP_NUM,
    // Comment states
    S_COMMENT_READ,
    S_COMMENT_END,
    //String states
    S_STR_READ,
    S_STR_ESC,
    S_STR_HEX,
    S_STR_HEX1,
    S_STR_END,
    //States for operators
    S_LESSER_THAN,
    S_LESSER_EQUAL_THAN,
    S_GREATER_THAN,
    S_GREATER_EQUAL_THAN,
    S_ASSIGNMENT,
    S_EQUAL_TO,
    S_EXCLAMATION_MARK,
    S_NOT_EQUAL_TO,
    S_MULTIPLY,
    S_DIVIDE,
    S_ADD,
    S_SUBTRACT,
    S_LBRACKET, 
    S_RBRACKET,
    S_COMMA,
    S_EOF,
    //Block Comment
    S_BLOCK_COMMENT0,
    S_BLOCK_COMMENT1,
    S_BLOCK_COMMENT2,
    S_BLOCK_COMMENT3,
    S_BLOCK_COMMENT4,
    S_BLOCK_COMMENT5,
    S_BLOCK_COMMENT,
    S_BLOCK_COMMENT_END0,
    S_BLOCK_COMMENT_END1,
    S_BLOCK_COMMENT_END2,
    S_BLOCK_COMMENT_END3,
    S_BLOCK_COMMENT_END4,
    //Error state
    S_ERROR
} FSMState_t;
static const FSMState_t final_states[] = {
    S_EOL,
    S_IDENTIFIER_READ,
    S_POTENTIAL_IDENTIFIER_READ,
    S_FUNCTION, 
    S_INT_READ, 
    S_INT_ZERO, 
    S_INT_BIN1, 
    S_INT_OCT, 
    S_INT_HEX1, 
    S_DOUBLE_DEC_READ, 
    S_DOUBLE_EXP_NUM, 
    S_COMMENT_END, 
    S_STR_END, 
    S_LESSER_THAN, 
    S_GREATER_THAN, 
    S_ASSIGNMENT, 
    S_EQUAL_TO, 
    S_NOT_EQUAL_TO, 
    S_MULTIPLY, 
    S_DIVIDE, 
    S_ADD, 
    S_SUBTRACT, 
    S_ERROR, 
    S_GREATER_EQUAL_THAN, 
    S_LESSER_EQUAL_THAN,
    S_LBRACKET,
    S_RBRACKET,
    S_COMMA,
    S_EOF
};
static const char *states_names[] = {
    //Initial state
    "S_INITIAL", 
    //End of line
    "S_EOL",
    //State for identifiers and keywords
    "S_IDENTIFIER_READ",
    "S_POTENTIAL_IDENTIFIER_READ",
    "S_FUNCTION",
    //Stases for 0 and other integer values
    "S_INT_READ", 
    "S_INT_ZERO", 
    //Possible BASE extention states
    "S_INT_BIN0", 
    "S_INT_OCT", 
    "S_INT_HEX0",
    "S_INT_BIN1", 
    "S_INT_HEX1", 
    // States for reading real values
    // 0. 1.
    "S_DOUBLE_POINT",
    // 0.123 
    "S_DOUBLE_DEC_READ",
    // 0.123e 0.123E 123e 123E 
    "S_DOUBLE_EXP", 
    // 0.123E+ 123e-
    "S_DOUBLE_EXP_SIGN",
    // 0.123E+456 123e-456
    "S_DOUBLE_EXP_NUM",
    // Comment states
    "S_COMMENT_READ",
    "S_COMMENT_END",
    //String states
    "S_STR_READ",
    "S_STR_ESC",
    "S_STR_HEX",
    "S_STR_HEX1",
    "S_STR_END",
    //States for operators
    "S_LESSER_THAN",
    "S_LESSER_EQUAL_THAN",
    "S_GREATER_THAN",
    "S_GREATER_EQUAL_THAN",
    "S_ASSIGNMENT",
    "S_EQUAL_TO",
    "S_EXCLAMATION_MARK",
    "S_NOT_EQUAL_TO",
    "S_MULTIPLY",
    "S_DIVIDE",
    "S_ADD",
    "S_SUBTRACT",
    "S_LBRACKET",
    "S_RBRACKET",
    "S_COMMA",
    "S_EOF",
    //Error state
    "S_ERROR"
};
typedef enum {
    K_DEF,
    K_DO,
    K_ELSE,
    K_END,
    K_IF,
    K_NOT,
    K_NIL,
    K_THEN,
    K_WHILE
} Keyword_t;

static int no_read = 0;
static int read_char = 0;
static int char_from_newline = 0;
/**
 * Function will return converted int value, if value
 * is out of range the function will return -1
 * @param string String to convert
*/
static int HexStringToInt(const char *string)
{
    char *endptr;
    long int result = strtol(string+2, &endptr, 16);
    if(*endptr != '\0' || result > INT_MAX)
        return -1;
    return result;
}
/**
 * Function will return converted int value, if value
 * is out of range the function will return -1
 * @param string String to convert
*/
static int OctStringToInt(const char *string)
{
    char *endptr;
    long int result = strtol(string+1, &endptr, 8);
    if(*endptr != '\0' || result > INT_MAX)
        return -1;
    return result;
}

/**
 * Function will return converted int value, if value
 * is out of range the function will return -1
 * @param string String to convert
*/
static int BinStringToInt(const char *string)
{
    char *endptr;
    long int result = strtol(string+2, &endptr, 2);
    if(*endptr != '\0' || result > INT_MAX)
        return -1;
    return result;
}
/**
 * Function will return converted int value, if value
 * is out of range the function will return -1
 * @param string String to convert
*/
static int StringToInt(const char *string)
{
    char *endptr;
    long int result = strtol(string, &endptr, 10);
    if(*endptr != '\0' || result > INT_MAX)
        return -1;
    return result;
}
/**
 * Function converts string to doble if error occures the
 * function will return -1
 * @param string Imput string
*/
static double StringToDouble(const char *string)
{
    char *endptr;
    double result = strtod(string, &endptr);
    if(*endptr != '\0')
        return -1;
    return result;
}
/**
 * If input string is keyword then function will return
 * Integer representing keyword in enum Keyword_t
 * @param potential_keyword Input string to test
*/
static int IsKeyword(const char *potential_keyword)
{
    if(strcmp(potential_keyword, "def") == 0)
    {
        return TK_DEF;
    }
    else if(strcmp(potential_keyword, "do") == 0)
    {
        return TK_DO;
    }
    else if(strcmp(potential_keyword, "else") == 0)
    {
        return TK_ELSE;
    }
    else if(strcmp(potential_keyword, "end") == 0)
    {
        return TK_END;
    }
    else if(strcmp(potential_keyword, "if") == 0)
    {
        return TK_IF;
    }
    else if(strcmp(potential_keyword, "not") == 0)
    {
        return TK_NOT;
    }
    else if(strcmp(potential_keyword, "nil") == 0)
    {
        return TK_NIL;
    }
    else if(strcmp(potential_keyword, "then") == 0)
    {
        return TK_THEN;
    }
    else if(strcmp(potential_keyword, "while") == 0)
    {
        return TK_WHILE;
    }
    else
    {
        return -1;
    }
}
/**
 * Function will take given FINAL state and process DStr if needed,
 * It will return value of TokneType_t of processed token or -1 in
 * case of error.
 * @param token Token whre to save ew token info
 * @param DStr input DStr
 * @param state What was the fsm final state
 */
static int PocessToToken(Token_t *token, DStr_t *DStr, FSMState_t state)
{
    if(state == S_POTENTIAL_IDENTIFIER_READ || state == S_IDENTIFIER_READ || state == S_FUNCTION)
    {
        TokenKewordType_t keyword;
        if((keyword = IsKeyword(DStrStr(DStr))) != -1 && state == S_POTENTIAL_IDENTIFIER_READ)
        {
            token->type = T_KEYWORD;
            token->keywordType = keyword;
            return T_KEYWORD;
        }
        else
        {
            token->type = T_IDENTIFIER;
            return T_IDENTIFIER;
        }
    }
    else if(state == S_INT_ZERO)
    {
        token->type = T_INTEGER;
        token->intValue = 0;
        return T_INTEGER;
    }
    else if(state == S_INT_READ || state == S_INT_BIN1 || state == S_INT_OCT || state == S_INT_HEX1)
    {
        token->type = T_INTEGER;
        if(state == S_INT_READ && (token->intValue = StringToInt(DStrStr(DStr))) == -1)
            return -1;
        else if(state == S_INT_BIN1 && (token->intValue = BinStringToInt(DStrStr(DStr))) == -1)
            return -1;
        else if(state == S_INT_OCT && (token->intValue = OctStringToInt(DStrStr(DStr))) == -1)
            return -1;
        else if(state == S_INT_HEX1 && (token->intValue = HexStringToInt(DStrStr(DStr))) == -1)
            return -1;
        return T_INTEGER;
    }
    else if(state == S_DOUBLE_DEC_READ || state == S_DOUBLE_EXP_NUM)
    {
        token->type = T_DOUBLE;
        if((token->doubleValue = StringToDouble(DStrStr(DStr))) == -1)
            return -1;
        return T_DOUBLE;
    }
    else if(state == S_STR_END)
    {
        token->type = T_STRING;
        //Convert string to just values
        return T_STRING;
    }
    else if(state == S_LESSER_THAN || state == S_GREATER_THAN || state == S_ASSIGNMENT || state == S_EQUAL_TO || state == S_NOT_EQUAL_TO || state == S_MULTIPLY || state == S_DIVIDE || state == S_ADD || state == S_SUBTRACT || state == S_GREATER_EQUAL_THAN|| state == S_LESSER_EQUAL_THAN || state == S_LBRACKET || state == S_RBRACKET || state == S_COMMA)
    {
        token->type = T_OPERATION;
        if(state == S_LESSER_THAN)
            token->operationType = TO_LESSER_THAN;
        else if(state == S_GREATER_THAN)
            token->operationType = TO_GREATER_THAN;
        else if(state == S_GREATER_THAN)
            token->operationType = TO_GREATER_THAN;
        else if(state == S_ASSIGNMENT)
            token->operationType = TO_ASSIGNMENT;
        else if(state == S_EQUAL_TO)
            token->operationType = TO_EQUAL_TO;
        else if(state == S_NOT_EQUAL_TO)
            token->operationType = TO_NOT_EQUAL_TO;
        else if(state == S_MULTIPLY)
            token->operationType = TO_MULTIPLY;
        else if(state == S_DIVIDE)
            token->operationType = TO_DIVIDE;
        else if(state == S_ADD)
            token->operationType = TO_ADD;
        else if(state == S_SUBTRACT)
            token->operationType = TO_SUBTRACT;
        else if(state == S_GREATER_EQUAL_THAN)
            token->operationType = TO_GREATER_EQUAL_THAN;
        else if(state == S_LESSER_EQUAL_THAN)
            token->operationType = TO_LESSER_EQUAL_THAN;
        else if(state == S_LBRACKET)
            token->operationType = TO_LBRACKET;
        else if(state == S_RBRACKET)
            token->operationType = TO_RBRACKET;
        else if(state == S_COMMA)
            token->operationType = TO_COMMA;
        return T_OPERATION;
    }
    else if(state == S_EOL)
    {
        token->type = T_EOL;
        return T_EOL;
    }
    else if(state == S_EOF)
    {
        token->type = T_EOF;
        return T_EOF;
    }
    /*
    S_EOL,
    S_ERROR,
    */
}

/**
 * If state is final state function is going to return its 
 * index in the final_states else it is going to return -1
 * @param state State to check
*/
static int IsFinalState(FSMState_t state)
{
    for(int i = 0; i < sizeof(final_states)/sizeof(FSMState_t); i++)
    {
        if(final_states[i] == state)
        {
            return i;
        }
    }
    return -1;
}
/**
 * Function uses fsm to find out if scanned string from
 * input file is part of IFJ2018. If lexical error occures
 * it will return -1 if internal error occures -2 else 0
 * @param Input file to read from
 * @param DStr Pointer to dynamic string function shou use to
 * store read tokens
 * @param token pointer to token to store info about tokens
*/ 
int GetToken(FILE * Input, DStr_t **DStr, Token_t *token)
{
    //DebugFPuts("-------------- Get Token Start ------------\n", stdout);
    FSMState_t state = S_INITIAL;

    int stop = 0;

    DStrClear(*DStr);
    //DebugFPuts("Starting fsm...\n", stdout);
    while(!stop)
    {
        //DebugFPrintf(stdout, "Reading char: %c\n", (char)read_char);
        //DebugFPrintf(stdout, "State: %s -> ", states_names[state]);

        read_char = (no_read)?read_char:fgetc(Input);
        no_read = 0;

        if(DStrAddChar(DStr, read_char) == 0)
        {
            return GET_TOKEN_INT_ERR;
        }
        char_from_newline++;
        /*
         ______      ______     _         _
        |  ____|    / _____|   | \       / |
        | |        | /         |  \     /  |
        | |        | |         | \ \   / / |
        | |____    | \_____    | |\ \_/ /| |
        |  ____|    \____  \   | | \   / | |
        | |              \ |   | |  \_/  | |
        | |              | |   | |       | |
        | |         _____/ |   | |       | |
        |_|        |______/    |_|       |_|

        */
        switch(state)
        {

            /*
             ____       ______    _____     _______    _     _
            |  _ \     |  ____|  |  __ \   |__   __|  | |   | |
            | | \ \    | |       | |  \ \     | |     | |   | |
            | |  \ \   | |       | |   | |    | |     | |   | |
            | |   | |  | |____   | |__/ /     | |     | |___| |
            | |   | |  |  ____|  |  ___/      | |     |  ___  |
            | |   | |  | |       | |          | |     | |   | |
            | |  / /   | |       | |          | |     | |   | |
            | |_/ /    | |____   | |          | |     | |   | |
            |____/     |______|  |_|          |_|     |_|   |_|
               ____
              / __ \
             / /  \ \
            / /    \ \
            | |    | |
            | |    | |
            | |    | |
            \ \    / /
             \ \__/ /    _
              \____/    |_|

            */
            case S_INITIAL:
            {
                /*if (read_char == '\n')
                {
                    DStrDeleteLast(*DStr);
                    state = S_EOL;
                    stop = 1;
                }*/
                if (read_char == ' ' || read_char == '\t')
                {
                    DStrDeleteLast(*DStr);
                    state = S_INITIAL;
                }
                else if(read_char == -1)
                {
                    DStrDeleteLast(*DStr);
                    state = S_EOF;
                    stop = 1;
                }
                else if(read_char == '\n')
                {
                    DStrDeleteLast(*DStr);
                    char_from_newline = 0;
                    state = S_EOL;
                }
                else if(read_char == '=' && char_from_newline == 1)
                {
                    DStrDeleteLast(*DStr);
                    state = S_BLOCK_COMMENT0;
                }
                else if(read_char >= 'a' && read_char <= 'z')
                {
                    state = S_POTENTIAL_IDENTIFIER_READ;
                }
                else if(read_char == '_')
                {
                    state = S_IDENTIFIER_READ;
                }
                else if(read_char >= '1' && read_char <= '9')
                {
                    state = S_INT_READ;
                }
                else if(read_char == '0')
                {
                    state = S_INT_ZERO;
                }
                else if(read_char == '"')
                {
                    state = S_STR_READ;
                }
                else if(read_char == '#')
                {
                    state = S_COMMENT_READ;
                }
                else if(read_char == '<')
                {
                    state = S_LESSER_THAN;
                }
                else if(read_char == '>')
                {
                    state = S_GREATER_THAN;
                }
                else if(read_char == '=')
                {
                    state = S_ASSIGNMENT;
                }
                else if(read_char == '!')
                {
                    state = S_EXCLAMATION_MARK;
                }
                else if(read_char == '*')
                {
                    state = S_MULTIPLY;
                    stop = 1;
                }
                else if(read_char == '/')
                {
                    state = S_DIVIDE;
                    stop = 1;
                }
                else if(read_char == '+')
                {
                    state = S_ADD;
                    stop = 1;
                }
                else if(read_char == '-')
                {
                    state = S_SUBTRACT;
                    stop = 1;
                }
                else if(read_char == '(')
                {
                    state = S_LBRACKET;
                    stop = 1;
                }
                else if(read_char == ')')
                {
                    state = S_RBRACKET;
                    stop = 1;
                }
                else if(read_char == ',')
                {
                    state = S_COMMA;
                    stop = 1;
                }
                else
                {
                    state = S_ERROR;
                    stop = 1;
                }
                break;
            }
            /*
             ____       ______    _____     _______    _     _
            |  _ \     |  ____|  |  __ \   |__   __|  | |   | |
            | | \ \    | |       | |  \ \     | |     | |   | |
            | |  \ \   | |       | |   | |    | |     | |   | |
            | |   | |  | |____   | |__/ /     | |     | |___| |
            | |   | |  |  ____|  |  ___/      | |     |  ___  |
            | |   | |  | |       | |          | |     | |   | |
            | |  / /   | |       | |          | |     | |   | |
            | |_/ /    | |____   | |          | |     | |   | |
            |____/     |______|  |_|          |_|     |_|   |_|


             _____
            |_   _|
              | |
              | |
              | |
              | | 
              | |
              | |
             _| |_    _
            |_____|  |_|

            */
            case S_EOL:
            {
                DStrDeleteLast(*DStr);
                char_from_newline = 0;
                if(read_char == '\t' || read_char == ' ' || read_char == '\n')
                {
                    state = S_EOL;
                }
                else
                {
                    no_read = 1;
                    stop = 1;
                }
                break;
            }
            case S_POTENTIAL_IDENTIFIER_READ:
            {
                if(read_char >= 'a' && read_char <= 'z')
                {
                    state = S_POTENTIAL_IDENTIFIER_READ;
                }
                else if((read_char >= 'A' && read_char <= 'Z') || (read_char >= '0' && read_char <= '9'))
                {
                    state = S_IDENTIFIER_READ;
                }
                else if(read_char == '!' || read_char == '?')
                {
                    state = S_FUNCTION;
                }
                else
                {
                    no_read = 1;
                    stop = 1;
                }
                break;
            }
            case S_IDENTIFIER_READ:
            {
                if((read_char >= 'a' && read_char <= 'z') || (read_char >= 'A' && read_char <= 'Z') || (read_char >= '0' && read_char <= '9'))
                {
                    state = S_IDENTIFIER_READ;
                }
                else if(read_char == '!' || read_char == '?')
                {
                    state = S_FUNCTION;
                    stop = 1;
                }
                else
                {
                    no_read = 1;
                    stop = 1;
                }
                break;
            }
            case S_INT_READ:
            {
                if(read_char >= '0' && read_char <= '9')
                {
                    state = S_INT_READ;
                }
                else if(read_char == '.')
                {
                    state = S_DOUBLE_POINT;
                }
                else if(read_char == 'e' || read_char == 'E')
                {
                    state = S_DOUBLE_EXP;
                }
                else
                {
                    no_read = 1;
                    stop = 1;
                }
                break;
            }
            case S_INT_ZERO:
            {
                if(read_char == 'b')
                {
                    state = S_INT_BIN0;
                }
                else if(read_char >= '0' && read_char <= '7')
                {
                    state = S_INT_OCT;
                }
                else if(read_char == 'x')
                {
                    state = S_INT_HEX0;
                }
                else if (read_char >= '8' && read_char <= '9')
                {
                    state = S_ERROR;
                    stop = 1;
                }
                else if(read_char == '.')
                {
                    state = S_DOUBLE_POINT;
                }
                else
                {
                    no_read = 1;
                    stop = 1;
                }
                break;
            }
            case S_STR_READ:
            {
                if(read_char != '\n' && read_char != '"' && read_char != '\\')
                {
                    state = S_STR_READ;
                }
                else if(read_char == '"')
                {
                    state = S_STR_END;
                    stop = 1;
                }
                else if(read_char == '\\')
                {
                    state = S_STR_ESC;
                }
                else
                {
                    state = S_ERROR;
                    stop = 1;
                }
                break;
            }
            case S_COMMENT_READ:
            {
                if(read_char != '\n')
                {
                    state = S_COMMENT_READ;
                }
                else
                {
                    DStrClear(*DStr);
                    state = S_INITIAL;
                }
                break;
            }
            case S_LESSER_THAN:
            {
                if(read_char == '=')
                {
                    state = S_LESSER_EQUAL_THAN;
                    stop = 1;
                }
                else
                {
                    no_read = 1;
                    stop = 1;
                }
                break;
            }
            case S_GREATER_THAN:
            {
                if(read_char == '=')
                {
                    state = S_GREATER_EQUAL_THAN;
                    stop = 1;
                }
                else
                {
                    no_read = 1;
                    stop = 1;
                }
                break;
            }
            case S_ASSIGNMENT:
            {
                if(read_char == '=')
                {
                    state = S_EQUAL_TO;
                    stop = 1;
                }
                else
                {
                    no_read = 1;
                    stop = 1;
                }
                break;
            }
            case S_EXCLAMATION_MARK:
            {
                if(read_char == '=')
                {
                    state = S_NOT_EQUAL_TO;
                    stop = 1;
                }
                else
                {
                    no_read = 1;
                    stop = 1;
                }
                break;
            }
            /*
             ____       ______    _____     _______    _     _
            |  _ \     |  ____|  |  __ \   |__   __|  | |   | |
            | | \ \    | |       | |  \ \     | |     | |   | |
            | |  \ \   | |       | |   | |    | |     | |   | |
            | |   | |  | |____   | |__/ /     | |     | |___| |
            | |   | |  |  ____|  |  ___/      | |     |  ___  |
            | |   | |  | |       | |          | |     | |   | |
            | |  / /   | |       | |          | |     | |   | |
            | |_/ /    | |____   | |          | |     | |   | |
            |____/     |______|  |_|          |_|     |_|   |_|

             _____   _____
            |_   _| |_   _|
              | |     | |
              | |     | |
              | |     | |
              | |     | |
              | |     | |
              | |     | |
             _| |_   _| |_    _
            |_____| |_____|  |_|

            */

            case S_DOUBLE_POINT:
            {
                if(read_char >= '0' && read_char <= '9')
                {
                    state = S_DOUBLE_DEC_READ;
                }
                else
                {
                    state = S_ERROR;
                    stop = 1;
                }
                break;
            }
            case S_DOUBLE_EXP:
            {
                if(read_char >= '0' && read_char <= '9')
                {
                    state = S_DOUBLE_EXP_NUM;
                }
                else if(read_char == '+' || read_char == '-')
                {
                    state = S_DOUBLE_EXP_SIGN;
                }
                else
                {
                    state = S_ERROR;
                    stop = 1;
                }
                break;
            }
            case S_INT_BIN0:
            {
                if(read_char >= '0' && read_char <= '1')
                {
                    state = S_INT_BIN1;
                }
                else
                {
                    state = S_ERROR;;
                    stop = 1;
                }
                break;
            }
            case S_INT_OCT:
            {
                if(read_char >= '0' && read_char <= '7')
                {
                    state = S_INT_OCT;
                }
                else
                {
                    no_read = 1;
                    stop = 1;
                }
                break;
            }
            case S_INT_HEX0:
            {
                if((read_char >= '0' && read_char <= '9') || (read_char >= 'a' && read_char <= 'f') || (read_char >= 'A' && read_char <= 'F'))
                {
                    state = S_INT_HEX1;
                }
                else
                {
                    state = S_ERROR;
                    stop = 1;
                }
                break;
            }
            case S_STR_ESC:
            {
                if(read_char == '"' || read_char == 'n' || read_char == 's' || read_char == 't' || read_char == '\\')
                {
                    state = S_STR_READ;
                }
                else if (read_char == 'x')
                {
                    state = S_STR_HEX;
                }
                else
                {
                    state = S_ERROR;
                    stop = 1;
                }
                break;
            }
           /*
             ____       ______    _____     _______    _     _
            |  _ \     |  ____|  |  __ \   |__   __|  | |   | |
            | | \ \    | |       | |  \ \     | |     | |   | |
            | |  \ \   | |       | |   | |    | |     | |   | |
            | |   | |  | |____   | |__/ /     | |     | |___| |
            | |   | |  |  ____|  |  ___/      | |     |  ___  |
            | |   | |  | |       | |          | |     | |   | |
            | |  / /   | |       | |          | |     | |   | |
            | |_/ /    | |____   | |          | |     | |   | |
            |____/     |______|  |_|          |_|     |_|   |_|

             _____   _____   _____
            |_   _| |_   _| |_   _|
              | |     | |     | |
              | |     | |     | |
              | |     | |     | |
              | |     | |     | |
              | |     | |     | |
              | |     | |     | |
             _| |_   _| |_   _| |_    _
            |_____| |_____| |_____|  |_|

            */
            case S_DOUBLE_DEC_READ:
            {
                if(read_char >= '0' && read_char <= '9')
                {
                    state = S_DOUBLE_DEC_READ;
                }
                else if(read_char == 'e' || read_char == 'E')
                {
                    state = S_DOUBLE_EXP;
                }
                else
                {
                    no_read = 1;
                    stop = 1;
                }
                break;
            }
            case S_DOUBLE_EXP_NUM:
            {
                if(read_char >= '0' && read_char <= '9')
                {
                    state = S_DOUBLE_EXP_NUM;
                }
                else
                {
                    no_read = 1;
                    stop = 1;
                }
                break;
            }
            case S_DOUBLE_EXP_SIGN:
            {
                if(read_char >= '0' && read_char <= '9')
                {
                    state = S_DOUBLE_EXP_NUM;
                }
                else
                {
                    state = S_ERROR;
                    stop = 1;
                }
                break;
            }
            case S_INT_BIN1:
            {
                if(read_char >= '0' && read_char <= '1')
                {
                    state = S_INT_BIN1;
                }
                else
                {
                    no_read = 1;
                    stop = 1;
                }
                break;
            }
            case S_INT_HEX1:
            {
                if((read_char >= '0' && read_char <= '9') || (read_char >= 'a' && read_char <= 'f') || (read_char >= 'A' && read_char <= 'F'))
                {
                    state = S_INT_HEX1;
                }
                else
                {
                    no_read = 1;
                    stop = 1;
                }
                break;
            }
            case S_STR_HEX:
            {
                if((read_char >= '0' && read_char <= '9') || (read_char >= 'a' && read_char <= 'f') || (read_char >= 'A' && read_char <= 'F'))
                {
                    state = S_STR_HEX1;
                }
                else
                {
                    state = S_ERROR;
                    stop = 1;
                }
                break;
            }
            /*
             ____       ______    _____     _______    _     _   
            |  _ \     |  ____|  |  __ \   |__   __|  | |   | |  
            | | \ \    | |       | |  \ \     | |     | |   | |  | |   | |
            | |  \ \   | |       | |   | |    | |     | |   | |  | |   | |
            | |   | |  | |____   | |__/ /     | |     | |___| |  | |   | |
            | |   | |  |  ____|  |  ___/      | |     |  ___  |  | |   | |
            | |   | |  | |       | |          | |     | |   | |  | |   | |
            | |  / /   | |       | |          | |     | |   | |   
            | |_/ /    | |____   | |          | |     | |   | |    
            |____/     |______|  |_|          |_|     |_|   |_|     

             _____    _     _
            |_   _|  | |   | |
              | |    | |   | |
              | |    | |   | |
              | |    | |   | |
              | |    | |   | |
              | |    | |   | |
              | |     \ \ / /
             _| |_     \ V /   _
            |_____|     \_/   |_|

            */
            case S_STR_HEX1:
            {
                if((read_char >= '0' && read_char <= '9') || (read_char >= 'a' && read_char <= 'f') || (read_char >= 'A' && read_char <= 'F'))
                {
                    state = S_STR_READ;
                }
                else
                {
                    state = S_ERROR;
                    stop = 1;
                }
                break;
            }


            //block Comment
            case S_BLOCK_COMMENT0:
            {
                DStrDeleteLast(*DStr);
                if(read_char == 'b')
                {
                    state = S_BLOCK_COMMENT1;
                }
                else
                {
                    state = S_ERROR;
                    stop = 1;
                }
                break;
            }
            case S_BLOCK_COMMENT1:
            {
                DStrDeleteLast(*DStr);
                if(read_char == 'e')
                {
                    state = S_BLOCK_COMMENT2;
                }
                else
                {
                    state = S_ERROR;
                    stop = 1;
                }
                break;
            }
            case S_BLOCK_COMMENT2:
            {
                DStrDeleteLast(*DStr);
                if(read_char == 'g')
                {
                    state = S_BLOCK_COMMENT3;
                }
                else
                {
                    state = S_ERROR;
                    stop = 1;
                }
                break;
            }
            case S_BLOCK_COMMENT3:
            {
                DStrDeleteLast(*DStr);
                if(read_char == 'i')
                {
                    state = S_BLOCK_COMMENT4;
                }
                else
                {
                    state = S_ERROR;
                    stop = 1;
                }
                break;
            }
            case S_BLOCK_COMMENT4:
            {
                DStrDeleteLast(*DStr);
                if(read_char == 'n')
                {
                    state = S_BLOCK_COMMENT5;
                }
                else
                {
                    state = S_ERROR;
                    stop = 1;
                }
                break;
            }
            case S_BLOCK_COMMENT5:
            {
                DStrDeleteLast(*DStr);
                if(read_char == '\n')
                {
                    state = S_BLOCK_COMMENT_END0;
                }
                else
                {
                    state = S_ERROR;
                    stop = 1;
                }
                break;
            }
            case S_BLOCK_COMMENT:
            {
                DStrDeleteLast(*DStr);
                if(read_char == -1)
                {
                    state = S_ERROR;
                    stop = 1;
                }
                else if(read_char == '\n')
                {
                    state = S_BLOCK_COMMENT_END0;
                }
                break;
            }
            case S_BLOCK_COMMENT_END0:
            {
                DStrDeleteLast(*DStr);
                if(read_char == '=')
                {
                    state = S_BLOCK_COMMENT_END1;
                }
                else if(read_char == -1)
                {
                    state = S_ERROR;
                    stop = 1;
                }
                else
                {
                    state = S_BLOCK_COMMENT;
                }
                break;
            }
            case S_BLOCK_COMMENT_END1:
            {
                DStrDeleteLast(*DStr);
                if(read_char == 'e')
                {
                    state = S_BLOCK_COMMENT_END2;
                }
                else if(read_char == -1)
                {
                    state = S_ERROR;
                    stop = 1;
                }
                else
                {
                    state = S_BLOCK_COMMENT;
                }
                break;
            }
            case S_BLOCK_COMMENT_END2:
            {
                DStrDeleteLast(*DStr);
                if(read_char == 'n')
                {
                    state = S_BLOCK_COMMENT_END3;
                }
                else if(read_char == -1)
                {
                    state = S_ERROR;
                    stop = 1;
                }
                else
                {
                    state = S_BLOCK_COMMENT;
                }
                break;
            }
            case S_BLOCK_COMMENT_END3:
            {
                DStrDeleteLast(*DStr);
                if(read_char == 'd')
                {
                    state = S_BLOCK_COMMENT_END4;
                }
                else if(read_char == -1)
                {
                    state = S_ERROR;
                    stop = 1;
                }
                else
                {
                    state = S_BLOCK_COMMENT;
                }
                break;
            }
            case S_BLOCK_COMMENT_END4:
            {
                DStrDeleteLast(*DStr);
                if(read_char == '\n')
                {
                    state = S_BLOCK_COMMENT_END4;
                }
                else if(read_char == -1)
                {
                    state = S_EOF;
                    stop = 1;
                }
                else
                {
                    state = S_INITIAL;
                    no_read = 1;
                }
                break;
            }
        }
        //DebugFPuts(states_names[state], stdout);
        //DebugFPuts("\n", stdout);
    }

    if(no_read == 1)
    {
       DStrDeleteLast(*DStr);
    }

    //DebugFPrintf(stdout," <- [%s] \n", states_names[state]);
    //DebugFPrintf(stdout,"Token: %s\n", DStrStr(*DStr));
    //DebugFPuts("--------------- Get Token End -------------\n", stdout);
    
    if(state == S_ERROR)
        return GET_TOKEN_LEX_ERR;
    
    return PocessToToken(token, *DStr, state);
}
