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

    return_value = CodeDeclareVariable("var1");
    return_value = CodeDeclareVariable("var2");
    return_value = CodeDeclareVariable("var3");
    return_value = CodeDeclareVariable("var4");

    return_value = CodeAddFunctionStart("my_fun1");

    return_value = CodeAddFunctionParameter("id");
    return_value = CodeAddFunctionParameter("a");
    return_value = CodeAddFunctionParameter("b");
    return_value = CodeAddFunctionParameter("c");

    return_value = CodeAddInstruction(WRITE);
    return_value = CodeAddTextToBody(" ");
    return_value = CodeAddVariable("id");

    return_value = CodeAddInstruction(WRITE);
    return_value = CodeAddTextToBody(" ");
    return_value = CodeAddVariable("a");

    return_value = CodeAddInstruction(WRITE);
    return_value = CodeAddTextToBody(" ");
    return_value = CodeAddVariable("b");

    return_value = CodeAddInstruction(WRITE);
    return_value = CodeAddTextToBody(" ");
    return_value = CodeAddVariable("c");

    return_value = CodeAddFunctionEnd();

    return_value = CodeAddFunctionCallStart();

    return_value = CodeAddFunctionCallInt(10);
    return_value = CodeAddFunctionCallDouble(3.141592654359);
    return_value = CodeAddFunctionCallString("lol\\020boy");
    return_value = CodeAddFunctionCallVariable("var1");
    return_value = CodeAddFunctionCall("my_fun1");


    CodeFinalize();
    return 0;
}
