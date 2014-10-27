// main.c

#include <horizon/ipc.h>
#include <sys/sched.h>
#include <sys/svc.h>
#include <sys/msg.h>

ipcport_t get_tty(ipcport_t devmgr)
{
	struct msg request = {{0}};
	request.to = devmgr;
	request.code = 0;
	request.payload.buf  = "tty";
	request.payload.size = 4;

	send(&request);
	wait(devmgr);

	struct msg response = {{0}};
	recv(&response);
	
	return response.code;
}

void main()
{
	ipcport_t devmgr;
	while ((devmgr = svcid(SVC_DEVMGR)) == 0);

	ipcport_t tty;
	while ((tty = get_tty(devmgr)) == 0);

	static const char to_print[] = "Test program connected to TTY.";

	struct msg request = {{0}};
	request.to = tty;
	request.code = 0;
	request.payload.buf  = (char*)to_print;
	request.payload.size = sizeof(to_print);

	send(&request);

	for (;;);
}
