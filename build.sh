#!/bin/sh
set -e
. ./headers.sh

for PROJECT in $PROJECTS; do
	(DESTDIR="$SYSROOT" $MAKE install -C $PROJECT -j$(nproc))
done
gcc -m32 -ffreestanding -O0 -static -nostdlib -o isodir/boot/test.elf test.c -Wl,--entry=init,--build-id=none