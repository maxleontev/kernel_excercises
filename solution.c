#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/cdev.h>


static dev_t first;
static unsigned int count = 1;
static int max_chr_drv_major = 500;
static int max_chr_drv_minor = 0;
static struct cdev *maxs_cdev;

#define MAX_DEV_NAME "maxchrdev"
#define KBUF_SIZE (size_t) 10*PAGE_SIZE

static struct class *maxs_class;

static int max_chr_drv_open(struct inode *inode, struct file *file) {
	static int counter = 0;
	char *kbuf = kmalloc(KBUF_SIZE, GFP_KERNEL);

	file->private_data = kbuf;

	printk( KERN_INFO "Opening device: %s \n", MAX_DEV_NAME);
	counter++;

	printk( KERN_INFO "counter: %d\n", counter);
	printk( KERN_INFO "module refcounter: %d\n", module_refcount(THIS_MODULE));

	return 0;
}

static int max_chr_drv_release(struct inode *inode, struct file *file) {
	char *kbuf = file->private_data;
	printk( KERN_INFO "Releasing device: %s \n", MAX_DEV_NAME);
	if (kbuf) kfree(kbuf);
	kbuf = NULL;
	file->private_data = NULL;

	return 0;
}

static ssize_t max_chr_drv_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos) {
	char *kbuf = file->private_data;
	int nbytes = lbuf - copy_to_user(buf, kbuf + *ppos, lbuf);

	*ppos += nbytes;
	printk( KERN_INFO "Reading device: %s, bytes readed = %d, ppos = %d\n", MAX_DEV_NAME, nbytes, (int) *ppos );
	return nbytes;
}

static ssize_t max_chr_drv_write(struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos) {
	char *kbuf = file->private_data;
	int nbytes = lbuf - copy_from_user(kbuf + *ppos, buf, lbuf);
	*ppos += nbytes;

	printk( KERN_INFO "Writing device: %s, bytes writed = %d, ppos = %d\n", MAX_DEV_NAME, nbytes, (int) *ppos );
	return nbytes;
}

static loff_t max_chr_drv_lseek (struct file *file, loff_t offset, int orig) {
    loff_t testpos;

    switch (orig) {
        case SEEK_SET :
            testpos = offset;
            break;
        case SEEK_CUR : 
            testpos = file->f_pos + offset;
            break;
        case SEEK_END : 
            testpos = KBUF_SIZE + offset;
            break;
        default:
            return -EINVAL;
            break;
    }

    if (testpos >= KBUF_SIZE) testpos = KBUF_SIZE - 1;
    if (testpos < 0) testpos = 0;
    file->f_pos = testpos;

    printk ( KERN_INFO "Seeking to %ld position\n", (long)testpos);

    return testpos;
}

static const struct file_operations maxs_cdev_fops = {
	.owner = THIS_MODULE,
	.read = max_chr_drv_read,
	.write = max_chr_drv_write,
	.open = max_chr_drv_open,
	.release = max_chr_drv_release,
    .llseek = max_chr_drv_lseek
};

static int __init init_module_chrdrv(void) {
	printk( KERN_INFO "Hello, solution module loaded!\n" );

	first = MKDEV (max_chr_drv_major, max_chr_drv_minor);
	register_chrdev_region (first, count, MAX_DEV_NAME);

	maxs_cdev = cdev_alloc();

	cdev_init (maxs_cdev, &maxs_cdev_fops);
	cdev_add (maxs_cdev, first, count);

    maxs_class = class_create (THIS_MODULE, "maxs_class");
    device_create (maxs_class, NULL, first, "%s", "chrdrv");

    printk ( KERN_INFO "Created device class :%s\n", MAX_DEV_NAME);

	return 0;
}

static void __exit exit_module_chrdrv(void) {

    device_destroy (maxs_class, first);
    class_destroy (maxs_class);

	if (maxs_cdev)
		cdev_del (maxs_cdev);
	unregister_chrdev_region(first, count);
	printk( KERN_INFO "Hello, solution module leaved!\n" );
}

module_init(init_module_chrdrv);
module_exit(exit_module_chrdrv);

MODULE_LICENSE("GPL");