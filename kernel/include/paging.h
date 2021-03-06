#ifndef __PAGING_H
#define __PAGING_H

typedef struct page {
	u32int	present		: 1;
	u32int	rw		: 1;
	u32int	user		: 1;
	u32int	accessed	: 1;
	u32int	dirty		: 1;
	u32int	unused		: 7;
	u32int	frame		: 20;
} page_t;

typedef struct page_table {
	page_t	pages[1024];
} page_table_t;

typedef struct page_directory {
	page_table_t	*tables[1024];
	u32int		 tables_physical[1024];
	u32int		 physical_addr;
} page_directory_t;

void init_paging();
void switch_page_directory(page_directory_t *);
page_t *get_page(u32int, int, page_directory_t *);
void page_faul(registers_t);
void alloc_frame(page_t *, int, int);
void free_frame(page_t *);

#define INDEX_FROM_BIT(a)	(a / (8*4))
#define OFFSET_FROM_BIT(a)	(a % (8*4))
#define PAGESIZE		0x1000
#define ALIGNEDMASK		0xFFFFF000
#define NOTALIGNEDMASK		0x00000FFF
#define METADATASIZE		(sizeof(header_t) + sizeof(footer_t))

#endif
