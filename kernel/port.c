
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
