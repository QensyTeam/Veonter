#!/bin/sh
set -e
. ./headers.sh

for PROJECT in $PROJECTS; do
	(DESTDIR="$SYSROOT" $MAKE install -C $PROJECT -j$(nproc))
done
