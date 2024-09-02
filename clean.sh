#!/bin/sh
set -e
. ./config.sh

for PROJECT in $PROJECTS; do
  ($MAKE -C $PROJECT clean)
done

rm -rf sysroot
rm -rf Veonter.iso
