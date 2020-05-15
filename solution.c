#include <linux/module.h>

#include "checker.h"

extern int array_sum(short *arr, size_t n);
extern ssize_t generate_output(int sum, short *arr, size_t size, char *buf);

int array_sum_local(short *arr, size_t n) {
    int sum = 0;
    int i;
    
    for (i = 0; i < n; i++)
        sum += arr[i];
    return sum;
}

static int __init init_solution(void) {
    int i, sum;
    char str_buff[500];
    short arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};

//    CHECKER_MACRO;
    printk( KERN_INFO "Hello, solution module loaded!\n" );

    for (i=0;i<15;i++) {
        sum = array_sum(arr, 5 + i);
        generate_output(sum, arr, 5 + i, str_buff);
        if (sum == array_sum_local(arr, 5 + i)) printk( KERN_INFO "%s\n", str_buff);
        else printk( KERN_ERR "%s\n", str_buff);
    }
	return 0;
}

static void __exit exit_solution(void) {
//    CHECKER_MACRO;
    printk( KERN_INFO "Hello, solution module leaved!\n" );
}

module_init(init_solution);
module_exit(exit_solution);

MODULE_LICENSE("GPL");