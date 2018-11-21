#include "token.h"
typedef enum {
    T_IDENTIFIER,
    T_INTEGER,
    T_DOUBLE,
    T_STRING,
    T_KEYWORD,
    T_OPERATION,
    T_EOL
} TokenType_t;
const char * TokenNames[] = {"IDENTIFIER", "ASSIGNMENT", "INTEGER", "REAL", "PLUS", "MINUS", "DIVIDE", "MULTIPLY"};