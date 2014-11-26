/* See COPYRIGHT for copyright information. */

#include <inc/x86.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>

#include <kern/env.h>
#include <kern/pmap.h>
#include <kern/trap.h>
#include <kern/syscall.h>
#include <kern/console.h>

// print a string to the system console.
static void
sys_cputs(char *s)
{
	printf("%s", s);
}

// read a character from the system console
static int
sys_cgetc(void)
{
	int c;

	// The cons_getc() primitive doesn't wait for a character,
	// but the sys_cgetc() system call does.
	while ((c = cons_getc()) == 0)
		; /* spin */

	return c;
}

// return the current environment's envid
static u_int
sys_getenvid(void)
{
	return curenv->env_id;
}

// destroy a given environment
// (possibly the currently running environment)
static int
sys_env_destroy(u_int envid)
{
	int r;
	struct Env *e;

	if ((r=envid2env(envid, &e, 1)) < 0)
		return r;
	if (e == curenv)
		printf("[%08x] exiting gracefully\n", curenv->env_id);
	else
		printf("[%08x] destroying %08x\n", curenv->env_id, e->env_id);
	env_destroy(e);
	return 0;
}



// Dispatches to the correct kernel function, passing the arguments.
int
syscall(u_int sn, u_int a1, u_int a2, u_int a3, u_int a4, u_int a5)
{
	// printf("syscall %d %x %x %x from env %08x\n", sn, a1, a2, a3, curenv->env_id);

	// Your code here
	//panic("syscall not implemented");
	if(sn == SYS_cputs)
	{
		sys_cputs((char*)a1);
	}
	else
		return -E_INVAL;

	
}

