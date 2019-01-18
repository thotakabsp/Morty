/* 
 * amptest.c
 * LM48100 audio amplifier test program using i2c interface
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <errno.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>


#define LM48100_ADDR	0x7c
#define I2C_DEV		"/dev/i2c-0"

int main(int argc, char *argv[])
{
	char buf[10];
	int steps = 0;
	int status, option;
	int fd, value = 0;

	value = atoi(argv[1]);
	if(value > 10 || value < 1)
	{
		printf(" value ( %d ) not in range (1 ~ 10) \n", (value));
		return 0;
	}
	value += 21;
	// OPENING I2C DEVICE
	fd = open(I2C_DEV, O_RDWR);
	if (fd < 0) {
		printf("ERROR: open(%d) failed\n", fd);
		return -1;
	}
	printf("\nSUCCESS: open(%d) passed\n", fd);

	status = ioctl(fd, I2C_SLAVE, LM48100_ADDR);
	if (status < 0)
	{
		printf("ERROR: ioctl(fd, I2C_SLAVE, 0x%02X) failed\n", LM48100_ADDR);
		close(fd);
		return -1;
	}
	printf("\nSUCCESS: ioctl(fd, I2C_SLAVE, 0x%02X>>1) passed\n", LM48100_ADDR);
	// 0 MODE CONTROL 0 0 0 POWER_ON INPUT_2 INPUT_1 0 0
	buf[0] = 0x1C; // 0 0 0 1 	 1 	 1 	 0 0 
	buf[1] = 0x3C;
	buf[2] = 0x40;
//	buf[3] = 0x7F;
//	buf[4] = 0x9F;
//	for ( steps = 0x10; steps <0x1B; steps++)
//	{
	printf("<<<<<<<<<<<<<<<<<<< value = %d >>>>>>>>>>>>>>>\n", value);
		buf[3] = (0x60 | value);
		buf[4] = (0x80 | value);
	if (write(fd, buf, 5) != 5) {
		printf("\n 0x%02X>>1 error\n", LM48100_ADDR);
		/* ERROR HANDLING: i2c transaction failed */
		close(fd);
		return -1;
//	}
//		sleep(1);
	}
	close(fd);
	return 0;
}
