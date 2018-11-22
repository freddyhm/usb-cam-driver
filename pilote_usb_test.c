#include <fcntl.h>      /* open */ 
#include <unistd.h>     /* exit */
#include <sys/ioctl.h> 
#include <stdio.h>

#define MAJOR_NUM 180
#define IOCTL_STREAMON _IO(MAJOR_NUM, 0x30)
#define IOCTL_STREAMOFF _IO(MAJOR_NUM, 0x40)
#define IOCTL_PANTILT _IO(MAJOR_NUM, 0x60)
#define IOCTL_PANTILT_RESET _IO(MAJOR_NUM, 0x70)

int main(void){
    int file_desc, ret_val;
    file_desc = open("/dev/ele784-4", 0);
    if (file_desc < 0) {
        printf ("Can't open device file\n");
        exit(-1);
    }

    ret_val = ioctl(file_desc, IOCTL_STREAMON);

    printf("%d", ret_val);

    if (ret_val < 0) {
        printf ("ioctl_set_msg failed:%d\n", ret_val);
        exit(-1);
    }


    close(file_desc); 

    return 0;
}