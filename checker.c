#include <linux/module.h>
#include "checker.h"

EXPORT_SYMBOL(get_void_size);
EXPORT_SYMBOL(submit_void_ptr);
EXPORT_SYMBOL(get_int_array_size);
EXPORT_SYMBOL(submit_int_array_ptr);
EXPORT_SYMBOL(submit_struct_ptr);
EXPORT_SYMBOL(checker_kfree);


static int __init init_mod(void) {
    printk( KERN_INFO "Hello, checker module loaded!\n" );
	return 0;
}

static void __exit exit_mod(void) {
    printk( KERN_INFO "Hello, checker module leaved!\n" );
}

module_init(init_mod);
module_exit(exit_mod);

MODULE_LICENSE("GPL");


