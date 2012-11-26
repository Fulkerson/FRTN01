#include <termios.h>
#include <error.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 
#include <sys/ioctl.h>
#include <stdint.h>

#include <sys/times.h>
#include <sys/time.h>
#include "cook.h"

#define MORE_BIT		(0x80)
#define GET_VAL			(3 << 5)

static int fd = -1;

/* 
	Before using this method, make sure that the the cooker is
	reseted after the cables are connected. The tank will die otherwise 
*/

int init(const char* path)
{
	struct termios  config = { 0 };

	fd = open(path, O_RDWR | O_NOCTTY);// | O_NDELAY);

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

	config.c_cc[VMIN]  = 0;
    	config.c_cc[VTIME] = 10;

	if (tcsetattr(fd, TCSANOW, &config) < 0) 
		return -1;

	return 1;
}

int get(enum read_target chan)
{
	unsigned char	data;
	unsigned char 	head[50] = { 0 };
	unsigned char*	buff;
	int		value;
	unsigned char	c;
	int		i;

	data = chan;
	data |= GET_VAL;

	if (-1 == write(fd, &data, 1))
		return -1;
	
	c = MORE_BIT;
	buff = head;
	while (c & MORE_BIT) {	
		if (-1 == read(fd, buff, 1)) 
			return -2;

		c = buff[0];
		buff = &buff[1];	
	}

	value = 0;
	for (i = 0; head[i] & MORE_BIT; i+=1) 
		value = (value << 7) | (head[i] & 0x7f);

	value = (value << 2) | ((head[i] & 0x60) >> 5);	
	return value;
} 



/* At most 1 byte can be set, i.e 0-255 */
int set(enum set_target target, int value) 
{
	unsigned char 	data[2];	

	data[1] = target;
	data[1] |= (value & 3) << 5;
	value = value >> 2;
	data[0] = MORE_BIT;
	data[0] |= value;

	if (-1 == write(fd, data, 2))
		return -1;

	return 1;
}
int destroy(void)
{
	return close(fd);
}
