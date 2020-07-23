#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>

#include <linux/sort.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Max Leontev");
MODULE_DESCRIPTION("Linux module for the modules development course");
MODULE_VERSION("0.1");

#define MAX_DEV_NAME "maxlistmodule"

#define MAX_MODULES_COUNT 500

static char* modules_str_arr[MAX_MODULES_COUNT];


static void kern_log(char *fmt, ...) { // arguments should be the same as for printk
	char s[256];
	va_list args;
	va_start(args, fmt);
	vsprintf(s, fmt, args);
	va_end(args);
	printk(KERN_INFO "[%s] %s\n", MAX_DEV_NAME, s); // you should add [kernel_mooc] at the beginning of the string for stepik.org debuging
}

// Defining comparator function as per the requirement 
static int myCompare(const void* a, const void* b) {
    return strcmp(*(const char**)a, *(const char**)b);
}

// Function to sort the array 
void str_list_sort(char* arr[], int n) { 
	sort((void *)arr, n, sizeof(const char*), myCompare, NULL);
} 

static ssize_t my_sys_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {

	int i, j=0, rt = 0;
	struct module *m_m;
	struct list_head *p;
	struct list_head *head;
	char *st;

	// list cleaning
	for (i=0; i<MAX_MODULES_COUNT; i++) {
		st = modules_str_arr[i];
		st[0] = 0;
	}

	kern_log("*THIS_MODULE : %p  THIS_MODULE->name : %s", THIS_MODULE, THIS_MODULE->name);
	head = THIS_MODULE->list.prev;

	i=0;

	list_for_each(p, head) {
		m_m = list_entry(p, struct module, list);
		if (strcmp(m_m->name, "") != 0 && (i < MAX_MODULES_COUNT)) {
			kern_log("i: %d, mm->name : %s", i, m_m->name);
			sprintf(modules_str_arr[i], "%s", m_m->name);
			i++;
		}
	}

	str_list_sort(modules_str_arr, i);

	for (j=0; j<i; j++) {
		if (j == 0)
			rt = sprintf(buf, "%s", modules_str_arr[j]);
		else 
			rt = sprintf(buf, "%s\n%s", buf, modules_str_arr[j]);
	}

	return rt;
}

static ssize_t my_sys_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
	return 0;
}

static struct kobj_attribute my_sys_attribute = __ATTR(my_sys, 0664, my_sys_show, my_sys_store);

static struct attribute *attrs[] = {
	&my_sys_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static struct kobject *my_kobj;

static int __init init_mod(void) {
	int res, i;

	my_kobj = kobject_create_and_add("my_kobject", kernel_kobj);
	if (!my_kobj) return -ENOMEM;

	res = sysfs_create_group(my_kobj, &attr_group);
	if (res) kobject_put(my_kobj);

	for (i=0; i<MAX_MODULES_COUNT; i++)
		modules_str_arr[i] = (char *) kmalloc(PAGE_SIZE, GFP_ATOMIC);

	kern_log("Loaded!");
	return res;
}

static void __exit exit_mod(void) {
	int i;

	kobject_put(my_kobj);

	for (i=0; i<MAX_MODULES_COUNT; i++) {
		if (modules_str_arr[i])
			kfree(modules_str_arr[i]);
	}

	kern_log("Leaved!");
}

module_init(init_mod);
module_exit(exit_mod);