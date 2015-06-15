#ifndef _DYNAMIC_ARRAY_H_
#define _DYNAMIC_ARRAY_H_

typedef struct darray {
	int cap;
	int size;
	int meta_size;
	void *entry;
}darray_t;

#define darray_nth_addr(a, n) (void*)((char*)((a)->entry) + (n)*((a)->meta_size))

darray_t* darray_init(int, int);
int darray_addback(darray_t*, void*);
int darray_fitsize(darray_t* arr);
void darray_destory(darray_t*);

#endif
