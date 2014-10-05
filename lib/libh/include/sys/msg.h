#ifndef _SYS_MSG_H
#define _SYS_MSG_H

#include <horizon/msg.h>

long send(struct msg* info);
long recv(struct msg* info);
long peek();
long drop(struct msg* info);

#endif
