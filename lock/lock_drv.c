/*设备驱动中，有些设备只允许打开一次，这里用count计数，lock对count进行保护*/
#include <linux/module.h>  
#include <linux/init.h>  
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/spinlock.h>//自旋锁头文件

static struct class *testdrv_class;
static struct class_device	*testdrv_class_dev;

int count = 0 ;
spinlock_t lock;

static int test_drv_open(struct inode *inode, struct file *file)
{
	spin_lock(&lock);
	if(count)
	{
		spin_unlock(&lock);
		printk("kernel: open fail! count = %d\n",count);
		return -EBUSY;
	}
	count++;
	spin_unlock(&lock);
	printk("kernel: open ok! count = %d\n",count);

	return 0;
}

static int test_drv_release(struct inode *inode, struct file *file)
{
	spin_lock(&lock);
	count--;
	spin_unlock(&lock);
	printk("kernel: release ok! count = %d\n",count);

	return 0;
}

static struct file_operations test_drv_fops = {
    .owner   =   THIS_MODULE,   
    .open    =   test_drv_open,     
	.release =	 test_drv_release,	   
};

int major;
static int test_drv_init(void)
{
	major = register_chrdev(0, "test_drv", &test_drv_fops);

	testdrv_class = class_create(THIS_MODULE, "testdrv");

	testdrv_class_dev = device_create(testdrv_class, NULL, MKDEV(major, 0), NULL, "locktest"); 

	spin_lock_init(&lock);

	printk("kernel: init ok!\n");
	
	return 0;
}

static void test_drv_exit(void)
{
	unregister_chrdev(major, "test_drv"); 

	device_destroy(testdrv_class,  MKDEV(major, 0));	
	
	class_destroy(testdrv_class);

	printk("kernel: exit ok!\n");
}

module_init(test_drv_init);
module_exit(test_drv_exit);

MODULE_LICENSE("GPL");  
MODULE_AUTHOR("hceng");  
MODULE_DESCRIPTION("Learn for lock");
