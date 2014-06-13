#! /bin/bash

cd ${ROOT_DIR}/kernel/build
cp kernel-${2}.elf ${1}/kernel.elf
echo "Copied kernel binary to disk image."
