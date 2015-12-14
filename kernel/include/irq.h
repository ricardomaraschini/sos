#ifndef __IRQ_H
#define __IRQ_H

void irq_install();
void irq_install_handler(int irq, void (*h)(struct regs *r));
void enable_hw_interrupts();

#define IRQ_PIT		0
#define IRQ_KEYBOARD	1
#define IRQ_PAGEFAULT	14

/**
 * CHANNELS      command   data
 * master pic     0x20     0x21
 * slave  pic     0xA0     0XA1
 */

// pic channels
#define MPIC			0x20
#define SPIC			0xA0
#define MPIC_COMMAND		MPIC
#define MPIC_DATA		(MPIC_COMMAND + 1)
#define SPIC_COMMAND		SPIC
#define SPIC_DATA		(SPIC_COMMAND + 1)

// pic commands
#define PIC_INIT		0x11
#define PIC_EOI			0x20
#define PIC_THERES_SLAVE	0x04
#define PIC_SLAVE_PIC_POS	0x02

// pic data
#define MPIC_OFFSET		0x20 // 32, cause 0 to 31 are processor reserved
#define SPIC_OFFSET		0x28 
#define PIC_MODE8886		0x01

#endif
