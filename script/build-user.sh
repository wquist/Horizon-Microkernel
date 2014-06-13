#! /bin/bash

FULLTARGET=${2}-${1}
if [ ${3} == 1 ] # This is a generic target
then
	FULLTARGET=${2}-all
fi

cd ${ROOT_DIR}/user
if [ ! -d ${FULLTARGET} ]
then
	echo "Usermode program '${FULLTARGET}' does not exist!"
	exit 1
fi

make TARGET=${2} ARCH={1}
if [ $? != 0 ]
then
	echo "Compilation for '${1}' failed!"
	exit 1
fi
