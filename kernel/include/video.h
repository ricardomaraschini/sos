#ifndef __VIDEO_H
#define __VIDEO_H

extern void cls();
extern void putch(unsigned char c);
extern void puts(char *str);
extern void settextcolor(unsigned char forecolor, unsigned char backcolor);
extern void init_video();
extern void putint(int d);

struct cursor_position {
	int x;
	int y;
};

#define VIDEOOUTADDR	0xB8000
#define BLANK		0x20


#endif
