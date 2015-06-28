#pragma once

#include <horizon/ipc.h>
#include <sys/sched.h>
#include <sys/msg.h>
#include <stdarg.h>
#include <memory.h>

static void msg_create(struct msg* m, ipcport_t to, msgdata_t code)
{
	memset(m, 0, sizeof(struct msg));

	m->to   = to;
	m->code = code;
}

static void msg_set_args(struct msg* m, size_t cnt, ...)
{
	va_list args;

	va_start(args, cnt);
	for (size_t i = 0; i != cnt; ++i)
		m->args[i] = va_arg(args, msgdata_t);
	va_end(args);
}

static void msg_attach_payload(struct msg* m, void* ptr, size_t sz)
{
	m->payload.buf  = ptr;
	m->payload.size = sz;
}

static int msg_get_waiting(struct msg* m)
{
	wait(IPORT_ANY);

	int sz = peek();
	if (sz < 0)
		return -1;

	if (sz > 0)
	{
		void* buf = malloc(sz);
		msg_attach_payload(m, buf, sz);
	}

	int bytes = recv(m);
	if (bytes < 0)
		return -1;

	return bytes;
}
