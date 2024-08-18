#!/bin/sh
set -e

. ./iso.sh
fallocate -l 64M disk.img
qemu-system-i386 -m 64M -hda disk.img -audiodev pa,id=audio0 -M pcspk-audiodev=audio0 -cdrom Veonter.iso -serial mon:stdio 
. ./clean.sh

