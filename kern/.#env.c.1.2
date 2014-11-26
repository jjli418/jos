/* See COPYRIGHT for copyright information. */

#include <inc/x86.h>
#include <inc/mmu.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>
#include <inc/elf.h>

#include <kern/env.h>
#include <kern/pmap.h>
#include <kern/trap.h>
#include <kern/monitor.h>
#include <kern/sched.h>

struct Env *envs = NULL;		// All environments
struct Env *curenv = NULL;	        // The current env

static struct Env_list env_free_list;	// Free list

//
// Calculates the envid for env e.  
//
static u_int
mkenvid(struct Env *e)
{
	static u_long next_env_id = 0;
	// lower bits of envid hold e's position in the envs array
	u_int idx = e - envs;
	// high bits of envid hold an increasing number
	return(++next_env_id << (1 + LOG2NENV)) | idx;
}

//
// Converts an envid to an env pointer.
//
// RETURNS
//   0 on success, -error on error.
//   on success, sets *penv to the environment
//   on error, sets *penv to NULL.
//
int
envid2env(u_int envid, struct Env **penv, int checkperm)
{
	struct Env *e;

	// If envid is specified as zero,
	// just assume the current environment by default.
	if (envid == 0) {
		*penv = curenv;
		return 0;
	}

	// Look up the Env structure via the index part of the envid,
	// then check the env_id field in that struct Env
	// to ensure that the envid is not stale
	// (i.e., does not refer to a _previous_ environment
	// that used the same slot in the envs[] array).
	e = &envs[ENVX(envid)];
	if (e->env_status == ENV_FREE || e->env_id != envid) {
		*penv = 0;
		return -E_BAD_ENV;
	}

	// Check that the calling environment has legitimate permission
	// to manipulate the specified environment.
	// If checkperm is set, the specified environment
	// must be either the current environment
	// or an immediate child of the current environment.
	if (checkperm) {
		// Your code here in Lab 4
		return -E_BAD_ENV;
	}
	*penv = e;
	return 0;
}

//
// Marks all environments in 'envs' as free and inserts them into 
// the env_free_list.  Insert in reverse order, so that
// the first call to env_alloc() returns envs[0].
//
void
env_init(void)
{
	int i;

	LIST_INIT(&env_free_list);

	for( i = 0; i < NENV; i++)
		LIST_INSERT_HEAD(&env_free_list, &envs[i], env_link);
}

//
// Initializes the kernel virtual memory layout for environment e.
// Allocates a page directory and initializes
// the kernel portion of the new environment's address space.
// Also sets e->env_cr3 and e->env_pgdir accordingly.
// We do NOT (yet) map anything into the user portion
// of the environment's virtual address space.
//
// RETURNS
//   0 -- on sucess
//   <0 -- otherwise 
//
static int
env_setup_vm(struct Env *e)
{
	int i, r;
	struct Page *p = NULL, *p1 = NULL, *p2 = NULL;
	Pte *pTable = NULL;
	u_long uStackBottom = USTACKTOP - BY2PG;

	// Allocate a page for the page directory
	if ((r = page_alloc(&p)) < 0)
		return r;


	e->env_pgdir = KADDR(page2pa(p));
	e->env_cr3 = page2pa(p);

	printf("setupvm:cr3=%x\n", e->env_cr3);

	memcpy(e->env_pgdir, boot_pgdir, BY2PG); // dangeous

	// map user statck
	if ((r = page_alloc(&p1)) < 0)
	{
		page_free(p1);
		return r;
	}
	
	pTable = (Pte*)KADDR( page2pa(p1) );
	e->env_pgdir[ PDX(uStackBottom) ] = page2pa(p1) | PTE_U |PTE_W| PTE_P;

	if (( r = page_alloc(&p2)) < 0)
	{
		page_free(p1);
		page_free(p2);
		return r;
	}

	pTable[ PTX(uStackBottom) ] = page2pa(p2) | PTE_U | PTE_W | PTE_P;

	// map UTEXT address space
	/*
	int npages = 1024; // 4M address

	if (( r = page_alloc(&p) ) < 0)
	{
		return r;
	}

	e->env_pgdir[ PDX(UTEXT) ] = page2pa(p) | PTE_U | PTE_P;
	pTable = (Pte*)KADDR( page2pa(p) );

	for ( i = 0; i < npages; i++)
	{
		page_alloc(&p);
		pTable[i] = page2pa(p) | PTE_U | PTE_P;
	}
	*/


	// Hint:
	//    - The VA space of all envs is identical above UTOP
	//      (except at VPT and UVPT).
	//    - Use boot_pgdir as a template.
	//    - You do not need to make any more calls to page_alloc.
	//    - Note: pp_ref is not maintained
	//	for physical pages mapped above UTOP.


	// ...except at VPT and UVPT.  These map the env's own page table
	e->env_pgdir[PDX(VPT)]   = e->env_cr3 | PTE_P | PTE_W;
	e->env_pgdir[PDX(UVPT)]  = e->env_cr3 | PTE_P | PTE_U;

	return 0;
}

//
// Allocates and initializes a new env.
//
// RETURNS
//   0 -- on success, sets *new to point at the new env 
//   <0 -- on failure
//
int
env_alloc(struct Env **new, u_int parent_id)
{
	int r;
	struct Env *e;

	if (!(e = LIST_FIRST(&env_free_list)))
		return -E_NO_FREE_ENV;

	// Allocate and set up the page directory for this environment.
	if ((r = env_setup_vm(e)) < 0)
		return r;

	// Generate an env_id for this environment,
	// and set the basic status variables.
	e->env_id = mkenvid(e);
	e->env_parent_id = parent_id;
	e->env_status = ENV_RUNNABLE;

	// Clear out all the saved register state,
	// to prevent the register values
	// of a prior environment inhabiting this Env structure
	// from "leaking" into our new environment.
	memset(&e->env_tf, 0, sizeof(e->env_tf));

	// Set up appropriate initial values for the segment registers.
	// GD_UD is the user data segment selector in the GDT, and 
	// GD_UT is the user text segment selector (see inc/pmap.h).
	// The low 2 bits of each segment register
	// contains the Requestor Privilege Level (RPL);
	// 3 means user mode.
	e->env_tf.tf_ds = GD_UD | 3;
	e->env_tf.tf_es = GD_UD | 3;
	e->env_tf.tf_ss = GD_UD | 3;
	e->env_tf.tf_esp = USTACKTOP;
	e->env_tf.tf_cs = GD_UT | 3;


	// You also need to set tf_eip to the correct value at some point.
	// Hint: see load_icode

	// Clear the page fault handler until user installs one.
	e->env_pgfault_entry = 0;

	// Also clear the IPC receiving flag.
	e->env_ipc_recving = 0;


	// commit the allocation
	LIST_REMOVE(e, env_link);
	*new = e;

	printf("[%08x] new env %08x\n", curenv ? curenv->env_id : 0, e->env_id);
	return 0;
}

// Allocate and map all required pages into an env's address space
// to cover virtual addresses va through va+len-1 inclusive.
// Does not zero or otherwise initialize the mapped pages in any way.
// Panic if any allocation attempt fails.
//
// Warning: Neither va nor len are necessarily page-aligned!
// You may assume, however, that nothing is already mapped
// in the pages touched by the specified virtual address range.
static void
map_segment(struct Env *e, u_int va, u_int len)
{
	// Your code here. simplely assuming len < 4M, and the new entry for the mapped va is not in use
	struct Page *p;
	int pdx = PDX(va);
	int ptx = PTX(va);
	int pages = ROUND(len, BY2PG) / BY2PG;
	int count = 0;
	Pte *pTable, *pTableKern;

	// map in kernel space to prepare the code for the Env
	if (!(boot_pgdir[pdx] & PTE_P))
	{
		if (page_alloc(&p) < 0)
			panic("Unable to allocat a page in map_segment()");


		memset(KADDR(page2pa(p)), 0, BY2PG);
		boot_pgdir[pdx] = page2pa(p) | PTE_W | PTE_P | PTE_U;
	}
	

	// check the secondory table existence
	if (!(e->env_pgdir[pdx] & PTE_P))
	{
		printf("The page for va:%x not exist.\n", va);
		if (page_alloc(&p) < 0)
			panic("Unable to allocat a page in map_segment()");

		memset(KADDR(page2pa(p)), 0, BY2PG);
		e->env_pgdir[pdx] = page2pa(p) | PTE_P | PTE_U | PTE_W;
	}
	else
		printf("The entry exist for:%x and content is:%x\n", va, e->env_pgdir[pdx]);

	pTable = (Pte*)KADDR(PTE_ADDR(e->env_pgdir[pdx]));
	pTableKern = (Pte*)KADDR(PTE_ADDR(boot_pgdir[pdx]));

	printf("Will alloate:%x page(s) for va:%x\n", pages, va);
	// check if some pages for va is mapped
	for( count = 0; count < pages; count++)
	{
		if (! (pTable[ptx+count] & PTE_P))
		{
			if(page_alloc(&p) < 0)
				panic("Unable to allocat a page in map_segment()");
			pTable[ptx+count] = page2pa(p) | PTE_P | PTE_U | PTE_W;
			pTableKern[ptx+count] = page2pa(p) | PTE_P | PTE_W | PTE_U;
		}
	}
	
}

//
// Set up the the initial stack and program binary for a user process.
// This function is ONLY called during kernel initialization,
// before running the first user-mode environment.
//
// This function loads all loadable segments from the ELF binary image
// into the environment's user memory, starting at the appropriate
// virtual addresses indicated in the ELF program header.
// At the same time it clears to zero any portions of these segments
// that are marked in the program header as being mapped
// but not actually present in the ELF file - i.e., the program's bss section.
//
// Finally, this function maps one page for the program's initial stack
// at virtual address USTACKTOP - BY2PG.
//
static void
load_icode(struct Env *e, u_char *binary, u_int size)
{
	// Hint: 
	//  Use map_segment() to map memory for each program segment.
	//  Only use segments with ph->p_type == ELF_PROG_LOAD.
	//  Each segment's virtual address can be found in ph->p_va
	//  and its size in memory can be found in ph->p_memsz.
	//  For each segment, load the first ph->p_filesz bytes from the ELF
	//  binary and clear any remaining bytes in the segment to zero.
	//
	// Hint:
	//  Loading the segments is much simpler if you set things up
	//  so that you can access them via the user's virtual addresses!
	
	struct Elf *elfHeader = (struct Elf*)binary;
	struct Proghdr *prgHeader = (struct Proghdr*)(binary + elfHeader->e_phoff);
	u_short phNum = elfHeader->e_phnum;
	int i = 0, count;
	int pages, pdx, ptx;

	printf("^^^^^^^^^^^^^^^^^^^^^^^^%x\n", *(int*)binary);
	printf("icode Entry point is:%x\n", elfHeader->e_entry);
	printf("Phnum is:%x\n", phNum);
	for( ; i < phNum; i++)
	{
		printf("Pgr header type is:%x, addr=%x, size=%x\n", prgHeader->p_type, prgHeader->p_va, prgHeader->p_filesz);
		if ( prgHeader->p_type == ELF_PROG_LOAD )
		{
			map_segment(e, prgHeader->p_va, prgHeader->p_memsz);

			// map the physical address in kernel page dir
			printf("contents in pa:%x\n", *(int*)prgHeader->p_va);
			memcpy( prgHeader->p_va, binary + prgHeader->p_offset, prgHeader->p_filesz);
			printf("First struction is:%x\n", *((int*)prgHeader->p_va));
		}

		prgHeader++;
	}

	e->env_tf.tf_eip = elfHeader->e_entry;
	
}

//
// Allocates a new env and loads the elf binary into it.
// This function is ONLY called during kernel initialization,
// before running the first user-mode environment.
// The new env's parent env id is set to 0.
void
env_create(u_char *binary, int size)
{
	struct Env *env;

	printf("User binaray addr=%x, size=%d\n", (int)binary, size);

	int result = env_alloc(&env, 0);

	printf("allocated cr3=%x\n", env->env_cr3);

	load_icode(env, binary, size);
}

//
// Frees env e and all memory it uses.
// 
void
env_free(struct Env *e)
{
	Pte *pt;
	u_int pdeno, pteno, pa;

	// Note the environment's demise.
	printf("[%08x] free env %08x\n", curenv ? curenv->env_id : 0, e->env_id);

	// Flush all mapped pages in the user portion of the address space
	static_assert(UTOP%PDMAP == 0);
	for (pdeno = 0; pdeno < PDX(UTOP); pdeno++) {

		// only look at mapped page tables
		if (!(e->env_pgdir[pdeno] & PTE_P))
			continue;

		// find the pa and va of the page table
		pa = PTE_ADDR(e->env_pgdir[pdeno]);
		pt = (Pte*)KADDR(pa);

		// unmap all PTEs in this page table
		for (pteno = 0; pteno <= PTX(~0); pteno++) {
			if (pt[pteno] & PTE_P)
				page_remove(e->env_pgdir,
					(pdeno << PDSHIFT) |
					(pteno << PGSHIFT));
		}

		// free the page table itself
		e->env_pgdir[pdeno] = 0;
		page_decref(pa2page(pa));
	}

	// free the page directory
	pa = e->env_cr3;
	e->env_pgdir = 0;
	e->env_cr3 = 0;
	page_decref(pa2page(pa));

	// return the environment to the free list
	e->env_status = ENV_FREE;
	LIST_INSERT_HEAD(&env_free_list, e, env_link);
}

//
// Frees env e.  And schedules a new env
// if e was the current env.
//
void
env_destroy(struct Env *e) 
{
	env_free(e);

	if (curenv == e) {
		curenv = NULL;
		sched_yield();
	}
}


//
// Restores the register values in the Trapframe
//  (does not return)
//
void
env_pop_tf(struct Trapframe *tf)
{
#if 0
	printf(" --> %d 0x%x\n", ENVX(curenv->env_id), tf->tf_eip);
#endif

	asm volatile("movl %0,%%esp\n"
		"\tpopal\n"
		"\tpopl %%es\n"
		"\tpopl %%ds\n"
		"\taddl $0x8,%%esp\n" /* skip tf_trapno and tf_errcode */
		"\tiret"
		:: "g" (tf) : "memory");
	panic("iret failed");  /* mostly to placate the compiler */
}

//
// Context switch from curenv to env e.
// Note: if this is the first call to env_run, curenv is NULL.
//  (This function does not return.)
//
void
env_run(struct Env *e)
{
	// save the register state of the previously executing environment
	if (curenv) {
		// Your code here.
		// Hint: this can be done in a single line of code.
		panic("need to save previous env's register state!");
	}

	// step 1: set curenv to the new environment to be run.
	// step 2: use lcr3 to switch to the new environment's address space.
	// step 3: use env_pop_tf() to restore the new environment's registers
	//	and drop into user mode in the new environment.
	
	curenv = e;

	printf("env_run(env_run(env_run(env_run(env_run(env_run(env_run(env_run:%x\n", e->env_cr3);
	lcr3(e->env_cr3);
	printf("env_run(env_run(env_run(env_run(env_run(env_run(env_run(env_run(\n");
	env_pop_tf(&e->env_tf);
}


