#include <inc/asm.h>
#include <inc/mmu.h>
	
.set PROT_MODE_CSEG,0x8		# code segment selector
.set PROT_MODE_DSEG,0x10        # data segment selector
.set CR0_PE_ON,0x1		# protected mode enable flag
	
###################################################################################
# ENTRY POINT	
#   This code should be stored in the first sector of the hard disk.
#   After the BIOS initializes the hardware on startup or system reset,
#   it loads this code at physical address 0x7c00 - 0x7d00 (512 bytes).
#   Then the BIOS jumps to the beginning of it, address 0x7c00,
#   while running in 16-bit real-mode (8086 compatibility mode).
#   The Code Segment register (CS) is initially zero on entry.
#	
# This code switches into 32-bit protected mode so that all of
# memory can accessed, then calls into C.
###################################################################################
	
.globl start					# Entry point	
start:		.code16				# This runs in real mode
		cli				# Disable interrupts
		cld				# String operations increment

		# Set up the important data segment registers (DS, ES, SS).
		xorw	%ax,%ax			# Segment number zero
		movw	%ax,%ds			# -> Data Segment
		movw	%ax,%es			# -> Extra Segment
		movw	%ax,%ss			# -> Stack Segment

		# Set up the stack pointer, growing downward from 0x7c00.
		movw	$start,%sp         	# Stack Pointer

		movb	%dl, BOOTDRIVE
	
#### Enable A20:
####   For fascinating historical reasons (related to the fact that
####   the earliest 8086-based PCs could only address 1MB of physical memory
####   and subsequent 80286-based PCs wanted to retain maximum compatibility),
####   physical address line 20 is tied to low when the machine boots.
####   Obviously this a bit of a drag for us, especially when trying to
####   address memory above 1MB.  This code undoes this.
	
seta20.1:	inb	$0x64,%al		# Get status
		testb	$0x2,%al		# Busy?
		jnz	seta20.1		# Yes
		movb	$0xd1,%al		# Command: Write
		outb	%al,$0x64		#  output port
seta20.2:	inb	$0x64,%al		# Get status
		testb	$0x2,%al		# Busy?
		jnz	seta20.2		# Yes
		movb	$0xdf,%al		# Enable
		outb	%al,$0x60		#  A20
	#### Read the kernel image to 0x7d00 in real mode, and then move it to 0x100000 in PE mode
#### Assume the image is not big enough to cross over the 1M boundary

		mov 	$hello, %si
		call	bios_print
		
### read disk parameters
		mov	$8, %ah
		int	$0x13

		mov	%cl, %al
		and	$0x3F, %al
		movb	%al, sectors_per_track  # strore sectors

		xor	%ax, %ax
		movb	%ch,%al
		shr	$6, %cl
		or	%cl,%ah
		movw	%ax, cylinders 		# store cylinders

		movb	%dh, headers

### read sectors, sector by sector
		movw	$0x1000, %ax
		movw	%ax, %es

		movw	$1000, %cx # 800k kernel, should adjust if it's larger than 800k

		xor	%dx,%dx	# dx:ax is the sector LBA, starts 0
		mov	$1, %ax

read_disk:
		call read_sector

		movw	kern_offset, %bx 			# es:bx
		add	$512, %bx

		jc	change_es
		jmp	not_change
change_es:
		# es:bx bx is full
		push	%ax
		movw	%es, %ax
		addw	$0x1000, %ax
		movw	%ax,%es
		pop	%ax

not_change:
		movw	%bx,kern_offset 


		inc	%ax
		#adc	$0,%dx
		

		loop read_disk

read_done:

		#mov 	$hello, %si
		#call	bios_print
#### Reading the kernel image is over

#### Switch from real to protected mode	
####     The descriptors in our GDT allow all physical memory to be accessed.
####     Furthermore, the descriptors have base addresses of 0, so that the
####     segment translation is a NOP, ie. virtual addresses are identical to
####     their physical addresses.  With this setup, immediately after
####	 enabling protected mode it will still appear to this code
####	 that it is running directly on physical memory with no translation.
####	 This initial NOP-translation setup is required by the processor
####	 to ensure that the transition to protected mode occurs smoothly.
	
real_to_prot:	cli				# Mandatory since we dont set up an IDT
		lgdt	gdtdesc			# load GDT -- mandatory in protected mode
		movl	%cr0, %eax		# turn on protected mode
		orl	$CR0_PE_ON, %eax	# 
		movl	%eax, %cr0		# 
	        ### CPU magic: jump to relocation, flush prefetch queue, and reload %cs
		### Has the effect of just jmp to the next instruction, but simultaneous
		### loads CS with $PROT_MODE_CSEG.
		ljmp	$PROT_MODE_CSEG, $protcseg
	
# read a sector, sectors is in dx:ax and starts from 1, get data at es:bx

read_sector:
	pusha

	and 	$0, %bx
	movb	sectors_per_track, %bl
	div	%bx
	add	$1, %dl 	# remainder, add 1 to S

	push 	%dx
	and	$0, %dx

	movb	headers, %bl
	div	%bx		# dx: remainder, headers; ax: quotient, cylinders

#	set read parameters
	pop	%cx	       # get sector in stack
	movb	%al,%ch
	and	$0x3, %ah
	shl	$6, %ah	# get high 2 bits for cylinders
	or	%ah, %cl # add two higher bits to cl

	movb	%dl, %dh       # header
	movb	$1,  %al       # count

	
	movb	$2, %ah        #read function
	movb	BOOTDRIVE, %dl
	movw	kern_offset, %bx

	int 	$0x13

	popa

	jc read_sector	# reread the sectors

	ret

#print string
bios_print:

	lodsb  # load [ds:si] -> al
	or	%al,%al
	jz 	print_done
	mov	$0xE, %ah
	int	$0x10
	jmp bios_print

print_done:
	ret


#### we are in 32-bit protected mode (hence the .code32)
.code32
protcseg:	
		# Set up the protected-mode data segment registers
		movw	$PROT_MODE_DSEG, %ax	# Our data segment selector
		movw	%ax, %ds		# -> DS: Data Segment
		movw	%ax, %es		# -> ES: Extra Segment
		movw	%ax, %fs		# -> FS
		movw	%ax, %gs		# -> GS
		movw	%ax, %ss		# -> SS: Stack Segment

		call cmain			# finish the boot load from C.
						# cmain() should not return
spin:		jmp spin			# ..but in case it does, spin

.p2align 2					# force 4 byte alignment
gdt:
	SEG_NULL				# null seg
	SEG(STA_X|STA_R, 0x0, 0xffffffff)	# code seg
	SEG(STA_W, 0x0, 0xffffffff)	        # data seg
	
gdtdesc:
	.word	0x17			# sizeof(gdt) - 1
	.long	gdt			# address gdt

BOOTDRIVE: .byte 0
hello:	   .asciz "\n\rHello world\n\r"
kern_offset: .word 0x0  # kern initial offset
kern_sector: .byte 0x2     # kern start from 2nd sector
sectors_per_track: .byte 0
cylinders: .word 0
headers: .word 0
