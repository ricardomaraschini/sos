#ifndef __IDT_H
#define __IDT_H

void idt_install();
void idt_set_gate(int, unsigned long, unsigned short, unsigned char);

#endif
