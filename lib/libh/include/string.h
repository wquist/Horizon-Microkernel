#ifndef _STRING_H
#define _STRING_H

#include <features.h>
#include <stddef.h>

void* memset(void* ptr, int ch, size_t len);
void* memcpy(void* dst, const void* src, size_t len);
int   memcmp(const void* a, const char* b, size_t len);

size_t strlen(const char* s);
char*  strcpy(char* dst, const char* src);
char* strncpy(char* dst, const char* src, size_t n);
int    strcmp(const char* a, const char* b);

#endif
