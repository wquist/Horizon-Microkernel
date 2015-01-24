#! /bin/bash

# Setup some path variables for the sub-scripts
_WD="$(pwd)"
ROOT_DIR="${_WD}/.."

# Get the path to store the binaries
source config.cfg

# Setup some defaults
TARGET="kernel" # kernel, lib, or user
BINARY="" # define the specific binary for lib and user
ARCH="i586"
GENERIC=0 # If 1, the binary will be compiled for ARCH, but sourced from BINARY-all
UPDATE=0 # If 1, copy the build results to the path defined in config.cfg

# Reset so getopts() works correctly
OPTIND=1

while getopts "t:b:a:gu" opt; do
	case "${opt}" in
		t)	TARGET=${OPTARG}
			;;
		b)	BINARY=${OPTARG}
			;;
		a)	ARCH=${OPTARG}
			;;
		g)	GENERIC=1
			;;
		u)	UPDATE=1
			;;
	esac
done

echo "Compiling '${TARGET}'..."
source build-${TARGET}.sh ${ARCH} ${BINARY} ${GENERIC}
cd ${_WD} # Return to the proper directory

# Nothing else needs to be done if binary is not to be copied
if [ ${UPDATE} == 0 ]
then
	exit 0
fi

# Sub-script to copy must exist
if [ ! -f update-${TARGET}.sh ]
then
	exit 0
fi

# Check if disk image needs to be mounted
if [ ! -d "${BOOT_MOUNT_DIR}" ]
then
	echo "Mounting disk image..."
	${BOOT_MOUNT_CMD} "${BOOT_IMAGE}" || exit 1
else
	echo "Disk image already mounted."
fi

echo "Copying binary to disk image..."
source update-${TARGET}.sh "${BOOT_MOUNT_DIR}" ${BINARY} ${ARCH}
cd ${_WD}
