#! /bin/bash

cd ${ROOT_DIR}/kernel

make ARCH=${1}
if [ $? != 0 ]
then
	echo "Kernel compilation for '${1}' failed!"
	exit 1
fi
