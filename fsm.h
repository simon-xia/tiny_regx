#ifndef _FSM_H_
#define _FSM_H_

#include "hash_table.h"

#define FSM_DEFALUT_STATE_SLOT_SIZE	 10
#define FSM_DEFALUT_EDGE_SLOT_SIZE	 10

typedef struct fsm{
	int state_cnt, edge_cnt;
	ht_t *state, *edge;
}fsm_t;

fsm_t* fsm_init();
int fsm_add_state(fsm_t *, state_t *);
int fsm_add_edge(fsm_t *, edge_t *);
int fsm_del_state(fsm_t *, state_t *);
int fsm_del_edge(fsm_t *, edge_t *);
int fsm_destory(fsm_t *);

#endif
