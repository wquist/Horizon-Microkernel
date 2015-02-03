// main.c

#include <horizon/ipc.h>
#include <sys/sched.h>
#include <sys/svc.h>
#include <sys/msg.h>
#include <stdbool.h>
#include <string.h>

#include "../vfsd-all/fs.h"

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
	print("Shell connected to TTY.\n");

	while ((keyboard = open("dev://kbd")) == -1);
	print("Shell connected to keyboard.\n");

	char input_buffer[256];
	size_t input_pos = 0;
	while (true)
	{
		memset(input_buffer, 0, 256);
		input_pos = 0;
		print("$> ");

		char last_key = '\0';
		while (last_key != '\n')
		{
			while (read(keyboard, &last_key, 1) < 1);

			char show_key = last_key;
			if (show_key < 32)
			{
				if (show_key == '\b' && input_pos)
				{
					input_pos -= 1;
					print("\b");
				}

				show_key = '\0';
			}

			if (show_key && input_pos >= 255)
				continue;

			if (show_key)
			{
				input_buffer[input_pos++] = show_key;

				char key_str[] = { show_key, '\0' };
				print(key_str);
			}
		}

		input_buffer[input_pos] = '\0';
		char* cmd = input_buffer;

		char* arg = strchr(input_buffer, ' ');
		if (arg)
		{
			*arg = '\0';
			arg += 1;
		}

		print("\n");
		if (strcmp(cmd, "echo") == 0)
		{
			if (arg)
				print(arg);
		}
		else if (strcmp(cmd, "clear") == 0)
		{
			print("\e[2J");
			continue;
		}
		else
		{
			print("Unknown command '");
			print(cmd);
			print("'.");
		}

		print("\n");
	}

	return 0;
}
