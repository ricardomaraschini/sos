#!/bin/sh

BOOTFLOPPY=boot.img

# boot from boot.img as fd0
qemu-system-i386 -fda $BOOTFLOPPY -boot a -cpu pentium -m 256

