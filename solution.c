#include <linux/module.h>
#include "checker.h"

extern void call_me(const char* message);

static int __init init_solution(void) {
    printk( KERN_INFO "Hello, solution module loaded!\n" );
    call_me("Kernel message!");
	return 0;
}

static void __exit exit_solution(void) {
    printk( KERN_INFO "Hello, solution module leaved!\n" );
}

module_init(init_solution);
module_exit(exit_solution);

MODULE_LICENSE("GPL");