#include <system.h>
#include <port.h>
#include <int.h>
#include <video.h>
#include <memutil.h>

isr_t		interrupt_handlers[256];
idt_entry_t	idt[256];	// 256 interrupt handlers
idt_ptr_t	idtp;
extern void	isr0();
extern void	isr1();
extern void	isr2();
extern void	isr3();
extern void	isr4();
extern void	isr5();
extern void	isr6();
extern void	isr7();
extern void	isr8();
extern void	isr9(); 
extern void	isr10(); 
extern void	isr11(); 
extern void 	isr12(); 
extern void	isr13(); 
extern void	isr14(); 
extern void	isr15(); 
extern void	isr16(); 
extern void	isr17(); 
extern void	isr18(); 
extern void	isr19(); 
extern void	isr20(); 
extern void	isr21(); 
extern void	isr22(); 
extern void	isr23(); 
extern void	isr24(); 
extern void	isr25(); 
extern void	isr26(); 
extern void	isr27(); 
extern void	isr28(); 
extern void	isr29(); 
extern void	isr30(); 
extern void	isr31(); 
extern void	irq0();
extern void	irq1();
extern void	irq2();
extern void	irq3();
extern void	irq4();
extern void	irq5();
extern void	irq6();
extern void	irq7();
extern void	irq8();
extern void	irq9();
extern void	irq10();
extern void	irq11();
extern void	irq12();
extern void	irq13();
extern void	irq14();
extern void	irq15();


char *
exception_messages[] =
{
	"Division By Zero\0",
	"Debug\0",
	"Non Maskable Interrupt\0",
	"Breakpoint Exception\0",
	"Into Detected Overflow Exception\0",
	"Out of Bounds Exception\0",
	"Invalid Opcode Exception\0",
	"No Coprocessor Exception\0",
	"Double Fault Exception\0",
	"Coprocessor Segment Overrun Exception\0",
	"Bad TSS Exception\0",
	"Segment Not Present Exception\0",
	"Stack Fault Exception\0",
	"General Protection Fault Exception\0",
	"Page Fault Exception\0",
	"Unknown Interrupt Exception\0",
	"Coprocessor Fault Exception\0",
	"Alignment Check Exception\0",
	"Machine Check Exception\0",
	"Reserved\0",
	"Reserved\0",
	"Reserved\0",
	"Reserved\0",
	"Reserved\0",
	"Reserved\0",
	"Reserved\0",
	"Reserved\0",
	"Reserved\0",
	"Reserved\0",
	"Reserved\0",
	"Reserved\0",
	"Reserved\0"
};

void 
isr_handler(struct regs r)
{
	isr_t	handler;

	if (interrupt_handlers[r.int_no] == 0)
		return;

	handler = interrupt_handlers[r.int_no];
	handler(r);
}


static void 
idt_set_gate( u8int num,
              u32int base,
              u16int sel,
              u8int flags )
{
	idt[num].base_lo = base & 0xFFFF;
	idt[num].base_hi = (base >> 16) & 0xFFFF;
	idt[num].sel = sel;
	idt[num].always0 = 0;
	idt[num].flags = flags /*| 0x60 */;
}


void
idt_install()
{
	memset(
	    (unsigned char *)&interrupt_handlers, 
	    0,
	    sizeof(isr_t) * 256
	);

	// put hardware interrupts from 
	// 32 to 47 instead of 0 to 15
	// that are used internally by
	// the processor
	reprogram_pic();

	/* 
	 * ! important to remember as a life lesson:
	 * ALWAYS set memory to zero before use it! 
	 */
	memset((unsigned char *)&idt, 0, sizeof(idt_entry_t) * 256);

	//idtp.base = (int)&idt;
	idtp.limit = (sizeof(idt_entry_t) * 256) - 1;
	idtp.base = (u32int)&idt;

	// define our idts here
	idt_set_gate(0, (u32int)isr0, 0x08, 0x8E);
	idt_set_gate(1, (u32int)isr1, 0x08, 0x8E);
	idt_set_gate(2, (u32int)isr2, 0x08, 0x8E);
	idt_set_gate(3, (u32int)isr3, 0x08, 0x8E);
	idt_set_gate(4, (u32int)isr4, 0x08, 0x8E);
	idt_set_gate(5, (u32int)isr5, 0x08, 0x8E);
	idt_set_gate(6, (u32int)isr6, 0x08, 0x8E);
	idt_set_gate(7, (u32int)isr7, 0x08, 0x8E);
	idt_set_gate(8, (u32int)isr8, 0x08, 0x8E);
	idt_set_gate(9, (u32int)isr9, 0x08, 0x8E);
	idt_set_gate(10, (u32int)isr10, 0x08, 0x8E);
	idt_set_gate(11, (u32int)isr11, 0x08, 0x8E);
	idt_set_gate(12, (u32int)isr12, 0x08, 0x8E);
	idt_set_gate(13, (u32int)isr13, 0x08, 0x8E);
	idt_set_gate(14, (u32int)isr14, 0x08, 0x8E);
	idt_set_gate(15, (u32int)isr15, 0x08, 0x8E);
	idt_set_gate(16, (u32int)isr16, 0x08, 0x8E);
	idt_set_gate(17, (u32int)isr17, 0x08, 0x8E);
	idt_set_gate(18, (u32int)isr18, 0x08, 0x8E);
	idt_set_gate(19, (u32int)isr19, 0x08, 0x8E);
	idt_set_gate(20, (u32int)isr20, 0x08, 0x8E);
	idt_set_gate(21, (u32int)isr21, 0x08, 0x8E);
	idt_set_gate(22, (u32int)isr22, 0x08, 0x8E);
	idt_set_gate(23, (u32int)isr23, 0x08, 0x8E);
	idt_set_gate(24, (u32int)isr24, 0x08, 0x8E);
	idt_set_gate(25, (u32int)isr25, 0x08, 0x8E);
	idt_set_gate(26, (u32int)isr26, 0x08, 0x8E);
	idt_set_gate(27, (u32int)isr27, 0x08, 0x8E);
	idt_set_gate(28, (u32int)isr28, 0x08, 0x8E);
	idt_set_gate(29, (u32int)isr29, 0x08, 0x8E);
	idt_set_gate(30, (u32int)isr30, 0x08, 0x8E);
	idt_set_gate(31, (u32int)isr31, 0x08, 0x8E);
	idt_set_gate(32, (u32int)irq0, 0x08, 0x8E);
	idt_set_gate(33, (u32int)irq1, 0x08, 0x8E);
	idt_set_gate(34, (u32int)irq2, 0x08, 0x8E);
	idt_set_gate(35, (u32int)irq3, 0x08, 0x8E);
	idt_set_gate(36, (u32int)irq4, 0x08, 0x8E);
	idt_set_gate(37, (u32int)irq5, 0x08, 0x8E);
	idt_set_gate(38, (u32int)irq6, 0x08, 0x8E);
	idt_set_gate(39, (u32int)irq7, 0x08, 0x8E);
	idt_set_gate(40, (u32int)irq8, 0x08, 0x8E);
	idt_set_gate(41, (u32int)irq9, 0x08, 0x8E);
	idt_set_gate(42, (u32int)irq10, 0x08, 0x8E);
	idt_set_gate(43, (u32int)irq11, 0x08, 0x8E);
	idt_set_gate(44, (u32int)irq12, 0x08, 0x8E);
	idt_set_gate(45, (u32int)irq13, 0x08, 0x8E);
	idt_set_gate(46, (u32int)irq14, 0x08, 0x8E);
	idt_set_gate(47, (u32int)irq15, 0x08, 0x8E);


	__asm__ __volatile__ ("lidt (%0)" : : "r"((u32int)&idtp));
}

void
enable_hw_interrupts()
{
	__asm__ __volatile__ ("sti" : : : "memory"); // enable hw irqs
}

void 
reprogram_pic(void)
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

// install call back routines
void 
irq_install_handler(u8int n, isr_t h)
{
	interrupt_handlers[n] = h;
}

// uninstall irq route
void 
uninstall_irq(int irq) 
{
	interrupt_handlers[irq] = NULL;
}

void
irq_handler(registers_t r)
{
	isr_t	handler;

	if (r.int_no >= 40)
		// send reset signal to slave
		outportb(SPIC, PIC_EOI);

	outportb(MPIC, PIC_EOI);
	if (interrupt_handlers[r.int_no] == 0)
		return;

	handler = interrupt_handlers[r.int_no];
	handler(r);
}

