#! /bin/bash

cd ${ROOT_DIR}/user/build/${2}-${3}
cp ${2}.elf ${1}/mod/${2}.elf
echo "Copied usermode binary to disk image."
