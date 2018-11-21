#ifndef HASH_H

#define HASH_H

unsigned int BKDRHash(const char *str);
unsigned int DJBHash(const char *str);
unsigned int IALHash(const char *str);

#endif