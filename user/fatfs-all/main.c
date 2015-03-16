#include <horizon/ipc.h>
#include <sys/sched.h>
#include <sys/svc.h>
#include <sys/msg.h>
#include <sys/proc.h>
#include <stdbool.h>
#include <string.h>

#include "fat.h"
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

	int disk;
	while ((disk = open("/dev/ata")) == -1);

	fat_volume_t vol = {0};
	fat_init(disk, &vol);

	size_t iter = 0;
	while (true)
	{
		fat_file_t file = {{0}};
		iter = fat_enumerate(&vol, NULL, iter, &file);
		if (iter == -1)
			break;

		print("Dirent: ");
		print(file.name);
		print("\n");
	}

	return 0;
}
