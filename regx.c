#include <stdlib.h>
#include <ctype.h>

#include "dynamic_stack.h"

#define EPSILON        -1
#define ANY_CHAR       -2
#define CONNECT_CHAR   '#'

struct edge;

typedef struct state {
    int is_terminal;
    struct edge **in, **out;
}state_t;

typedef struct edge {
    int val;
    struct state *from, *to;
}edge_t;

typedef struct state_unit {
    struct state *head, *tail;
}state_unit_t;

void set_state_terminal(state_t *st)
{
    st->is_terminal = 1;
}

edge_t* new_edge()
{
    return (edge_t*)calloc(1, sizeof(edge_t));
}

edge_t* new_epsilon_edge()
{
    edge_t *e = new_edge();
    if (e)
        e->val = EPSILON;
    return e;
}

state_t* new_state(int in_cnt, int out_cnt)
{
    state_t *st = (state_t*)calloc(1, sizeof(state_t));
    if (!st)
        goto err;

    if ((st->in = (edge_t**)calloc(in_cnt+1, sizeof(edge_t*))) == NULL)
        goto err;

    if ((st->out = (edge_t**)calloc(out_cnt+1, sizeof(edge_t*))) == NULL)
        goto err;

    return st;    

err:
    if (st)
        free(st->in);
    free(st);
    return NULL;
}

void delete_state(state_t *st)
{
    if (st) {
        free(st->in);
        free(st->out);
    }
	free(st);
}

state_unit_t* new_state_unit(state_t *st)
{
    state_unit_t * st_un = (state_unit_t*)malloc(sizeof(state_unit_t));
    if (st_un) 
        st_un->head = st_un->tail = st;

    return st_un;
}

state_t * new_state_normal_char(char c)
{
    state_t *st = new_state(1, 1);
    if (!st)
        return NULL;

    edge_t *e = NULL, *out = NULL; 
    if (!(e = new_edge()))
        goto err;

    if (!(out = new_epsilon_edge()))
        goto err;

    out->from = st;
    e->val = c;
    e->to = st;

    st->in[0] = e;
    st->out[0] = out;
    return st;

err:
	free(e);
    delete_state(st);
    return NULL;
}

state_unit_t* state_connect(state_unit_t *st_un1, state_unit_t *st_un2)
{
    state_t *st = new_state(1, 1); 
    if (!st)
        return NULL;

    st->in[0] = st_un1->tail->out[0];
    st->out[0] = st_un2->head->in[0];

    st_un1->tail->out[0]->to = st;
    st_un2->head->in[0]->from = st;

    st_un1->tail = st_un2->tail;

    free(st_un2);
    return st_un1;
}

state_unit_t* state_alternate(state_unit_t *st_un1, state_unit_t *st_un2)
{
	state_t *st_start = NULL, *st_end = NULL;
	edge_t *in = NULL, *out = NULL;

    st_start = new_state(1, 2);
    st_end = new_state(2, 1);
    in = new_epsilon_edge();
    out = new_epsilon_edge();

	if (!st_start || !st_end || !in || !out)
		goto err;

    st_start->in[0] = in;
    st_start->out[0] = st_un1->head->in[0];
    st_start->out[1] = st_un2->head->in[0];

    in->to = st_start;
    st_un1->head->in[0]->from = st_start;
    st_un2->head->in[0]->from = st_start;

    st_end->out[0] = out;
    st_end->in[0] = st_un1->tail->out[0];
    st_end->in[1] = st_un2->tail->out[0];

    out->from = st_end;
    st_un1->tail->out[0]->to = st_end;
    st_un2->tail->out[0]->to = st_end;

    st_un1->head = st_start;
    st_un1->tail = st_end;
    free(st_un2);
    return st_un1;

err:
    delete_state(st_start);
    delete_state(st_end);
	free(in);
	free(out);
    return NULL;
}

state_unit_t* state_closure(state_unit_t *st_un, char type)
{
    state_t *st_start = NULL, *st_end = NULL;
    edge_t *in = NULL, *out = NULL, *back = NULL;

    if (type == '*')
        st_start = new_state(2, 2);
    else if (type == '+') {
        st_start = new_state(2, 1);
        st_end = new_state(1, 2);
    }
    else if (type == '?') {
        st_start = new_state(1, 2);
        st_end = new_state(2, 1);
    }

    in = new_epsilon_edge(); 
    out = new_epsilon_edge();

    if (!st_start || !st_end || !in || !out)
        goto err;

    st_start->in[0] = in;
    in->to = st_start;

    if (type == '*') 
        st_start->out[1] = st_un->head->in[0];
    else 
        st_start->out[0] = st_un->head->in[0];

    st_un->head->in[0]->from = st_start;

    if (type == '*') {
        st_start->out[0] = out;
        out->from = st_start;

        st_un->tail->out[0]->to = st_start;
        st_start->in[1] = st_un->tail->out[0];
    }
    else{
        st_end->out[0] = out;
        out->from = st_end;

        st_un->tail->out[0]->to = st_end;
        st_end->in[0] = st_un->tail->out[0];

        if ((back = new_epsilon_edge()) == NULL )
            goto err;

        if (type == '+') {
            back->from = st_end;
            back->to = st_start;
            st_end->out[1] = back;
            st_start->in[1] = back;
        }
        else if (type == '?') {
            back->from = st_start;
            back->to = st_end;
            st_end->in[1] = back;
            st_start->out[1] = back;
        }
    }

	st_un->head = st_start;
	if(type == '*')
		st_un->tail = st_start;
	else
		st_un->tail = st_end;

	return st_un;

err:
    delete_state(st_start);
    delete_state(st_end);
    free(in);
    free(out);
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
 *                    out[0] st_un  in[0]
 *
 */
state_unit_t* state_closure_0or1(state_unit_t *st_un)
{
    return state_closure(st_un, '?');
}

/*
state_t* new_state_add_start(state_t *st)
{
    state_t *st = (state_t*)calloc(1, sizeof(state_t));
    if (!st)
        return NULL;
    st->out = (edge_t**)calloc(2, sizeof(edge_t*));
    st->out[0] = st->in[0];
    st->in[0]->from = st;
    return st;
}
*/

state_unit_t* build_nfa_from_regx(char *regx)
{
	struct flag_t {
		int alt_cnt;
		int st_unit_cnt;
	}tmp_flag, *tmp_flag_p;

	state_unit_t **tmp_stu, **tmp_stu1;
	int st_unit_cnt = 0, alt_cnt = 0;

	dstack_t *nfa_stack = dstack_init(0, sizeof(state_unit_t*));
	dstack_t *flag_stack = dstack_init(5, sizeof(struct flag_t));

	while(isspace(*regx)) regx++;

	for (; *regx; regx++) {
		switch(*regx) {
			case '*':
				if (!st_unit_cnt) 
					goto err;

				tmp_stu = (state_unit_t**)dstack_pop(nfa_stack);
				*tmp_stu1 = state_closure_star(*tmp_stu);
				dstack_push(nfa_stack, (void*)tmp_stu1);
				break;
			case '+':
				if (!st_unit_cnt) 
					goto err;

				tmp_stu = (state_unit_t**)dstack_pop(nfa_stack);
				*tmp_stu1 = state_closure_plus(*tmp_stu);
				dstack_push(nfa_stack, (void*)tmp_stu1);
				break;
			case '?':
				if (!st_unit_cnt) 
					goto err;

				tmp_stu = (state_unit_t**)dstack_pop(nfa_stack);
				*tmp_stu1 = state_closure_0or1(*tmp_stu);
				dstack_push(nfa_stack, (void*)tmp_stu1);
				break;
			case '(':
				// while ?
				if (st_unit_cnt > 1) {
					tmp_stu = (state_unit_t**)dstack_pop(nfa_stack);
					tmp_stu1 = (state_unit_t**)dstack_pop(nfa_stack);
					dstack_push(nfa_stack, state_connect(*tmp_stu, *tmp_stu1));
					st_unit_cnt--;
				}

				tmp_flag.alt_cnt = alt_cnt;
				tmp_flag.st_unit_cnt = st_unit_cnt;
				dstack_push(flag_stack, (void*)&tmp_flag);

				alt_cnt = 0;
				st_unit_cnt = 0;

				break;
			case ')':
				if (dstack_isempty(flag_stack) || !st_unit_cnt)
					goto err;

				while (--st_unit_cnt > 0) {
					tmp_stu = (state_unit_t**)dstack_pop(nfa_stack);
					tmp_stu1 = (state_unit_t**)dstack_pop(nfa_stack);
					dstack_push(nfa_stack, state_connect(*tmp_stu, *tmp_stu1));
				}

				while (alt_cnt-- > 0) {
					tmp_stu = (state_unit_t**)dstack_pop(nfa_stack);
					tmp_stu1 = (state_unit_t**)dstack_pop(nfa_stack);
					dstack_push(nfa_stack, state_alternate(*tmp_stu, *tmp_stu1));
				}

				tmp_flag_p = (struct flag_t*)dstack_pop(flag_stack);
				st_unit_cnt = tmp_flag_p->st_unit_cnt;
				alt_cnt = tmp_flag_p->alt_cnt;

				st_unit_cnt++;

				break;
			case '|':
				if (!st_unit_cnt) 
					goto err;

				alt_cnt++;
				while (--st_unit_cnt > 0) {
					tmp_stu = (state_unit_t**)dstack_pop(nfa_stack);
					tmp_stu1 = (state_unit_t**)dstack_pop(nfa_stack);
					dstack_push(nfa_stack, state_connect(*tmp_stu, *tmp_stu1));
				}

				/* priority is lowest
				if (alt_cnt > 1) {
					tmp_stu = (state_unit_t**)dstack_pop(nfa_stack);
					tmp_stu1 = (state_unit_t**)dstack_pop(nfa_stack);
					dstack_push(nfa_stack, state_alternate(*tmp_stu, *tmp_stu1));
					alt_cnt--;
				}
				*/

				break;
			default:
				if (st_unit_cnt > 1) {
					tmp_stu = (state_unit_t**)dstack_pop(nfa_stack);
					tmp_stu1 = (state_unit_t**)dstack_pop(nfa_stack);
					dstack_push(nfa_stack, state_connect(*tmp_stu, *tmp_stu1));
					st_unit_cnt--;
				}
				dstack_push(nfa_stack, new_state_unit(new_state_normal_char(*regx)));
				st_unit_cnt++;
				break;
		}
	}

	if (!dstack_isempty(flag_stack))
		goto err;

	while (--st_unit_cnt > 0) {
		tmp_stu = (state_unit_t**)dstack_pop(nfa_stack);
		tmp_stu1 = (state_unit_t**)dstack_pop(nfa_stack);
		dstack_push(nfa_stack, state_connect(*tmp_stu, *tmp_stu1));
	}

	while (alt_cnt-- > 0) {
		tmp_stu = (state_unit_t**)dstack_pop(nfa_stack);
		tmp_stu1 = (state_unit_t**)dstack_pop(nfa_stack);
		dstack_push(nfa_stack, state_alternate(*tmp_stu, *tmp_stu1));
	}

	*tmp_stu = (state_unit_t*)dstack_pop(nfa_stack);

	dstack_destory(nfa_stack);
	dstack_destory(flag_stack);

	return *tmp_stu;

err:
	// nfa destory
	dstack_destory(nfa_stack);
	dstack_destory(flag_stack);

	return NULL;
}
