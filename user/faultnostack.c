// test user fault handler being called with no exception stack mapped

#include <inc/lib.h>

void _pgfault_entry();

void
umain(void)
{
	sys_set_pgfault_entry(0, (u_long)_pgfault_entry);
	*(int*)0 = 0;
}
