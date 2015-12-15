#include <system.h>
#include <ordarray.h>
#include <kheap.h>
#include <string.h>
#include <memutil.h>

u8int
default_cmpfnc(type_t a, type_t b)
{
	return (a < b) ? 1 : 0;
}

ordarray_t
create_ordarray(u32int max_size, cmpfnc_t cmpfnc)
{
	ordarray_t	oa;

	oa.max_size = max_size;
	oa.array = (type_t *)kmalloc(max_size * sizeof(type_t));
	oa.size = 0;
	oa.cmpfnc = cmpfnc;

	return oa;
}


ordarray_t
place_ordarray(u32int addr, u32int max_size, cmpfnc_t cmpfnc)
{
	ordarray_t	oa;

	oa.max_size = max_size;
	oa.array = (type_t *)addr;
	memset((unsigned char *)&oa.array, 0, sizeof(type_t) * max_size);
	oa.size = 0;
	oa.cmpfnc = cmpfnc;

	return oa;
}

void
insert_into_ordarray(type_t item, ordarray_t *oa)
{
	int	i;
	type_t	aux;
	type_t	aux2;

	for(i=0; i<oa->size; i++) {
		if (!oa->cmpfnc(oa->array[i], item))
			break;
	}

	if (i == oa->size) {
		oa->array[oa->size] = item;
		oa->size += 1;
		return;
	}

	aux = oa->array[i];
	oa->array[i] = item;
	while(i < oa->size) {
		i++;
		aux2 = oa->array[i];
		oa->array[i] = aux;
		aux = aux2;
	}

	oa->size += 1;
}

type_t
lookup_ordarray(u32int i, ordarray_t *oa)
{
	if (i >= oa->size)
		return NULL;
	return oa->array[i];
}


void
remove_from_ordarray(u32int i, ordarray_t *oa)
{
	if (i >= oa->size)
		return;

	while(i < oa->size) {
		oa->array[i] = oa->array[i + 1];
		i += 1;
	}
	oa->size += 1;
}
