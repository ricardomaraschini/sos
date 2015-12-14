#include <system.h>
#include <port.h>
#include <idt.h>
#include <irq.h>

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

// irq handlers

void * irq_routines[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

// install call back routines
void 
irq_install_handler(int irq, void (*h)(struct regs *r)) 
{
	irq_routines[irq] = h;
}

// uninstall irq route
void 
uninstall_irq(int irq) 
{
	irq_routines[irq] = 0;
}


void 
irq_remap(void)
{

	/**
	 * issuing PIC_INIT makes pics wait for
	 * 3 extra commands, first is `offset', 
	 * then `positioning'(inform that we have
	 * a slave and it's cascade) and to finish,
	 * the mode to operate.
	 */

	// initialize pics
	outportb(MPIC_COMMAND, PIC_INIT);
	outportb(SPIC_COMMAND, PIC_INIT);

	// define new offsets for master and slave
	outportb(MPIC_DATA, MPIC_OFFSET);
	outportb(SPIC_DATA, SPIC_OFFSET);

	// let pics knows each other
	outportb(MPIC_DATA, PIC_THERES_SLAVE); 
	outportb(SPIC_DATA, PIC_SLAVE_PIC_POS);

	// put pics to work on the right mode
	outportb(MPIC_DATA, PIC_MODE8886);
	outportb(SPIC_DATA, PIC_MODE8886);

	// zero data fields, we're ready to go
	outportb(MPIC_DATA, 0x0);
	outportb(SPIC_DATA, 0x0);
}

void
irq_install()
{
	irq_remap();

	idt_set_gate(32, (unsigned)irq0, 0x08, 0x8E);
	idt_set_gate(33, (unsigned)irq1, 0x08, 0x8E);
	idt_set_gate(34, (unsigned)irq2, 0x08, 0x8E);
	idt_set_gate(35, (unsigned)irq3, 0x08, 0x8E);
	idt_set_gate(36, (unsigned)irq4, 0x08, 0x8E);
	idt_set_gate(37, (unsigned)irq5, 0x08, 0x8E);
	idt_set_gate(38, (unsigned)irq6, 0x08, 0x8E);
	idt_set_gate(39, (unsigned)irq7, 0x08, 0x8E);
	idt_set_gate(40, (unsigned)irq8, 0x08, 0x8E);
	idt_set_gate(41, (unsigned)irq9, 0x08, 0x8E);
	idt_set_gate(42, (unsigned)irq10, 0x08, 0x8E);
	idt_set_gate(43, (unsigned)irq11, 0x08, 0x8E);
	idt_set_gate(44, (unsigned)irq12, 0x08, 0x8E);
	idt_set_gate(45, (unsigned)irq13, 0x08, 0x8E);
	idt_set_gate(46, (unsigned)irq14, 0x08, 0x8E);
	idt_set_gate(47, (unsigned)irq15, 0x08, 0x8E);
}

void
irq_handler(struct regs *r)
{
	void (*handler)(struct regs *r);
	handler = irq_routines[r->int_no - 32];
	if (handler) {
		handler(r);
	}

	// send EOI to controller(end of interruption)
	if (r->int_no >= 40) {
		outportb(0xA0,0x20); // 0x20 interrupt finished
		                     // 0XA0 port of slave pic
	}

	outportb(0x20,0x20);
}

void
enable_hw_interrupts()
{
	__asm__ __volatile__ ("sti"); // enable hw irqs
}
