#ifndef _ARCH_I586_SYSCALL_H
#define _ARCH_I586_SYSCALL_H

#include <features.h>

static inline long __syscall0(long n)
{
	long __ret;
	__asm ("int $0x95" : "=a" (__ret) : "a" (n) : "memory");
	return __ret;
}

static inline long __syscall1(long n, long a1)
{
	long __ret;
	__asm ("int $0x95" : "=a" (__ret) : "a" (n), "b" (a1) : "memory");
	return __ret;
}

static inline long __syscall2(long n, long a1, long a2)
{
	long __ret;
	__asm ("int $0x95" : "=a" (__ret) : "a" (n), "b" (a1), "c" (a2) : "memory");
	return __ret;
}

static inline long __syscall3(long n, long a1, long a2, long a3)
{
	long __ret;
	__asm ("int $0x95" : "=a" (__ret) : "a" (n), "b" (a1), "c" (a2), "d" (a3) : "memory");
	return __ret;
}

#endif
