#include <stdlib.h>
#include <string.h>

#include "dynamic_array.h"

darray_t *darray_init(int cap, int meta_size)
{
	darray_t *arr = (darray_t*)malloc(sizeof(darray_t));
	if (!arr)
		return NULL;

	arr->cap = cap;
	arr->size = 0;
	arr->meta_size = meta_size;
	arr->entry = calloc(cap, meta_size);
	if (!arr->entry)
		return NULL;

	return arr;
}

int darray_addback(darray_t *arr, void *ele)
{
	if (!arr)
		return -1;

	if (arr->cap == arr->size) {
		void*tmp = realloc(arr->entry, arr->meta_size * 2 * arr->cap);
		if (!tmp) 
			return -1;
		else
			arr->entry = tmp;
		arr->cap *= 2;
	}
	memcpy(darray_nth_addr(arr, arr->size), ele, arr->meta_size);
	arr->size++;
	return 0;
}

int darray_fitsize(darray_t *arr)
{
	if (arr->cap > arr->size) {
		arr->entry = realloc(arr->entry, arr->meta_size * arr->size);
		arr->cap = arr->size;
	}
}

void darray_destory(darray_t *arr)
{
	if (arr)
		free(arr->entry);
	free(arr);
}

#ifdef UNIT_TEST_DARRAY

#include <stdio.h>

int main()
{
	darray_t *arr_int = darray_init(4, sizeof(int));
	darray_t *arr_char = darray_init(4, sizeof(char));

	int i, *p_i;
	char c, *p_c;
	for (i = 97; i < 120; i++)
	{
		darray_addback(arr_int, (void*)&i);
		c = i;
		darray_addback(arr_char, (void*)&c);
	}

	for (i = 0; i < arr_int->size; i++)
	{
		p_i = darray_nth_addr(arr_int, i);
		printf("%d ", *p_i);
	}
	printf("\n");

	for (i = 0; i < arr_char->size; i++)
	{
		p_c = darray_nth_addr(arr_char, i);
		printf("%c ", *p_c);
	}
	printf("\n");

	darray_destory(arr_int);
	darray_destory(arr_char);
	return 0;
}

#endif
