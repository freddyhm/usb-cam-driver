#include <fcntl.h>      /* open */ 
#include <unistd.h>     /* exit */
#include <sys/ioctl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ioctl_cmds.h"
#include "dht_data.h"

/*				Function declaration				*/
int takePicture();
int moveLeft();
int moveRight();
int moveUp();
int moveDown();
int resetPosition();
int setParam();
int getParam();
int getGrab();

/*				Global Variable				*/
int file_desc;

/*				main function				*/
int main(void){
	char *buffer;
	size_t bufsize = 12;
	size_t command;
	bool to_stay = true;
	int readWriteState;
	
	/**********			Open Driver			**********/
    file_desc = open("/dev/ele784", 0);
    if (file_desc < 0) {
        printf ("Can't open device file\n");
        exit(-1);
    }

	/**********			User Command			**********/
	do
	{
		buffer = (char *)malloc(bufsize * sizeof(char));

		if( buffer == NULL)
			perror("Unable to allocate buffer\n");

		printf("\nType 'photo, get, set, mleft, mright, mup, mdown, reset, exit or help: \n");
		command = getline(&buffer,&bufsize,stdin);
		//printf("%zu characters were read.\n",command);
		//printf("You typed: %s \n",buffer);
		
		if (strcmp(buffer, "photo\n") == 0) 
		{
			printf("Taking photo \n");	
			if (takePicture())
				printf("Error photo \n");	
		} 
		else if (strcmp(buffer, "get\n") == 0) 
		{
			printf("Fetching parameter \n");	
			if (getParam())
				printf("Error fetching parameter \n");	
		} 
		else if (strcmp(buffer, "set\n") == 0) 
		{
			printf("Writting parameter\n");	
			if (setParam())
				printf("Error writting parameter \n");	
		} 
		else if (strcmp(buffer, "mleft\n") == 0)
		{
		 	printf("moving left \n");
			if (moveLeft())
				printf("Error moving left \n");	
		}
		else if (strcmp(buffer, "mright\n") == 0)
		{
			printf("moving right \n");
			if (moveRight())
				printf("Error moving right \n");		
		}
		else if (strcmp(buffer, "mup\n") == 0)
		{
			printf("moving up \n");
			if (moveUp())
				printf("Error moving up \n");		
		}
		else if (strcmp(buffer, "mdown\n") == 0)
		{
			printf("moved down\n");
			if (moveDown())
				printf("Error move right \n");		
		}
		else if (strcmp(buffer, "reset\n") == 0)
		{
			printf("resetting position\n");
			if (resetPosition())
				printf("Error in resetting position\n");		
		}
		else if (strcmp(buffer, "exit\n") == 0)
		{
			to_stay = false;
				printf("Exitting...\n");		
		}
		else if (strcmp(buffer, "help\n") == 0)
		{
				printf("Available camera operation:\n\r"
						"    - 'photo' (take a picture with the camera)\n\r"
						"    - 'get' (read a parameter of the cameras function)\n\r"
						"    - 'set' (set a parameter of the cameras function)\n\r"
						"    - 'mleft' (move camera to the left)\n\r"
						"    - 'mright' (move camera to the right)\n\r"
						"    - 'mup' (move camera to the up)\n\r"
						"    - 'mdown' (move camera to the down)\n\r"
						"    - 'reset' (return the camera position to default)\n\r"
						"    - 'exit' (exit program)\n\r");		
		}
		else 
		{
			printf("Wrong command. See help for more details\n");
		}

	}while(to_stay);

    close(file_desc); 

    return 0;
}

int turnStreamOn(){

	int ret_val = ioctl(file_desc, IOCTL_STREAMON);

    if (ret_val < 0) {
        printf ("ioctl_turn_stream_on_msg failed:%d\n", ret_val);
        return (-1);
    }
	
	return EXIT_SUCCESS;
}

int takePicture()
{
    FILE *foutput;
	unsigned char * inBuffer;
	unsigned char * finalBuf;
	inBuffer = malloc((42666)* sizeof(unsigned char));
	finalBuf = malloc((42666 * 2)* sizeof(unsigned char));
	if((inBuffer == NULL) || (finalBuf == NULL)){
		return -1;
	}
	
	// Etape #1
	foutput = fopen("testPic.jpg", "wb");

	if(foutput != NULL){
		// Etape #2
		turnStreamOn();
		// Etape #3
		getGrab();
		// Etape #4
		int fd;
		fd = open("/dev/ele784", O_RDONLY);

		if(fd < 0){
			printf("Erreur d'ouverture = %d\n", fd);
			return -1;
		}

		int mySize = read(fd, inBuffer, 42666);
		if (mySize < 0){
			printf("Failed to read!");
		}

		// Etape #5
		int ret_val = ioctl(file_desc, IOCTL_STREAMOFF);

		if (ret_val < 0) {
			printf ("ioctl_set_msg failed:%d\n", ret_val);
			return (-1);
		}

		// Etape #6
		memcpy (finalBuf, inBuffer, HEADERFRAME1);
		memcpy (finalBuf + HEADERFRAME1, dht_data, DHT_SIZE);
		memcpy (finalBuf + HEADERFRAME1 + DHT_SIZE, inBuffer + HEADERFRAME1, (mySize -HEADERFRAME1));
		
		// Etape #7
		fwrite (finalBuf, mySize + DHT_SIZE, 1, foutput);
		
		// Etape #8
		fclose(foutput);
	}else{
		printf("CANNOT CREATE IMAGE");
	}

	return EXIT_SUCCESS;
}


int getGrab()
{
	int ret_val = ioctl(file_desc, IOCTL_GRAB);
	if (ret_val < 0) {
		printf ("ioctl_grab failed:%d\n", ret_val);
		return (-1);
	}

	return EXIT_SUCCESS;
}

int getParam()
{
    int ret_val = ioctl(file_desc, IOCTL_GET);

    printf("IOCTL returned: %d\n", ret_val);

    if (ret_val < 0) {
        printf ("ioctl_get_param failed:%d\n", ret_val);
        return (-1);
    }
	return EXIT_SUCCESS;
}

int setParam()
{
    int ret_val = ioctl(file_desc, IOCTL_SET, 0x01);

    printf("IOCTL returned: %d\n", ret_val);

    if (ret_val < 0) {
        printf ("ioctl_set_param failed:%d\n", ret_val);
        return (-1);
    }
	return EXIT_SUCCESS;
}

int moveLeft()
{
	unsigned long move_cmd;
	move_cmd = 0x00000080;
    int ret_val = ioctl(file_desc, IOCTL_PANTILT, move_cmd);

    printf("IOCTL returned: %d\n", ret_val);

    if (ret_val < 0) {
        printf ("ioctl_move_left failed:%d\n", ret_val);
        return (-1);
    }
	return EXIT_SUCCESS;
}

int moveRight()
{
	unsigned long move_cmd;
	move_cmd= 0x0000FF80;
    int ret_val = ioctl(file_desc, IOCTL_PANTILT, move_cmd);

    printf("IOCTL returned: %d\n", ret_val);

    if (ret_val < 0) {
        printf ("ioctl_move_right failed:%d\n", ret_val);
        return (-1);
    }
	return EXIT_SUCCESS;
}

int moveUp()
{
	unsigned long move_cmd;
	move_cmd = 0xFF800000;
    int ret_val = ioctl(file_desc, IOCTL_PANTILT, move_cmd);

    printf("IOCTL returned: %d\n", ret_val);

    if (ret_val < 0) {
        printf ("ioctl_move_up failed:%d\n", ret_val);
        return (-1);
    }
	return EXIT_SUCCESS;
}

int moveDown()
{
	unsigned long move_cmd;
	move_cmd = 0x00800000;
    int ret_val = ioctl(file_desc, IOCTL_PANTILT, move_cmd);

    printf("IOCTL returned: %d\n", ret_val);

    if (ret_val < 0) {
        printf ("ioctl_move_down failed:%d\n", ret_val);
        return (-1);
    }
	return EXIT_SUCCESS;
}

int resetPosition()
{
    int ret_val = ioctl(file_desc, IOCTL_PANTILT_RESET);

    printf("IOCTL returned: %d\n", ret_val);

    if (ret_val < 0) {
        printf ("ioctl_reset_position failed:%d\n", ret_val);
        return (-1);
    }
	return EXIT_SUCCESS;
}
