#pragma once

typedef enum VFSOP vfsop_t;
enum VFSOP
{
	VFS_FSD_MOUNTSELF,

	VFS_FILE_OPEN,
	VFS_FILE_READ,
	VFS_FILE_WRITE,

	VFS_CTL_GETCTL
};

typedef enum VFSREQ vfsreq_t;
enum VFSREQ
{
	VFS_DRQ_WRITE,
	VFS_DRQ_READ,

	VFS_DRQ_FINDDIR
};
