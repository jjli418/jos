
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

	// Writing to the console always during printf rather
	// than to fd 1 is a break with traditional Unix, but in
	// this code, printf is more of a debugging statement
	// than a generic output statement.  It is very important
	// that it always go to the console, especially when 
	// debugging the file descriptor code!

	s[0] = ch;
	s[1] = 0;
	sys_cputs(s);
}

int
getchar(void)
{
	return sys_cgetc();
}


