#include <system.h>
#include <video.h>

/**
 * a gdt entry
 * why do they choose this layout? 
 * 64 bits(8 bytes) for every entry
 */
struct gdt_entry {
	unsigned short limit_low;
	unsigned short base_low;
	unsigned char base_middle;
	unsigned char access;
	unsigned char granularity;
	unsigned char base_high;
} __attribute__((packed)); // packet avoids gcc `optimizations'

/**
 * gdt:
 * access (8 bits):
 *
 *  ,7,6,,,,54,,3,,,,,,,,,,,,0
 * | P | DPL |DT| Type       |
 *  '''''''''''''''''''''''''
 * P    -> is segment present? (1 bit )
 * DPL  -> wich ring (0-3)     (2 bits)
 * DT   -> descriptor table    (1 bit )
 * Type -> wich type           (4 bits)
 *
 *
 * granularity (8 bits):
 *
 *  ,7,,,6,,,5,,,4,,3,,,,,,,,0
 * | G | D | 0 | A | seg len |
 *  '''''''''''''''''''''''''
 *
 *  G   -> granularity(0 - 1byte || 1 - 4kbytes)   (1 bit)
 *         XXX - some references point that a value of 1 on
 *               `G' field indicates a granularity of 1kbyte
 *               instead of 4. we need further clarification
 *               on this.
 *  D   -> operand size(0 - 16bits || 1 - 32bits)  (1 bit)
 *  0   -> always defined to zero
 *  A   -> available for system(always 0)
 */

/**
 * where we have our gdt
 * entries, the base and
 * the limit
 */
struct gdt_ptr {
	unsigned short limit;
	unsigned int base;
}__attribute__((packed));

/**
 * gdt_ptr points to our list
 * of gdt_entry.
 *
 * limit -> 16 bits pointing to the end
 *          of gdt's entries
 * base  -> start of our gdt entries
 *
 * gdt_ptr is 48 bits long, duh.
 */


struct gdt_entry gdt[5]; // our 3 gdts entries 
struct gdt_ptr gp;
extern void gdt_flush();

void
gdt_set_gate( int num, 
              unsigned long base, 
              unsigned long limit, 
              unsigned char access, 
              unsigned char gran )
{

	/* descriptor base address */
	gdt[num].base_low = (base & 0xFFFF);
	gdt[num].base_middle = (base >> 16) & 0xFF;
	gdt[num].base_high = (base >> 24) & 0xFF;

	/* descriptor limits */
	gdt[num].limit_low = (limit & 0xFFFF);
	gdt[num].granularity = (limit >> 16) & 0x0F;

	gdt[num].granularity |= gran & 0xF0;
	gdt[num].access = access;

}


void
gdt_install()
{
	puts("installing global descriptor tables...\0");

	// gdt base is at &gdt
	gp.base = (int)&gdt;

	// end of our gdt entries - 1
	// as we have 5 gdt entries(dummy, kernel code and data segments and
	// user code and data segments)
	gp.limit = (sizeof(struct gdt_entry) * 5) - 1;

	// XXX
	// code and data overlap each other
	gdt_set_gate(0, 0, 0x00000000, 0, 0); // dummy gdt entry
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // kernel mode code segment
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // kernel mode data segment
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // user mode code segment
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // user mode data segment

	// 0x9A -> designs a kernel code segment
	// 0x92 -> designs a kernel data segment
	// 0xFA -> designs a user code segment
	// 0xF2 -> designs a user data segment

	gdt_flush(); // look start.asm

	puts("done.\r\n\0");
}
