#ifndef _FEATURES_H
#define _FEATURES_H

#if defined(__GNUC__)
#define __packed __attribute__ ((__packed__))
#else
#endif

#if defined(__GNUC__)
#define __weak __attribute__ ((__weak__))
#else
#endif

#if __STDC_VERSION__ >= 201112L
#define __noreturn _Noreturn
#elif defined(__GNUC__)
#define __noreturn __attribute__ ((__noreturn__))
#else
#define __noreturn
#endif

#if __STDC_VERSION__ >= 199901L
#define __inline inline
#define __forceinline __attribute__ ((__always_inline__))
#else
#endif

#if defined(__GNUC__)
#define __asm __asm__ __volatile__
#else
#define __asm asm
#endif

#if !defined(_KERNEL)
#define _HAS_STDIO
#endif

#endif
