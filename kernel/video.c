#include <system.h>
#include <video.h>
#include <memutil.h>
#include <port.h>
#include <string.h>

struct cursor_position cursor = {0,0};
unsigned short *textmemptr;
int attrib = 0x0F;

void 
scroll(void)
{
	unsigned blank, temp;

	blank = 0x20 | (attrib << 8);

	if (cursor.y >= 25) {

		temp = cursor.y - 25 + 1;
		memcpy(textmemptr, textmemptr + temp * 80, (25 - temp) * 80 * 2);

		memsetw (textmemptr + (25 - temp) * 80, blank, 80);
		cursor.y = 25 - 1;

	}
}

void 
move_csr(void)
{
	unsigned temp;

	temp = cursor.y * 80 + cursor.x;

	/*  http://www.brackeen.com/home/vga */
	outportb(0x3D4, 14);
	outportb(0x3D5, temp >> 8);
	outportb(0x3D4, 15);
	outportb(0x3D5, temp);
}

/* Clear the screen */
void 
cls() 
{
	unsigned blank;
	int i;

	// 8 bits for color | 8 bits for char
	blank = BLANK | (attrib << 8);

	// 80 columns X 25 lines
	for(i = 0; i < 25; i++)
		memsetw(textmemptr + i * 80, blank, 80);

	cursor.x = 0;
	cursor.y = 0;
	move_csr();
}

void
puthex(u32int val)
{
	s32int	 tmp;
	char	 nozeroes = 1;
	int	 i;

	puts("0x\x0");

	for(i=28; i>0; i-=4) {
		tmp = (val >> i) & 0xF;
		if (tmp == 0 && nozeroes != 0)
			continue;

		if (tmp >= 0xA) {
			nozeroes = 0;
			putch(tmp - 0xA + 'a');
		} else {
			nozeroes = 0;
			putch(tmp + '0');
		}

	}
	

	tmp = val & 0xF;
	if (tmp >= 0xA) {
		putch(tmp - 0xA + 'a');
		return;
	}

	putch(tmp + '0');

}

void 
putint(int d)
{
	s32int	acc;
	char	c[32];
	char	c2[32];
	int	i = 0;
	int	j = 0;

	if (d == 0) {
		putch('0');
		return;
	}

	acc = d;
	while(acc > 0) {
		c[i] = '0' + acc % 10;
		acc /= 10;
		i++;
	}
	c[i] = '\x0';

	c2[i--] = '\x0';
	while(i >= 0) {
		c2[i--] = c[j++];
	}

	puts(c2);
}

/* 
 * Puts a single character on the screen 
 * http://www.asciitable.com/
 */
void 
putch(unsigned char c)
{
	unsigned short *where;
	unsigned att = attrib << 8;

	if (c == 0x08 && cursor.x != 0) { /* backspace */
		cursor.x--;

	} else if (c == 0x09) { /* tab */
		// jump to a multiple of 8
		cursor.x = (cursor.x + 8) & ~(8 - 1);

	} else if (c == '\r') {
		cursor.x = 0; /* carriage return */

	} else if (c == '\n') { // on unix \n == \r\n
		cursor.x = 0;
		cursor.y++; /* new line */

	} else if (c >= ' ') {
		/* printable chars */
		where = textmemptr + (cursor.y * 80 + cursor.x);
		*where = c | att;
		cursor.x++;
	}

	/* end of line */
	if (cursor.x >= 80) {
		cursor.x = 0;
		cursor.y++;
	}

	/* Scroll the screen if needed, and finally move the cursor */
	scroll();
	move_csr();

}

void
puts(char *text)
{
	int i;

	for(i=0; i<strlen(text); i++) {
		putch(*(text+i));
	}

}

void
settextcolor(unsigned char backcolor, unsigned char forecolor)
{

	/**
	 * 15      12 11      8 7                       0
	 *  _____________________________________________
	 * | bgcolor | fgcolor |          char           |
	 *  ---------------------------------------------
	 *
	 *  - accepts up to 16 colors (4^2)
	 *  - char in ascii format
	 */

	attrib = (backcolor << 4) | (forecolor & 0xF);
}

void 
init_video(void)
{
	textmemptr = (unsigned short *)VIDEOOUTADDR;
	cls();
}

extern void
panic_assert(const char *file, u32int line, const char *desc)
{
	asm volatile("cli"); // Disable interrupts.
	puts("ASSERTION-FAILED(");
	puts((char *)desc);
	puts(") at ");
	puts((char *)file);
	puts(":");
	putint(line);
	puts("\n");
	for(;;);
}
