#!/bin/sh
set -x

DIR=$PWD

cd $DIR/kernel

nasm -f aout -o start.o start.asm
nasm -f aout -o interrupts.o interrupts.asm
/usr/bin/gcc -m32 -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o main.o main.c
/usr/bin/gcc -m32 -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o int.o int.c
/usr/bin/gcc -m32 -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o kb.o kb.c
/usr/bin/gcc -m32 -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o memutil.o memutil.c
/usr/bin/gcc -m32 -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o pit.o pit.c
/usr/bin/gcc -m32 -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o port.o port.c
/usr/bin/gcc -m32 -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o string.o string.c
/usr/bin/gcc -m32 -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o video.o video.c
/usr/bin/gcc -m32 -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o gdt.o gdt.c
/usr/bin/gcc -m32 -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o kheap.o kheap.c
/usr/bin/gcc -m32 -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o paging.o paging.c
ld -mi386linux -T link.ld -o kernel.bin 	\
	start.o					\
	interrupts.o				\
	main.o					\
	int.o					\
	kb.o					\
	memutil.o				\
	pit.o					\
	port.o					\
	string.o				\
	video.o					\
	gdt.o					\
	kheap.o					\
	paging.o

sudo mount -o loop -t vfat $DIR/boot.img /mnt/floppy
sudo cp $DIR/kernel/kernel.bin /mnt/floppy/boot
sudo umount /mnt/floppy
cd $DIR
./runvm.sh
