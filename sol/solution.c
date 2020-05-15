#include <linux/module.h>
#include "checker.h"

extern void call_me(const char* message);

int init_module(void) {
	call_me("Hello from my module!");
	return 0;
}

void cleanup_module(void) {
	return;
}

MODULE_LICENSE("GPL");