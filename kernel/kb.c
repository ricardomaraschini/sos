#include <system.h>
#include <port.h>
#include <video.h>
#include <irq.h>

#define KB_BUF 0x60

/* US Keyboard Layout */
unsigned char kbdus[128] =
{
	   0, 27,  '1', '2', '3', '4', '5', '6', '7', '8',
	 '9', '0', '-', '=','\b','\t', 'q', 'w', 'e', 'r',
	 't', 'y', 'u', 'i', 'o', 'p', '[', ']','\n',  0,
	 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
	'\'', '`',   0,'\\', 'z', 'x', 'c', 'v', 'b', 'n',
	 'm', ',', '.', '/',   0, '*',   0, ' ',   0,   0,
	   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	   0,   0,   0,   0, '-',   0,   0,   0, '+',   0,
	   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};


void
keyboard_handler(struct regs *r)
{
	unsigned char scancode;

	scancode = inportb(KB_BUF);

	// invalid stuff
	if (scancode < 0  || scancode > 127)
		return;

	if (scancode & 0x80) { // top bit indicates key release
		//XXX
	} else {
		putch(kbdus[scancode]);
	}
}

void
keyboard_install()
{
	irq_install_handler(IRQ_KEYBOARD,keyboard_handler); 
}
