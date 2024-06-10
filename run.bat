@wsl ./iso.sh
@qemu-system-i386 -cdrom Veonter.iso -serial mon:stdio -m 128M
@wsl ./clean.sh