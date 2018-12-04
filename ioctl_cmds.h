#ifndef IOCTL_H
#define IOCTL_H

#include <linux/ioctl.h>


#define MAJOR_NUM 180

#define IOCTL_GET 						_IO(MAJOR_NUM, 0x10)
#define IOCTL_SET						_IO(MAJOR_NUM, 0x20)
#define IOCTL_STREAMON 			_IO(MAJOR_NUM, 0x30)
#define IOCTL_STREAMOFF 			_IO(MAJOR_NUM, 0x40)
#define IOCTL_GRAB 					_IO(MAJOR_NUM, 0x50)
#define IOCTL_PANTILT 				_IO(MAJOR_NUM, 0x60)
#define IOCTL_PANTILT_RESET 	_IO(MAJOR_NUM, 0x70)

#endif