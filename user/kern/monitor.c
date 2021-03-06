// Simple command-line kernel monitor useful for
// controlling the kernel and exploring the system interactively.

#include <inc/stdio.h>
#include <inc/string.h>
#include <inc/pmap.h>
#include <inc/assert.h>
#include <inc/x86.h>

#include <kern/console.h>
#include <kern/monitor.h>
#include <kern/trap.h>

#include <kern/pmap.h>

#define CMDBUF_SIZE	80	// enough for one VGA text line

struct Command {
	const char *name;
	const char *desc;
	void (*func)(int argc, char **argv);
};

static struct Command commands[] = {
	{"help",	"Display this list of commands", mon_help},
	{"kerninfo",	"Display information about the kernel", mon_kerninfo},
	{"alloc_page",	"Allocate a physical page", mon_alloc_page},
	{"page_status",	"Show status of a page at the physical address", mon_page_status},
	{"free_page",	"Free a page at the pysical address", mon_free_page},
	{"halt",	"Halt the processor", mon_halt}
};
#define NCOMMANDS (sizeof(commands)/sizeof(commands[0]))



/***** Implementations of basic kernel monitor commands *****/

void
mon_help(int argc, char **argv)
{
	int i;

	for (i = 0; i < NCOMMANDS; i++)
		printf("%s - %s\n", commands[i].name, commands[i].desc);
}

void
mon_kerninfo(int argc, char **argv)
{
	extern char _start[], etext[], edata[], end[];

	printf("Special kernel symbols:\n");
	printf("  _start %08x (virt)  %08x (phys)\n", _start, _start-KERNBASE);
	printf("  etext  %08x (virt)  %08x (phys)\n", etext, etext-KERNBASE);
	printf("  edata  %08x (virt)  %08x (phys)\n", edata, edata-KERNBASE);
	printf("  end    %08x (virt)  %08x (phys)\n", end, end-KERNBASE);
	printf("Kernel executable memory footprint: %dKB\n",
		(end-_start+1023)/1024);
}

void 
mon_alloc_page(int argc, char **argv)
{
	struct Page * pPage;

	if(page_alloc( &pPage) != 0)
		panic("unable to allocate a free page");

	else
	{
		pPage->pp_ref++;
		printf("  0x%x\n", page2pa(pPage));
	}
}

void 
mon_page_status(int argc, char **argv)
{
	if(argc < 2)
		panic("Please input a physical address.\n");

	char* strAddr = argv[1];
	char* end;

	u_long phyAddr = strtol(strAddr, &end, 16) & (~(BY2PG - 1));

	struct Page * pPage = pa2page(phyAddr);

	if( pPage->pp_ref > 0)
		printf("  Allocated\n");
	else
		printf("  Free\n");

}

void 
mon_free_page(int argc, char **argv)
{	
	if(argc < 2)
		panic("Please input a physical address.\n");

	char* strAddr = argv[1];
	char* end;

	u_long phyAddr = strtol(strAddr, &end, 16) & (~(BY2PG - 1));

	struct Page * pPage = pa2page(phyAddr);

	pPage->pp_ref = 0;
	page_free(pPage);

}

u_char* find_symbol(u_int);

void
mon_backtrace(int argc, char **argv)
{
	// Your code here.
	printf("mon_backtrace:\n");


	u_int *pebp = (u_int*)read_ebp();
	u_int ebp, ip, args[5], i, tip, offset_fun;
	
	while(pebp)
	{
		ebp = *pebp;
		ip  = *(pebp+1);

		for(i = 0; i < 5; i++)
			args[i] = *(pebp + 2 + i);

		
		// get the called function address
		offset_fun = ip - 4;
		tip = ip + *((u_int*)offset_fun);
		u_char *pfun_name = find_symbol(tip);


		if(pfun_name != NULL)
			// print the stack
			printf("	ebp %08x  function %s eip %08x args %08x %08x %08x %08x %08x\n", \
				    ebp, pfun_name, ip,  args[0], args[1], args[2], args[3], args[4]);


		pebp = (u_int*)ebp;

	}

}


/***** Kernel monitor command interpreter *****/

#define WHITESPACE " \t\r\n"
#define MAXARGS 16

static void
runcmd(char *buf)
{
	int argc;
	char *argv[MAXARGS];
	int i;

	// Parse the command buffer into whitespace-separated arguments
	argc = 0;
	argv[argc] = 0;
	while (1) {
		// gobble whitespace
		while (*buf && strchr(WHITESPACE, *buf))
			*buf++ = 0;
		if (*buf == 0)
			break;

		// save and scan past next arg
		if (argc == MAXARGS-1) {
			printf("Too many arguments (max %d)\n", MAXARGS);
			return;
		}
		argv[argc++] = buf;
		while (*buf && !strchr(WHITESPACE, *buf))
			buf++;
	}
	argv[argc] = 0;

	// Lookup and invoke the command
	if (argc == 0)
		return;
	for (i = 0; i < NCOMMANDS; i++) {
		if (strcmp(argv[0], commands[i].name) == 0) {
			commands[i].func(argc, argv);
			return;
		}
	}
	printf("Unknown command '%s'\n", argv[0]);
}

void 
mon_halt(int argc, char **argv) {
	asm("STI\nHLT");
}


void
monitor(struct Trapframe *tf)
{
	char *buf;

	printf("Welcome to the JOS kernel monitor!\n");
	printf("Type 'help' for a list of commands.\n");

	if (tf != NULL)
		print_trapframe(tf);

	while (1) {
		buf = readline("K> ");
		if (buf != NULL)
			runcmd(buf);
	}
}

