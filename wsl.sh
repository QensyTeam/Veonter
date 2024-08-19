#!/bin/bash
bash iso.sh
@-mkdir /mnt/c/Veonter/
mv Veonter.iso /mnt/c/Veonter/Veonter.iso
"/mnt/c/Program Files/qemu/qemu-system-i386.exe" -cdrom "C:\\Veonter\\Veonter.iso" -serial mon:stdio -m 128M -name "Veonter DEV WSL MODE" -d guest_errors -rtc base=localtime -netdev user,id=net1,net=192.168.222.0,dhcpstart=192.168.222.128 -device virtio-net-pci,netdev=net1,id=mydev1,mac=52:54:00:6a:40:f8 $(QEMU_FLAGS_WSL)