#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

#define MAJOR_NUMBER 42
#define DRIVER_NAME "foobar_chrdev"

static int foobar_chrdev_open(struct inode *inode, struct file *file);

static ssize_t foobar_chrdev_read(struct file *file, char __user *user_buffer, 
                              size_t size, loff_t *offset);

static ssize_t foobar_chrdev_write(struct file *file, 
                               const char __user *user_buffer,
                               size_t size, loff_t * offset);

static int foobar_chrdev_close(struct inode *inode, struct file *file);

const struct file_operations foobar_chrdev_fops = {
    .owner = THIS_MODULE,
    .open = foobar_chrdev_open,
    .read = foobar_chrdev_read,
    .write = foobar_chrdev_write,
    .release = foobar_chrdev_close
};

static int foobar_chrdev_open(struct inode *inode, struct file *file) 
{	
	printk(KERN_ALERT "%s\n", __FUNCTION__);
	return 0;
}

static ssize_t foobar_chrdev_read(struct file *file, char __user *user_buffer, 
                              size_t size, loff_t *offset)
{
	printk(KERN_ALERT "%s\n", __FUNCTION__);
	return 0;
}

static ssize_t foobar_chrdev_write(struct file *file, 
                               const char __user *user_buffer,
                               size_t size, loff_t * offset)
{
	printk(KERN_ALERT "%s\n", __FUNCTION__);
	return size;
}

static int foobar_chrdev_close(struct inode *inode, struct file *file) 
{	
	printk(KERN_ALERT "%s\n", __FUNCTION__);
	return 0;
}


int foobar_chrdev_init(void)
{
	int err;

	printk(KERN_ALERT "%s\n", __FUNCTION__);

	/* Register character device driver */
	err = register_chrdev(MAJOR_NUMBER, DRIVER_NAME, &foobar_chrdev_fops);

	if (err != 0)
		return err;

	return 0;
}

void foobar_chrdev_exit(void)
{
	printk(KERN_ALERT "Inside %s\n", __FUNCTION__);

	/* Register character device driver */
	unregister_chrdev(MAJOR_NUMBER, DRIVER_NAME); 
}

module_init(foobar_chrdev_init);
module_exit(foobar_chrdev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("KHBX");
