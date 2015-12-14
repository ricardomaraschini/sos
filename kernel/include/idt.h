#ifndef __IDT_H
#define __IDT_H

void idt_install();
void idt_set_gate(int, unsigned long, unsigned short, unsigned char);

struct idt_entry {
	unsigned short	base_lo;	// lower 16 bits of the address to jump to when this interrupt fires.
	unsigned short	sel; 		// kernel segment selector
	unsigned char	always0;
	unsigned char	flags;
	unsigned short	base_hi; 	// upper 16 bits of the address to jump to
}__attribute__((packed));
typedef struct idt_entry idt_entry_t;

struct idt_ptr {
	unsigned short limit;
	unsigned int base;
}__attribute__((packed));
typedef struct idt_ptr idt_ptr_t;

#endif
