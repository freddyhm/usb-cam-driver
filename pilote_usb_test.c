#include <fcntl.h>      /* open */ 
#include <unistd.h>     /* exit */
#include <sys/ioctl.h> 
#include <stdio.h>

#define MAJOR_NUM 180
#define IOCTL_PANTILT _IO(MAJOR_NUM, 0x60)

int main(void){
    int file_desc, ret_val;
    file_desc = open("/dev/ele784-5", 0);
    if (file_desc < 0) {
        printf ("Can't open device file\n");
        exit(-1);
    }

    ret_val = ioctl(file_desc, IOCTL_PANTILT);

    if (ret_val < 0) {
        printf ("ioctl_set_msg failed:%d\n", ret_val);
        exit(-1);
    }


    close(file_desc); 

    return 0;
}