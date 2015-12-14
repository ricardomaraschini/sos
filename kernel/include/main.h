#ifndef __MAIN_H
#define __MAIN_H

extern void *memcpy(void *dest, void *src, int count);
extern unsigned char *memset(unsigned char *dest, unsigned char val, int count);
extern unsigned short *memsetw(unsigned short *dest, unsigned short val, int count);
extern int strlen(char *str);
extern unsigned char inportb (unsigned short _port);
extern void outportb (unsigned short _port, unsigned char _data);
extern int memcmp(void *s1, void *s2, int n);

#endif
