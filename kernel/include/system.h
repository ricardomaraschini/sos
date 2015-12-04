#ifndef __SYSTEM_H
#define __SYSTEM_H

/* main.c */
extern void *memcpy(void *dest, void *src, int count);
extern unsigned char *memset(unsigned char *dest, unsigned char val, int count);
extern unsigned short *memsetw(unsigned short *dest, unsigned short val, int count);
extern int strlen(char *str);
extern unsigned char inportb (unsigned short _port);
extern void outportb (unsigned short _port, unsigned char _data);
extern int memcmp(void *s1, void *s2, int n);

/* video.c */
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

#define VIDEOOUTADDR 0xB8000
#define NULL '\0'


/* gdt.c */
extern void gdt_set_gate(int, unsigned long, unsigned long, unsigned char, unsigned char);
extern void gdt_install();

/* idt.c */
void idt_install();
void idt_set_gate(int, unsigned long, unsigned short, unsigned char);

struct regs {
	unsigned int gs, fs, es, ds;      /* pushed the segs last */
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
	unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
	unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
};

/* irq.c */
void irq_install();
void irq_install_handler(int irq, void (*h)(struct regs *r));

/* kb.c */
void keyboard_install();

// irqs
#define IRQ_PIT		0
#define IRQ_KEYBOARD	1

// pit
void init_timer(int frequency);

struct scheduled_event {
	unsigned long every;
	void (*handler)(void);
};


#endif
