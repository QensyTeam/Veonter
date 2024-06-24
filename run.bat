@wsl export PATH=/usr/local/cross/bin:$PATH
@wsl ./iso.sh
@qemu-system-i386 -cdrom Veonter.iso -serial mon:stdio -m 128M -vga std
@wsl ./clean.sh