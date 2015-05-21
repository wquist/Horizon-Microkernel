#! /bin/bash

cd ${ROOT_DIR}/test/build/${2}-${3}
cp ${2}.elf ${1}/mod/${2}.elf
echo "Copied test binary to disk image."

cat ${ROOT_DIR}/script/menu-lst.cfg > ${1}/boot/grub/menu.lst
for f in ${1}/mod/*.elf
do
	echo -e "\t\tmodule /mod/$(basename ${f})" >> ${1}/boot/grub/menu.lst
done
echo "Updated GRUB menu.lst to include test binary."
