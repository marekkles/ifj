#include "token.h"
#include "dstr.h"
#include "return.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#ifndef CODEGEN_H

#define CODEGEN_H

typedef enum {
    MOVE,
    CREATEFRAME,
    PUSHFRAME,
    POPFRAME,
    DEFVAR,
    CALL,
    RETURN,
    PUSHS,
    POPS,
    CLEARS,
    ADD,
    SUB,
    MUL,
    DIV,
    IDIV,
    ADDS,
    SUBS,
    MULS,
    DIVS,
    IDIVS,
    LT,
    GT,
    EQ,
    LTS,
    GTS,
    EQS,
    AND,
    OR,
    NOT,
    ANDS,
    ORS,
    NOTS,
    INT2FLOAT,
    INT2CHAR,
    STRI2INT,
    INT2FLOATS,
    FLOAT2INTS,
    INT2CHARS,
    STRI2INTS,
    READ,
    WRITE,
    CONCAT,
    STRLEN,
    GETCHAR,
    SETCHAR,
    TYPE,
    LABEL,
    JUMP,
    JUMPIFEQ,
    JUMPIFNEQ,
    JUMPIFEQS,
    JUMPIFNEQS,
    EXIT,
    BREAK,
    DPRINT
} CodeInstructions_t;

typedef struct {
    DStr_t *functionVariableList;
    DStr_t *functionBody;
    DStr_t *mainVariableList;
    DStr_t *mainBody;

    bool inFunction;
    int functionParameterCount;

    int whileCount;
    int ifCount;
} CodeInstructionList_t;


int CodeAddFunctionStart(const char *label);
int CodeAddFunctionEnd(void);
int CodeAddFunctionParameter(const char *parameterName);
int CodeInitialize(void);
void CodeFinalize(void);


int CodeAddString(const char *str);
int CodeAddInt(int intValue);
int CodeAddDouble(double doubleValue);
int CodeAddVariable(const char *str);


int CodeAddInstruction(CodeInstructions_t instructionCode);
int CodeAddLabel(const char *labelName);
int CodeAddFunctionCallStart();
int CodeAddFunctionCallParameter();
int CodeAddFunctionCallString(const char *str);
int CodeAddFunctionCallInt(int intValue);
int CodeAddFunctionCallDouble(double doubleValue);
int CodeAddFunctionCallVariable(const char *str);
int CodeAddFunctionCall(const char *functionName);
int CodeAddTextToBody(const char *text);
int CodeAddTextToVariables(const char *text);
int CodeAddInstruction(CodeInstructions_t instructionCode);
int CodeDeclareVariable(const char *variableName);

#endif