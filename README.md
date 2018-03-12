Horizon Microkernel
=
A hobby operating system written in C for the x86 architecture. The kernel is mostly complete and includes multitasking, messaging IPC, and a full set of system calls. Work is still being done on userspace drivers and services; an experimental VFS with keyboard and screen drivers has been implemented so far.

Features
-
The microkernel tries not to be a direct copy of any existing operating systems. Special focus went into the areas of inter-process communication and system calls.

### General:
- No dynamic allocation occurs in the kernel; the maximum number of procecsses, threads per process, and messages per process are fixed.
- Almost all functions in the microkernel run in O(1) time, including memory allocation, process creation/destruction, and message passing; it should be possible to calculate a definite maximum time for each kernel operation.
- Any architecture-specific code is strictly separated from the main kernel logic. All architecture functions are called with fairly generic names, so it should be possible to port the kernel to any architecture (with paging) by re-implementing the functions in the `arch/` directory, requiring no changes to code in the `core/` hierarchy.

### Multitasking:
- Support for multiple processes, each with multiple threads. Each thread is its own IPC endpoint, so a single process can neatly separate is functionality/services.
- "Versioned" processes and threads, hopefully making IPC more secure and eliminating the possibility of incorrectly sent messages.

### Inter-Process Communication:
- Kernel support for message passing and shared memory.
- Messages are variably sized and can be synchronous or asynchronous; messages sent with little data are asynchronous and stored in a queue, while messages with large payloads block the sender until the receiver accepts or denies the message. This allows message passing to be handled in a fixed amount of memory, with no allocating or storing in the kernel. The limited async capabilities can also be combined with shared memory for fully asynchronous messaging in userspace.

### System Calls:
- Instead of providing the kernel with a binary to create a new process, empty processes are `spawn`ed, and code/data/other memory is `grant`ed to the new process from any sufficiently privileged usermode service before being `launch`ed. This way, the kernel does not have to understand binary formats, or even know about a filesystem. A process can be also cloned (in the same vein as the UNIX `fork` call) with the same set of system calls by mapping the calling process' memory into the new one.
- Processes can register as "services" in the kernel to make userspace discovery simpler. Owning a service also allows usermode processes to access hardware features, like interrupts and port I/O.

Building & Running
-
**Important:** None of the project can be built correctly without a properly set-up [Cross Compiler](http://wiki.osdev.org/GCC_Cross-Compiler) targeting `i586-elf`. The Makefiles and build scripts expect the compiler programs to be located in `/usr/local/cross/bin`.

Each section of the project has its own top-level Makefile. The kernel's can be run with `make ARCH=myarch`. The library Makefile can be run with `make ARCH=myarch TARGET=mylib` (eg. `make ARCH=i586 TARGET=libh`), and the user one is called with `make ARCH=myarch TARGET=myapp`. The user makefile also takes an optional `TARGET_DIR=mydir` so platform independent programs (like `vfsd-all`) can be compiled properly. 

Both the kernel and usermode programs require library support, so the binaries in `lib` should be compiled first.

To simplify compilation, the `script` directory contains a set of bash scripts that automatically compile the program and copy it to the target disk image. The scripts require two configuration files to operate properly:

- `config.cfg`: This config file defines three variables; `BOOT_IMAGE` is an absolute path to the kernel disk image, `BOOT_MOUNT_DIR` is the directory where the disk image will be mounted,
and `BOOT_MOUNT_CMD` is the command used to actually mount the image.
- `menu-lst.cfg`: This is a template stub for the GRUB `menu.lst` file. When a usermode program is compiled with the build script, the existing `menu.lst` is replaced with this file, and each program located in `/mod` is appended as a module, in the format `module /mod/myprogram`.

`build.sh` is the main build script, capable of compiling the kernel, as well as libraries and usermode programs. The script takes a set of arguments to determine what and how to compile:

- `-t mytarget`: Specifies the *target* to compile; either `kernel`, `lib`, or `user`.
- `-b myprogram`: Specifies the *binary* if the target is `lib` or `user`. This is the specific program to compile (eg. `libh` or `vgatty`).
- `-a myarch`: The *architecture* to compile under; currently, only `i586` is supported.
- `-g`: Only for `user` targets, identifies a *generic* target; a program that can be compiled for any architecture. When set, the build script looks for a `myprogram-all` directory instead of `myprogram-myarch`.
- `-u`: Successful compilation shoud *update* the kernel disk image. When applied to the `kernel` target, the resulting `kernel.elf` is copied to the root of the disk image. When used with the `user` target, the resulting binary is copied to the `/mod` directory of the image, and the `menu.lst` file is updated. This flag has no effect on the `lib` target.

Project Structure
-
The repository is divided into 3 main portions: `kernel`, `lib`, and `user`. Each portion has its own set of git branches, containing experimental changes before they are finalized in `develop`.

The two major halves of the kernel section are `core` and `arch`. There are additional support directories in the kernel section; `debug` for debug printing/logging, `spec` for hardware descriptions that may exist across multiple platforms, and `util` for implementations of the data structures used in the microkernel.

Inside of the architecture directory, a folder exists for each implemented platform (currently only `i586`). The organization of each platform's files can vary; `i586` divides its code into `boot`, `sw`, `hw`, and (assembly) `stub`. Each platform also contains a `link.ld` and `Makefile.inc` for use during compilation. 

The core directory is divided into `ipc` for message and service control, `memory` for physical and virtual memory management, `multitask` for process and thread control, and `system` for system calls and support functions. This directory also contains the kernel's C entry point, `main.c`. `kmain` is the first function that runs after the arch boot stub.

The library section contains only one library for now, `libh`. This library contains portions of the C standard library, as well as constants, structures, and functions used to interact with the microkernel. These definitions are used in both usermode programs and the kernel itself. Each library also contains a `Makefile.inc` for compiling.

The user section contains subdirectories for each program created. Currently, there is a device manager and virtual file system, a filesystem and IDE driver, a screen and keyboard driver, and some test programs. Each program is suffixed with its target architecture. The user section also contains an `arch` folder containing Makefiles and a basic `crt0` for each platform, for use during compilation.
