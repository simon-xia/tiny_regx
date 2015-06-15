#include "hash_table.h"

#include <stdlib.h>
#include <string.h>

ht_iter_t* ht_iter_init(ht_t* ht) 
{
	ht_iter_t *iter = (ht_iter_t*)malloc(sizeof(ht_iter_t));
	if (!iter)
		return NULL;

	iter->ht = ht;
	iter->slot_idx = 0;
	iter->cur_node = ht->entry[0];

	return iter;
}

void ht_iter_rewind(ht_iter_t *iter)
{
	iter->slot_idx = 0;
	iter->cur_node = iter->ht->entry[0];
}

ht_node_t* ht_iter_next(ht_iter_t *iter)
{
	if (iter->cur_node == NULL) {
		while (iter->cur_node == NULL) {
			iter->slot_idx++;
			if (iter->slot_idx >= iter->ht->slot_size)
				return NULL;
			iter->cur_node = iter->ht->entry[iter->slot_idx];
		}
	}
	ht_node_t *tmp = iter->cur_node;
	iter->cur_node = iter->cur_node->next;

	return tmp;
}

ht_t* ht_init(int slot_size, hash_func hfunc, key_cmp_func key_cmp, destory_func key_d, destory_func val_d)
{
	ht_t *ht = (ht_t*)calloc(1, sizeof(ht_t));
	if (!ht)
		goto err;

	ht->entry = (ht_node_t**)calloc(slot_size, sizeof(ht_node_t*));
	if (!ht->entry)
		goto err;

	ht->hfunc = hfunc;
	ht->key_cmp = key_cmp;
	ht->slot_size = slot_size;
	ht->node_cnt = 0;
	ht->key_destory = key_d;
	ht->val_destory = val_d;

	return ht;
err:
	free(ht);
	return NULL;
}

ht_node_t* ht_new_node(void *key, void* val)
{
	ht_node_t *node = (ht_node_t*)calloc(1, sizeof(ht_node_t));
	if (node) {
		node->key = key;
		node->val = val;
	}
	return node;
}

// if key exist, update val
int ht_add(ht_t *ht, void* key, void* val)
{
	ht_node_t *tmp = ht_find(ht, key);
	if (tmp) {
		if (ht->val_destory)
			ht->val_destory(tmp->val);
		tmp->val = val;
	}
	else {
		ht_node_t *new_node = ht_new_node(key, val);
		if (!new_node)
			return -1;

		int i = ht->hfunc(key);
		int idx = i & (ht->slot_size - 1);

		new_node->next = ht->entry[idx];
		ht->entry[idx] = new_node;

		ht->node_cnt++;
		return 0;
	}
}

ht_node_t* ht_find(ht_t *ht, void* key)
{
	int i = ht->hfunc(key);
	int idx = i & (ht->slot_size - 1);

	ht_node_t *tmp = ht->entry[idx];
	while(tmp) {
		if (ht->key_cmp(tmp->key, key) == 0)
			return tmp;
		tmp = tmp -> next;
	}
	return NULL;
}

void * ht_get_val_from_key(ht_t *ht, void*key)
{
	ht_node_t* tmp = ht_find(ht, key);
	if (tmp)
		return tmp->val;

	return NULL;
}

int ht_delete(ht_t *ht, void* key)
{
	int i = ht->hfunc(key);
	int idx = i & (ht->slot_size - 1);

	ht_node_t *tmp = ht->entry[idx], *pre = NULL;
	while(tmp) {
		if (ht->key_cmp(tmp->key, key) == 0) {
			if (pre) 
				pre->next = tmp->next;
			else 
				ht->entry[idx] = tmp->next;

			if (ht->key_destory)
				ht->key_destory(tmp->key);

			if (ht->val_destory)
				ht->val_destory(tmp->val);

			free(tmp);
			return 0;
		}
		pre = tmp;
		tmp = tmp -> next;
	}

	return -1;
}

int ht_destory(ht_t *ht)
{
	ht_node_t *tmp;
	ht_iter_t *iter = ht_iter_init(ht);

	while ((tmp = ht_iter_next(iter)) != NULL) {
		if (ht->key_destory)
			ht->key_destory(tmp->key);

		if (ht->val_destory)
			ht->val_destory(tmp->val);
		free(tmp);
	}

	free(iter);

	free(ht->entry);
	free(ht);
	return 0;
}

#ifdef UNIT_TEST_HT

#include <stdio.h>

char *test_str[] = {
	"simon",
	"jack",
	"qmm",
	"haha",
	"hehe",
	"simonxia"
};

int hash(void *s) {
	int sum = 0;
	char *str = (char*)s;
	while (*str) {
		sum += *str++;
	}
	return sum;
}

int key_cmp(void *k1, void *k2)
{
	return strcmp((char*)k1, (char*)k2);
}

int main() {
	int i;

	ht_t *ht = ht_init(8, hash, key_cmp, NULL, NULL);
	ht_iter_t *iter;
	ht_node_t *tmp;

	for (i = 0; i < sizeof(test_str)/sizeof(char*); i++)
	{
		ht_add(ht, (void*)test_str[i], (void*)(i+100));
	}

	for (i = 0; i < 20; i++)
		ht_add(ht, (void*)test_str[0], (void*)(i));

	iter = ht_iter_init(ht);
	while ((tmp = ht_iter_next(iter)) != NULL) {
		printf("%s\t%d\n", (char*)tmp->key, (int)tmp->val);
	}

	for (i = 0; i < sizeof(test_str)/sizeof(char*); i++)
		printf("find %s: %d\n", test_str[i], (int)ht_get_val_from_key(ht, test_str[i]));

	ht_delete(ht, "simon");

	ht_iter_rewind(iter);
	while ((tmp = ht_iter_next(iter)) != NULL) {
		printf("%s\t%d\n", (char*)tmp->key, (int)tmp->val);
	}

	free(iter);
	ht_destory(ht);
	return 0;
}

#endif
