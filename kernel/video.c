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
putint(int d)
{

	int sign;
	int i = 0;
	int j = 0;
	char revstr[12];
	char str[12];

	sign = d;
	if (sign < 0)
		d = -d;
	
	do {
		revstr[i++] = d % 10 + '0';
		d /= 10;
	} while((d/10) > 0);
	revstr[i] = d % 10 + '0';

	if (sign < 0)
		revstr[++i] = '-';

	while(i>=0)
		str[j++] = revstr[i--];
	str[j] = '\0';

	puts(str);
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
	if(cursor.x >= 80) {
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
