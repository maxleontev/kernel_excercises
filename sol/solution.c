#include <linux/slab.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Max Leontev"); 
MODULE_DESCRIPTION("Linux char driver for the modules development course");
MODULE_VERSION("0.1");

static int max_chr_drv_major_number;

static struct class* max_chr_drv_class = NULL; 	// The device-driver class struct pointer
static struct device* max_chr_drv_device = NULL;// The device-driver device struct pointer

#define MAX_CLASS_NAME  "maxchrdev"				// The device class -- this is a character device driver

#define KBUF_SIZE (size_t) 10*PAGE_SIZE
static char *kbuf;								//define main single buffer for all 
static char *msg_ptr;

static char *node_name = "chrdrv";

module_param(node_name, charp, S_IRUGO);

static int max_chr_drv_open(struct inode *inode, struct file *file) {
	msg_ptr = kbuf;
	printk( KERN_INFO "Solution module: Opening device: %s \n", MAX_CLASS_NAME);
	return 0;
}

static int max_chr_drv_release(struct inode *inode, struct file *file) {
	printk( KERN_INFO "Solution module: Releasing device: %s \n", MAX_CLASS_NAME);
	return 0;
}

static ssize_t max_chr_drv_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos) {
	int nbytes = 0;

	sprintf(kbuf, "%d\n", max_chr_drv_major_number);

	if (*msg_ptr == 0) return 0;
	while ( lbuf && *msg_ptr ) {
		put_user(*(msg_ptr++), buf++);
		lbuf--;
    	nbytes++;
	}
	*ppos += nbytes;
	printk( KERN_INFO "Solution module: Reading device: %s, bytes readed = %d, ppos = %d\n", MAX_CLASS_NAME, nbytes, (int) *ppos );
	return nbytes;
}

static ssize_t max_chr_drv_write(struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos) {
	int nbytes = lbuf - copy_from_user(kbuf + *ppos, buf, lbuf);
	*ppos += nbytes;

	printk( KERN_INFO "Solution module: Writing device: %s, bytes writed = %d, ppos = %d\n", MAX_CLASS_NAME, nbytes, (int) *ppos );
	return nbytes;
}

static const struct file_operations maxs_cdev_fops = {
	.owner = THIS_MODULE,
	.read = max_chr_drv_read,
	.write = max_chr_drv_write,
	.open = max_chr_drv_open,
	.release = max_chr_drv_release
};

static int __init init_mod(void) {

	printk( KERN_INFO "Solution module: Loaded!\n" );

	// Try to dynamically allocate a major number for the device -- more difficult but worth it
	max_chr_drv_major_number = register_chrdev(0, node_name, &maxs_cdev_fops);
	if (max_chr_drv_major_number < 0) {
		printk(KERN_ALERT "Solution module: failed to register a major number\n");
		return max_chr_drv_major_number;
	}
	
	printk(KERN_INFO "Solution module: registered major number %d\n", max_chr_drv_major_number);
	
	// Register the device class
	max_chr_drv_class = class_create(THIS_MODULE, MAX_CLASS_NAME);
	if (IS_ERR(max_chr_drv_class)) {										// Check for error and clean up if there is
		unregister_chrdev(max_chr_drv_major_number, node_name);
		printk(KERN_ALERT "Solution module: Failed to register device class\n");
		return PTR_ERR(max_chr_drv_class);									// Correct way to return an error on a pointer
	}
	printk(KERN_INFO "Solution module: device class registered correctly\n");

	// Register the device driver
	max_chr_drv_device = device_create(max_chr_drv_class, NULL, MKDEV(max_chr_drv_major_number, 0), NULL, node_name);
	if (IS_ERR(max_chr_drv_device)) {										// Clean up if there is an error
		class_destroy(max_chr_drv_class); 									// Repeated code but the alternative is goto statements
		unregister_chrdev(max_chr_drv_major_number, node_name);
		printk(KERN_ALERT "Solution module: Failed to create the device\n");
		return PTR_ERR(max_chr_drv_device);
	}
	printk(KERN_INFO "Solution module: device class created correctly\n"); // Made it! device was initialized

	kbuf = kmalloc(KBUF_SIZE, GFP_KERNEL);

	return 0;
}

static void __exit exit_mod(void) {

	if (kbuf) kfree(kbuf);

	device_destroy(max_chr_drv_class, MKDEV(max_chr_drv_major_number, 0));	// remove the device
	class_unregister(max_chr_drv_class);									// unregister the device class
	class_destroy(max_chr_drv_class);										// remove the device class
	unregister_chrdev(max_chr_drv_major_number, node_name);			// unregister the major number

	printk( KERN_INFO "Solution module: Leaved!\n" );
}

module_init(init_mod);
module_exit(exit_mod);