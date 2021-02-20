#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/uaccess.h>

#define DRIVER_NAME "foobar_chrdev"

#define GPIO_LINE 16 

static int major_number;

static int bytes_read;

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
	/* Reset number of bytes read */
	bytes_read = 0;
	return 0;
}

static int foobar_chrdev_close(struct inode *inode, struct file *file) 
{	
	return 0;
}

static ssize_t foobar_chrdev_read(struct file *file, char __user *user_buffer, 
                                  size_t size, loff_t *offset)
{
 	int val, cnt;
 	size_t length = size;
	char buff[3];

	/* If we're at the end of buffer, return 0 signifying end of file */
	if (bytes_read == sizeof(buff)) {
		return 0;
	}

	/* Get GPIO value */
 	val = gpio_get_value(GPIO_LINE);
	buff[0] = val + '0';
	buff[1] = '\n';
	buff[2] = '\0';

	/* Copy value to user space buffer */
	printk(KERN_INFO "%s: gpio value: %d\n", DRIVER_NAME, val);

	while (length != 0 && bytes_read < sizeof(buff))
	{
		cnt = put_user(buff[bytes_read++], user_buffer++);
		if (cnt != 0)
		{
        	printk(KERN_INFO "put_user failed: %d", cnt);
		}
		length--;
	}

	return bytes_read;
}

static ssize_t foobar_chrdev_write(struct file *file, 
                                   const char __user *user_buffer,
                                   size_t size, loff_t * offset)
{
	/* Check user input */
	if (user_buffer[0]=='1')
		gpio_set_value(GPIO_LINE, 1); /* GPIO ON  */
	else if (user_buffer[0]=='0')
		gpio_set_value(GPIO_LINE, 0); /* GPIO OFF */
	else
		printk(KERN_INFO "%s: wrong user input: %c\n", DRIVER_NAME, user_buffer[0]);

	return size;
}

int foobar_chrdev_init(void)
{
	int err;

	printk(KERN_INFO "%s: %s\n", DRIVER_NAME, __FUNCTION__);

	/* Register character device driver, major number dynamically allocated */
	major_number = register_chrdev(0, DRIVER_NAME, &foobar_chrdev_fops);
	if (major_number < 0) {
		printk(KERN_INFO "%s: register_chrdev failed: %d\n", DRIVER_NAME, major_number);
		return major_number;
	}

	printk(KERN_INFO "%s: run 'mknod /dev/%s c %d 0' to create a dev file.\n", 
		DRIVER_NAME, DRIVER_NAME, major_number);

	/* Check GPIO validity */
	err = gpio_is_valid(GPIO_LINE);
	if(err < 0) {
		printk(KERN_INFO "%s: gpio_is_valid failed: %d\n", DRIVER_NAME, err);
		return err;
	}

	/* Request GPIO */
	err = gpio_request(GPIO_LINE, "GPIO_LINE");
	if (err < 0) {
		printk(KERN_INFO "%s: gpio_request failed: %d\n", DRIVER_NAME, err);
		return err;
	}

	/* Set GPIO direction and output */
	err = gpio_direction_output(GPIO_LINE, 0);
	if (err < 0) {
		printk(KERN_INFO "%s: gpio_direction_output failed: %d\n", DRIVER_NAME, err);
		return err;
	}

    return 0;
}

void foobar_chrdev_exit(void)
{
	printk(KERN_INFO "%s: %s\n", DRIVER_NAME, __FUNCTION__);

	/* Free claimed GPIO */
	gpio_free(GPIO_LINE);

	/* Register character device driver */
	unregister_chrdev(major_number, DRIVER_NAME); 
}

module_init(foobar_chrdev_init);
module_exit(foobar_chrdev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("KHBX");
