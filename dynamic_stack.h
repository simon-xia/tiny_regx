#ifndef _DYNAMIC_STACK_H_
#define _DYNAMIC_STACK_H_

#include "dynamic_array.h"

#define DSTACK_DEFAULT_SIZE	16

typedef struct dstack {
	darray_t *arr;
}dstack_t;

dstack_t* dstack_init(int, int);
int dstack_isempty(dstack_t *);
void* dstack_pop(dstack_t *);
int dstack_push(dstack_t *, void *);
void dstack_destory(dstack_t *);

#endif
