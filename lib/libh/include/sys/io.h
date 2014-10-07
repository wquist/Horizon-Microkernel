#ifndef _SYS_IO_H
#define _SYS_IO_H

#include <horizon/io.h>
#include <stddef.h>

long sysio(sysop_t action, unsigned long arg, volatile void* param);

#endif
