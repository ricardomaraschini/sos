#include <system.h>
#include <paging.h>
#include <ordarray.h>
#include <kheap.h>
#include <video.h>

extern page_directory_t	*current_directory;
extern page_directory_t	*kernel_directory;
extern u32int		 end;
u32int			 placement_address = (u32int)&end;
heap_t			*kheap;

u32int
kmalloc_internal(u32int sz, int align, u32int *phys)
{
	u32int	tmp;

	if (align == 1 && (placement_address & NOTALIGNEDMASK)) {
		placement_address &= ALIGNEDMASK; // begin of last allocation
		placement_address += PAGESIZE; // next page
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
			if ((dtloc & NOTALIGNEDMASK) != 0) {

				// bytes beyound page alignment
				bytes_beyound = dtloc % PAGESIZE;

				// how many bytes we gonna 
				// loose on align process
				offset = PAGESIZE - bytes_beyound;
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

	if ((start % PAGESIZE) != 0)
		return NULL;

	if ((end % PAGESIZE) != 0) 
		return NULL;


	heap = (heap_t *)kmalloc(sizeof(heap_t));
	heap->index = place_ordarray(
	    start,
	    KHEAP_IDXSIZE,
	    &sort_mem_slots
	);

	// keep a safe place for our ordarray indexes
	// after this address we may start to alloc
	start += KHEAP_IDXSIZE * sizeof(type_t);
	if ((start & NOTALIGNEDMASK) != 0) {
		start &= ALIGNEDMASK;
		start += PAGESIZE;
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

	if ((newsize & NOTALIGNEDMASK) != 0) {
		newsize &= ALIGNEDMASK;
		newsize += PAGESIZE;
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
		i += PAGESIZE;
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

	if ((newsize & PAGESIZE) != 0) {
		newsize &= PAGESIZE;
		newsize += PAGESIZE;
	}

	if (newsize < KHEAP_MINSIZE)
		newsize = KHEAP_MINSIZE;

	i = cursize - PAGESIZE; // last allocated page
	while(i > newsize) {
		curpage = get_page(
		    h->start_addr + i,
		    0,
		    kernel_directory
		);

		free_frame(curpage);
		i -= PAGESIZE;
	}

	h->end_addr = h->start_addr + newsize;

	return newsize;
}

void *
alloc(u32int size, u8int align, heap_t *h)
{
	s32int		 i;
	s32int		 idx;
	u32int		 aux;
	u32int		 cval;
	u32int		 datapageoffset;
	u32int		 newsize;
	u32int		 oholesize;
	u32int		 oheaderpos;
	u32int		 nheaderpos;
	u32int		 odatapos;
	u32int		 newloc;
	u32int		 oldlen;
	u32int		 newlen;
	u32int		 oldendaddr;
	header_t	*oheader;
	header_t	*header;
	header_t	*blockheader;
	footer_t	*footer;
	footer_t	*blockfooter;

	// size must be summed up with 
	// header and footer size
	newsize = size + METADATASIZE;
	i = find_hole(newsize, align, h);

	// there is no hole
	if (i < 0) { 
		oldlen = h->end_addr - h->start_addr;
		oldendaddr = h->end_addr;

		expand(oldlen+newsize, h);
		newlen = h->end_addr - h->start_addr;

		idx = -1;
		cval = i = 0;
		while(i < h->index.size) {
			aux = (u32int)lookup_ordarray(i, &h->index);
			if (aux > cval) {
				cval = aux;
				idx = i;
			}
			i++;
		}


		if (idx == -1) {

			header = (header_t *)oldendaddr;
			header->magic = KHEAP_MAGIC;
			header->ishole = 1;
			header->size = newlen - oldlen;

			footer = (footer_t *)(oldendaddr + header->size - sizeof(footer_t));
			footer->magic = KHEAP_MAGIC;
			footer->header = header;
			insert_into_ordarray((void *)header, &h->index);

		} else {

			header = (header_t *)lookup_ordarray(idx, &h->index);
			header->size += newlen - oldlen;

			footer = (footer_t *)((u32int)header + header->size - sizeof(footer_t));
			footer->header = header;
			footer->magic = KHEAP_MAGIC;
		}

		return alloc(size, align, h);

	}

	oheader = (header_t *)lookup_ordarray(i, &h->index);
	nheaderpos = oheaderpos = (u32int)oheader;
	odatapos = (u32int)oheader + sizeof(header_t);
	oholesize = oheader->size;

	// we are unable to split the hole in two
	if ((oholesize - newsize) < METADATASIZE) {
		// increase size to get all space
		size += oholesize - newsize;
		newsize = oholesize;
	}

	datapageoffset = (odatapos & NOTALIGNEDMASK);
	if (align && datapageoffset) {

		// get the address of the next page's begin
		// and subtract sizeof(header_t) of it
		newloc = odatapos + PAGESIZE - datapageoffset - sizeof(header_t);

		header = (header_t *)oheaderpos;
		header->size = PAGESIZE - datapageoffset - sizeof(header_t);
		header->magic = KHEAP_MAGIC;
		header->ishole = 1;
		
		footer = (footer_t *)(newloc - sizeof(footer_t));
		footer->magic = KHEAP_MAGIC;
		footer->header = header;

		nheaderpos = newloc;
		oholesize -= header->size;
	} else {
		remove_from_ordarray(i, &h->index);
	}

	blockheader = (header_t *)nheaderpos;
	blockheader->magic = KHEAP_MAGIC;
	blockheader->ishole = 0;
	blockheader->size = newsize;

	blockfooter = (footer_t *)(nheaderpos + sizeof(header_t) + size);
	blockfooter->magic = KHEAP_MAGIC;
	blockfooter->header = blockheader;

	if (oholesize - newsize > 0) {
		header = 
		    (header_t *)(nheaderpos + sizeof(header_t) + size + sizeof(footer_t));
		header->magic = KHEAP_MAGIC;
		header->ishole = 1;
		header->size = oholesize - newsize;
		footer = (footer_t *)((u32int)header + oholesize - newsize - sizeof(footer_t));
		if ((u32int)footer < h->end_addr) {
			footer->magic = KHEAP_MAGIC;
			footer->header = header;
		}

		insert_into_ordarray((void*)header, &h->index);
	}

	return (void *)((u32int)blockheader + sizeof(header_t));
}


void
free(void *p, heap_t *h)
{
	u8int		 add;
	u32int		 osize;
	u32int		 i;
	u32int		 oldlen;
	u32int		 newlen;
	header_t	*header;
	header_t	*nextheader;
	footer_t	*footer;
	footer_t	*previousfooter;

	if (p == 0)
		return;


	header = (header_t *)((u32int)p - sizeof(header_t));
	footer = (footer_t *)((u32int)header + header->size - sizeof(footer_t));

	if (header->magic != KHEAP_MAGIC) //XXX
		return;

	if (footer->magic != KHEAP_MAGIC) //XXX
		return;

	header->ishole = 1;
	add = 1;
	previousfooter = (footer_t *)(header - sizeof(footer_t));
	
	if (
	    previousfooter->magic == KHEAP_MAGIC &&
	    previousfooter->header->ishole == 1
	) {
		// merge holes
		osize = header->size;
		header = previousfooter->header;
		footer->header = header;
		header->size += osize;
		add = 0;
	}


	nextheader = (header_t *)((u32int)footer + sizeof(footer_t));
	if (
	    nextheader->magic == KHEAP_MAGIC &&
	    nextheader->ishole == 1
	) {
		header->size += nextheader->size;
		footer = (footer_t *)((u32int)nextheader + nextheader->size - sizeof(footer_t));
		
		i = 0;
		while((i < h->index.size) &&
		    lookup_ordarray(i, &h->index) != (void *)header) {
			i++;
		}

		if (i == h->index.size) //XXX
			return;

		remove_from_ordarray(i, &h->index);
	}


	// contract
	if ((u32int)footer + sizeof(footer_t) == h->end_addr) {
		oldlen = h->end_addr - h->start_addr;
		newlen = contract((u32int)header - h->start_addr, h);

		if (header->size - (oldlen - newlen) > 0) {
			header->size -= oldlen - newlen;
			footer = (footer_t *)((u32int)header + header->size - sizeof(footer_t));
			footer->magic = KHEAP_MAGIC;
			footer->header = header;
		} else {
			i = 0;
			while ((i < h->index.size) &&
			    lookup_ordarray(i, &h->index) != (type_t)nextheader) {
				i++;
			}

			if (i == h->index.size)
				return; //XXX

			remove_from_ordarray(i, &h->index);
		}

	}

	if (add)
		insert_into_ordarray(header, &h->index);

}
