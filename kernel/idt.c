#include <system.h>
#include <video.h>
#include <memutil.h>
#include <port.h>
#include <idt.h>


/**
 *   system interruptions, traps or exceptions
 *
 *   0 - Division by zero exception
 *   1 - Debug exception
 *   2 - Non maskable interrupt
 *   3 - Breakpoint exception
 *   4 - 'Into detected overflow'
 *   5 - Out of bounds exception
 *   6 - Invalid opcode exception
 *   7 - No coprocessor exception
 *   8 - Double fault (pushes an error code)
 *   9 - Coprocessor segment overrun
 *   10 - Bad TSS (pushes an error code)
 *   11 - Segment not present (pushes an error code)
 *   12 - Stack fault (pushes an error code)
 *   13 - General protection fault (pushes an error code)
 *   14 - Page fault (pushes an error code)
 *   15 - Unknown interrupt exception
 *   16 - Coprocessor fault
 *   17 - Alignment check exception
 *   18 - Machine check exception
 *   19-31 - Reserved
 */

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


/* interrupt handler functions from start.asm */
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9(); 
extern void isr10(); 
extern void isr11(); 
extern void isr12(); 
extern void isr13(); 
extern void isr14(); 
extern void isr15(); 
extern void isr16(); 
extern void isr17(); 
extern void isr18(); 
extern void isr19(); 
extern void isr20(); 
extern void isr21(); 
extern void isr22(); 
extern void isr23(); 
extern void isr24(); 
extern void isr25(); 
extern void isr26(); 
extern void isr27(); 
extern void isr28(); 
extern void isr29(); 
extern void isr30(); 
extern void isr31(); 


idt_entry_t	idt[256];	// 256 interrupt handlers
idt_ptr_t	idtp;
extern void	idt_load();	// start.asm


void 
idt_set_gate( int num,
              unsigned long base,
              unsigned short sel,
              unsigned char flags )
{
	idt[num].base_lo = base & 0xFFFF;
	idt[num].base_hi = (base >> 16) & 0xFFFF;
	idt[num].sel = sel;
	idt[num].always0 = (unsigned char)0;
	idt[num].flags = flags;
}


void
idt_install()
{

	puts("installing interruption description tables...\0");

	/* 
	 * ! important to remember as a life lesson:
	 * ALWAYS set memory to zero before use it! 
	 */
	memset((unsigned char *)&idt, 0, sizeof(idt_entry_t) * 256);
	memset((unsigned char *)&idtp, 0, sizeof(idt_ptr_t));

	idtp.base = (int)&idt;
	idtp.limit = (sizeof(idt_entry_t) * 256) - 1;

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

	idt_load();

	puts("done.\r\n\0");
}


void 
fault_handler(struct regs *r)
{
	/* Is this a fault whose number is from 0 to 31? */
	if (r->int_no < 32) {
		puts(exception_messages[r->int_no]);
	} else {
		puts("unexpected error\0");
	}
	for (;;);
}
