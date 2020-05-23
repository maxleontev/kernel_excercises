#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>


static int my_sys;

static int a = 0;
module_param (a, int, 0);

static int b = 0;
module_param (b, int, 0);

static int c[] = {0,0,0,0,0};
static int c_count = sizeof(c)/sizeof(c[0]);
module_param_array (c, int, &c_count, S_IRUGO|S_IWUSR);


static ssize_t my_sys_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
	return sprintf(buf, "%d\n", my_sys);
}

static ssize_t my_sys_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
	int ret;

	ret = kstrtoint(buf, 10, &my_sys);
	if (ret < 0)
		return ret;

	return count;
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

	int res, i, arr_cnt;

	my_kobj = kobject_create_and_add("my_kobject", kernel_kobj);
	if (!my_kobj)
		return -ENOMEM;

	res = sysfs_create_group(my_kobj, &attr_group);
	if (res) kobject_put(my_kobj);

	printk( KERN_INFO "Hello, solution module loaded!\n" );
	arr_cnt = 0;
	for (i=0; i < c_count; i++) {
		arr_cnt = arr_cnt + c[i];
	}
	my_sys = a + b + arr_cnt;

	return res;
}

static void __exit exit_mod(void) {
	kobject_put(my_kobj);
	printk( KERN_INFO "Hello, solution module leaved!\n" );
}

module_init(init_mod);
module_exit(exit_mod);

MODULE_LICENSE("GPL");