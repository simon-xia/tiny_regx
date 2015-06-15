#include <stdlib.h>
#include <string.h>

#include "dynamic_stack.h"

dstack_t* dstack_init(int cap, int meta_size)
{
	if (!cap)
		cap = DSTACK_DEFAULT_SIZE;

	dstack_t* dst = (dstack_t*)calloc(1, sizeof(dstack_t));
	if (!dst)
		return NULL;

	dst->arr = darray_init(cap, meta_size);

	return dst;
}

int dstack_isempty(dstack_t *st)
{
	return st->arr->size == 0;
}

void* dstack_pop(dstack_t *st)
{
	if (!dstack_isempty(st)) {
		st->arr->size--;
		return darray_nth_addr(st->arr, st->arr->size);
	}
	return NULL;
}

int dstack_push(dstack_t *st, void *ele)
{
	return darray_addback(st->arr, ele);
}

void dstack_destory(dstack_t *st)
{
	if (st)
		darray_destory(st->arr);
	free(st);
}

#ifdef UNIT_TEST_DSTACK
#include <stdio.h>

int main()
{
	dstack_t* char_st = dstack_init(4, sizeof(char));
	dstack_t* int_st = dstack_init(0, sizeof(int));

	int i, *p_i;
	char c, *p_c;
	for (i = 97; i < 120; i++)
	{
		dstack_push(int_st, (void*)&i);
		c = i;
		dstack_push(char_st, (void*)&c);
	}

	while(!dstack_isempty(int_st)) {
		p_i = dstack_pop(int_st);
		printf("%d ", *p_i);
	}
	printf("\n");

	while(!dstack_isempty(char_st)) {
		p_c = dstack_pop(char_st);
		printf("%c ", *p_c);
	}
	printf("\n");

	dstack_destory(char_st);
	dstack_destory(int_st);
	return 0;
}
#endif
