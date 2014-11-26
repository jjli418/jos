/* See COPYRIGHT for copyright information. */

#include <inc/asm.h>
#include <inc/mmu.h>
#include <inc/pmap.h>
#include <inc/trap.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* For certain traps the CPU automatically pushes an error code, for 
 * all other traps the IDTFUNC_NOEC() pushes a 0 in place of the error code,
 * so the trap frame has the same format.
 */
#define IDTFNC(name,num)      ENTRY(name)           pushl $(num); jmp _alltraps
#define IDTFNC_NOEC(name,num) ENTRY(name) pushl $0; pushl $(num); jmp _alltraps 

.text


