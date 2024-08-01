#!/bin/sh
set -e

. ./iso.sh
qemu-system-i386 -cdrom Veonter.iso 
. ./clean.sh
