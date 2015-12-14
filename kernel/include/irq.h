#ifndef __IRQ_H
#define __IRQ_H

void irq_install();
void irq_install_handler(int irq, void (*h)(struct regs *r));
void enable_hw_interrupts();

#define IRQ_PIT		0
#define IRQ_KEYBOARD	1
#define IRQ_PAGEFAULT	14

#endif
