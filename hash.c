#include "hash.h"

unsigned int BKDRHash(const char *str)
{
    unsigned int hash = 0, seed = 131;
    char c;
    while(c = *str++)
    {
        hash = (hash*seed) + c;
    }
    return hash;
}
unsigned int DJBHash(const char *str)
{
    unsigned int hash = 5381;
    char c;
    while(c = *str++)
    {
        hash = (hash << 5) + hash + c;
    }
    return hash;
}

unsigned int IALHash(const char *str)
{
    unsigned int hash = 1;
    unsigned char c;
	while(c = *str++)
		hash += c;
	return hash;
}