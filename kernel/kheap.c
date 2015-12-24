#include <system.h>
#include <paging.h>
#include <ordarray.h>
#include <kheap.h>

extern page_directory_t	*current_directory;
extern page_directory_t	*kernel_directory;
extern u32int		 end;
u32int			 placement_address = (u32int)&end;

u32int
kmalloc_internal(u32int sz, int align, u32int *phys)
{
	u32int	tmp;

	if (align == 1 && (placement_address & 0xFFFFF000)) {
		placement_address &= 0xFFFFF000; // begin of last allocation
		placement_address += 0x1000; // next page
	}

	// return the physical address
	if (phys)
		*phys = placement_address;

	tmp = placement_address;
	placement_address += sz;
	return tmp;
}


u32int
kmalloc_a(u32int sz)
{
	return kmalloc_internal(
	   sz,
	   1,
	   0
	);
}

u32int
kmalloc_p(u32int sz, u32int *phys)
{
	return kmalloc_internal(
	   sz,
	   0,
	   phys
	);
}

u32int
kmalloc_ap(u32int sz, u32int *phys)
{
	return kmalloc_internal(
	   sz,
	   1,
	   phys
	);
}

u32int
kmalloc(u32int sz)
{
	return kmalloc_internal(
	   sz,
	   0,
	   0
	);
}


// heap management
static s32int
find_hole(u32int size, u8int align, heap_t *hp)
{
	u32int		 i = 0;
	header_t	*header;
	u32int		 dtloc = 0;
	u32int		 loc  = 0;
	s32int		 offset = 0;
	s32int		 hsize;
	u32int		 bytes_beyound;

	while(i < hp->index.size) {
		header = (header_t *)lookup_ordarray(i, &hp->index);
		if (align > 0) {

			offset = 0;
			loc = (u32int)header;

			/* 
			 * H - header
			 * D - data space
			 * F - footer
			 * |HHHHHHH|DDDDDDDDDDDDDDDD|FFFFF|
			 */

			/*
			 * |HHHHHHH|DDDDDDDDDDDDDDDD|FFFFF|
			 *         ^--> dtloc
			 */
			dtloc = loc + sizeof(header_t);

			// if dtloc is not page aligned
			if ((dtloc & 0XFFFF0000) != 0) {

				// bytes beyound page alignment
				bytes_beyound = dtloc % 0x1000;

				// how many bytes we gonna 
				// loose on align process
				offset = 0x1000 - bytes_beyound;
			}

			hsize = (s32int)header->size - offset;
			if (hsize >= (s32int)size)
				break;

		} else if (header->size >= size) {
			break;
		}

		i++;
	}

	if (i == hp->index.size)
		return -1;

	return i;

}


// our cmpfnc to sort slots by size, asc
static s8int
sort_mem_slots(void *a, void *b)
{
	header_t	*x;
	header_t	*y;

	x = (header_t *)a;
	y = (header_t *)b;

	return (x->size < y->size) ? 1 : 0;
}

heap_t *
create_heap(u32int start, u32int end, u32int max, u8int sup, u8int ro)
{
	heap_t		*heap;
	header_t	*huge_hole;

	if ((start & 0xFFFF0000) != 0)
		return NULL;

	if ((end & 0xFFFF0000) != 0) 
		return NULL;

	heap = (heap_t *)kmalloc(sizeof(heap_t));
	heap->index = place_ordarray(
	    start,
	    KHEAP_IDXSIZE,
	    &sort_mem_slots
	);

	// keep a safe place for our ord array indexes
	// after this address we may start to alloc
	start += KHEAP_IDXSIZE * sizeof(type_t);
	if ((start & 0xFFFF0000) != 0) {
		start &= 0xFFFF0000;
		start += 0x1000;
	}

	heap->start_addr = start;
	heap->end_addr = end;
	heap->max_addr = max;
	heap->supervisor = sup;
	heap->readonly = ro;

	huge_hole = (header_t *)kmalloc(sizeof(header_t));
	huge_hole->magic = KHEAP_MAGIC;
	huge_hole->size = end - start;
	huge_hole->ishole = 1;

	insert_into_ordarray((type_t)huge_hole, &heap->index);

	return heap;
}

static void
expand(u32int newsize, heap_t *h)
{
	u32int	 cursize;
	u32int	 i;
	page_t	*newpage;

	cursize = h->end_addr - h->start_addr;
	if (newsize <= cursize)
		return;

	if ((newsize & 0xFFFF0000) != 0) {
		newsize &= 0xFFFF0000;
		newsize += 0x1000;
	}

	if (newsize > h->max_addr)
		return;

	i = cursize;
	while(i < newsize) {

		newpage = get_page(
		    h->start_addr + i,
		    1,
		    kernel_directory 
		);

		alloc_frame(
		    newpage,
		    (h->supervisor) ? 1 : 0,
		    (h->readonly) ? 1 : 0
		);
		i += 0x1000;
	}


	h->end_addr = h->start_addr + newsize;
}

static u32int
contract(u32int newsize, heap_t *h)
{
	page_t	*curpage;
	u32int	 cursize;
	u32int	 i;

	cursize = h->end_addr - h->start_addr;
	if (newsize > cursize)
		return 0;

	if ((newsize & 0x1000) != 0) {
		newsize &= 0x1000;
		newsize += 0x1000;
	}

	if (newsize < KHEAP_MINSIZE)
		newsize = KHEAP_MINSIZE;

	i = cursize - 0x1000; // last allocated page
	while(i > newsize) {
		curpage = get_page(
		    h->start_addr + i,
		    0,
		    kernel_directory
		);

		free_frame(curpage);
		i -= 0x1000;
	}

	h->end_addr = h->start_addr + newsize;

	return newsize;
}

void *
alloc(u32int size, u8int align, heap_t *h)
{
	u32int		 newsize;
	s32int		 i;
	header_t	*oheader;
	u32int		 oholesize;
	u32int		 oholepos;

	newsize += size + sizeof(header_t) + sizeof(footer_t);
	i = find_hole(newsize, align, h);

	if (i < 0) {
		// well well well //XXX
	}

	oheader = (header_t *)lookup_ordarray(i, &h->index);
	oholepos = (u32int)oheader;
	oholesize = oheader->size;

	// we are unable to split the hole in two
	if ((oholesize - newsize) < (sizeof(header_t) + sizeof(footer_t))) {
		// increase size to get all space
		size += oholesize - newsize;
		newsize = oholesize;
	}


}
