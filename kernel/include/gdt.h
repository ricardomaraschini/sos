#ifndef __GDT_H
#define __GDT_H

extern void gdt_set_gate(int, unsigned long, unsigned long, unsigned char, unsigned char);
extern void gdt_install();

#endif
