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

static unsigned char session_counter = 0; // device access counter

static void kern_log(char *fmt, ...) { // arguments should be the same as for printk
	char s[256];
	va_list args;
	va_start(args, fmt);
	vsprintf(s, fmt, args);
	va_end(args);
	printk(KERN_INFO "[%s] %s\n", MAX_DEV_NAME, s); // you should add [kernel_mooc] at the beginning of the string for stepik.org debuging
}

static char get_session_id(struct file *file) {
	char *kbuf = (char *) file->private_data;
	return kbuf[0];
}

static ssize_t max_chr_drv_read (struct file *file, char __user *buf, size_t lbuf, loff_t *ppos) {
	char *kbuf = file->private_data;
	char *read_ptr = kbuf + *ppos;
	int nbytes = 0;

	kern_log("Session: %c, Reading func beginning: lbuf = %d, ppos = %d, file->f_pos = %d", get_session_id(file), lbuf, (int) *ppos, file->f_pos);

	while (lbuf && *read_ptr) {
		put_user(*(read_ptr++), buf++);
		lbuf--;
		nbytes++;
	}
	*ppos += nbytes;

	kern_log("Session: %c, Reading, bytes readed = %d, ppos = %d", get_session_id(file), nbytes, (int) *ppos);
	return nbytes;
}

static ssize_t max_chr_drv_write (struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos) {
	char *kbuf = file->private_data;
	int nbytes;

	kern_log("Session: %c, Writing func beginning: lbuf = %d, ppos = %d, file->f_pos = %d", get_session_id(file), lbuf, (int) *ppos, file->f_pos);

	nbytes = lbuf - copy_from_user(kbuf + *ppos, buf, lbuf);
	*ppos += nbytes;

	kern_log("Session: %c, Writing, bytes writed = %d, ppos = %d", get_session_id(file), nbytes, (int) *ppos);
	return nbytes;
}

static loff_t max_chr_drv_lseek (struct file *file, loff_t offset, int whence) {
    loff_t testpos;
	int size = 0;
	char *kbuf = file->private_data;

	kern_log("Session: %c, Seeking func beginning: offset = %d, file->f_pos = %d, whence = %d", get_session_id(file), offset, file->f_pos, whence);

	// data size calculation
	while (size < KBUF_SIZE && *kbuf) {kbuf ++; size ++;}

	kern_log("Seeking func size = %d", size);
	
    switch (whence) {
        case SEEK_SET : // 0
            testpos = offset;
            break;
        case SEEK_CUR : // 1
            testpos = file->f_pos + offset;
            break;
        case SEEK_END : // 2
            testpos = size + offset;
            break;
        default:
            return -EINVAL;
            break;
    }

    if (testpos >= size)
		testpos = size;
    if (testpos < 0)
		testpos = 0;
    file->f_pos = testpos;

	kern_log("Session: %c, Seeking to %ld position", get_session_id(file), (long) testpos);

    return testpos;
}

static int max_chr_drv_open(struct inode *inode, struct file *file) {
	int i;
	char *kbuf = kmalloc(KBUF_SIZE, GFP_KERNEL);
	if (!kbuf) {
		kern_log("Opening, kmalloc failed");
		return -EFAULT;
	}
	
	sprintf(kbuf, "%d", session_counter++);
	for (i=1;i<KBUF_SIZE;i++)
		kbuf[i] = 0;

	file->private_data = (void *)kbuf;	

	kern_log("Opening, session id: %c * * * * * * * * * * * * * * * * * *", get_session_id(file));
	return 0;
}

static int max_chr_drv_release(struct inode *inode, struct file *file) {
	void *kbuf = file->private_data;
	kern_log("Session: %c, Releasing", get_session_id(file));

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
	kern_log("Module loaded!");

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
	kern_log("Module leaved!");
}

module_init(init_mod);
module_exit(exit_mod);