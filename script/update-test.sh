#! /bin/bash

rm ${1}/mod/*.elf
echo "Removed existing binaries on disk image."

cd ${ROOT_DIR}/test/build/${2}-${3}
for file in *.elf; do cp $file ${1}/mod/$file; done
echo "Copied test binaries to disk image."

cat ${ROOT_DIR}/script/menu-lst.cfg > ${1}/boot/grub/menu.lst
for f in ${1}/mod/*.elf
do
	echo -e "\t\tmodule /mod/$(basename ${f})" >> ${1}/boot/grub/menu.lst
done
echo "Updated GRUB menu.lst to include test binary."
