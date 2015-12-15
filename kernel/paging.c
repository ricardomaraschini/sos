#include <system.h>
#include <paging.h>
#include <kheap.h>
#include <video.h>
#include <memutil.h>
#include <int.h>

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

	frame = frame_addr / 0x1000; //4kb
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

	frame = frame_addr / 0x1000; // 4kb
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

	set_frame(idx * 0x1000);
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
page_fault(register_t *regs)
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

	puts("page fault\0");
	for(;;);

}

void
initialise_paging()
{
	u32int	 mem_end_page = 0x1000000; // 16 MB
	int	 i = 0;

	puts("initialising paging system\n\0");

	nframes = mem_end_page / 0x1000;
	frames = (u32int *)kmalloc(INDEX_FROM_BIT(nframes));
	memset((unsigned char *)frames, 0, INDEX_FROM_BIT(nframes));

	kernel_directory = 
	    (page_directory_t *)kmalloc_a(sizeof(page_directory_t));

	memset((unsigned char *)kernel_directory, 0, sizeof(page_directory_t));
	current_directory = kernel_directory;

	while(i < placement_address) {
		alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
		i += 0x1000;
	}

	irq_install_handler(IRQ_PAGEFAULT, (isr_t)page_fault);

	switch_page_directory(kernel_directory);
	puts("paging system initialised\n\0");
}


void
switch_page_directory(page_directory_t *dir)
{
	u32int	 cr0;

	current_directory = dir;
	asm volatile("mov %0, %%cr3" : : "r"(dir->tables_physical));
	asm volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000; // enable paging
	asm volatile("mov %0, %%cr0" : : "r"(cr0));
}

page_t *
get_page(u32int address, int make, page_directory_t *dir)
{
	u32int	 table_idx;
	u32int	 tmp;

	address /= 0x1000;
	table_idx = address / 1024;

	if (dir->tables[table_idx])
		return &dir->tables[table_idx]->pages[address % 1024];

	if (!make)
		return 0;

	dir->tables[table_idx] = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &tmp);	
	memset((unsigned char *)dir->tables[table_idx], 0, 0x1000);
	dir->tables_physical[table_idx] = tmp | 0x7; // PRESENT, RW, US.
	return &dir->tables[table_idx]->pages[address % 1024];
}

