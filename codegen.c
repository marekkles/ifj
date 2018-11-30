#include "codegen.h"

CodeInstructionList_t *instructionList;

static void CodeInstructionListClear(CodeInstructionList_t *codeInstructionList)
{
    DStrClear(codeInstructionList->functionBody);
    DStrClear(codeInstructionList->functionVariableList);
    DStrClear(codeInstructionList->mainBody);
    DStrClear(codeInstructionList->mainVariableList);
    codeInstructionList->inFunction = false;
    codeInstructionList->ifCount = 0;
    codeInstructionList->whileCount = 0;
}

static void CodeInstructionListFree(CodeInstructionList_t **codeInstructionList)
{
    if((*codeInstructionList)->functionBody != NULL)
        free((*codeInstructionList)->functionBody);
    if((*codeInstructionList)->functionVariableList != NULL)
        free((*codeInstructionList)->functionVariableList);
    if((*codeInstructionList)->mainBody != NULL)
        free((*codeInstructionList)->mainBody);
    if((*codeInstructionList)->mainVariableList != NULL)
        free((*codeInstructionList)->mainVariableList);
    free(*codeInstructionList);
    *codeInstructionList = NULL;
}

static void CodeInstructionListAllocate(CodeInstructionList_t **codeInstructionList)
{
    int error_occured = 0;
    (*codeInstructionList) = malloc(sizeof(CodeInstructionList_t));
    if((*codeInstructionList) == NULL)
        return;
    DStrInit(&((*codeInstructionList)->functionBody), DSTR_DEFAULT_SIZE);
    DStrInit(&((*codeInstructionList)->functionVariableList), DSTR_DEFAULT_SIZE);
    DStrInit(&((*codeInstructionList)->mainBody), DSTR_DEFAULT_SIZE);
    DStrInit(&((*codeInstructionList)->mainVariableList), DSTR_DEFAULT_SIZE);
    error_occured = (*codeInstructionList)->functionBody == NULL;
    error_occured = (*codeInstructionList)->functionVariableList == NULL;
    error_occured = (*codeInstructionList)->mainBody == NULL;
    error_occured = (*codeInstructionList)->mainVariableList == NULL;
    if(error_occured)
        CodeInstructionListFree(codeInstructionList);
    CodeInstructionListClear(*codeInstructionList);
}

int CodeInitialize(void)
{
    fputs( ""\
    ".IFJcode18\n"\
    "JUMP $$main\n"\
    , stdout); 
    const char MainBodyBegin[] = ""\
    "\nLABEL $$main\n"\
    "CREATEFRAME\n"\
    "PUSHFRAME\n"\
    ;
    //Alocate space for istructions
    CodeInstructionListAllocate(&instructionList);
    if(instructionList == NULL)
        return PARSE_INT_ERR;

    if(!DStrCat(&(instructionList->mainVariableList), MainBodyBegin))
    {
        CodeInstructionListFree(&instructionList);
        return PARSE_INT_ERR;
    }

    return PARSE_OK;
}
void CodeFinalize(void)
{
    if(instructionList == NULL)
        return;
    fputs(DStrStr(instructionList->mainVariableList), stdout);
    fputs("\n", stdout);
    fputs(DStrStr(instructionList->mainBody), stdout);
    fputs("\n", stdout);
    CodeInstructionListFree(&instructionList);
}
int CodeAddFunctionStart(const char *label)
{
    instructionList->inFunction = true;
    instructionList->functionParameterCount = 0;
    fputs("\nLABEL $", stdout);
    fputs(label, stdout);
    fputs("\nPUSHFRAME\n\n"
         "DEFVAR LF@%return\n"
         "MOVE LF@%return nil@nil\n\n", stdout);
    return PARSE_OK;
}
int CodeAddFunctionEnd(void)
{
    instructionList->inFunction = false;
    puts(DStrStr(instructionList->functionVariableList));
    puts(DStrStr(instructionList->functionBody));
    puts("\nPOPFRAME\n"
         "RETURN\n");
    return PARSE_OK;
}

int CodeAddFunctionParameter(const char *parameterName)
{
    instructionList->functionParameterCount++;
    printf("DEFVAR LF@%s\nMOVE LF@%s LF@%%%d\n", parameterName, parameterName, instructionList->functionParameterCount);
    return PARSE_OK;
}

int CodeAddFunctionCallStart()
{
    instructionList->functionParameterCount = 0;
    return CodeAddInstruction(CREATEFRAME);
}

int CodeAddFunctionCallParameter()
{
    int return_value = PARSE_OK;
    char int_to_string[21] = {0,};
    instructionList->functionParameterCount++;
    sprintf(int_to_string, "%d", instructionList->functionParameterCount);

    if((return_value = CodeAddTextToBody("\nDEFVAR TF@%")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(int_to_string)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody("\nMOVE TF@%")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(int_to_string) )!= PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    return return_value;
}

int CodeAddString(const char *str)
{
    int return_value = PARSE_OK;
    if((return_value = CodeAddTextToBody("string@")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(str)) != PARSE_OK)
        return return_value;
    return return_value;
} 
int CodeAddInt(int intValue)
{
    char int_to_string[21] = {0,};
    sprintf(int_to_string, "%d", intValue);
    int return_value = PARSE_OK;
    if((return_value = CodeAddTextToBody("int@")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(int_to_string)) != PARSE_OK)
        return return_value;
    return return_value;
}
int CodeAddDouble(double doubleValue)
{
    char double_to_string[32] = {0,};
    sprintf(double_to_string, "%a", doubleValue);
    int return_value = PARSE_OK;
    if((return_value = CodeAddTextToBody("float@")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(double_to_string)) != PARSE_OK)
        return return_value;
    return return_value;
}
int CodeAddVariable(const char *str)
{
    int return_value = PARSE_OK;
    if((return_value = CodeAddTextToBody("LF@")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(str)) != PARSE_OK)
        return return_value;
    return return_value;
}

int CodeAddFunctionCallString(const char *str)
{
    int return_value = PARSE_OK;
    if((return_value = CodeAddFunctionCallParameter()) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddString(str)) != PARSE_OK)
        return return_value;
    return return_value;
}
int CodeAddFunctionCallInt(int intValue)
{
    int return_value = PARSE_OK;
    if((return_value = CodeAddFunctionCallParameter()) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddInt(intValue)) != PARSE_OK)
        return return_value;
    return return_value;
}
int CodeAddFunctionCallDouble(double doubleValue)
{
    int return_value = PARSE_OK;
    if((return_value = CodeAddFunctionCallParameter()) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddDouble(doubleValue)) != PARSE_OK)
        return return_value;
    return return_value;
}
int CodeAddFunctionCallVariable(const char *str)
{
    int return_value = PARSE_OK;
    if((return_value = CodeAddFunctionCallParameter()) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddVariable(str)) != PARSE_OK)
        return return_value;
    return return_value;
}
int CodeAddFunctionCall(const char *functionName)
{
    int return_value = PARSE_OK;
    if((return_value = CodeAddTextToBody("\nCALL $")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(functionName)) != PARSE_OK)
        return return_value;
    return return_value;
}

int CodeGetUniqueWhile()
{
    instructionList->whileCount++;
    return instructionList->whileCount;
}
int CodeGetUniqueIf()
{
    instructionList->ifCount++;
    return instructionList->ifCount;
}

int CodeAddWhileStart(int uniqueWhileNumber)
{
    int return_value = PARSE_OK;
    char int_to_string[21] = {0,};
    sprintf(int_to_string, "%d", uniqueWhileNumber);

    if((return_value = CodeAddTextToBody("\n#While Loop\nLABEL $$while")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(int_to_string)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody("start")) != PARSE_OK)
        return return_value;
    return return_value;
    
}
int CodeAddWhileBody(int uniqueWhileNumber)
{
    int return_value;
    if((return_value = CodeAddTextToBody("\n#While Loop Body")) != PARSE_OK)
        return return_value;
    return return_value;
}
int CodeAddWhileEnd(int uniqueWhileNumber)
{
    int return_value = PARSE_OK;
    char int_to_string[21] = {0,};
    sprintf(int_to_string, "%d", uniqueWhileNumber);
    if((return_value = CodeAddTextToBody("\nJUMP $$while")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(int_to_string)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody("start")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody("\nLABEL $$while")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(int_to_string)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody("end")) != PARSE_OK)
        return return_value;
    return return_value;
}

int CodeAddIfStart(int uniqueIfNumber)
{
    int return_value = PARSE_OK;

    if((return_value = CodeAddTextToBody("\n#If Start")) != PARSE_OK)
        return return_value;
    return return_value;
}
int CodeAddIfElse(int uniqueIfNumber)
{
    int return_value = PARSE_OK;
    char int_to_string[21] = {0,};
    sprintf(int_to_string, "%d", uniqueIfNumber);

    if((return_value = CodeAddTextToBody("\n#If Else\nLABEL $$if")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(int_to_string)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody("else")) != PARSE_OK)
        return return_value;
    return return_value;
}
int CodeAddIfEnd(int uniqueIfNumber)
{
    int return_value = PARSE_OK;
    char int_to_string[21] = {0,};
    sprintf(int_to_string, "%d", uniqueIfNumber);

    if((return_value = CodeAddTextToBody("\n#If End\nLABEL $$if")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(int_to_string)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody("end")) != PARSE_OK)
        return return_value;
    return return_value;
}


int CodeAddTextToBody(const char *text)
{
    if(instructionList->inFunction == 1)
    {
        if(!DStrCat(&(instructionList->functionBody), text))
            return PARSE_INT_ERR;
    }
    else
    {
        if(!DStrCat(&(instructionList->mainBody), text))
            return PARSE_INT_ERR;
    }
    return PARSE_OK;
}
int CodeAddTextToVariables(const char *text)
{
    if(instructionList->inFunction == 1)
    {
        if(!DStrCat(&(instructionList->functionVariableList), text))
            return PARSE_INT_ERR;
    }
    else
    {
        if(!DStrCat(&(instructionList->mainVariableList), text))
            return PARSE_INT_ERR;
    }
    return PARSE_OK;
}
int CodeAddInstruction(CodeInstructions_t instructionCode)
{
    int return_value = PARSE_OK;
    const char * instructionNames[] = {
        "MOVE",
        "CREATEFRAME",
        "PUSHFRAME",
        "POPFRAME",
        "DEFVAR",
        "CALL",
        "RETURN",
        "PUSHS",
        "POPS",
        "CLEARS",
        "ADD",
        "SUB",
        "MUL",
        "DIV",
        "IDIV",
        "ADDS",
        "SUBS",
        "MULS",
        "DIVS",
        "IDIVS",
        "LT",
        "GT",
        "EQ",
        "LTS",
        "GTS",
        "EQS",
        "AND",
        "OR",
        "NOT",
        "ANDS",
        "ORS",
        "NOTS",
        "INT2FLOAT",
        "INT2CHAR",
        "STRI2INT",
        "INT2FLOATS",
        "FLOAT2INTS",
        "INT2CHARS",
        "STRI2INTS",
        "READ",
        "WRITE",
        "CONCAT",
        "STRLEN",
        "GETCHAR",
        "SETCHAR",
        "TYPE",
        "LABEL",
        "JUMP",
        "JUMPIFEQ",
        "JUMPIFNEQ",
        "JUMPIFEQS",
        "JUMPIFNEQS",
        "EXIT",
        "BREAK",
        "DPRINT"
    };

    if((return_value = CodeAddTextToBody("\n")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(instructionNames[instructionCode])) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    return return_value;
}
int CodeDeclareVariable(const char *variableName)
{
    int return_value = PARSE_OK;
    if((return_value = CodeAddTextToVariables("\nDEFVAR LF@")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToVariables(variableName)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToVariables("\nMOVE LF@")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToVariables(variableName)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToVariables(" nil@nil")) != PARSE_OK)
        return return_value;
    return return_value;

}

