#include "error.h"


int main(int argc, char const *argv[])
{
    for(int i = 0; i < 7; i++)
    {
        ErrorPrintErrorCode(i, "blablabla");
    }
    return 0;
}
