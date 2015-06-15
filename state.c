#include "state.h"

int state_add_inedge(state_t *st, edge_t *e)
{
	return darray_addback(st->in, (void*)&e);
}

int state_add_outedge(state_t *st, edge_t *e)
{
	return darray_addback(st->out, (void*)&e);
}

void set_state_terminal(state_t *st)
{
    st->is_terminal = 1;
}

edge_t* new_edge()
{
    return (edge_t*)calloc(1, sizeof(edge_t));
}

edge_t* new_cond_edge(int c)
{
    edge_t *e = new_edge();
    if (e)
        e->val = c;
    return e;
}

edge_t* new_epsilon_edge()
{
	return new_cond_edge(EPSILON);
}

state_t* new_state(int in_cnt, int out_cnt)
{
    state_t *st = (state_t*)calloc(1, sizeof(state_t));
    if (!st)
        goto err;

    if ((st->in = darray_init(in_cnt, sizeof(edge_t*))) == NULL)
        goto err;

    if ((st->out = darray_init(out_cnt, sizeof(edge_t*))) == NULL)
        goto err;

    return st;    

err:
    if (st)
        free(st->in);
    free(st);
    return NULL;
}

void state_destory(void *st)
{
	state_t *s = (state_t*)st;
    if (s) {
        darray_destory(s->in);
        darray_destory(s->out);
    }
	free(s);
}

state_unit_t* new_state_unit(state_t *st_start, state_t *st_end)
{
    state_unit_t * st_un = (state_unit_t*)malloc(sizeof(state_unit_t));
	if (st_un){
		st_un->head = st_start;
		st_un->tail = st_end;
	}

    return st_un;
}

/*                           
 *  +-----+   c   +-----+
 *  | st1 |--->---| st2 |  
 *  +-----+       +-----+ 
 */                        
state_unit_t * new_state_normal_char(char c)
{
    edge_t *e = NULL;
    state_t *st1 = new_state(0, 1);
    state_t *st2 = new_state(1, 0);
    if (!st1 || !st2)
		goto err;

    if (!(e = new_cond_edge(c)))
        goto err;

    e->from = st1;
    e->to = st2;

	state_add_outedge(st1, e);
	state_add_inedge(st2, e);

    return st;

err:
	free(e);
    state_destory(st1);
    state_destory(st2);
    return NULL;
}

state_unit_t* state_connect(state_unit_t *st_un1, state_unit_t *st_un2)
{
    edge_t *e = NULL;
    if (!(e = new_epsilon_edge()))

	state_add_outedge(st_un1->tail, e);
	state_add_inedge(st_un2->head, e);

    st_un1->tail = st_un2->tail;

    free(st_un2);
    return st_un1;
}

state_unit_t* state_alternate(state_unit_t *st_un1, state_unit_t *st_un2)
{
	state_t *st_start = NULL, *st_end = NULL;
	edge_t *in1 = NULL, *in2 = NULL, *out1 = NULL, *out2 = NULL;

    st_start = new_state(0, 2);
    st_end = new_state(2, 0);

    in1 = new_epsilon_edge();
    in2 = new_epsilon_edge();
    out1 = new_epsilon_edge();
    out2 = new_epsilon_edge();

	if (!st_start || !st_end || !in1 || !in2 || !out1 || !out2)
		goto err;

	state_add_outedge(st_start, in1);
	state_add_outedge(st_start, in2);

	state_add_inedge(st_end, out1);
	state_add_inedge(st_end, out2);

	state_add_inedge(st_un1->head, in1);
	state_add_inedge(st_un2->head, in2);

	state_add_outedge(st_un1->tail, out1);
	state_add_outedge(st_un2>tail, out2);

    st_un1->head = st_start;
    st_un1->tail = st_end;
    free(st_un2);
    return st_un1;

err:
    state_destory(st_start);
    state_destory(st_end);
	free(in1);
	free(in2);
	free(out1);
	free(out2);
    return NULL;
}

state_unit_t* state_closure(state_unit_t *st_un, char type)
{
    state_t *st_start = NULL, *st_end = NULL;
    edge_t *e1 = NULL, *e2 = NULL, *back = NULL;

	st_start = new_state(0, 1);
	st_end = new_state(1, 0);

    e1 = new_epsilon_edge(); 
    e2 = new_epsilon_edge(); 
	back = new_epsilon_edge();

    if (!st_start || !st_end || !e1 || !e2 || !back)
        goto err;

	state_add_outedge(st_start, e1);
	state_add_inedge(st_un->head, e1);

	state_add_inedge(st_end, e2);

    if (type == '?') {
		state_add_inedge(st_un->tail, back);
		state_add_outedge(st_un->head, back);

		state_add_outedge(st_un->tail, e2);
	}
	else {
		state_add_inedge(st_un->head, back);
		state_add_outedge(st_un->tail, back);

		if (type == '*')
			state_add_outedge(st_un->head, e2);
		else 
			state_add_outedge(st_un->tail, e2);

	}

	st_un->head = st_start;
	st_un->tail = st_end;

	return st_un;

err:
    state_destory(st_start);
    state_destory(st_end);
    free(e1);
    free(e2);
    free(back);
    return NULL;
}

/*
 *            in[0]      out[0]
 *    ---e-->--+--------+-->--e--        
 *             |st_start|
 *       +-->--+--------+----+
 *       |  in[1]     out[1] |
 *       |      st_un        |
 *       +-------------------+
 *       
 */
state_unit_t* state_closure_star(state_unit_t *st_un)
{
    return state_closure(st_un, '*');
}

/*
 *            in[0]     in[1]        out[1]       out[0]
 *    ---e-->--+--------+--<--e----<--+---------+-->--e--
 *             |st_start|             | st_end  |
 *             +--------+-->------->--+---------+
 *                    out[0] st_un  in[0]
 *
 */
state_unit_t* state_closure_plus(state_unit_t *st_un)
{
    return state_closure(st_un, '+');
}

/*
 *            in[0]      out[1]        in[1]       out[0]
 *    ---e-->--+--------+-->--e---->--+---------+-->--e--
 *             |st_start|             | st_end  |
 *             +--------+-->------->--+---------+
 *                     st_un  
 *                                              
 */
state_unit_t* state_closure_0or1(state_unit_t *st_un)
{
    return state_closure(st_un, '?');
}

state_unit_t* state_add_terminal(state_unit_t *st_un)
{
	edge_t *e = NULL;
	state_t *st = new_state(1, 0);
	if (!st)
		goto err;

	set_state_terminal(st);

	if (!(e = new_epsilon_edge()))
		goto err;

	state_add_inedge(st, e);
	state_add_outedge(st_un->tail, e);

	st_un->tail = st;
	return st_un;

err:
	state_destory(st);
	free(e);
	return NULL;
}
