#! /bin/bash

cd ${ROOT_DIR}/lib
if [ ! -d ${2} ]
then
	echo "Library '${2}' does not exist!"
	exit 1
fi

make TARGET=${2} ARCH=${1}
if [ $? != 0 ]
then
	echo "Compilaion for '${1}' failed!"
	exit 1
fi
