void * 
memcpy(void *dest, void *src, int count)
{

	int i;
	char *source = (char *)src;
	char *destination = (char *)dest;

	for (i=0; i<count; i++) {
		*(destination+i) = *(source+i);
	}

	return (void *)destination;
}

int
memcmp(void *s1, void *s2, int n)
{
	unsigned char u1, u2;

	for(; n--; s1++, s2++) {
		u1 = *(unsigned char *)s1;
		u2 = *(unsigned char *)s2;

		if ( u1 != u2)
			return (u1-u2);
	}

	return 0;
}

/*
int 
inw(int port) {
	int w;
	__asm__ __volatile__("inw %w1,%0" : "=a" (w) : "d" (port) );
	return w;
}

void 
outb(char value, int port) {
	__asm__ __volatile__( "outb %b0,%w1" : : "a" (value), "d" (port) );
}

void 
outw(int value, int port)
{
	__asm__ __volatile__( "outw %w0,%w1" : : "a" (value), "d" (port) );
}
*/

unsigned char 
*memset(unsigned char *dest, unsigned char val, int count)
{
	int i;

	for(i=0; i<count; i++) {
		*(dest+i) = val;
	}

	return dest;
}

/**
 * write val on dest count times
 */
unsigned short  *
memsetw(unsigned short *dest, unsigned short val, int count)
{
	int i;

	for(i=0; i<count; i++)
		*(dest+i) = val;

	return dest;
}
