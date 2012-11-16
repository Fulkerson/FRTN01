#include <termios.h>
#include <error.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 
#include <sys/ioctl.h>

#include "cook.h"

#define CONFIG_CHAN	(31)
#define ERROR_bit	(0x80)

enum command { 
	cmd_clear_bit, 
	cmd_set_bit,
	cmd_read_bit, 
	cmd_read_chan 
};

static int fd = -1;
static struct termios  config;

int init(const char* path)
{

	fd = open(path, O_RDWR | O_NOCTTY | O_NDELAY);

	if (-1 == fd) 
		return -1;

	if (!isatty(fd)) 
		return -1;

    	tcgetattr(fd, &config);

	/* NO parity */
	config.c_cflag &= ~PARENB;
	config.c_cflag &= ~CSTOPB;
	config.c_cflag &= ~CSIZE;
	config.c_cflag |= CS8;


	if (cfsetispeed(&config, B115200) < 0 || cfsetospeed(&config, B115200) < 0) 
		return -1;

	config.c_cflag |= (CLOCAL | CREAD);

	config.c_oflag &= ~OPOST;

	if (tcsetattr(fd, TCSAFLUSH, &config) < 0) 
		return -1;

	return 1;
}

int get(enum channel chan)
{
	unsigned char	data;
	unsigned char 	ret[50] = { 0 };
	int		avail = 0;
	
	data = 0;
	data |= 31;
	data |= 3 << 5;

	if (-1 == write(fd, &data, 1))
		printf("write failed\n");

	
	while (!avail) {	
		ioctl(fd, FIONREAD, &avail);
	}


	if (-1 == read(fd, ret, 50))
		perror(""); 

	for (int i = 0; i < 50; i+=1)
		printf("%d\n", ret[i]);

	return ret[0];
}

int set(enum channel chan, int value)
{

	return 0;
}

int destroy(void)
{
	return close(fd);
}


