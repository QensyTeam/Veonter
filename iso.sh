#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/Veonter.kernel isodir/boot/Veonter.kernel
cat > isodir/boot/grub/grub.cfg << EOF

set timeout=0

# Установить цвета текста и фона
set menu_color_normal=magenta/white
set menu_color_highlight=white/magenta

menuentry "Veonter - 0.0.1 Wolf(Pre-Alpha)" {
 	multiboot /boot/Veonter.kernel
}

# Добавить записи для перезагрузки и выключения
menuentry "Reboot" {
  reboot
}

menuentry "Shutdown" {
  halt
}
EOF
grub-mkrescue -o Veonter.iso isodir
