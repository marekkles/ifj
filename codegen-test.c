#include <stdio.h>
#include "codegen.h"
#include "return.h"
#include "debug.h"


int main(int argc, char const *argv[])
{
    int return_value = PARSE_OK;

    if((return_value = CodeInitialize()) != PARSE_OK)
    {
        DebugFPuts("Failed to intitialize\n", stdout);
    }

    int uniqueIfNumber = CodeGetUniqueIf();


    return_value = CodeDeclareVariable("%condition");
    return_value = CodeDeclareVariable("%conditionType");
    return_value = CodeDeclareVariable("a");
    return_value = CodeMoveInt("a", 10);
    return_value = CodeAddTextToBody("\nWRITE LF@a");


    //return_value = CodeMoveString("%condition", "AHOY");
    //return_value = CodeMoveInt("%condition", 0);
    //return_value = CodeMoveFloat("%condition", 1.10);
    //return_value = CodeMoveNil("%condition");
    //return_value = CodeMoveBool("%condition", true);
    int uniqueWhileNumber = CodeGetUniqueWhile();


    return_value = CodeAddWhileStart(uniqueWhileNumber);

    return_value = CodeAddTextToBody("\nGT LF@%condition LF@a int@0");

    return_value = CodeAddWhileBody(uniqueWhileNumber);

    return_value = CodeAddTextToBody("\nWRITE LF@a");
    return_value = CodeAddTextToBody("\nWRITE string@\\032");
    return_value = CodeAddTextToBody("\nSUB LF@a LF@a int@1");

    return_value = CodeAddWhileEnd(uniqueWhileNumber);

    CodeFinalize();
    return 0;
}
