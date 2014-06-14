#ifndef _LIMITS_H
#define _LIMITS_H

#include <features.h>
#include <bits/limits.h>

#define ULONG_MAX (2UL*LONG_MAX+1)
#define LONG_MIN  (   -LONG_MAX-1)

#define UCHAR_MAX  (   0xFF)
#define SCHAR_MAX  (   0x7F)
#define SCHAR_MIN  (-1-0x7F)
#define USHRT_MAX  (   0xFFFF)
#define SHRT_MAX   (   0x7FFF)
#define SHRT_MIN   (-1-0x7FFF)
#define UINT_MAX   (   0xFFFFFFFFU)
#define INT_MAX    (   0x7FFFFFFF)
#define INT_MIN    (-1-0x7FFFFFFF)
#define ULLONG_MAX (   0xFFFFFFFFFFFFFFFFULL)
#define LLONG_MAX  (   0x7FFFFFFFFFFFFFFFLL)
#define LLONG_MIN  (-1-0x7FFFFFFFFFFFFFFFLL)

#endif
