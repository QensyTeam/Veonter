@cd kernel/arch/i386/
@objcopy -I binary -O elf32-i386 -B i386 font.psf font.o
@cd ../../../
@wsl ./iso.sh
@qemu-system-i386 -cdrom Veonter.iso -serial mon:stdio -m 128M -vga std
@wsl ./clean.sh