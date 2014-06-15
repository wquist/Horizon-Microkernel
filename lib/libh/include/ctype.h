#ifndef _CTYPE_H
#define _CTYPE_H

#include <features.h>

#define isalpha(a) ((((unsigned)(a)|0x20)-'a') < 26)
#define isdigit(a) (((unsigned)(a)-'0') < 10)
#define islower(a) (((unsigned)(a)-'a') < 26)
#define isupper(a) (((unsigned)(a)-'A') < 26)

#define _toupper(a) ((a)|0x20)
#define _tolower(a) ((a)&0x5F)

#endif
