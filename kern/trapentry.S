/* See COPYRIGHT for copyright information. */

#include <inc/asm.h>
#include <inc/mmu.h>
#include <inc/pmap.h>
#include <inc/trap.h>



###################################################################
# exceptions/interrupts
###################################################################

/* For certain traps the CPU automatically pushes an error code, for 
 * all other traps the IDTFUNC_NOEC() pushes a 0 in place of the error code,
 * so the trap frame has the same format.
 */
#define IDTFNC(name,num)      ENTRY(name)           pushl $(num)# jmp _alltraps
#define IDTFNC_NOEC(name,num) ENTRY(name) pushl $0; pushl $(num)# jmp _alltraps 


.text
IDTFNC(myint0, 0x0)
	
	# push trap frame
	
	pushl 	$0x0 		# trap num
	pushl	%ds
	pushl	%es
	pushal
	push	%esp		# frame pointer

	movw	$GD_KD, %ax
	movw	%ax,	%ds
	movw	%ax, 	%es

	call	trap

	pop	%eax
	popal
	pop	%es
	pop	%ds
	addl	$4,	%esp

IDTFNC(myint14, 0xE)
	
	# push trap frame
	
	pushl 	$0xE 		# trap num
	pushl	%ds
	pushl	%es
	pushal
	push	%esp		# frame pointer

	movw	$GD_KD, %ax
	movw	%ax,	%ds
	movw	%ax, 	%es

	call	trap

	pop	%eax
	popal
	pop	%es
	pop	%ds
	addl	$4,	%esp


IDTFNC(myint30, 0x30)
	
	# push trap frame
	
	pushl 	$T_SYSCALL 	# trap num
	pushl	%ds
	pushl	%es
	pushal
	push	%esp		# frame pointer

	movw	$GD_KD, 	%ax
	movw	%ax,	%ds
	movw	%ax, 	%es

	call	trap

	pop	%eax
	popal
	pop	%es
	pop	%ds
	addl	$4,	%esp

	iret


