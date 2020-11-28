CC = cc
LD = ld
CFLAGS = -Wall -Wextra -Werror -O2 -pipe

.PHONY: all clean run

all: image.hdd

limine:
	git clone https://github.com/limine-bootloader/limine.git --depth=1 --branch=v0.6.4
	$(MAKE) -C limine limine-install

image.hdd: limine kernel.elf
	rm -f image.hdd
	dd if=/dev/zero bs=1M count=0 seek=64 of=image.hdd
	parted -s image.hdd mklabel msdos
	parted -s image.hdd mkpart primary 2048s 100%
	echfs-utils -m -p0 image.hdd quick-format 512
	echfs-utils -m -p0 image.hdd import limine.cfg limine.cfg
	echfs-utils -m -p0 image.hdd import kernel.elf kernel.elf
	limine/limine-install limine/limine.bin image.hdd

compiler: compiler.c
	$(CC) $(CFLAGS) compiler.c -o compiler

kernel.elf: prelude.o hello.o
	$(LD) -Tlinker.ld -static -nostdlib -no-pie prelude.o hello.o -o kernel.elf

hello.b: hello.asm
	bfmake -t hello.asm

hello.x86asm: compiler hello.b
	./compiler hello.b hello.x86asm

hello.o: hello.x86asm
	nasm hello.x86asm -felf64 -o hello.o

prelude.o: prelude.asm
	nasm prelude.asm -felf64 -o prelude.o

clean:
	rm -f kernel.elf image.hdd compiler *.o *.x86asm *.b

run:
	qemu-system-x86_64 -m 2G -net none -enable-kvm -cpu host -hda image.hdd
