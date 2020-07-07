#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Max Leontev"); 
MODULE_DESCRIPTION("Linux char driver for the modules development course");
MODULE_VERSION("0.1");

static dev_t first;
static unsigned int count = 1;
static int max_chr_drv_major = 240;
static int max_chr_drv_minor = 0;
static struct cdev *maxs_cdev;

#define MAX_DEV_NAME "maxchrdev"
#define KBUF_SIZE (size_t) 10*PAGE_SIZE

static int m_count = -1; // device access count

static void kern_log(char *fmt, ...) { // arguments should be the same as for printk
	char s[256];
	va_list args;
	va_start(args, fmt);
	vsprintf(s, fmt, args);
	va_end(args);
	printk(KERN_INFO "[%s] %s\n", MAX_DEV_NAME, s); //you should add [kernel_mooc] at the beginning of the string for stepik.org debuging
}

static ssize_t max_chr_drv_read (struct file *file, char __user *buf, size_t lbuf, loff_t *ppos) {
	char *kbuf = file->private_data;
	int nbytes = lbuf - copy_to_user(buf, kbuf + *ppos, lbuf);

	*ppos += nbytes;
	kern_log("Reading, bytes readed = %d, ppos = %d", nbytes, (int) *ppos);
	return nbytes;
}

static ssize_t max_chr_drv_write (struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos) {
	char *kbuf = file->private_data;
	int nbytes = lbuf - copy_from_user(kbuf + *ppos, buf, lbuf);
	*ppos += nbytes;

	kern_log("Writing, bytes writed = %d, ppos = %d", nbytes, (int) *ppos);
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

	kern_log("Seeking to %ld position", (long)testpos);

    return testpos;
}

static int max_chr_drv_open(struct inode *inode, struct file *file) {

	char *kbuf = kmalloc(KBUF_SIZE, GFP_KERNEL);
	file->private_data = kbuf;

	m_count++;
	
	kern_log("Opening, m_count: %d", m_count);
	return 0;
}

static int max_chr_drv_release(struct inode *inode, struct file *file) {
	char *kbuf = file->private_data;
	kern_log("Releasing");
	if (kbuf) kfree(kbuf);
	kbuf = NULL;
	file->private_data = NULL;
	return 0;
}

static const struct file_operations maxs_cdev_fops = {
	.owner = THIS_MODULE,
	.read = max_chr_drv_read,
	.write = max_chr_drv_write,
	.llseek = max_chr_drv_lseek,
	.open = max_chr_drv_open,
	.release = max_chr_drv_release
};

static int __init init_mod(void) {
	kern_log("Loaded!");

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
	kern_log("Leaved!");
}

module_init(init_mod);
module_exit(exit_mod);