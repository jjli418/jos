#include <inc/mmu.h>
#include <inc/pmap.h>


// Page fault handler entrypoint -
// this is where we ask the kernel
// (by calling sys_set_pgfault_handler in pgfault.c)
// to redirect us to whenever we cause a page fault in user space.
//
// When a page fault actually occurs, 
// the kernel switches our ESP to point to the user exception stack
// if we're not already on the user exception stack,
// and then it pushes the following minimal trap frame
// onto our user exception stack:
//
//	[ 5 spare words ]
//	trap-time eip
//	trap-time eflags
//	trap-time esp
//	trap-time errcode
//	trap-time va		<-- %esp
//
// We then have to save additional caller-saved registers
// and call up to the appropriate page fault handler in C code,
// pointed to by the global variable '_pgfault_handler' declared above.

.text
.globl _pgfault_entry
_pgfault_entry:
	// Save the caller-save registers
	//	(your code here)

	// Call the C page fault handler
	movl _pgfault_handler, %eax
	call *%eax

	// Push trap-time eip and eflags onto trap-time stack.
	// In the case of a recursive fault on the exception stack,
	// note that the two words we're pushing now
	// overlap with the exception frame we're currently using!
	//	(your code here)

	// Restore the caller-save registers.
	//	(your code here)

	// Switch back to the trap-time stack.
	//	(your code here)

	// Restore eflags and eip from the stack.
	//	(your code here)

