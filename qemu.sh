#!/bin/sh
set -e

if [ -z "$QEMU" ]; then
    QEMU=qemu-system-i386
fi

. ./iso.sh

$QEMU -m 64M -audiodev pa,id=audio0 -M pcspk-audiodev=audio0 -cdrom Veonter.iso -serial mon:stdio -boot d \
    -drive file=disk.img,index=0,if=ide,format=raw 

#. ./clean.sh

