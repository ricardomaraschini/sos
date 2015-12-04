#include <system.h>


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

struct idt_entry {
	unsigned short base_lo; // lower 16 bits of the address to jump to when this interrupt fires.
	unsigned short sel; // kernel segment selector
	unsigned char always0;
	unsigned char flags;
	unsigned short base_hi; // upper 16 bits of the address to jump to
}__attribute__((packed));

struct idt_ptr {
	unsigned short limit;
	unsigned int base;
}__attribute__((packed));

struct idt_entry idt[256]; // 256 interrupt handlers
struct idt_ptr idtp;

extern void idt_load(); // start.asm


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
	memset((unsigned char *)&idt, 0, sizeof(struct idt_entry) * 256);
	memset((unsigned char *)&idtp, 0, sizeof(struct idt_ptr));

	idtp.base = (int)&idt;
	idtp.limit = (sizeof(struct idt_entry) * 256) - 1;

	// define our idts here
	idt_set_gate(0, (unsigned)isr0, 0x08, 0x8E);
	idt_set_gate(1, (unsigned)isr1, 0x08, 0x8E);
	idt_set_gate(2, (unsigned)isr2, 0x08, 0x8E);
	idt_set_gate(3, (unsigned)isr3, 0x08, 0x8E);
	idt_set_gate(4, (unsigned)isr4, 0x08, 0x8E);
	idt_set_gate(5, (unsigned)isr5, 0x08, 0x8E);
	idt_set_gate(6, (unsigned)isr6, 0x08, 0x8E);
	idt_set_gate(7, (unsigned)isr7, 0x08, 0x8E);
	idt_set_gate(8, (unsigned)isr8, 0x08, 0x8E);
	idt_set_gate(9, (unsigned)isr9, 0x08, 0x8E);
	idt_set_gate(10, (unsigned)isr10, 0x08, 0x8E);
	idt_set_gate(11, (unsigned)isr11, 0x08, 0x8E);
	idt_set_gate(12, (unsigned)isr12, 0x08, 0x8E);
	idt_set_gate(13, (unsigned)isr13, 0x08, 0x8E);
	idt_set_gate(14, (unsigned)isr14, 0x08, 0x8E);
	idt_set_gate(15, (unsigned)isr15, 0x08, 0x8E);
	idt_set_gate(16, (unsigned)isr16, 0x08, 0x8E);
	idt_set_gate(17, (unsigned)isr17, 0x08, 0x8E);
	idt_set_gate(18, (unsigned)isr18, 0x08, 0x8E);
	idt_set_gate(19, (unsigned)isr19, 0x08, 0x8E);
	idt_set_gate(20, (unsigned)isr20, 0x08, 0x8E);
	idt_set_gate(21, (unsigned)isr21, 0x08, 0x8E);
	idt_set_gate(22, (unsigned)isr22, 0x08, 0x8E);
	idt_set_gate(23, (unsigned)isr23, 0x08, 0x8E);
	idt_set_gate(24, (unsigned)isr24, 0x08, 0x8E);
	idt_set_gate(25, (unsigned)isr25, 0x08, 0x8E);
	idt_set_gate(26, (unsigned)isr26, 0x08, 0x8E);
	idt_set_gate(27, (unsigned)isr27, 0x08, 0x8E);
	idt_set_gate(28, (unsigned)isr28, 0x08, 0x8E);
	idt_set_gate(29, (unsigned)isr29, 0x08, 0x8E);
	idt_set_gate(30, (unsigned)isr30, 0x08, 0x8E);
	idt_set_gate(31, (unsigned)isr31, 0x08, 0x8E);

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
