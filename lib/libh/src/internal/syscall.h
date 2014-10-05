#ifndef _SRC_INTERNAL_SYSCALL_H
#define _SRC_INTERNAL_SYSCALL_H

#include <syscall_arch.h>
#include <sys/syscall.h>

#ifndef __sca
#define __sca(x) ((long)(x))
#endif

#define __syscall1(n,a) __syscall1(n, __sca(a))
#define __syscall2(n,a,b) __syscall2(n, __sca(a), __sca(b))
#define __syscall3(n,a,b,c) __syscall3(n, __sca(a), __sca(b), __sca(c))

#define __SYSCALL_NARGS_X(a,b,c,d,n,...) n
#define __SYSCALL_NARGS(...) __SYSCALL_NARGS_X(__VA_ARGS__,3,2,1,0,)
#define __SYSCALL_CONCAT_X(a,b) a##b
#define __SYSCALL_CONCAT(a,b) __SYSCALL_CONCAT_X(a,b)

#define __SYSCALL_FMT(b,...) __SYSCALL_CONCAT(b, __SYSCALL_NARGS(__VA_ARGS__))(__VA_ARGS__)
#define syscall(...) __SYSCALL_FMT(__syscall, __VA_ARGS__)

#endif
