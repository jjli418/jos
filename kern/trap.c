
#include <inc/mmu.h>
#include <inc/x86.h>
#include <inc/assert.h>

#include <kern/pmap.h>
#include <kern/trap.h>
#include <kern/env.h>
#include <kern/console.h>
#include <kern/syscall.h>
#include <kern/monitor.h>
#include <kern/sched.h>
#include <kern/kclock.h>
#include <kern/picirq.h>

u_int page_fault_mode = PFM_NONE;
static struct Taskstate ts;

/* Interrupt descriptor table.  (Must be built at run time because
 * shifted function addresses can't be represented in relocation records.)
 */
struct Gatedesc idt[256] = { {0}, };
struct Pseudodesc idt_pd =
{
	0, sizeof(idt) - 1, (unsigned long) idt,
};


static const char *trapname(int trapno)
{
	static const char *excnames[] = {
		"Divide error",
		"Debug",
		"Non-Maskable Interrupt",
		"Breakpoint",
		"Overflow",
		"BOUND Range Exceeded",
		"Invalid Opcode",
		"Device Not Available",
		"Double Falt",
		"Coprocessor Segment Overrun",
		"Invalid TSS",
		"Segment Not Present",
		"Stack Fault",
		"General Protection",
		"Page Fault",
		"(unknown trap)",
		"x87 FPU Floating-Point Error",
		"Alignment Check",
		"Machine-Check",
		"SIMD Floating-Point Exception"
	};

	if (trapno < sizeof(excnames)/sizeof(excnames[0]))
		return excnames[trapno];
	if (trapno == T_SYSCALL)
		return "System call";

	return "(unknown trap)";
}


void
idt_init(void)
{
	extern struct Segdesc gdt[];

	// Setup a TSS so that we get the right stack
	// when we trap to the kernel.
	ts.ts_esp0 = KSTACKTOP;
	ts.ts_ss0 = GD_KD;

	// Love to put this code in the initialization of gdt,
	// but the compiler generates an error incorrectly.
	gdt[GD_TSS >> 3] = SEG16(STS_T32A, (u_long) (&ts),
					sizeof(struct Taskstate), 0);
	gdt[GD_TSS >> 3].sd_s = 0;

	// Load the TSS
	ltr(GD_TSS);

	// Load the IDT
	asm volatile("lidt idt_pd+2");
}


void
print_trapframe(struct Trapframe *tf)
{
	printf("TRAP frame at %p\n", tf);
	printf("  edi  0x%08x\n", tf->tf_edi);
	printf("  esi  0x%08x\n", tf->tf_esi);
	printf("  ebp  0x%08x\n", tf->tf_ebp);
	printf("  oesp 0x%08x\n", tf->tf_oesp);
	printf("  ebx  0x%08x\n", tf->tf_ebx);
	printf("  edx  0x%08x\n", tf->tf_edx);
	printf("  ecx  0x%08x\n", tf->tf_ecx);
	printf("  eax  0x%08x\n", tf->tf_eax);
	printf("  es   0x----%04x\n", tf->tf_es);
	printf("  ds   0x----%04x\n", tf->tf_ds);
	printf("  trap 0x%08x %s\n", tf->tf_trapno, trapname(tf->tf_trapno));
	printf("  err  0x%08x\n", tf->tf_err);
	printf("  eip  0x%08x\n", tf->tf_eip);
	printf("  cs   0x----%04x\n", tf->tf_cs);
	printf("  flag 0x%08x\n", tf->tf_eflags);
	printf("  esp  0x%08x\n", tf->tf_esp);
	printf("  ss   0x----%04x\n", tf->tf_ss);
}

void
trap(struct Trapframe *tf)
{
	// print_trapframe(tf);

	// Handle processor exceptions
	// Your code here.

	// Handle external interrupts
	if (tf->tf_trapno == IRQ_OFFSET+0) {
		// irq 0 -- clock interrupt
		sched_yield();
	}
	if (tf->tf_trapno == IRQ_OFFSET+4) {
		serial_intr();
		return;
	}
	if (IRQ_OFFSET <= tf->tf_trapno 
			&& tf->tf_trapno < IRQ_OFFSET+MAX_IRQS) {
		// just ingore spurious interrupts
		printf("spurious interrupt on irq %d\n",
			tf->tf_trapno - IRQ_OFFSET);
		print_trapframe(tf);
		return;
	}

	// the user process or the kernel has a bug.
	print_trapframe(tf);
	if (tf->tf_cs == GD_KT)
		panic("unhandled trap in kernel");
	else {
		env_destroy(curenv);
		return;
	}
}


void
page_fault_handler(struct Trapframe *tf)
{
	u_int fault_va;

	// Read processor's CR2 register to find the faulting address
	fault_va = rcr2();


	// User-mode exception - destroy the environment.
	printf("[%08x] user fault va %08x ip %08x\n",
		curenv->env_id, fault_va, tf->tf_eip);
	print_trapframe(tf);
	env_destroy(curenv);
}

