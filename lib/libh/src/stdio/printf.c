#include <stdio.h>

int printf(const char* format, ...)
{
	va_list arg;
	int done;

	va_start(arg, format);
	done = vprintf(format, &arg);
	va_end(arg);

	return done;
}
