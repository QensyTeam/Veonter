#!/bin/sh
set -e

export PATH="$HOME/opt/cross/bin:$PATH"  # Добавляем эту команду для обновления переменной PATH

. ./iso.sh

qemu-system-i386 -cdrom Veonter.iso -audiodev pa,id=pa0 -M pcspk-audiodev=pa0  -serial mon:stdio -m 128M

. ./clean.sh