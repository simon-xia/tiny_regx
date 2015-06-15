#ifndef _STATE_H_
#define _STATE_H_

#include "dynamic_array.h"

#define STATE_DEFAULT_IN_EDGE	2
#define STATE_DEFAULT_OUT_EDGE	2

#define EPSILON        -1

struct edge;

typedef struct state {
    int is_terminal;
	int id;
	darray_t *in, *out;
}state_t;

typedef struct edge {
    int val;
    struct state *from, *to;
}edge_t;

typedef struct state_unit {
    struct state *head, *tail;
}state_unit_t;

#endif
