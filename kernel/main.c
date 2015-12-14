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

	init_video();
	gdt_install();
	idt_install();
	irq_install();
	init_timer(1000); // pit stuff
	enable_hw_interrupts();
	keyboard_install();

	initialise_paging();
	puts(system_ready);

	// generating an exception
	//__asm__ __volatile__ ("int $0x4");

	for (;;);

	// make gcc stop crying
	return 0; 
}
