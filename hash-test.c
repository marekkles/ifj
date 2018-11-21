#include <stdio.h>
#include <limits.h>
#include "hash.h"

int main(int argc, char const *argv[])
{
    const char * str[10] = {
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        "20 rocny passat",
        "Bravcove usi",
        "s#!t",
        "while",
        "i",
        "A",
        "e",
        "o",
        "20"
    };
    int j = 71;
    puts("+------------------------------+----------+----------+----------+");
    printf("|%-30s|%10s|%10s|%10s|\n", "String", "BKDRHash", "DJBHash", "IALHash");
    puts("+------------------------------+----------+----------+----------+");
    for(int i = 0; i < 10; i++)
    {
        printf("|%-30s|%10d|%10d|%10d|\n", str[i], BKDRHash(str[i])%j, DJBHash(str[i])%j, IALHash(str[i])%j);
    }
    puts("+------------------------------+----------+----------+----------+");
    return 0;
}

