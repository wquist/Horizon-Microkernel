#ifndef _FEATURES_H
#define _FEATURES_H

#if defined(__GNUC__)
#define _Packed __attribute__ ((__packed__))
#elif defined(_KERNEL_SOURCE)
#error "Packed attribute required for kernel compilation."
#endif

#if __STDC_VERSION__ < 201112L
#if defined(__GNUC__)
#define _Noreturn __attribute__ ((__noreturn__))
#else
#define _Noreturn
#endif
#endif

#if !defined(_KERNEL_SOURCE)
#define HAS_STDIO
#endif

#endif
