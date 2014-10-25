#ifndef _STDIO_H
#define _STDIO_H

#include <features.h>
#include <stdarg.h>

int putchar(char c);

int  printf(const char* format, ...);
int vprintf(const char* format, va_list* arg);

#endif
