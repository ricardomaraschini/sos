#ifndef __INT_H
#define __INT_H

/**
 * PIC
 * CHANNELS      command   data
 * master pic     0x20     0x21
 * slave  pic     0xA0     0XA1
 */

// pic channels
#define MPIC			0x20
#define SPIC			0xA0
#define MPIC_COMMAND		0x20
#define MPIC_DATA		0x21
#define SPIC_COMMAND		0xA0
#define SPIC_DATA		0xA1

// pic commands
#define PIC_INIT		0x11
#define PIC_EOI			0x20
#define PIC_THERES_SLAVE	0x04
#define PIC_SLAVE_PIC_POS	0x02

// pic data
#define MPIC_OFFSET		0x20 // 32, cause 0 to 31 are processor reserved
#define SPIC_OFFSET		0x28 
#define PIC_MODE8886		0x01

#define IRQ_PIT			0x0
#define IRQ_KEYBOARD		0x1
#define IRQ_PAGEFAULT		0x14

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

typedef void (*isr_t)(register_t);

void reprogram_pic();
void isr_handler(struct regs);
void idt_install();
void enable_hw_interrupts();
void irq_install_handler(u8int, isr_t);
void uninstall_irq(int irq);
void irq_handler(struct regs);

#endif
