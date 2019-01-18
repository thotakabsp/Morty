/* Test program for watchdog */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
	int fd = -1;

	if((fd = open("/dev/tfswatchdog", O_WRONLY))>=0)
	{
		while(1){
			write(fd, "\0", 1);
			sleep(1);
		}
	}
	else{
		printf("unable to open watchdog \n");
	}


	return 0;
}
