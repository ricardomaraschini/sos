#include <system.h>
#include <paging.h>
#include <video.h>
#include <kb.h>
#include <gdt.h>
#include <pit.h>
#include <int.h>


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
	puts("ready\n\0");

	/*
	u32int *ptr = (u32int*)0xA0000000;
	u32int do_page_fault = *ptr;
	*/

	for (;;);

	// makes gcc stop crying
	return 0; 
}
