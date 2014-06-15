#ifndef _INTTYPES_H
#define _INTTYPES_H

#include <features.h>
#include <limits.h>

typedef signed   char int8_t;
typedef unsigned char uint8_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef int int32_t;
typedef unsigned int uint32_t;

#if (LONG_BIT == 32)
typedef          long long int64_t;
typedef unsigned long long uint64_t;
#else
typedef          long int64_t;
typedef unsigned long uint64_t;
#endif

typedef long long intmax_t;
typedef unsigned long long uintmax_t;

typedef unsigned long size_t;
typedef unsigned long uintptr_t;

typedef long ssize_t;
typedef long intptr_t;
typedef long ptrdiff_t;

#endif
