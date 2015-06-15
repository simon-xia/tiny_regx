#include "common.h"
#include "state.h"
#include "dynamic_stack.h"

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

				/* |'s priority is lowest
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

	return state_add_terminal(*tmp_stu);

err:
	// nfa destory
	dstack_destory(nfa_stack);
	dstack_destory(flag_stack);

	return NULL;
}

char *new_fingerprint(int n)
{
	return (char*)calloc((n-1)/8, sizeof(char)) 
}

char *set_fingerprint(char *fp, int i)
{
	fp[i/8] |= 0x01 << (i%8); 
	return fp;
}

int nfa2dfa(fsm_t *fsm)
{
	ht_t *st_exist = ht_init()
}
