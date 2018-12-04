
/**
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                     *
 *                  Projekt IFJ                        *
 *                                                     *
 * Implementace překladače imperativního jazyka IFJ18. *
 *                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  
                       Súbor:
                      token.h
                      
                       Popis:
     Hlavickovy subor obsahuje definiciu typu Token_t
      pouzivanych v parser a fsm, definuje aj pomocne
typy pouzivane v Token_t (TokenType_t, TokenOperationType_t
                TokenKewordType_t)

                      Autori:
               Marek Vaško (xvasko16)
              Handzuš Daniel (xhandz01)
                Alexaj Adam (xalexa07)
*/

#ifndef TOKEN_H

#define TOKEN_H

typedef enum {
    T_IDENTIFIER,
    T_INTEGER,
    T_DOUBLE,
    T_STRING,
    T_KEYWORD,
    T_OPERATION,
    T_EOL,
    T_EOF
} TokenType_t;

typedef enum {
    TK_DEF,
    TK_DO,
    TK_ELSE,
    TK_END,
    TK_IF,
    TK_NOT,
    TK_NIL,
    TK_THEN,
    TK_WHILE
} TokenKewordType_t;

typedef enum {
    TO_LESSER_THAN, 
    TO_GREATER_THAN, 
    TO_ASSIGNMENT, 
    TO_EQUAL_TO, 
    TO_NOT_EQUAL_TO, 
    TO_MULTIPLY, 
    TO_DIVIDE, 
    TO_ADD, 
    TO_SUBTRACT, 
    TO_GREATER_EQUAL_THAN, 
    TO_LESSER_EQUAL_THAN,
    TO_LBRACKET,
    TO_RBRACKET,
    TO_COMMA
} TokenOperationType_t;

typedef struct {
    TokenType_t type;
    union{
        TokenKewordType_t keywordType;
        TokenOperationType_t operationType;
        int intValue;
        double doubleValue;
    };
} Token_t;

#endif
