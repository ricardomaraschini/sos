#ifndef __KHEAP_H
#define __KHEAP_H

#define KHEAP_START	0xC0000000
#define KHEAP_INISIZE	0x100000
#define KHEAP_INIEND	(KHEAP_START + KHEAP_INISIZE)
#define KHEAP_IDXSIZE	0x20000
#define KHEAP_MAGIC	0xADEADBEE
#define KHEAP_MINSIZE	0x70000

typedef struct header {
	u32int		magic;
	u8int		ishole;
	u32int		size;
} header_t;


typedef struct footer {
	u32int		 magic;
	header_t	*header;
} footer_t;

typedef struct heap {
	ordarray_t	index;
	u32int		start_addr;
	u32int		end_addr;
	u32int		max_addr;
	u8int		supervisor;
	u8int		readonly;
} heap_t;

u32int kmalloc_internal(u32int sz, int align, u32int *phys);
u32int kmalloc_a(u32int sz);
u32int kmalloc_p(u32int sz, u32int *phys);
u32int kmalloc_ap(u32int sz, u32int *phys);
u32int kmalloc(u32int sz);
void * alloc(u32int, u8int, heap_t *);
void kfree(void *);
heap_t *create_heap(u32int, u32int, u32int, u8int, u8int);

#endif
