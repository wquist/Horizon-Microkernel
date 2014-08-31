#ifndef _ERRNO_H
#define _ERRNO_H

int* __errno_location();
#define errno (*__errno_location())

#endif
