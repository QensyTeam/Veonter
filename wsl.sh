#!/bin/bash

# Запуск скрипта создания ISO-образа
bash iso.sh

# Создание папки Veonter на диске C: если её нет
mkdir -p /mnt/c/Veonter/

# Создание виртуального диска и перемещение его и ISO-образа в папку Veonter на диске C:
fallocate -l 64M disk.img
mv Veonter.iso /mnt/c/Veonter/Veonter.iso
mv disk.img /mnt/c/Veonter/disk.img

# Запуск qemu с использованием перемещённого ISO-образа и виртуального диска
"/mnt/c/Program Files/qemu/qemu-system-i386.exe" \
    -m 64M \
    -hda "C:\\Veonter\\disk.img" \
    -audiodev dsound,id=audio0 \
    -M pcspk-audiodev=audio0 \
    -cdrom "C:\\Veonter\\Veonter.iso" \
    -serial mon:stdio \
    -name "Veonter DEV WSL MODE"

# Очистка папки Veonter на диске C:
rm -rf /mnt/c/Veonter/*

# Очистка рабочей директории WSL (если требуется)
. ./clean.sh
