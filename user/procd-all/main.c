#include <horizon/types.h>
#include <horizon/ipc.h>
#include <sys/sched.h>
#include <sys/msg.h>
#include <sys/svc.h>
#include <malloc.h>
#include <memory.h>
#include <stdbool.h>

#include "pnotify.h"

typedef struct pwatch pwatch_t;
struct pwatch
{
	ipcport_t port;
	unsigned event;

	pwatch_t* next;
};

typedef struct pstate pstate_t;
struct pstate
{
	pwatch_t* watchers;
};

pstate_t* states;

void listen(pid_t target, unsigned event, ipcport_t listener)
{
	pwatch_t* watch = malloc(sizeof(pwatch_t));
	watch->port  = listener;
	watch->event = event;

	watch->next = states[target].watchers;
	states[target].watchers = watch;
}

void notify(pid_t changed, unsigned event)
{
	pwatch_t* curr = states[changed].watchers;
	while (curr)
	{
		struct msg response = {{0}};
		response.to = curr->port;

		response.code = PN_STATECH;
		response.args[0] = changed;
		response.args[1] = event;

		send(&response);

		pwatch_t* old = curr;
		curr = curr->next;

		free(old);
	}

	states[changed].watchers = NULL;
}

int main()
{
	if (svcown(SVC_PROCMGR) < 0)
		return 1;

	// FIXME: process limit constants.
	states = malloc(sizeof(pstate_t) * 1024);
	memset(states, 0, sizeof(pstate_t) * 1024);

	while (true)
	{
		wait(IPORT_ANY);

		struct msg msg_in = {{0}};
		if (recv(&msg_in) < 0)
		{
			drop(NULL);
			continue;
		}

		switch (msg_in.from)
		{
			case IPORT_KERNEL:
			{
				notify(msg_in.args[0], PN_ONDEATH);
				break;
			}
			default:
			{
				struct msg msg_out = {{0}};
				msg_out.to = msg_in.from;

				switch (msg_in.code)
				{
					// FIXME: check if target process is alive?
					case PN_LISTEN:
					{
						listen(msg_in.args[0], msg_in.args[1], msg_in.from);

						msg_out.code = 0;
						break;
					}
					default:
					{
						msg_out.code = -1;
						break;
					}
				}

				send(&msg_out);
				break;
			}
		}
	}

	return 0;
}
