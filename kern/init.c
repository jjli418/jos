/* See COPYRIGHT for copyright information. */

#include <inc/asm.h>
#include <inc/stdio.h>
#include <inc/string.h>
#include <inc/assert.h>

#include <kern/monitor.h>
#include <kern/console.h>
#include <kern/pmap.h>
#include <kern/kclock.h>
#include <kern/env.h>
#include <kern/trap.h>
#include <kern/sched.h>
#include <kern/picirq.h>

#include <kern/keyboard.h>

#include <inc/elf.h>


u_int elf_hdr   = 0xf0007E00;     // elf header
u_int sh_addr   = 0xf01E0000;     // section header
u_int sym_table = 0xf01C0000;     // symbol section
u_int str_table = 0xf01A0000;     // string section

u_int sym_entry_count = -1;


void
readseg_kern(u_int va, u_int count, u_int offset);

void
test_backtrace(int x)
{
	printf("entering test_backtrace %d\n", x);
	if (x > 0)
		test_backtrace(x-1);
	else
		mon_backtrace(0, 0);
	printf("leaving test_backtrace %d\n", x);
}



void
load_tables(void)
{
	// get the section header
	struct Elf *elf  = (struct Elf *)elf_hdr;
	u_int sh_off     = elf->e_shoff;
	u_int sh_num     = elf->e_shnum;
	u_int sh_entsize = elf->e_shentsize;
	u_int i;

	readseg_kern(sh_addr, sh_num * sh_entsize, sh_off);

	// rectify the address
	//sh_addr += (sh_off & 511);

	printf("section header sh off=%x, shaddr=%x, sh_num=%d sh_entsize=%d\n", sh_off, sh_addr, sh_num, sh_entsize);

	
	Elf32_Hsdr *sh = (Elf32_Hsdr*) sh_addr;

	int sym_table1 = sym_table;
	int str_table1 = str_table;

	for(i = 0; i < sh_num; i++, sh++)
	{
		if(sh->sh_type == SHT_SYMTAB)
		{
		    readseg_kern(sym_table1, sh->sh_size, sh->sh_offset);
		    printf("sym table offset:%x\n", sh->sh_offset);

		    //rectify the address
		    sym_table1 += sh->sh_offset ;
		    //sym_table += (sh->sh_offset & 511);

		    // get the size
		    sym_entry_count = sh->sh_size >> 4; // size/16
		}
		else if(sh->sh_type == SHT_STRTAB && i != elf->e_shstrndx)
		{
		    readseg_kern(str_table1, sh->sh_size, sh->sh_offset);
		    printf("str table offset:%x\n", sh->sh_offset);
		    
		    //rectify the address
		    //str_table += (sh->sh_offset & 511);
		    str_table1 += sh->sh_offset;
		}
	}

	printf("symbol addr= %p, string table addr= %p\n", sym_table, str_table);

}

u_char*
find_symbol(u_int address)
{
	Elf32_Sym  *psym = (Elf32_Sym*)sym_table;
	u_char     *pstr = (u_char *)str_table;
	u_char 	   *result = NULL;
	u_int i;

	printf("entry count is:%d\n", sym_entry_count);
	for( i = 0; i < sym_entry_count; i++)
	{
		if( psym[i].st_value == address)
		{
			result = pstr + psym[i].st_name;
		}
	}

	printf("find symbol:%s\n", result);


    	return result;
}

void detect_mem()
{

char hi, low;

outb(0x34, 0x70);
low = inb(0x71);

outb(0x35, 0x70);
hi = inb(0x71);

int val = 0;
val += (hi & 0xFF);
val <<= 8;

val += (low & 0xFF);

printf("total mem=%x\n", val);
val <<= 16;

printf("total mem=%x\n", val);
}

void
i386_init(void)
{
	extern char edata[], end[];

	printf("1\n");
	// Before doing anything else,
	// clear the uninitialized global data (BSS) section of our program.
	// This ensures that all static/global variables start out zero.
	memset(edata, 0, end-edata);

	printf("2\n");
	// Initialize the console.
	// Can't call printf until after we do this!
	cons_init();

	printf("3\n");
	// load symbol and string table
	load_tables();

	printf("480 decimal is %o octal!\n", 480);

	u_int i = 0x00646c72;
	warn("H%x Wo%s", 57616, &i);

	int x = 1, y = 3, z = 4;
	warn("x %d, y %x, z %d\n", x, y, z);


	//test_backtrace(5); //??????????????????

	// Lab 2 memory management initialization functions
	i386_detect_memory();
	i386_vm_init();
	page_init();
	page_check();

	// Lab 3 user environment initialization functions
	env_init();
	idt_init();

	// Lab 4 multitasking initialization functions
	pic_init();
	kclock_init();

	// Should always have an idle process as first one.
	ENV_CREATE(user_idle);

#if defined(TEST)
	// Don't touch -- used by grading script!
	ENV_CREATE2(TEST, TESTSIZE)
#else
	// Touch all you want.
	ENV_CREATE(user_primes);
#endif // TEST*

	// detect_mem(); ?????????????????????
	// Lab 2 memory management initialization functions
	i386_detect_memory();

	printf("4\n");
	i386_vm_init();
	printf("5\n");
	page_init();
	printf("6\n");
	page_check();

	printf("7\n");
	// Lab 3 user environment initialization functions
	env_init();
	idt_init();


	// Temporary test code specific to LAB 3
#if defined(TEST)
	// Don't touch -- used by grading script!
	ENV_CREATE2(TEST, TESTSIZE);
#else
	// Touch all you want.
	//ENV_CREATE(user_faultread);
	//ENV_CREATE(user_evilhello);
#endif // TEST*

	// Schedule and run the first user environment!
	sched_yield();

	// We only have one user environment for now, so just run it.
	env_run(&envs[NENV-1]);

	// Drop into the kernel monitor.
	while (1)
		monitor(NULL);
}


/*
 * Variable panicstr contains argument to first call to panic; used as flag
 * to indicate that the kernel has already called panic.
 */
static const char *panicstr;

/*
 * Panic is called on unresolvable fatal errors.
 * It prints "panic: mesg", and then enters the kernel monitor.
 */
void
_panic(const char *file, int line, const char *fmt,...)
{
	va_list ap;

	if (panicstr)
		goto dead;
	panicstr = fmt;

	va_start(ap, fmt);
	printf("kernel panic at %s:%d: ", file, line);
	vprintf(fmt, ap);
	printf("\n");
	va_end(ap);

dead:
	/* break into the kernel monitor */
	while (1)
		monitor(NULL);
}

/* like panic, but don't */
void
_warn(const char *file, int line, const char *fmt,...)
{
	va_list ap;

	va_start(ap, fmt);
	printf("kernel warning at %s:%d: ", file, line);
	vprintf(fmt, ap);
	printf("\n");
	va_end(ap);
}

