// main.c

#include <horizon/ipc.h>
#include <sys/sched.h>
#include <sys/svc.h>
#include <sys/msg.h>
#include <stdbool.h>
#include <string.h>

#include "../vfsd-i586/fs.h"

ipcport_t filesystem;
int screen, keyboard;

int open(const char* path)
{
	struct msg request = {{0}};
	request.to = filesystem;

	request.code = VFS_OPEN;
	request.payload.buf  = (void*)path;
	request.payload.size = strlen(path)+1;

	send(&request);
	wait(filesystem);

	struct msg response = {{0}};
	recv(&response);

	return response.code;
}

int read(int fd, char* buffer, size_t size)
{
	struct msg request = {{0}};
	request.to = filesystem;

	request.code = VFS_READ;
	request.args[0] = fd;
	request.args[1] = size;

	send(&request);
	wait(filesystem);

	struct msg response = {{0}};
	response.payload.buf  = buffer;
	response.payload.size = size;

	recv(&response);
	return response.code;
}

int write(int fd, char* buffer, size_t size)
{
	struct msg request = {{0}};
	request.to = filesystem;

	request.code = VFS_WRITE;
	request.args[0] = fd;
	request.args[1] = size;

	request.payload.buf  = buffer;
	request.payload.size = size;

	send(&request);
	wait(filesystem);

	struct msg response = {{0}};
	recv(&response);

	return response.code;
}

void print(char* msg)
{
	write(screen, msg, strlen(msg)+1);
}

int main()
{
	while ((filesystem = svcid(SVC_VFS)) == 0);

	while ((screen = open("dev://tty")) == -1);
	print("Test program connected to TTY.\n");

	while ((keyboard = open("dev://kbd")) == -1);
	print("Test program connected to keyboard.\n");
	print("> ");

	char buffer;
	while (true)
	{
		int size = read(keyboard, &buffer, 1);
		if (size < 1)
			continue;

		char to_print[] = { buffer, '\0' };
		print(to_print);
	}

	return 0;
}
