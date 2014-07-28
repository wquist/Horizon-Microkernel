#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <ctype.h>

// No libgcc in kernel for now; use 32-bit maximums
#define uintmax_t unsigned long
#define  intmax_t long

#define FMT(s, x) char* prev = s; x; if (s == prev) *--s = '0';

static const char xdigits[16] = { "0123456789ABCDEF" };
static char* fmt_x(uintmax_t x, char* s, char lower)
{
	FMT(s, for (; x; x >>= 4) *--s = xdigits[(x & 0xF)] | (lower & 0x20));
	return s;
}

static char* fmt_o(uintmax_t o, char* s)
{
	FMT(s, for (; o; o >>= 3) *--s = '0' + (o & 0x7));
	return s;
}

static char* fmt_u(uintmax_t u, char* s)
{
	FMT(s, for (; u; u /= 10) *--s = '0' + (u % 10));
	return s;
}

static const char* get_n(const char* str, unsigned* nret)
{
	const char* curr = str;
	while (*curr && isdigit(*curr)) ++curr;

	const char* tmp = curr;

	unsigned n = 0, i = 1;
	while (curr != str)
	{
		n += (*--curr - '0') * i;
		i *= 10;
	}

	*nret = n;
	return tmp;
}

static void out(const char* str)
{
	while (*str) putchar(*str++);
}

int vprintf(const char* format, va_list* arg)
{
	if (!format) return 0;

	const char* curr = format;
	for (;;)
	{
		while (*curr && *curr != '%') putchar(*curr++);
		if (!*curr) break;

		if (*++curr == '%')
		{
			putchar(*curr++);
			continue;
		}

		int alt = 0, zext = 0;
		unsigned pad = 0;
		while (*curr && !isalpha(*curr))
		{
			if (*curr == '#') 
				alt = 1;
			else if (*curr == '0') 
				zext = 1;
			else if (isdigit(*curr))
				curr = get_n(curr, &pad)-1;

			++curr;
		}

		if (!*curr) break;

		char buf[32] = {0}; 
		char* putend = &(buf[31]);
		char* putn = NULL;

		uintmax_t popped = va_arg(*arg, long);
		switch(*curr)
		{
			case 'd':
			case 'i':
			{
				intmax_t d = (intmax_t)popped;
				int neg = 0;
				if (d < 0)
				{
					d *= -1;
					neg = 1;
				}

				putn = fmt_u((uintmax_t)d, putend);
				if (neg) *--putn = '-';

				break;
			}
			case 'u':
			{
				uintmax_t u = popped;
				putn = fmt_u(u, putend);

				break;
			}
			case 'x':
			case 'X':
			case 'p':
			{
				uintmax_t x = popped;
				putn = fmt_x(x, putend, *curr);

				if (alt || *curr == 'p')
				{
					*--putn = 'x';
					*--putn = '0';
				}

				break;
			}
			case 'o':
			{
				uintmax_t o = popped;
				putn = fmt_o(o, putend);

				if (alt) *--putn = 'O';

				break;
			}
			case 's':
			{
				char* str = (char*)(uintptr_t)popped;
				out(str);

				break;
			}
			case 'c':
			{
				char c = (char)popped;
				putchar(c);

				break;
			}
			default: return -1;
		}

		curr++;
		if (!putn) continue;

		int diff = putend - putn;
		while (pad-- > diff) putchar((zext) ? '0' : ' ');
		out(putn); 
	}

	return 0;
}
