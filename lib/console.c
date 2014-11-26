
#include <inc/string.h>
#include <inc/lib.h>

void
putchar(int ch)
{
	char s[2];

	// Putchar is only called in two places:
	// from readline when input is the console,
	// and from printf.  In both cases we want to
	// write to the console.

	s[0] = ch;
	s[1] = 0;
	sys_cputs(s);
}

int
getchar(void)
{
	return sys_cgetc();
}


