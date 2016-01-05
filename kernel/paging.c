#include <system.h>
#include <paging.h>
#include <ordarray.h>
#include <kheap.h>
#include <video.h>
#include <memutil.h>
#include <int.h>

extern heap_t		*kheap;
extern u32int		 placement_address;
page_directory_t	*kernel_directory;
page_directory_t	*current_directory;
u32int			*frames;
u32int			 nframes;

static void
set_frame(u32int frame_addr)
{
	u32int	 frame;
	u32int	 idx;
	u32int	 off;

	frame = frame_addr / PAGESIZE;
	idx = INDEX_FROM_BIT(frame);
	off = OFFSET_FROM_BIT(frame);

	frames[idx] |= (0x1 << off);
}

static void
clear_frame(u32int frame_addr)
{
	u32int	 frame;
	u32int	 idx;
	u32int	 off;

	frame = frame_addr / PAGESIZE;
	idx = INDEX_FROM_BIT(frame);
	off = OFFSET_FROM_BIT(frame);

	frames[idx] &= ~(0x1 << off);
}

/*
static u32int
test_frame(u32int frame_addr)
{
	u32int	 frame;
	u32int	 idx;
	u32int	 off;

	frame = frame_addr / 0x1000; // 4kb
	idx = INDEX_FROM_BIT(frame);
	off = OFFSET_FROM_BIT(frame);

	return (frames[idx] & (0x1 << off));
}
*/

static u32int
first_frame()
{
	u32int	 i;
	u32int	 j;
	u32int	 to_test;

	for(i=0; i<INDEX_FROM_BIT(nframes); i++) {
		if (frames[i] == 0xFFFFFFFF)
			continue;

		for(j=0; j<32; j++) {
			to_test = 0x1 << j;
			if ( !(frames[i] & to_test) ) {
				return i * 4 * 8 + j;
			}
		}
	}

	return (u32int)-1;
}

void
alloc_frame(page_t *page, int is_kernel, int is_writeable)
{
	u32int	 idx;

	if (page->frame != 0)
		return;

	idx = first_frame();
	if (idx == (u32int)-1) {
		puts("no free frames\n\0");
		for(;;);
	}

	set_frame(idx * PAGESIZE);
	page->present = 1;
	page->rw = (is_writeable) ? 1 : 0;
	page->user = (is_kernel) ? 0 : 1;
	page->frame = idx;
}

void
free_frame(page_t *page)
{
	u32int	 frame;

	if (!(frame = page->frame))
		return;

	clear_frame(frame);
	page->frame = 0x0;

}

void
page_fault(registers_t regs)
{
	/*
	u32int	 faulting_address;
	int	 present;
	int	 rw;
	int	 us;
	int	 reserved;
	int	 id;
	*/

	//asm volatile("mov %%cr2, %0" : "=r"(faulting_address));

	/*	
	present = !(regs.err_code & 0x1);
	rw = regs.err_code & 0x2;
	us = regs.err_code & 0x4;
	reserved = regs.err_code & 0x8;
	id = regs.err_code * 0x10;
	*/

	puts("page fault\n\0");
	for(;;);

}

void
init_paging()
{
	page_t	*curpage;
	u32int	 mem_end_page = 0x1000000; // 16 MB
	int	 i = 0;

	// with 16mb we have a total of 4096 frames
	nframes = mem_end_page / PAGESIZE;

	// with 16mb we have 128 bytes for frames
	frames = (u32int *)kmalloc(INDEX_FROM_BIT(nframes));
	memset((unsigned char *)frames, 0, INDEX_FROM_BIT(nframes));

	kernel_directory = 
	    (page_directory_t *)kmalloc_a(sizeof(page_directory_t));

	//memset((unsigned char *)kernel_directory, 0, sizeof(page_directory_t));
	current_directory = kernel_directory;

	for(i=KHEAP_START; i<KHEAP_INIEND; i+=PAGESIZE) {
		get_page(i, 1, kernel_directory);
	}

	i = 0;
	while(i <= placement_address) {
		curpage = get_page(i, 1, kernel_directory);
		alloc_frame(curpage, 0, 0);
		i += PAGESIZE;
	}

	for(i=KHEAP_START; i<KHEAP_INIEND; i+=PAGESIZE) {
		curpage = get_page(i, 1, kernel_directory);
		alloc_frame(curpage, 0, 0);
	}

	irq_install_handler(14, (isr_t)page_fault);
	switch_page_directory(kernel_directory);

	kheap = create_heap(KHEAP_START, KHEAP_INIEND, 0xCFFFF000, 0, 0);
}


void
switch_page_directory(page_directory_t *dir)
{
	u32int	 cr0;

	current_directory = dir;
	__asm__ __volatile__ ("mov %0, %%cr3" :: "r"(&dir->tables_physical));
	__asm__ __volatile__ ("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000; // enable paging and flushes
	__asm__ __volatile__ ("mov %0, %%cr0" :: "r"(cr0));
}



/**
 * PAGE DIRECTORY STRUCTURE
 *
 * A page directory contains entries for 1024
 * page tables, every page table contains entries
 * for 1024 pages, here called frames.
 * Every frames is 0x1000(4k) in size
 *
 * PT  = page table
 * PTe = page table entries
 *
 *     ,________,         
 *     |PT0     |-------->,________,          FRAMES
 *     |PT1     |-----,   |PTe0    |-------->,________,      MEM ADDRESSES
 *     |PT2     |     |   |PTe1    |-----,   | 0x1000 | aka   0x00000000
 *     |...     | .   |   |...     |     |   | bytes  | 4kb      ....
 *     |...     | .   |   |PTe1023 |     |   `--------'       0x00000FFF
 *     |...     | .   |   `--------'     `-->,________,
 *     |PT1022  |     `-->,________,         | 0x1000 | aka   0x00001000
 *     |PT1023  |--,      |PTe0    |         | bytes  | 4kb      ....
 *     `--------'  |      |...     |         `--------'       0x00001FFF
 *                 |      |PTe1023 |             .
 *                 |      `--------'             .            0x00002000
 *                 `----->,________,             .               ...
 *                        |PTe0    |             .            0x00002FFF
 *                        |PTe1    |             .                .
 *                        |...     |             .                .
 *                        |PTe1023 |-----,       .                .
 *                        `--------'     |       .                .
 *                                       `-->,________,           .
 *                                           | 0x1000 | aka   0xFFFFF000
 *                                           | bytes  | 4kb      ...
 *                                           `--------'       0xFFFFFFFF
 *
 * assuming address 0xFFFFF000, we first need to see
 * in what frame does it fit, as we have a page size of
 * 0x1000 we simple divide it to see that our frame is
 * the frame number 1048575(0xFFFFF000 / 0x1000);
 * 
 * on what page table we have to look to find page entry
 * for frame  number 1048575 ?  we devide it by 1024 
 * because we have 1024 page tables, so:
 * 1048575 / 1024 = 1023. 
 *
 * now we know that our address is stored somewhere 
 * inside page table number 1023 but where exactly?
 *
 * we may discover exactly where using a mod operation to
 * get the offset inside the page table, so the frame number
 * 1048575 % 1024 is has an offset of 1023 inside page table
 * 1023;
 *
 * in a nutshell, our frame 0xFFFFF000 is stored on page table
 * 1023 with an offset of 1023(page table entry), in other 
 * words the last frame in memory;
 */
page_t *
get_page(u32int address, int make, page_directory_t *dir)
{
	u32int	 frame_idx;
	u32int	 page_table_idx;
	u32int	 page_entry_offset;
	u32int	 tmp;

	// what frame does this address belong to
	frame_idx = address / PAGESIZE;

	// what page table do this frame belong
	page_table_idx = frame_idx / 1024;

	// what page table entry we need to look at
	page_entry_offset = frame_idx % 1024;

	if (dir->tables[page_table_idx])
		return &dir->tables[page_table_idx]->pages[page_entry_offset];

	if (!make)
		return 0;

	// create a new page table and with all its page entries
	dir->tables[page_table_idx] = 
	    (page_table_t *)kmalloc_ap(sizeof(page_table_t), &tmp);
	memset((unsigned char *)dir->tables[page_table_idx], 0, PAGESIZE);

	tmp |= 0x7; // PRESENT, RW, US.
	dir->tables_physical[page_table_idx] = tmp;
	return &dir->tables[page_table_idx]->pages[page_entry_offset];
}

