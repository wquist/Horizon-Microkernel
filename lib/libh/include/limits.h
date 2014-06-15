#ifndef _LIMITS_H
#define _LIMITS_H

#include <features.h>
#include <bits/limits.h>

#define CHAR_BIT  8
#define SCHAR_MAX 0x7F
#define SCHAR_MIN (-1 - SCHAR_MAX)
#define UCHAR_MAX (SCHAR_MAX * 2 + 1)

#define SHORT_MAX  0x7FFF
#define SHORT_MIN  (-1 - SHORT_MAX)
#define USHORT_MAX (SHORT_MAX * 2 + 1)

#define INT_MAX  0x7FFFFFFF
#define INT_MIN  (-1 - INT_MAX)
#define UINT_MAX (INT_MAX * 2 + 1)

#define LONG_MIN  (-1L - LONG_MAX)
#define ULONG_MAX (LONG_MAX * 2UL + 1)

#define LLONG_MAX  0x7FFFFFFFFFFFFFFFLL
#define LLONG_MIN  (-1LL - LLONG_MAX)
#define ULLONG_MAX (~0ULL)

#endif
