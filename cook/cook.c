#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define CONFIG_CHAN	(31)
#define TEMP_CHAN	(1)

typedef enum { cmd_clear_bit, cmd_set_bit,
cmd_read_bit, cmd_read_chan } command;

void error(const char* msg)
{
	fprintf(stderr,"%s\n", msg);
	exit(1);
}

void config(int fd)
{
	struct termios  config;

	if (-1 == fd)
		error("file descriptor is bad");

	if (!isatty(fd)) 
		error("atty failed");
	//
	// Input flags - Turn off input processing
	// convert break to null byte, no CR to NL translation,
	// no NL to CR translation, don't mark parity errors or breaks
	// no input parity check, don't strip high bit off,
	// no XON/XOFF software flow control
	//


    	tcgetattr(fd, &config);
	
	//config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL |
			   //INLCR | PARMRK | INPCK | ISTRIP | IXON);
	//
	// Output flags - Turn off output processing
	// no CR to NL translation, no NL to CR-NL translation,
	// no NL to CR translation, no column 0 CR suppression,
	// no Ctrl-D suppression, no fill characters, no case mapping,
	// no local output processing
	//
	// config.c_oflag &= ~(OCRNL | ONLCR | ONLRET |
	//                   options  ONOCR | ONOEOT| OFILL | OLCUC | OPOST);

	config.c_oflag = 0;
	//
	// No line processing:
	// echo off, echo newline off, canonical mode off, 
	// extended input processing off, signal chars off
	//
	config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
	//
	// Turn off character processing
	// clear current char size mask, no parity checking,
	// no output processing, force 8 bit input
	//
	config.c_cflag |= (CLOCAL | CREAD);
	//config.c_cflag &= ~(CSIZE | PARENB);
	//config.c_cflag |= CS8;
	//
	// One input byte is enough to return from read()
	// Inter-character timer off
	//
	config.c_cc[VMIN]  = 1;
	config.c_cc[VTIME] = 0;
	//
	// Communication speed (simple version, using the predefined
	// constants)
	//
	if(cfsetispeed(&config, B9600) < 0 || cfsetospeed(&config, B9600) < 0) 
		error("Yes, this is dog");
	//
	// Finally, apply the configuration
	//
	if(tcsetattr(fd, TCSAFLUSH, &config) < 0) 
		error("Yes, this is not dog");

}

void print_byte(unsigned char data)
{
	int i;

	for (i = 0; i < 8; i+=1) {
		if (data & (1 << i))
			printf("1");
		else 	
			printf("0");
	}
	printf("\r\n");
	
}
void read_cook(unsigned char* buff, int fp)
{
	int	c;
	int	i;

	if(!(c = read(fp, buff, 1)))
		error("Read failed");

	print_byte(c);
	while (c != 0 && i < 100) {
		print_byte(c);
		buff[i] = c;
		i += 1;
	if(!(c = read(fp, buff + i, 1)))
		error("Read failed");
	}
}


int main(int args, const char** argv)
{
	int		fd;
	int		c;
	unsigned char	data;
	unsigned char	in_data;
	unsigned char 	buff[100];

	printf("writing / reading do: %s\n", argv[1]);	

	fd = open(argv[1], O_RDWR | O_NOCTTY | O_NDELAY);
	config(fd);



	/* a = get_temperature */	
	while ('q' != (c = getchar())) {
		data = 0;
		switch (c) {
			case 'a':
				data |= cmd_read_chan << 5;
				data |= CONFIG_CHAN;
				print_byte(data);
				data = 0;
				data = 495;
				if (!write(fd, &data, 1))
					error("Write failed");
				read_cook(buff, fd);

				print_byte(in_data);
			break;
			case 'b':
				/* set cmd to get */
				data |= cmd_read_chan << 5;
				/* get config */
				data |= TEMP_CHAN;
				if (!write(fd, &data, 1))
					error("Write failed");
				read_cook(buff, fd);

				print_byte(in_data);
			break;
		}
	}

	close(fd);
	return 0;
}
