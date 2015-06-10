#define EPSILON	-1

struct edge;

typedef struct state {
	int is_terminal;
	struct edge **in, **out;
}state_t;

typedef struct edge {
	int val;
	struct state *from, *to;
}edge_t;

void set_state_terminal(state_t *st)
{
	st->is_terminal = 1;
}

edge_t *new_epsilon_edge()
{
	edge_t *e = (edge_t*)calloc(1, sizeof(edge_t));
	e->val = EPSILON;
	return e;
}

state_t* new_state_repetition(char c, char type)
{
	state_t *st = (state_t*)calloc(1, sizeof(state_t));

	if (!st)
		return NULL;

	st->in = (edge_t**)calloc(3, sizeof(edge_t*));
	st->out = (edge_t**)calloc(3, sizeof(edge_t*));

	edge_t *in = (edge_t*)calloc(1, sizeof(edge_t));
	edge_t *out = (edge_t*)calloc(1, sizeof(edge_t));
	edge_t *e = (edge_t*)calloc(1, sizeof(edge_t));

	if (!st->in || !st->out || !in || !out || !e)
		return NULL;

	if (type == '+')
		in->val = c;
	else
		in->val = EPSILON;

	out->val = EPSILON;
	e->val = c;
	in->to = out->from = e->from = st;

	if (type != '?')
		e->to = st;

	st->in[0] = in;
	st->in[1] = e;
	st->out[0] = out;
	st->out[1] = e;

	return st;
}

state_t* new_state_repetition_star(char c)
{
	return new_state_repetition(c, '*');
}

state_t* new_state_repetition_plus(char c)
{
	return new_state_repetition(c, '+');
}

state_t* new_state_repetition_choose(char c)
{
	return new_state_repetition(c, '?');
}

state_t * new_state_normal(char c)
{
	state_t* st = (state_t*)calloc(1, sizeof(state_t));
	if (!st)
		return NULL;

	st->in = (edge_t**)calloc(2, sizeof(edge_t*));
	st->out = (edge_t**)calloc(2, sizeof(edge_t*));

	edge_t *e = (edge_t*)calloc(1, sizeof(edge_t));
	edge_t *out = new_epsilon_edge() 
	out->from = st;

	if (!st->in || !st->out || !out || !e)
		return NULL;

	st->in[0] = e;
	st->out[0] = out;
	e->val = c;
	e->in = st;
	return st;
}

/*
state_t* state_connect(state_t *st1, state_t *st2)
{
	state_t *st = (state_t*)calloc(1, sizeof(state_t));
	if (!st)
		return NULL;

	edge_t *e;	
	int i;
	for (i = 0, e = st1->out[0]; e; e++, i++)
		e->to = st;
	st->in = (edge_t**)calloc(i+1, sizeof(edge_t*));
	memcpy(st->in, st1->out, i*sizeof(edge_t*));

	//only one up to now
	for (i = 0, e = st2->in[0]; e; e++, i++)
		e->from = st;
	st->out = (edge_t**)calloc(i+1, sizeof(edge_t*));
	memcpy(st->out, st2->in, i*sizeof(edge_t*));

	return st1;
}
*/

state_t* state_connect(state_t *st1, state_t *st2)
{
}
state_t* state_alternate(state_t *st1, state_t *st2)
{
	state_t *st_start = (state_t*)calloc(1, sizeof(state_t));
	if (!st_start)
		return NULL;
	st_start->in = (edge_t**)calloc(2, sizeof(edge_t*));
	st_start->out = (edge_t**)calloc(3, sizeof(edge_t*));
	if (!st_start->in || !st_start->out)
		return NULL;

	edgt_t *e = (edge_t*)calloc(1, sizeof(edge_t));
	e->val = EPSILON;
	e->to = st_start;
	st1->in[0]->from = st2->in[0]->from = st_start;

	st_start->in[0] = e;
	st_start->out[0] = st1->in[0];
	st_start->out[1] = st2->in[0];

	return st_start;
}

state_t* state_closure(state_t *st, char type)
{
	state_t *st_new = (state_t*)calloc(1, sizeof(state_t));
	if (!st_new)
		return NULL;

	edge_t *e;	
	int i;
	for (i = 0, e = st->out[0]; e; e++, i++)
		e->to = st_new;
	st_new->in = (edge_t**)calloc(i+2, sizeof(edge_t*));

	st_new->out = (edge_t**)calloc(3, sizeof(edge_t*));
	if (!st_new->in || !st_new->out)
		return NULL;

	memcpy(st_new->in[1], st->out, i*sizeof(edge_t*));

	edge_t *in = (edge_t*)calloc(1, sizeof(edge_t));
	edge_t *out = (edge_t*)calloc(1, sizeof(edge_t));

	if (!in || !out)
		return NULL;

	if (type == '*')
		in->val = EPSILON;

	out->val = EPSILON;
	st_new->in[0] = in;
	st_new->out[0] = out;
	st_new->out[1] = st->in[0];

	st->in[0]->from = st->new;
	in->to = st_new;
	out->from = st_new;
	
	return st_new;
}

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
