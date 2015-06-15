#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_

typedef int (*hash_func)(void *);
typedef void (*destory_func)(void *);
typedef int (*key_cmp_func)(void *, void *);

typedef struct ht_node{
	void *key;
	void *val;
	struct ht_node *next;
}ht_node_t;

typedef struct ht{
	int node_cnt;
	int slot_size;
	hash_func hfunc;
	key_cmp_func key_cmp;
	destory_func val_destory;
	destory_func key_destory;
	ht_node_t **entry;
}ht_t;

typedef struct ht_iter{
	ht_t *ht;
	int slot_idx;
	ht_node_t *cur_node;
}ht_iter_t;

ht_iter_t* ht_iter_init(ht_t*); 
void ht_iter_rewind(ht_iter_t *);
ht_node_t* ht_iter_next(ht_iter_t *);

ht_t* ht_init(int, hash_func, key_cmp_func, destory_func, destory_func);
ht_node_t* ht_new_node(void*, void*);
int ht_add(ht_t *, void*, void*);
ht_node_t* ht_find(ht_t *, void*);
int ht_delete(ht_t *, void*);
int ht_destory(ht_t *);

#endif
