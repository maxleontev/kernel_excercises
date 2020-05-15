#include <linux/module.h>
#include "checker.h"

EXPORT_SYMBOL(call_me);

static int __init init_checker(void) {
    printk( KERN_INFO "Hello, checker module loaded!\n" );
	return 0;
}

static void __exit exit_checker(void) {
    printk( KERN_INFO "Hello, checker module leaved!\n" );
}

module_init(init_checker);
module_exit(exit_checker);

MODULE_LICENSE("GPL");


