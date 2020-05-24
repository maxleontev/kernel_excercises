#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/cdev.h>


static dev_t first;
static unsigned int count = 1;
static int max_chr_drv_major = 240;
static int max_chr_drv_minor = 0;
static struct cdev *maxs_cdev;

#define MAX_DEV_NAME "maxchrdev"
#define KBUF_SIZE (size_t) 10*PAGE_SIZE
static char *kbuf; //define main single buffer for all 
static char *msg_ptr;

static int m_count = 0; // device access count
static int n_count = 0; // full writed count

static ssize_t max_chr_drv_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos) {
	int nbytes = 0;

	sprintf(kbuf, "%d %d\n", m_count, n_count);

	if (*msg_ptr == 0) return 0;
	while ( lbuf && *msg_ptr ) {
		put_user(*(msg_ptr++), buf++);
		lbuf--;
    	nbytes++;
	}
	*ppos += nbytes;
	printk( KERN_INFO "[kernel_mooc] Reading device: %s, bytes readed = %d, ppos = %d\n", MAX_DEV_NAME, nbytes, (int) *ppos );
	return nbytes;
}

static ssize_t max_chr_drv_write(struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos) {
	int nbytes = lbuf - copy_from_user(kbuf + *ppos, buf, lbuf);
	*ppos += nbytes;
	n_count += nbytes;

	printk( KERN_INFO "[kernel_mooc] Writing device: %s, bytes writed = %d, ppos = %d\n", MAX_DEV_NAME, nbytes, (int) *ppos );
	return nbytes;
}

static int max_chr_drv_open(struct inode *inode, struct file *file) {
	m_count++;

	msg_ptr = kbuf;

	printk( KERN_INFO "[kernel_mooc] Opening device: %s \n", MAX_DEV_NAME);
	printk( KERN_INFO "[kernel_mooc] m_count: %d n_count: %d\n", m_count, n_count);
	return 0;
}

static int max_chr_drv_release(struct inode *inode, struct file *file) {
	printk( KERN_INFO "[kernel_mooc] Releasing device: %s \n", MAX_DEV_NAME);
	return 0;
}

static const struct file_operations maxs_cdev_fops = {
	.owner = THIS_MODULE,
	.read = max_chr_drv_read,
	.write = max_chr_drv_write,
	.open = max_chr_drv_open,
	.release = max_chr_drv_release
};

static int __init init_mod(void) {
	printk( KERN_INFO "[kernel_mooc] Hello, solution module loaded!\n" );

	kbuf = kmalloc(KBUF_SIZE, GFP_KERNEL);

	first = MKDEV (max_chr_drv_major, max_chr_drv_minor);
	register_chrdev_region(first, count, MAX_DEV_NAME);

	maxs_cdev = cdev_alloc();

	cdev_init(maxs_cdev, &maxs_cdev_fops);
	cdev_add(maxs_cdev, first, count);

	return 0;
}

static void __exit exit_mod(void) {
	if (maxs_cdev)
		cdev_del (maxs_cdev);
	unregister_chrdev_region(first, count);
	if (kbuf) kfree(kbuf);
	printk( KERN_INFO "[kernel_mooc] Hello, solution module leaved!\n" );
}

module_init(init_mod);
module_exit(exit_mod);

MODULE_LICENSE("GPL");