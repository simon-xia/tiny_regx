#include "fsm.h"

extern void state_destory(void *);

static int addr_hash(void*);
static int addr_cmp(void*, void*);

static int addr_hash(void *addr)
{
	return 0x03 & (int)(addr);
}

static int addr_cmp(void *a1, void *a2)
{
	return (unsigned int)a1 == (unsigned int)a2;
}

fsm_t* fsm_init()
{
	fsm_t *fsm = (fsm_t*)malloc(sizeof(fsm_t));
	if (fsm) {
		fsm->state_cnt = 0;
		fsm->edge_cnt = 0;
		fsm->state = ht_init(FSM_DEFALUT_STATE_SLOT_SIZE, addr_hash, addr_cmp, state_destory, NULL);
		fsm->edge = ht_init(FSM_DEFALUT_EDGE_SLOT_SIZE, addr_hash, addr_cmp, free, NULL);
	}
	return fsm;
}

int fsm_add_state(fsm_t *fsm, state_t *st)
{
	fsm->state_cnt++;
	return ht_add(fsm->state, st, NULL);
	// if fail, need to recover state_cnt
}

int fsm_add_edge(fsm_t *fsm, edge_t *e)
{
	fsm->edge_cnt++;
	return ht_add(fsm->edge, e, NULL);
	// if fail, need to recover edge_cnt
}

int fsm_del_state(fsm_t *fsm, state_t *st)
{
	fsm->state_cnt--;
	return ht_delete(fsm->state, st);
}

int fsm_del_edge(fsm_t *fsm, edge_t *e)
{
	fsm->edge_cnt--;
	return ht_delete(fsm->edge, e);
}

int fsm_destory(fsm_t *fsm)
{
	ht_destory(fsm->state);
	ht_destory(fsm->edge);
	free(fsm);
}
