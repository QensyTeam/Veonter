#!/bin/sh
set -e

. ./iso.sh
cmd.exe /C "qemu-system-i386 -m 2024 -cdrom Veonter.iso -monitor stdio"
