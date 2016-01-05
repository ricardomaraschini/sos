#include <system.h>
#include <paging.h>
#include <video.h>
#include <kb.h>
#include <gdt.h>
#include <pit.h>
#include <int.h>
#include <ordarray.h>
#include <kheap.h>


int 
main()
{
	gdt_install();
	idt_install();
	enable_hw_interrupts();
	init_video();
	keyboard_install();
	init_timer(50); // pit stuff
	init_paging();

	u32int a = kmalloc(8);
	puts("a: \x0");
	puthex(a);

	u32int b = kmalloc(8);
	puts("\nb: \x0");
	puthex(b);

	kfree((void *)b);

	b = kmalloc(8);
	puts("\nb: \x0");
	puthex(b);
	/*
	u32int c = kmalloc(8);
	puts("\nc: \x0");
	puthex(c);
	kfree((void *)a);
	*/

	/*
	u32int *ptr = (u32int*)0xA0000000;
	u32int do_page_fault = *ptr;
	*/

	for (;;);

	// makes gcc stop crying
	return 0; 
}
