#include <system.h>
#include <paging.h>
#include <video.h>
#include <kb.h>
#include <gdt.h>
#include <idt.h>
#include <irq.h>
#include <pit.h>


int 
main()
{
	char *system_ready = "ready to run...\n\0";

	gdt_install();
	idt_install();
	init_video();
	irq_install();
	init_timer(1000); // pit stuff
	enable_hw_interrupts();
	keyboard_install();
	initialise_paging();
	puts(system_ready);

	u32int *ptr = (u32int*)0xABC00000;
	u32int do_page_fault = *ptr;

	// generating an exception
	int x = 10/0;
	//__asm__ __volatile__ ("int $0xE");

	for (;;);

	// makes gcc stop crying
	return 0; 
}
