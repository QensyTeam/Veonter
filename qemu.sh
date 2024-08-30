#!/bin/sh
set -e

. ./iso.sh
qemu-system-i386 -m 64M -audiodev pa,id=audio0 -M pcspk-audiodev=audio0 -cdrom Veonter.iso -serial mon:stdio -boot d \
    -drive file=disk.img,index=0,if=ide,format=raw 

#. ./clean.sh

