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
#include <kern/sched.h>

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

// Deschedule current environment and pick a different one to run.
static void
sys_yield(void)
{
	sched_yield();
}

//
// Allocate a page of memory and map it at 'va' with permission
// 'perm' in the address space of 'envid'.
//
// If a page is already mapped at 'va', that page is unmapped as a
// side-effect.
//
// perm -- PTE_U|PTE_P are required, 
//         PTE_AVAIL|PTE_W are optional,
//         but no other bits are allowed (return -E_INVAL)
//
// Return 0 on success, < 0 on error
//	- va must be < UTOP
//	- an environment may modify its own address space or the
//	  address space of its children
//
static int
sys_mem_alloc(u_int envid, u_int va, u_int perm)
{
	// Your code here.
	panic("sys_mem_alloc not implemented");
}

// Map the page of memory at 'srcva' in srcid's address space
// at 'dstva' in dstid's address space with permission 'perm'.
// Perm has the same restrictions as in sys_mem_alloc, except 
// that it also must not grant write access to a read-only 
// page.
//
// Return 0 on success, < 0 on error.
//
// Cannot access pages above UTOP.
static int
sys_mem_map(u_int srcid, u_int srcva, u_int dstid, u_int dstva, u_int perm)
{
	// Your code here.
	panic("sys_mem_map not implemented");
}

// Unmap the page of memory at 'va' in the address space of 'envid'
// (if no page is mapped, the function silently succeeds)
//
// Return 0 on success, < 0 on error.
//
// Cannot unmap pages above UTOP.
static int
sys_mem_unmap(u_int envid, u_int va)
{
	// Your code here.
	panic("sys_mem_unmap not implemented");
}

// Allocate a new environment.
//
// The new child is left as env_alloc created it, except that
// status is set to ENV_NOT_RUNNABLE and the register set is copied
// from the current environment.  In the child, the register set is
// tweaked so sys_env_alloc returns 0.
//
// Returns envid of new environment, or < 0 on error.
static int
sys_env_alloc(void)
{
	// Your code here (in lab 4).
	panic("sys_env_alloc not implemented");
}

// Set envid's trap frame to tf.
//
// Returns 0 on success, < 0 on error.
//
// Return -E_INVAL if the environment cannot be manipulated.
static int
sys_set_trapframe(u_int envid, struct Trapframe *tf)
{
	// Your code here (in lab 4).

	// HINT:
	// Should enforce some limits on tf_eflags and tf_cs
	// The case were envid is the current environment needs 
	//   to be handled specially.

	panic("sys_set_trapframe not implemented");
}

// Set envid's env_status to status. 
//
// Returns 0 on success, < 0 on error.
// 
// Return -E_INVAL if status is not a valid status for an environment.
static int
sys_set_status(u_int envid, u_int status)
{
	// Your code here (in lab 4).
	panic("sys_set_status not implemented");
}

// Set envid's pagefault handler entry point and exception stack.
// (xstacktop points one byte past exception stack).
//
// Returns 0 on success, < 0 on error.
static int
sys_set_pgfault_entry(u_int envid, u_int func)
{
	// Your code here.
	panic("sys_set_pgfault_entry not implemented");
}

// Try to send 'value' to the target env 'envid'.
// If va != 0, then also send page currently mapped at va,
// so that receiver gets a duplicate mapping of the same page.
//
// The send fails with a return value of -E_IPC_NOT_RECV if the
// target has not requested IPC with sys_ipc_recv.
//
// Otherwise, the send succeeds, and the target's ipc fields are
// updated as follows:
//    env_ipc_recving is set to 0 to block future sends
//    env_ipc_from is set to the sending envid
//    env_ipc_value is set to the 'value' parameter
// The target environment is marked runnable again.
//
// Return 0 on success, < 0 on error.
//
// If the sender sends a page but the receiver isn't asking for one,
// then no page mapping is transferred but no error occurs.
//
// srcva and perm should have the same restrictions as they had
// in sys_mem_map.
//
// Hint: you will find envid2env() useful.
static int
sys_ipc_can_send(u_int envid, u_int value, u_int srcva, u_int perm)
{
	// Your code here
	panic("sys_ipc_can_send not implemented");
}

// Block until a value is ready.  Record that you want to receive,
// mark yourself not runnable, and then give up the CPU.
//
// Again, dstva should have the same restrictions as it had in
// sys_mem_map.  If it violates these restrictions, assume that it is
// zero.
static void
sys_ipc_recv(u_int dstva)
{
	// Your code here
	panic("sys_ipc_recv not implemented");
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

