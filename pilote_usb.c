#include "pilote_usb.h"

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

    printk("in IOCTL!! %d", cmd);
    struct usb_interface *interface = file->private_data;
    struct usb_device *udev = interface_to_usbdev(interface);

     
    int data = 0x03;
    int response;

    int buf[4];
    buf[2] = 0x80;
    buf[3] = 0xFF;

    switch (cmd) {
        case IOCTL_PANTILT:
            response = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),0x01, USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE, 0x0100, 0x0900, &buf, 4, 0);
            break;
        case IOCTL_PANTILT_RESET:
            response = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),0x01, USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE, 0x0200, 0x0900, &data, 1, 0);
            break;  
        case IOCTL_STREAMON:
            response = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),0x0B, USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_INTERFACE, 0x0004, 0x0001, NULL, 0, 0);
            break;
        case IOCTL_STREAMOFF:
            response = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),0x0B, USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_INTERFACE, 0x0000, 0x0001, NULL, 0, 0);
            break;
    }

    printk("tilt command response: %d", response);

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