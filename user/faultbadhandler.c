// test bad pointer for user-level fault handler
// this is going to fault in the fault handler accessing eip (always!)
// so eventually the kernel kills it (PFM_KILL) because
// we outrun the stack with invocations of the user-level handler

#include <inc/lib.h>

void
umain(void)
{
	sys_mem_alloc(0, UXSTACKTOP-BY2PG, PTE_P|PTE_U|PTE_W);
	sys_set_pgfault_entry(0, 0xDeadBeef);
	*(int*)0 = 0;
}
