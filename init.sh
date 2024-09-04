#!/bin/bash

set -e

git submodule init
git submodule update --remote

for i in patches/*; do
    patch -p1 < patches/libvector-install-rule.patch
done
