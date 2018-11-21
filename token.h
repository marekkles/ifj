
#ifndef TOKEN_H

#define TOKEN_H

enum {IDENTIFIER, ASSIGNMENT, INTEGER, REAL, PLUS, MINUS, DIVIDE, MULTIPLY};
extern const char * TokenNames[];

typedef struct {
    int type;
    char * atribute;
} Token_t;

#endif
