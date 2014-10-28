// main.c

#include <horizon/ipc.h>
#include <sys/sched.h>
#include <sys/svc.h>
#include <sys/msg.h>
#include <stdbool.h>
#include <string.h>

ipcport_t devmgr;
ipcport_t tty, kbd;

ipcport_t get_tty()
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

ipcport_t get_kbd()
{
	struct msg request = {{0}};
	request.to = devmgr;
	request.code = 0;
	request.payload.buf  = "kbd";
	request.payload.size = 4;

	send(&request);
	wait(devmgr);

	struct msg response = {{0}};
	recv(&response);
	
	return response.code;
}

void print(char* str)
{
	struct msg request = {{0}};
	request.to = tty;
	request.code = 0;
	request.payload.buf  = str;
	request.payload.size = strlen(str)+1;

	send(&request);
}

int main()
{
	while ((devmgr = svcid(SVC_DEVMGR)) == 0);

	while ((tty = get_tty(devmgr)) == 0);
	print("Test program connected to TTY.\n");

	while ((kbd = get_kbd(devmgr)) == 0);
	print("Test program connected to keyboard.\n");
	print("> ");

	char buffer;
	while (true)
	{
		struct msg request = {{0}};
		request.to = kbd;
		request.code = 1;

		send(&request);
		wait(kbd);

		struct msg response = {{0}};
		response.payload.buf  = &buffer;
		response.payload.size = 1;

		int received = recv(&response);
		if (received < 0)
			return 1;
		if (received == 0)
			continue;

		if (response.code == -1)
			return 1;

		char to_print[] = { buffer, '\0' };
		print(to_print);
	}

	for (;;);
	return 0;
}
