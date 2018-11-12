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
#include <linux/usb.h>
#include <linux/usb/video.h>

MODULE_AUTHOR("Freddy Hidalgo-Monchez");
MODULE_LICENSE("Dual BSD/GPL");

static ssize_t module_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
static int module_open(struct inode *inode, struct file *filp);
static int module_release(struct inode *inode, struct file *filp);
static int module_probe(struct usb_interface *intf, const struct usb_device_id *devid);

// struct table containing list of all the different kind of USB devices
// driver can accept
// makes sure the callback function in the USB driver is called
static struct usb_device_id pilote_usb_id [] = {
    {USB_DEVICE(0x046d, 0x08cc)},
    {}
};

MODULE_DEVICE_TABLE(usb, pilote_usb_id);

static struct usb_driver pilote_usb_driver = {
	.name	 = "myUSBModule",
    .id_table = pilote_usb_id,
    .probe = module_probe,
}; 

static struct file_operations myUSBModule_fops = {
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

static int module_probe(struct usb_interface *intf, const struct usb_device_id *devid){
    printk("PROBE");
    return 0;
}

static __init int pilote_init(void)	{

    int retval = usb_register(&pilote_usb_driver);

    if(retval){
        printk("usb_register failed. Error number %d", retval);
    }

    printk(KERN_WARNING "Pilote : Hello, world (Pilote)\n");
    return retval;
}

static void __exit pilote_exit(void){
    usb_deregister(&pilote_usb_driver);
    printk(KERN_ALERT "Pilote: Goodbye, cruel world\n");
}


module_init(pilote_init);
module_exit(pilote_exit);