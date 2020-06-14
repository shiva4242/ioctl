#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/ioctl.h>
#include<string.h>
#include<fcntl.h>

#include"myioctl.h"

int main()
{
	char rbuf[20]={'\0'};
	char wbuf[20]="Hi";
	int fd,stat,len,op;
	fd=open("/dev/char_device",O_RDWR);
	if(fd<0)
	{
		printf("fail to open the file\n");
		exit(0);

	}
	printf("fd=%d\n",fd);
	/*read and dispaly the content */
	while(1)
	{
		printf("\tselect\n1.Read\n2.Write\n");
		scanf("%d",&op);

		switch(op)
		{
			case 1:
				ioctl(fd,RD,rbuf);
				printf("received data :%s\n",rbuf);
				break;
			case 2:
				printf("written  the data to driver:%s\n",wbuf);
				//scanf("%s",&wbuf);
				ioctl(fd,WR,wbuf);
				break;
			
		}
	}
	
}
