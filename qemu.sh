#!/bin/sh
set -e

. ./iso.sh

qemu-system-i386 -m 128M -hda disk.img -audiodev pa,id=audio0 -M pcspk-audiodev=audio0 -cdrom Veonter.iso 
. ./clean.sh

