/**
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                     *
 *                  Projekt IFJ                        *
 *                                                     *
 * Implementace překladače imperativního jazyka IFJ18. *
 *                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  
                       Súbor:
                      codegen.c
                      
                       Popis:
     Implementacia generovania kodu, praca so strukturou
     CodeInstructionList_t, generovanie zaciatku programu,
     koncu prgramu, vstavanych funkcii, pridavanie parametrov
     funkcii, deklaracia premennych ...

                      Autori:
               Marek Vaško (xvasko16)
              Handzuš Daniel (xhandz01)
                Alexaj Adam (xalexa07)
*/

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

void CodeAddBuiltInFunctions()
{
    const char * substr=""\
        "\nLABEL $substr\n"\
        "PUSHFRAME\n"\
        "DEFVAR LF@%return\n"\
        "MOVE LF@%return string@\n"\
        "DEFVAR LF@%condition\n"\
        "MOVE LF@%condition nil@nil\n"\
        "DEFVAR LF@%type\n"\
        "DEFVAR LF@s\n"\
        "MOVE LF@s LF@%1\n"\
        "DEFVAR LF@i\n"\
        "MOVE LF@i LF@%2\n"\
        "DEFVAR LF@n\n"\
        "MOVE LF@n LF@%3\n"\
        "TYPE LF@%type LF@%1\n"\
        "JUMPIFNEQ $$compatibilityError LF@%type string@string\n"\
        "TYPE LF@%type LF@%2\n"\
        "JUMPIFNEQ $$compatibilityError LF@%type string@int\n"\
        "TYPE LF@%type LF@%3\n"\
        "JUMPIFNEQ $$compatibilityError LF@%type string@int\n"\
        "#Get length of string\n"\
        "DEFVAR LF@stringLength\n"\
        "STRLEN LF@stringLength LF@s\n"\
        "DEFVAR LF@lastIndex\n"\
        "MOVE LF@lastIndex LF@i\n"\
        "ADD LF@lastIndex LF@lastIndex LF@n\n"\
        "#Check if index is in range\n"\
        "LT LF@%condition LF@i int@0\n"\
        "JUMPIFEQ $$substrErr  LF@%condition bool@true\n"\
        "LT LF@%condition LF@i LF@stringLength\n"\
        "JUMPIFEQ $$substrErr  LF@%condition bool@false\n"\
        "LT LF@%condition LF@n int@0\n"\
        "JUMPIFEQ $$substrErr  LF@%condition bool@true\n"\
        "#Main loop\n"\
        "DEFVAR LF@iterator\n"\
        "MOVE LF@iterator LF@i\n"\
        "DEFVAR LF@currentChar\n"\
        "LABEL $$substrLoopStart\n"\
        "#End loop if iterator >= stringLength || iterator >= lastIndex \n"\
        "LT LF@%condition LF@iterator LF@stringLength\n"\
        "JUMPIFEQ $$substrReturn LF@%condition bool@false\n"\
        "LT LF@%condition LF@iterator LF@lastIndex\n"\
        "JUMPIFEQ $$substrReturn LF@%condition bool@false\n"\
        "#Concatenate with char at index iterator\n"\
        "GETCHAR LF@currentChar LF@s LF@iterator\n"\
        "CONCAT LF@%return LF@%return LF@currentChar\n"\
        "#Increment iterator\n"\
        "ADD LF@iterator LF@iterator int@1\n"\
        "JUMP $$substrLoopStart\n"\
        "LABEL $$substrReturn\n"\
        "POPFRAME\n"\
        "RETURN\n"\
        "LABEL $$substrErr\n"\
        "MOVE LF@%return nil@nil\n"\
        "POPFRAME\n"\
        "RETURN\n";
    const char * ord = ""\
        "\nLABEL $ord\n"\
        "PUSHFRAME\n"\
        "DEFVAR LF@%return\n"\
        "MOVE LF@%return nil@nil\n"\
        "DEFVAR LF@%type\n"\
        "DEFVAR LF@%condition\n"\
        "TYPE LF@%type LF@%1\n"\
        "JUMPIFNEQ $$compatibilityError LF@%type string@string\n"\
        "TYPE LF@%type LF@%2\n"\
        "JUMPIFNEQ $$compatibilityError LF@%type string@int\n"\
        "DEFVAR LF@slen\n"\
        "STRLEN LF@slen LF@%1\n"\
        "LT LF@%condition LF@%2 LF@slen\n"\
        "JUMPIFEQ $$ordReturn LF@%condition bool@false\n"\
        "GT LF@%condition LF@%2 int@-1\n"\
        "JUMPIFEQ $$ordReturn LF@%condition bool@false\n"\
        "STRI2INT LF@%return LF@%1 LF@%2\n"\
        "LABEL $$ordReturn\n"\
        "POPFRAME\n"\
        "RETURN\n";
    const char * chr = ""\
        "\nLABEL $chr\n"\
        "PUSHFRAME\n"\
        "DEFVAR LF@%return\n"\
        "DEFVAR LF@%type\n"\
        "MOVE LF@%return nil@nil\n"\
        "DEFVAR LF@%condition\n"\
        "TYPE LF@%type LF@%1\n"\
        "JUMPIFNEQ $$compatibilityError LF@%type string@int\n"\
        "GT LF@%condition LF@%1 int@-1\n"\
        "JUMPIFEQ $$stringError LF@%condition bool@false\n"\
        "LT LF@%condition LF@%1 int@256\n"\
        "JUMPIFEQ $$stringError LF@%condition bool@false\n"\
        "INT2CHAR LF@%return LF@%1\n"\
        "POPFRAME\n"\
        "RETURN\n";
    const char * length = ""\
        "\nLABEL $length\n"\
        "PUSHFRAME\n"\
        "DEFVAR LF@%return\n"\
        "DEFVAR LF@%type\n"\
        "MOVE LF@%return nil@nil\n"\
        "TYPE LF@%type LF@%1\n"\
        "JUMPIFNEQ $$compatibilityError LF@%type string@string\n"\
        "STRLEN LF@%return LF@%1\n"\
        "POPFRAME\n"\
        "RETURN\n";
    fputs(length, stdout);
    fputs(ord, stdout);
    fputs(chr, stdout);
    fputs(substr, stdout);
}

int CodeInitialize(void)
{
    fputs( ""\
    ".IFJcode18\n"\
    "DEFVAR GF@%void\n"\
    "DEFVAR GF@%operand1\n"\
    "DEFVAR GF@%operand2\n"\
    "DEFVAR GF@%operand1type\n"\
    "DEFVAR GF@%operand2type\n"\
    "JUMP $$main\n"\
    ""\
    "LABEL $$operand1ToFloat\n"\
    "JUMPIFNEQ $$operand1ToFloatReturn GF@%operand1type string@int\n"\
    "INT2FLOAT GF@%operand1 GF@%operand1\n"\
    "LABEL $$operand1ToFloatReturn\n"\
    "TYPE GF@%operand1type GF@%operand1\n"\
    "RETURN\n"\
    ""\
    "LABEL $$operand2ToFloat\n"\
    "JUMPIFNEQ $$operand2ToFloatReturn GF@%operand2type string@int\n"\
    "INT2FLOAT GF@%operand2 GF@%operand2\n"\
    "LABEL $$operand2ToFloatReturn\n"\
    "TYPE GF@%operand2type GF@%operand2\n"\
    "RETURN\n"\
    ""\
    "LABEL $$operandNumberCompatibility\n"\
    "JUMPIFNEQ $$compatibilityError GF@%operand1type GF@%operand2type\n"\
    "JUMPIFEQ $$compatibilityError GF@%operand1type string@string\n"\
    "JUMPIFEQ $$compatibilityError GF@%operand1type string@bool\n"\
    "JUMPIFEQ $$compatibilityError GF@%operand1type string@nil\n"\
    "RETURN\n"\
    ""\
    "LABEL $$operandCompareCompatibility\n"\
    "JUMPIFNEQ $$compatibilityError GF@%operand1type GF@%operand2type\n"\
    "JUMPIFEQ $$compatibilityError GF@%operand1type string@bool\n"\
    "RETURN\n"\
    ""\
    "LABEL $$operandAdditionCompatibility\n"\
    "JUMPIFEQ $$compatibilityError GF@%operand1type string@bool\n"\
    "JUMPIFEQ $$compatibilityError GF@%operand1type string@nil\n"\
    "RETURN\n"\
    "LABEL $$compatibilityError\n"\
    "EXIT int@4\n"\
    "LABEL $$stringError\n"\
    "EXIT int@4\n"\

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

    CodeAddBuiltInFunctions();

    instructionList->ifCount = 0;
    instructionList->whileCount = 0;
    instructionList->operationCount = 0;

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
    DStrClear(instructionList->functionVariableList);
    DStrClear(instructionList->functionBody);
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

int CodeAddTempVariable(int tempVarNumber)
{
    int return_value = PARSE_OK;
    if((return_value = CodeAddVariable("%temp")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddNumberToBody(tempVarNumber)) != PARSE_OK)
        return return_value;
    return return_value;
}
int CodeAddLabelName(const char *labelName, int optionalNumber, const char *optionalEnd)
{
    int return_value = PARSE_OK;
    if((return_value = CodeAddTextToBody(labelName)) != PARSE_OK)
        return return_value;
    if(optionalNumber != -1)
    {
        if((return_value = CodeAddNumberToBody(optionalNumber)) != PARSE_OK)
            return return_value;
    }
    if(optionalEnd != NULL)
    {
        if((return_value = CodeAddTextToBody(optionalEnd)) != PARSE_OK)
            return return_value;
    }
    return return_value;
}

int CodeAddNil()
{
    int return_value = PARSE_OK;
    if((return_value = CodeAddTextToBody("nil@nil")) != PARSE_OK)
        return return_value;
    return return_value;
}

int CodeAddBool(bool boolValue)
{
    int return_value = PARSE_OK;
    if((return_value = CodeAddTextToBody("bool@")) != PARSE_OK)
        return return_value;

    if(boolValue)
    {
        if((return_value = CodeAddTextToBody("true")) != PARSE_OK)
            return return_value;
    }
    else
    {
        if((return_value = CodeAddTextToBody("false")) != PARSE_OK)
            return return_value;
    }
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
int CodeAddFunctionCallNil()
{
    int return_value = PARSE_OK;
    if((return_value = CodeAddFunctionCallParameter()) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddNil()) != PARSE_OK)
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
int CodeGetUniqueOperation()
{
    instructionList->operationCount++;
    return instructionList->operationCount;
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
    int return_value = PARSE_OK;
    char int_to_string[21] = {0,};
    sprintf(int_to_string, "%d", uniqueWhileNumber);

    if((return_value = CodeAddTextToBody("\n#While Loop Condition")) != PARSE_OK)
        return return_value;


    //Get type of variable which is stored in %condition
    if((return_value = CodeAddInstruction(TYPE)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddVariable("%conditionType")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddVariable("%condition")) != PARSE_OK)
        return return_value;

    //Add instruction to jump to $$while[n]end if it is nil
    if((return_value = CodeAddInstruction(JUMPIFEQ)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" $$while")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(int_to_string)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody("end ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddVariable("%conditionType")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddString("nil")) != PARSE_OK)
        return return_value;

    //Add jump to $$while[n]body it it is not bool
    if((return_value = CodeAddInstruction(JUMPIFNEQ)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" $$while")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(int_to_string)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody("body ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddVariable("%conditionType")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddString("bool")) != PARSE_OK)
        return return_value;

    //Now it is 100% bool so jump to $$if[n]else if %condition is false
    if((return_value = CodeAddInstruction(JUMPIFEQ)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" $$while")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(int_to_string)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody("end ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddVariable("%condition")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddBool(false)) != PARSE_OK)
        return return_value;


    if((return_value = CodeAddTextToBody("\n#While Loop Body")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody("\nLABEL $$while")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(int_to_string)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody("body")) != PARSE_OK)
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
int CodeAddIfCondition(int uniqueIfNumber)
{
    int return_value = PARSE_OK;
    char int_to_string[21] = {0,};
    sprintf(int_to_string, "%d", uniqueIfNumber);

    //Get type of variable which is stored in %condition
    if((return_value = CodeAddInstruction(TYPE)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddVariable("%conditionType")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddVariable("%condition")) != PARSE_OK)
        return return_value;

    //Add instruction to jump to $$if[n]else if it is nil
    if((return_value = CodeAddInstruction(JUMPIFEQ)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" $$if")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(int_to_string)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody("else ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddVariable("%conditionType")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddString("nil")) != PARSE_OK)
        return return_value;

    //Add jump to $$if[n]start it it is not bool
    if((return_value = CodeAddInstruction(JUMPIFNEQ)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" $$if")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(int_to_string)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody("start ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddVariable("%conditionType")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddString("bool")) != PARSE_OK)
        return return_value;

    //Now it is 100% bool so jump to $$if[n]else if %condition is false
    if((return_value = CodeAddInstruction(JUMPIFEQ)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" $$if")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(int_to_string)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody("else ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddVariable("%condition")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddBool(false)) != PARSE_OK)
        return return_value;
    
    //Add label for $$if[n]start
    if((return_value = CodeAddTextToBody("\n#If Start\nLABEL $$if")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(int_to_string)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody("start")) != PARSE_OK)
        return return_value;
    return return_value;
}

int CodeAddIfElse(int uniqueIfNumber)
{
    int return_value = PARSE_OK;
    char int_to_string[21] = {0,};
    sprintf(int_to_string, "%d", uniqueIfNumber);

    if((return_value = CodeAddTextToBody("\nJUMP $$if")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(int_to_string)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody("end")) != PARSE_OK)
        return return_value;

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

int CodeAddNumberToBody(int number)
{
    char int_to_string[21] = {0,};
    sprintf(int_to_string, "%d", number);
    int return_value = PARSE_OK;
    if((return_value = CodeAddTextToBody(int_to_string)) != PARSE_OK)
        return return_value;
    return return_value;
}

int CodeAddNumberToVariables(int number)
{
    char int_to_string[21] = {0,};
    sprintf(int_to_string, "%d", number);
    int return_value = PARSE_OK;
    if((return_value = CodeAddTextToVariables(int_to_string)) != PARSE_OK)
        return return_value;
    return return_value;
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

int CodeMoveReturn(const char *destinationVar)
{
    int return_value = PARSE_OK;

    if((return_value = CodeAddInstruction(MOVE)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddVariable(destinationVar)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" TF@%return")) != PARSE_OK)
        return return_value;

    return return_value;
}
int CodeMoveVar(const char *destinationVar, const char *sourceVar)
{
    int return_value = PARSE_OK;

    if((return_value = CodeAddInstruction(MOVE)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddVariable(destinationVar)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddVariable(sourceVar)) != PARSE_OK)
        return return_value;

    return return_value;
}

int CodeMoveInt(const char *destinationVar, int intValue)
{
    int return_value = PARSE_OK;

    if((return_value = CodeAddInstruction(MOVE)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddVariable(destinationVar)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddInt(intValue)) != PARSE_OK)
        return return_value;

    return return_value;
}

int CodeMoveFloat(const char *destinationVar, double doubleValue)
{
    int return_value = PARSE_OK;

    if((return_value = CodeAddInstruction(MOVE)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddVariable(destinationVar)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddDouble(doubleValue)) != PARSE_OK)
        return return_value;

    return return_value;
}

int CodeMoveString(const char *destinationVar, const char *str)
{
    int return_value = PARSE_OK;

    if((return_value = CodeAddInstruction(MOVE)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddVariable(destinationVar)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddString(str)) != PARSE_OK)
        return return_value;

    return return_value;
}

int CodeMoveNil(const char *destinationVar)
{
    int return_value = PARSE_OK;

    if((return_value = CodeAddInstruction(MOVE)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddVariable(destinationVar)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" nil@nil")) != PARSE_OK)
        return return_value;

    return return_value;
}
int CodeMoveBool(const char *destinationVar, bool boolValue)
{
    int return_value = PARSE_OK;

    if((return_value = CodeAddInstruction(MOVE)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddVariable(destinationVar)) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddTextToBody(" ")) != PARSE_OK)
        return return_value;
    if((return_value = CodeAddBool(boolValue)) != PARSE_OK)
        return return_value;
}