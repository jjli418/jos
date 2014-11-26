
#include <kern/keyboard.h>
#include<inc/stdio.h>

void set_led();
void test_0x20();
void test_keyboard()
{
	int state = read_8042_state();
	int data = read_8048_data();
	printf("\n state=%d,############ data=%d\n", state, data);

	set_led();

	state = read_8042_state();
	data = read_8048_data();
	printf("\n state=%d,############ data=%d\n", state, data);

	test_0x20();
}

void send_to_8048(int cmd)
{
	while( (read_8042_state() & 2) != 0);

	outb(KBOUTP, cmd);
}

void send_to_8042(int cmd)
{
	while( (read_8042_state() & 2) != 0);

	outb(KBCMDP, cmd);
}

int read_8042_state()
{
	return inb(KBSTATP);
}

int read_8048_data()
{
	return inb(KBDATAP);
}

void set_led()
{
	int data = 1 | 2 | 4; // turn on scroll, NUM, Caps
	
	send_to_8048(0xED);	// turn on LED command
	send_to_8048(data);
}

void set_capslock(int onoff)
{
	int data = onoff ? 4 : 0;
	send_to_8048(0xED);	// turn on LED command
	send_to_8048(data);
}

void test_0x20()
{
	send_to_8042(0xAE); // enable keyboard
	send_to_8042(0x20);
	int data = read_8048_data();

	printf("\n ********** 0x20 data = %x\n", data);
}
