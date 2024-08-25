#!/bin/bash

# Запуск скрипта создания ISO-образа
bash iso.sh

# Создание папки Veonter на диске C: если её нет
mkdir -p /mnt/c/Veonter/

# Создание виртуального диска и перемещение его и ISO-образа в папку Veonter на диске C:
fallocate -l 128M disk.img

sudo mkfs.fat -F 32 disk.img

sudo mount disk.img /mnt/y

cd ../../../

sudo mkdir /mnt/y

sudo mkdir /mnt/y/dir1
sudo mkdir /mnt/y/dir2
sudo mkdir /mnt/y/dir2/dir3
sudo mkdir /mnt/y/dir4
sudo mkdir /mnt/y/dir4/dir5

cd ../../../mnt/y/

sudo bash -c "echo 'YourTEXTTTTT123' > textfile1.txt"
sudo bash -c "echo 'YourTEXTTTTT1234' > textfile2.power"
sudo bash -c "echo 'YourTEXTTTTT123454454' > textfile3.neo"
cd ../../../mnt/y/dir4/dir5
sudo bash -c "echo 'YourTEXTTTTT12345DIR5' > textfile4.moon"

cd ../../../

sudo umount /mnt/y

cd $HOME/Veonter


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
    -name "Veonter DEV WSL MODE" \
    -boot d

# Очистка папки Veonter на диске C:
rm -rf /mnt/c/Veonter/*

# Очистка рабочей директории WSL (если требуется)
. ./clean.sh
