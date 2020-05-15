#include <linux/module.h>
#include <linux/device.h>
#include <linux/slab.h>

#include "checker.h"

extern ssize_t get_void_size(void);
extern void submit_void_ptr(void *p);
extern ssize_t get_int_array_size(void);
extern void submit_int_array_ptr(int *p);
extern void submit_struct_ptr(struct device *p);
extern void checker_kfree(void *p);


void * void_ptr;
int * int_arr_ptr;
struct device * struct_device_ptr;

static int __init init_solution(void) {
//	printk( KERN_INFO "Hello, solution module loaded!\n" );

    void_ptr = kmalloc(get_void_size(), GFP_ATOMIC);
    submit_void_ptr(void_ptr);

    int_arr_ptr = kmalloc(get_int_array_size()*sizeof(int), GFP_ATOMIC);
    submit_int_array_ptr(int_arr_ptr);

    struct_device_ptr = kmalloc(sizeof(struct device), GFP_ATOMIC);
    submit_struct_ptr(struct_device_ptr);

	return 0;
}

static void __exit exit_solution(void) {
    
    checker_kfree(void_ptr);
    checker_kfree(int_arr_ptr);
    checker_kfree(struct_device_ptr);

//	printk( KERN_INFO "Hello, solution module leaved!\n" );
}

module_init(init_solution);
module_exit(exit_solution);

MODULE_LICENSE("GPL");