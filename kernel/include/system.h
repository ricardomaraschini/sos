#ifndef __SYSTEM_H
#define __SYSTEM_H

typedef unsigned int	u32int;
typedef int		s32int;
typedef unsigned short	u16int;
typedef short		s16int;
typedef unsigned char	u8int;
typedef char		s8int;
typedef unsigned int	size_t;

#define NULL '\0'

typedef struct regs {
	u32int ds;					// Data segment selector
	u32int edi, esi, ebp, esp, ebx, edx, ecx, eax;	// Pushed by pusha.
	u32int int_no, err_code;			// Interrupt number and error code (if applicable)
	u32int eip, cs, eflags, useresp, ss;		// Pushed by the processor automatically.
} registers_t;


#endif
