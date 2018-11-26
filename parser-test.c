#include <stdio.h>
#include "debug.h"
#include "parser.h"

FILE *input;
FILE *output;

int main(int argc, char const *argv[])
{
    output = stdout;
    if(argc != 2)
    {
        DebugFPuts("Program needs 2 arguments\n", output);
        return -1;
    }
    if((input = fopen(argv[1], "r")) == NULL)
    {
        DebugFPuts("Invalid filename\n", output);
        return -1;
    }

    return Parse();
}
