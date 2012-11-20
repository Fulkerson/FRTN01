#include <termios.h>
#include <error.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 
#include <sys/ioctl.h>
#include <stdint.h>

#include "cook.h"

#define MORE_BIT		(0x80)
#define GET_VAL			(3 << 5)
#define IN_PUMP_RATE_BIT	(2)
#define OUT_PUMP_RATE_BIT	(3)



static int fd = -1;

/* 
	Before using this method, make sure that the the cooker is
	reseted after the cables are connected. The tank will die otherwise 
*/

int init(const char* path)
{
	struct termios  config = { 0 };

	fd = open(path, O_RDWR | O_NOCTTY | O_NDELAY);

	/* Solved some errors */	
	ioctl(fd, TCFLSH);

	if (-1 == fd) 
		return -1;

	if (!isatty(fd)) 
		return -1;
	
	if (tcsetattr(fd, TCSANOW, &config) < 0) 
		return -1;

	/* NO parity */
	config.c_cflag &= ~PARENB;
	config.c_cflag &= ~CSTOPB;
	config.c_cflag &= ~CSIZE;
	config.c_cflag |= CS8;
	/* Since PARENB is not set, then unset INPCK and ISTRIP */
	config.c_iflag &= ~(INPCK | ISTRIP);

	if (cfsetispeed(&config, B115200) < 0 || cfsetospeed(&config, B115200) < 0) 
		return -1;

	config.c_cflag |= (CLOCAL | CREAD);

	/* Raw input */
	config.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	config.c_oflag &= ~OPOST;

	/* Software flow control */
	config.c_iflag &= ~(IXON | IXOFF | IXANY);

	if (tcsetattr(fd, TCSANOW, &config) < 0) 
		return -1;

	return 1;
}

	
int get(enum read_target chan)
{
	unsigned char	data;
	unsigned char 	head[50] = { 0 };
	unsigned char*	buff;
	int		avail;
	int		value;
	unsigned char	c;
	int		i;
	
	data = chan;
	data |= GET_VAL;

	if (-1 == write(fd, &data, 1))
		return -1;
	
	c = MORE_BIT;
	buff = head;
	avail = 0;
	while (c & MORE_BIT) {	
		while (!avail)
			ioctl(fd, FIONREAD, &avail);
	
		if (-1 == read(fd, buff, avail)) 
			return -1;
	
		printf("values:\n");
		for (i = 0; i < avail; i+=1)
			printf("%d\n", buff[i]);

		c = buff[avail - 1];
		buff = &buff[avail];
		avail = 0;
	}

	ioctl(fd, TCFLSH);

	value = 0;
	for (i = 0; head[i] & MORE_BIT; i+=1) 
		value = (value << 7) | (head[i] & 0x7f);
	
	printf("valu: %d\n", value);
	return value;
} 
int set(enum set_target target, int value) 
{
	unsigned char 	data[50];	
	int		i;

	i = 0;
	do {
		data[i] = MORE_BIT;
		data[i] |= (value & 0x7f);
		value = value >> 7;
		i+=1;
	} while (value > 0 && i < 50);

	data[i] = target;
	i+=1;

	if (-1 == write(fd, data, i))
		return -1;

	return 1;
}
int destroy(void)
{
	return close(fd);
}


