#!/bin/sh
set -x

DIR=$PWD

cd $DIR/kernel

nasm -f aout -o start.o start.asm
/usr/bin/gcc -m32 -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o main.o main.c
/usr/bin/gcc -m32 -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o video.o video.c
/usr/bin/gcc -m32 -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o gdt.o gdt.c
/usr/bin/gcc -m32 -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o idt.o idt.c
/usr/bin/gcc -m32 -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o irq.o irq.c
/usr/bin/gcc -m32 -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o kb.o kb.c
/usr/bin/gcc -m32 -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o pit.o pit.c
ld -mi386linux -T link.ld -o kernel.bin pit.o start.o main.o video.o gdt.o idt.o irq.o kb.o

mount -o loop -t vfat $DIR/boot.img /mnt/floppy
cp $DIR/kernel/kernel.bin /mnt/floppy/boot
umount /mnt/floppy

cd $DIR
./runvm.sh
