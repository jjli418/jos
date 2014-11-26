
// Main public header file for our user-land support library,
// whose code lives in the lib directory.
// This library is roughly our OS's version of a standard C library,
// and is intended to be linked into all user-mode applications
// (NOT the kernel or boot loader).

#ifndef _INC_LIB_H_
#define _INC_LIB_H_ 1

#include <inc/types.h>
#include <inc/stdio.h>
#include <inc/stdarg.h>
#include <inc/string.h>
#include <inc/error.h>
#include <inc/assert.h>
#include <inc/env.h>
#include <inc/pmap.h>
#include <inc/syscall.h>

#define USED(x) (void)(x)

// libos.c or entry.S
extern char *binaryname;
extern struct Env *env;
extern struct Env envs[NENV];
extern struct Page pages[];
void	exit(void);

// pgfault.c
void	set_pgfault_handler(void(*)(u_int va, u_int err));

// readline.c
char *	readline(const char *buf);

// syscall.c
void	sys_cputs(char*);
int	sys_cgetc(void);
u_int	sys_getenvid(void);
int	sys_env_destroy(u_int);



/* File open modes */
#define	O_RDONLY	0x0000		/* open for reading only */
#define	O_WRONLY	0x0001		/* open for writing only */
#define	O_RDWR		0x0002		/* open for reading and writing */
#define	O_ACCMODE	0x0003		/* mask for above modes */

#define	O_CREAT		0x0100		/* create if nonexistent */
#define	O_TRUNC		0x0200		/* truncate to zero length */
#define	O_EXCL		0x0400		/* error if already exists */
#define O_MKDIR		0x0800		/* create directory, not regular file */

#endif	// not _INC_LIB_H_
