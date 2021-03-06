/* See COPYRIGHT for copyright information. */

#ifndef _KERN_ENV_H_
#define _KERN_ENV_H_

#include <inc/env.h>

LIST_HEAD(Env_list, Env);
extern struct Env *envs;		// All environments
extern struct Env *curenv;	        // the current env

void env_init(void);
int env_alloc(struct Env **e, u_int parent_id);
void env_free(struct Env *);
void env_create(u_char *binary, int size);
void env_destroy(struct Env *e);

int envid2env(u_int envid, struct Env **penv, int checkperm);
void env_run(struct Env *e);
void env_pop_tf(struct Trapframe *tf);

// for the grading script
#define ENV_CREATE2(x, y) \
{ \
	extern u_char x[], y[]; \
	env_create(x, (int)y); \
}

#define ENV_CREATE(x) \
{ \
	extern u_char _binary_obj_##x##_start[], \
		_binary_obj_##x##_size[]; \
	env_create(_binary_obj_##x##_start, \
		(int)_binary_obj_##x##_size); \
}

#endif // !_KERN_ENV_H_
