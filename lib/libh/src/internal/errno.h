#ifndef _SRC_INTERNAL_ERRNO_H
#define _SRC_INTERNAL_ERRNO_H

#include <horizon/errno.h>

#define __iserr(x) ((long)(x) < 0 && (long)(x) >= ENORES)

#endif
