#!/bin/sh
set -e

. ./iso.sh
cmd.exe /C "qemu-system-i386 -cdrom Veonter.iso -serial mon:stdio -m 128M -vga std"
