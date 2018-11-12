#include <generated/autoconf.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/syscalls.h>
#include <linux/types.h>
#include <linux/stat.h> 
#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/cdev.h>
#include <linux/spinlock.h>
#include <linux/cred.h>
#include <linux/semaphore.h>

MODULE_AUTHOR("Freddy Hidalgo-Monchez");
MODULE_LICENSE("Dual BSD/GPL");

static ssize_t module_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
static int module_open(struct inode *inode, struct file *filp);
static int module_release(struct inode *inode, struct file *filp);

static struct file_operations myModule_fops = {
	.owner 	 = THIS_MODULE,
	.read	 = module_read,
	.open	 = module_open,
	.release = module_release,
};



static int  pilote_init (void);
static void pilote_exit (void);

static int module_open(struct inode *inode, struct file *filp) {
    printk(KERN_WARNING"Pilote OPEN : Hello, world\n");
	return 0;
}


static int module_release(struct inode *inode, struct file *filp) {
    printk(KERN_WARNING"Pilote RELEASE : Hello, world\n");
    return 0;
}


static __init int pilote_init(void)	{
    printk(KERN_WARNING "Pilote : Hello, world (Pilote)\n");
    return 0;
}

static void __exit pilote_exit(void){
    printk(KERN_ALERT "Pilote: Goodbye, cruel world\n");
}


module_init(pilote_init);
module_exit(pilote_exit);