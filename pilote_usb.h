#include <generated/autoconf.h>
#include <linux/slab.h>
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
#include <linux/slab.h>
#include "usbvideo.h"

#define MAJOR_NUM 180
#define IOCTL_STREAMON _IO(MAJOR_NUM, 0x30)
#define IOCTL_STREAMOFF _IO(MAJOR_NUM, 0x40)
#define IOCTL_PANTILT _IO(MAJOR_NUM, 0x60)
#define IOCTL_PANTILT_RESET _IO(MAJOR_NUM, 0x70)

#define DEV_MINOR       0x00
#define DEV_MINORS      0x01

MODULE_AUTHOR("Freddy Hidalgo-Monchez");
MODULE_LICENSE("Dual BSD/GPL");

static ssize_t module_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
static int module_open(struct inode *inode, struct file *file);
static int module_probe(struct usb_interface *intf, const struct usb_device_id *devid);
static void module_disconnect(struct usb_interface *intf);
long module_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
//static int  pilote_init (void);
//static void pilote_exit (void);

// struct table containing list of all the different kind of USB devices
// driver can accept
// makes sure the callback function in the USB driver is called
static struct usb_device_id pilote_usb_id [] = {
    {USB_DEVICE(0x046d, 0x0994)},
    {}
};

MODULE_DEVICE_TABLE(usb, pilote_usb_id);

static struct usb_driver pilote_usb_driver = {
	.name	 = "ele784",
    .id_table = pilote_usb_id,
    .probe = module_probe,
    .disconnect = module_disconnect,
}; 

static struct file_operations fops = {
	.owner 	 = THIS_MODULE,  
	.read	 = module_read,
	.open	 = module_open,
    .unlocked_ioctl = module_ioctl,
};

static struct my_pilote_usb{
    struct usb_host_interface *interface;
    struct usb_device *dev;
}pilote_usb_data;

static struct usb_class_driver class_driver = {
  .name = "ele784-%d",
  .fops = &fops,
  .minor_base = DEV_MINOR,
};
