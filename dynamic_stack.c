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

	dst->cap = cap;
	dst->top = -1;
	dst->meta_size = meta_size;
	dst->entry = calloc(cap, meta_size);

	return dst;
}

int dstack_isempty(dstack_t *st)
{
	return st->top == -1;
}

void* dstack_pop(dstack_t *st)
{
	if (!dstack_isempty(st)) {
		st->top--;
		return dstack_nth_addr(st, st->top+1);
	}
	return NULL;
}

int dstack_push(dstack_t *st, void *ele)
{
	if (st->top == st->cap - 1) {
		void*tmp = realloc(st->entry, st->meta_size * 2 * st->cap);
		if (!tmp) 
			return -1;
		else
			st->entry = tmp;
		st->cap *= 2;
	}
	memcpy(dstack_nth_addr(st, st->top+1), ele, st->meta_size);
	st->top++;
	return 0;
}

void dstack_destory(dstack_t *st)
{
	if (st)
		free(st->entry);
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
