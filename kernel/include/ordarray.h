#ifndef __ORDARRAY_H
#define __ORDARRAY_H

typedef void *		type_t;
typedef s8int		(*cmpfnc_t)(type_t, type_t);

typedef struct ordarray {
	type_t		*array;
	u32int		 size;
	u32int		 max_size;
	cmpfnc_t	 cmpfnc;	 
} ordarray_t;


u8int default_cmpfnc(type_t, type_t);
ordarray_t create_ordarray(u32int, cmpfnc_t);
ordarray_t place_ordarray(void *, u32int, cmpfnc_t);
void insert_into_ordarray(type_t, ordarray_t *);
type_t lookup_ordarray(u32int, ordarray_t *);
void remove_from_ordarray(u32int, ordarray_t *);



#endif
