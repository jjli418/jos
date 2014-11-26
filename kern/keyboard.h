
#ifndef __KEYBOARD__H__
#define __KEYBOARD__H__

#include <inc/kbdreg.h>
#include <inc/x86.h>

void test_keyboard();
void send_to_8048(int cmd);
void send_to_8042(int cmd);
int read_8042_state();
int read_8048_data();
void set_capslock(int);

#endif 



