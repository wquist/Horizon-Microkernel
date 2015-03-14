#include <horizon/ipc.h>
#include <sys/sched.h>
#include <sys/svc.h>
#include <sys/msg.h>
#include <sys/proc.h>
#include <stdbool.h>
#include <string.h>

#include "idectl.h"
#include "../vfsd-all/fs.h"

ipcport_t filesystem;
int screen;

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

	while ((screen = open("/dev/tty")) == -1);
	print("Connected to TTY.\n");

	print("Identifying IDE devices... ");
	ide_controller_t ctl = { .base = 0x1F0 };
	idectl_identify(&ctl, IDE_MASTER);
	print("OK!\n");

	if (ctl.devices[IDE_MASTER].present)
		print("Found an IDE device.\n");
	else
		return 1;

	print("Device model: ");
	print(ctl.devices[IDE_MASTER].model);
	print("\nDevice serial: ");
	print(ctl.devices[IDE_MASTER].serial);
	print("\nDevice firmware: ");
	print(ctl.devices[IDE_MASTER].firmware);
	print("\n\n");

	print("Reading first block of disk... ");

	char buf[512];
	idectl_block_io(&ctl, IDE_MASTER, IDE_READ, 0, 1, buf);
	print("OK!\n");
	
	for (size_t i = 0; i != 256; ++i)
	{
		char str[] = { buf[i], '\0' };
		print(str);
	}

	for (;;);
	return 0;
}
