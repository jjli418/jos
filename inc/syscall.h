#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <inc/types.h>
#include <kern/trap.h>

/* system call numbers */
enum
{
	SYS_cputs	= 0,
	SYS_cgetc,
	SYS_getenvid,
	SYS_env_destroy,

	NSYSCALLS,
};

#endif /* !_SYSCALL_H_ */
