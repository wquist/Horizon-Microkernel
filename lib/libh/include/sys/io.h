#ifndef _SYS_IO_H
#define _SYS_IO_H

#include <horizon/io.h>
#include <stddef.h>

long sysio(size_t action, size_t arg, volatile void* data);

#endif
