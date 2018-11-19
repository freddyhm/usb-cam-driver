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
#include "usbvideo.h"

#define MAJOR_NUM 180
#define IOCTL_PANTILT _IO(MAJOR_NUM, 0x60)

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

static int module_open(struct inode *inode, struct file *file) {
    struct usb_interface *intf;
    int subminor;

    printk(KERN_WARNING "ELE784 -> Open\n");

    subminor = iminor(inode);
    intf = usb_find_interface(&pilote_usb_driver, subminor);
    if (!intf) {
        printk(KERN_WARNING "ELE784 -> Open: Ne peux ouvrir le peripherique\n");
        return -ENODEV;
    }

    file->private_data = intf;
    printk(KERN_WARNING"Pilote OPEN : Hello, world\n");
	return 0;
}

static int module_probe(struct usb_interface *intf, const struct usb_device_id *devid){

    printk("PROBE!!");
    const struct usb_host_interface *interface;
    interface = intf->cur_altsetting;

    if(interface->desc.bInterfaceClass == CC_VIDEO && interface->desc.bInterfaceSubClass == SC_VIDEOSTREAMING){
        printk("Found interface");
        struct usb_device *dev = interface_to_usbdev(intf);
        struct my_pilote_usb *skeldev = NULL;

        skeldev = kmalloc(sizeof(struct my_pilote_usb), GFP_KERNEL);
        skeldev->dev = usb_get_dev(dev);

        usb_set_intfdata(intf, skeldev);
        usb_register_dev(intf, &class_driver);
        usb_set_interface(dev, 1, 4);

         printk("interface set");
    }

   
    return 0;
}

ssize_t module_read(struct file *file, char __user *buffer, size_t count, loff_t *f_pos) {
  struct usb_interface *interface = file->private_data;
  struct usb_device *udev = interface_to_usbdev(interface);

  return 0;
}

static void module_disconnect(struct usb_interface *intf){
    usb_set_intfdata(intf, NULL);
    usb_deregister_dev(intf, &class_driver);
}

long module_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {

    printk("in IOCTL");
  struct usb_interface *interface = file->private_data;
  struct usb_device *udev = interface_to_usbdev(interface);

  switch (cmd) {
      
    case IOCTL_PANTILT:
        printk("Received a tilt command");
  }

  return 0;
}


/*

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
}*/


//module_init(pilote_init);
//module_exit(pilote_exit);

// replaces init and exit with boilerplate code 
module_usb_driver(pilote_usb_driver);