#ifndef _DYNAMIC_STACK_H_
#define _DYNAMIC_STACK_H_

#define DSTACK_DEFAULT_SIZE	16

typedef struct dstack {
	int cap;
	int top;
	int meta_size;
	void* entry;
}dstack_t;

#define dstack_nth_addr(s, n) (void*)((char*)((s)->entry) + (n)*((s)->meta_size))

dstack_t* dstack_init(int, int);
int dstack_isempty(dstack_t *);
void* dstack_pop(dstack_t *);
int dstack_push(dstack_t *, void *);
void dstack_destory(dstack_t *);

#endif
