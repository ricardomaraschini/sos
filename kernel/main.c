#include <system.h>

void * 
memcpy(void *dest, void *src, int count)
{

	int i;
	char *source = (char *)src;
	char *destination = (char *)dest;

	for (i=0; i<count; i++) {
		*(destination+i) = *(source+i);
	}

	return (void *)destination;
}

int
memcmp(void *s1, void *s2, int n)
{
	unsigned char u1, u2;

	for(; n--; s1++, s2++) {
		u1 = *(unsigned char *)s1;
		u2 = *(unsigned char *)s2;

		if ( u1 != u2)
			return (u1-u2);
	}

	return 0;
}

/*
int 
inw(int port) {
	int w;
	__asm__ __volatile__("inw %w1,%0" : "=a" (w) : "d" (port) );
	return w;
}

void 
outb(char value, int port) {
	__asm__ __volatile__( "outb %b0,%w1" : : "a" (value), "d" (port) );
}

void 
outw(int value, int port)
{
	__asm__ __volatile__( "outw %w0,%w1" : : "a" (value), "d" (port) );
}
*/

unsigned char 
*memset(unsigned char *dest, unsigned char val, int count)
{
	int i;

	for(i=0; i<count; i++) {
		*(dest+i) = val;
	}

	return dest;
}

/**
 * write val on dest count times
 */
unsigned short 
*memsetw(unsigned short *dest, unsigned short val, int count)
{
	int i;

	for(i=0; i<count; i++) {
		*(dest+i) = val;
	}

	return dest;
}

int
strlen(char *str)
{
	int i = 0;

	while(*(str + i) != '\0') {
		i++;
	}

	return i;
}

/**
 * reads a byte from _port address
 * to rv var and returns it
 */
unsigned char 
inportb (unsigned short _port)
{
	unsigned char rv;
	//                    "instruction": output    : input
	__asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
	return rv;
}

/**
 * writes a byte(_data) to port _port
 */
void 
outportb (unsigned short _port, unsigned char _data)
{
	//                    "instruction" :output: input
	__asm__ __volatile__ ("outb %1, %0" :      : "dN" (_port), "a" (_data));
}

void
enable_hw_interrupts()
{
	__asm__ __volatile__ ("sti"); // enable hw irqs
}


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

	puts(system_ready);

	// generating an exception
	//__asm__ __volatile__ ("int $0x4");

	for (;;);

	// make gcc stop crying
	return 0; 
}
