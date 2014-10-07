#ifndef _SYS_MSG_H
#define _SYS_MSG_H

#include <horizon/msg.h>

long send(struct msg* info);
size_t recv(struct msg* info);
size_t peek();
long drop(struct msg* info);

#endif
