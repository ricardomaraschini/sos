#ifndef __VIDEO_H
#define __VIDEO_H

extern void cls();
extern void putch(unsigned char);
extern void puts(char *);
extern void settextcolor(unsigned char, unsigned char);
extern void init_video();
extern void putint(int);
extern void panic_assert(const char *, u32int, const char *);
extern void puthex(u32int);

struct cursor_position {
	int x;
	int y;
};

#define VIDEOOUTADDR	0xB8000
#define BLANK		0x20
#define ASSERT(b)	((b) ? (void)0 : panic_assert(__FILE__, __LINE__, #b))


#endif
