#ifndef _STDDEF_H
#define _STDDEF_H

#include <features.h>

#ifdef __GNUC__
typedef __SIZE_TYPE__    size_t;
typedef __PTRDIFF_TYPE__ ptrdiff_t;
#else
typedef unsigned long size_t;
typedef long ptrdiff_t;
#endif

#define NULL ((void*)0L)

#endif
