#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define CONFIG_CHAN	(31)

typedef enum { cmd_clear_bit, cmd_set_bit,
cmd_read_bit, cmd_read_chan } command;

void error(const char* msg)
{
	fprintf(stderr, msg);
	printf("\n");
	exit(1);
}

void config(int fd)
{
	struct termios config;	

	if (-1 == fd)
		error("file descriptor is bad");

	if (!isatty(fd)) 
		error("atty failed");

	if (tcgetattr(fd, &config) < 0)
		error("tcgetattr failed");

	config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK
				| INPCK | ISTRIP | IXON);	

	options.c_vlag

}

void read_cook(unsigned char* buff, FILE* fp)
{
	int	c;
	int	i;

	c = fgetc(fp);
	while (c != '\0' && c != EOF && i < 100) {
		printf("%c\n", c);
		buff[i] = c;
		i += 1;
		c = fgetc(fp);
	}
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

	printf("\n");
}

int main(int args, const char** argv)
{
	int		fd;
	int		c;
	unsigned char	data;
	unsigned char 	buff[100];

	printf("writing / reading do: %s\n", argv[1]);	

	fd = open(argv[1], O_RDWR | O_NOCTTY | O_NDELAY);

	config(fileno(fp));

	/* a = get_temperature */	
	while (EOF != (c = getchar())) {
		data = 0;
		switch (c) {
			case 'a':
				/* set cmd to get */
				data |= cmd_read_chan << 5;
				/* get config */
				data |= CONFIG_CHAN;
				print_byte(data);
				fputc(data, fp);

				read_cook(buff, fp);
				printf("after read\n");
			break;
		}
	}

	close(fd);
	return 0;
}
