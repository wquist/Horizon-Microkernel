#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>

#if defined(HAS_STDIO)
int putchar(char c);
#else
extern int putchar(char c);
#endif

int  printf(const char* format, ...);
int vprintf(const char* format, va_list* arg);

#endif
