#include "pilote_usb.h"

static DECLARE_WAIT_QUEUE_HEAD(waitq); 

unsigned int myStatus = 0;
unsigned int myLengthUsed = 0;
unsigned int myLength = 42666;
char myData[42666];
static int receivedData = 0;
int nbUrbs = 5;

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
    
    const struct usb_host_interface *interface;
    interface = intf->cur_altsetting;

    if(interface->desc.bInterfaceClass == CC_VIDEO && interface->desc.bInterfaceSubClass == SC_VIDEOSTREAMING){
        printk("Found interface");
        struct usb_device *dev = interface_to_usbdev(intf);
        struct my_pilote_usb *pilote_usb = NULL;

        pilote_usb = kmalloc(sizeof(struct my_pilote_usb), GFP_KERNEL);
        pilote_usb->dev = usb_get_dev(dev);

        usb_set_intfdata(intf, pilote_usb);
        usb_register_dev(intf, &class_driver);
        usb_set_interface(dev, 1, 4);

         // initialiser les urbs
        int i;
        for (i = 0; i < nbUrbs; ++i) {
            pilote_usb->myUrb[i] = NULL;
        }
    }

    return 0;
}

ssize_t module_read(struct file *file, char __user *buffer, size_t count, loff_t *f_pos) {
    struct usb_interface *interface = file->private_data;
    struct usb_device *udev = interface_to_usbdev(interface);
    struct my_pilote_usb *pilote_usb = usb_get_intfdata(interface);

    while(receivedData < nbUrbs){
        if (wait_event_interruptible(waitq, receivedData >= nbUrbs)){
			return -ERESTARTSYS;
		}
    }

    // store length used so we can return it at the end of the function
    int dataSentCount = myLengthUsed;
    int error_num = copy_to_user(buffer, myData, dataSentCount);

    if(error_num == 0){
        printk("SENT URB INFO\n");
    }else{
        printk(KERN_WARNING"Failed to read from urb %d\n", error_num);
        return -EFAULT;
    }

    int i;
    for (i = 0; i < nbUrbs; ++i) {

        // clean up
        usb_kill_urb(pilote_usb->myUrb[i]);
        usb_free_coherent(pilote_usb->dev, pilote_usb->myUrb[i]->transfer_buffer_length, pilote_usb->myUrb[i]->transfer_buffer, pilote_usb->myUrb[i]->transfer_dma);
        usb_free_urb(pilote_usb->myUrb[i]);
        pilote_usb->myUrb[i] = NULL;

        // reset variables used in callback
        receivedData = 0;
        myLengthUsed = 0;
    }

  return dataSentCount;
}

static void module_disconnect(struct usb_interface *intf){
    usb_set_intfdata(intf, NULL);
    usb_deregister_dev(intf, &class_driver);
}

long module_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {

    printk("ELE784 -> IOCTL cmd:%04X and arg:%lX\n", cmd, arg);
    struct usb_interface *interface = file->private_data;
    struct usb_device *udev = interface_to_usbdev(interface);

     
    static int data = 0x03;
	int16_t mesg = (int16_t) arg; // mesg is 2 bytes long
    int response = -1;

    static int buf[4];	// usb_control_msg() n'est pas bloquant
    buf[3] = (arg & 0xFF000000) >> 24;
    buf[2] = (arg & 0x00FF0000) >> 16;
    buf[1] = (arg & 0x0000FF00) >> 8;
    buf[0] = (arg & 0x000000FF);
    printk("ELE784 -> buf0:%08X buf1:%08X buf2:%08X buf3:%08X ", buf[0], buf[1], buf[2], buf[3]);

/*
extern int usb_control_msg(
	struct usb_device *dev, 
	unsigned int pipe,
	__u8 request, 
	__u8 requesttype, 
	__u16 value, 
	__u16 index,
	void *data, 
	__u16 size, 
	int timeout
);
*/

    switch (cmd) {  
		case IOCTL_GET:
            response = usb_control_msg(udev, usb_rcvctrlpipe(udev, 0),
			GET_CUR, USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE, (PU_BACKLIGHT_COMPENSATION_CONTROL  << 8), 0x0200, NULL, 2, 0);
            break;
        case IOCTL_SET:
            response = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),
			SET_CUR, USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE, (PU_BACKLIGHT_COMPENSATION_CONTROL  << 8), 0x0200, &mesg, 2, 0);
            break;
 		case IOCTL_STREAMON:
            response = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),0x0B, USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_INTERFACE, 0x0004, 0x0001, NULL, 0, 0);
            break;
        case IOCTL_STREAMOFF:
            response = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),0x0B, USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_INTERFACE, 0x0000, 0x0001, NULL, 0, 0);
            break;
        case IOCTL_PANTILT:
            response = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),0x01, USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE, 0x0100, 0x0900, &buf, 4, 0);
            break;
        case IOCTL_PANTILT_RESET:
            response = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),0x01, USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE, 0x0200, 0x0900, &data, 1, 0);
            break; 
        case IOCTL_GRAB:
            grab(interface, udev);
            response = 0;
            break; 
    }

    printk("ELE784 -> command response is: %d\n", response);

    return response;
}

void grab(struct usb_interface *intf, struct usb_device *dev){
    
    struct usb_host_interface *cur_altsetting = intf->cur_altsetting;
    struct usb_endpoint_descriptor endpointDesc = cur_altsetting->endpoint[0].desc;

    int nbPackets = 40;  // The number of isochronous packets this urb should contain			
    int myPacketSize = le16_to_cpu(endpointDesc.wMaxPacketSize);			
    int size = myPacketSize * nbPackets;

    struct my_pilote_usb *pilote_usb = usb_get_intfdata(intf);
    int i;

    for (i = 0; i < nbUrbs; ++i) {
      
        usb_free_urb(pilote_usb->myUrb[i]); // Pour �tre certain
        pilote_usb->myUrb[i] = usb_alloc_urb(nbPackets, GFP_KERNEL);
        
        if (pilote_usb->myUrb[i] == NULL) {
            printk(KERN_WARNING "could not create urb");		
            return -ENOMEM;
        }

        pilote_usb->myUrb[i]->transfer_buffer = usb_alloc_coherent(dev, size, GFP_KERNEL, &pilote_usb->myUrb[i]->transfer_dma);

        if (pilote_usb->myUrb[i]->transfer_buffer == NULL) {
            printk(KERN_WARNING "urb transfer is NULL");		
            usb_free_urb(pilote_usb->myUrb[i]);
            return -ENOMEM;
        }

        pilote_usb->myUrb[i]->dev = dev;
        pilote_usb->myUrb[i]->context = pilote_usb;
        pilote_usb->myUrb[i]->pipe = usb_rcvisocpipe(dev, endpointDesc.bEndpointAddress);
        pilote_usb->myUrb[i]->transfer_flags = URB_ISO_ASAP | URB_NO_TRANSFER_DMA_MAP;
        pilote_usb->myUrb[i]->interval = endpointDesc.bInterval;
        pilote_usb->myUrb[i]->complete = complete_callback;
        pilote_usb->myUrb[i]->number_of_packets = nbPackets;
        pilote_usb->myUrb[i]->transfer_buffer_length = size;

        int j;
        for (j = 0; j < nbPackets; ++j) {
            pilote_usb->myUrb[i]->iso_frame_desc[j].offset = j * myPacketSize;
            pilote_usb->myUrb[i]->iso_frame_desc[j].length = myPacketSize;
        }				
    }

    int ret;
    for(i = 0; i < nbUrbs; i++){
        if ((ret = usb_submit_urb(pilote_usb->myUrb[i], GFP_KERNEL)) < 0) {
            printk(KERN_WARNING "CANNOT SUBMIT URB");		
            return ret;
        }
    }

}

static void complete_callback(struct urb *urb){

	int ret;
	int i;	
	unsigned char * data;
	unsigned int len;
	unsigned int maxlen;
	unsigned int nbytes;
	void * mem;

	if(urb->status == 0){
		
		for (i = 0; i < urb->number_of_packets; ++i) {
			if(myStatus == 1){
				continue;
			}
			if (urb->iso_frame_desc[i].status < 0) {
				continue;
			}
			
			data = urb->transfer_buffer + urb->iso_frame_desc[i].offset;
			if(data[1] & (1 << 6)){
				continue;
			}
			len = urb->iso_frame_desc[i].actual_length;
			if (len < 2 || data[0] < 2 || data[0] > len){
				continue;
			}
		
			len -= data[0];
			maxlen = myLength - myLengthUsed ;
			mem = myData + myLengthUsed;
			nbytes = min(len, maxlen);
			memcpy(mem, data + data[0], nbytes);
			myLengthUsed += nbytes;
	
			if (len > maxlen) {				
				myStatus = 1; // DONE
			}
	
			/* Mark the buffer as done if the EOF marker is set. */
			if ((data[1] & (1 << 1)) && (myLengthUsed != 0)) {						
				myStatus = 1; // DONE
			}					
		}
	
		if (!(myStatus == 1)){				
			if ((ret = usb_submit_urb(urb, GFP_ATOMIC)) < 0) {
				printk(KERN_WARNING "");
			}
		}else{
			///////////////////////////////////////////////////////////////////////
			//  Synchronisation
			///////////////////////////////////////////////////////////////////////
            
            receivedData += 1;
            myStatus = 0;
            
            if(receivedData == nbUrbs){
                printk("all data has been transfered");
	            wake_up_interruptible(&waitq);
            }
		}			
	}else{
		printk(KERN_WARNING "ERROR IN CALLBACK");
	}
}

module_usb_driver(pilote_usb_driver);