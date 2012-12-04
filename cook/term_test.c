#include <stdio.h>
#include <ctype.h>
#include "cook.h"
#include <sys/times.h>
#include <sys/time.h>

void reset(int* target, int* val, int* se)
{
	*target = -1;
	*val = 0;
	*se = -1;
}

double sec(void)
{
        struct timeval time;

        gettimeofday(&time, NULL);

	return  time.tv_usec;
}

int main(void)
{
	int	c;
	int	se;
	int 	target;
	int	val;
	int	gg;
	double	begin;
	double 	end;

	printf("Init\n");
	if (init("/dev/ttyUSB0") == -1) {
		printf("Could not init batch process.\n");
		destroy();
		return 1;
	}
	printf("Init done\n");

	printf("s:= se, g:= get\n"
		"i:=pump in\n "
		"m:=mixer\n"
		"h=heater\n"
		"c=cooler\n"
		"t=temp (only for get)\n"
		"l=level (only for get)\n"
		"v=tells that now comes an integer\n"
		"example: \"s i 10\" means se pump in to 10\n");


	val = 0;
	target = -1;
	se = -1;
	while (EOF != (c = getchar())) {
		switch (c) {
			case 'w':
				break;
			case 'f':
				for (int i = 0; i < 10000; i+=1) {
					set(COOLER, 255);
					printf("GET %d\n", get(TEMP));
				}
				break;
			case 's':
				se = 1;		
				break;
			case 'g':
				se = 0;
				break;
			case 'i':
				if (se) 
					target = IN_PUMP;
				else 
					target = IN_PUMP_RATE;
				break;
			case 'o':
				if (se)
					target = OUT_PUMP;
				else
					target = OUT_PUMP_RATE;
				break;
			case 'm':
				if (se) 
					target = MIXER;
				else 
					target = MIXER_RATE;
				break;
			case 'c':
				if (se) 
					target = COOLER;
				else 
					target = COOLER_RATE;
				break;
			case 'h': 
				if (se) 
					target = HEATER;
				else  
					target = HEATER_RATE;
				break;
			case 't':
				if (!se) 
					target = TEMP;
				 else {
					reset(&target, &val, &se);
					fprintf(stderr, "Can't set TEMP\n");
				}
				break;
			case 'l':
				if (!se) 
					target = LEVEL;
				 else {
					fprintf(stderr, "Can't set LEVEL\n");
					reset(&target, &val, &se);
				}
				break;
			case '\n':
				if (se && target != -1)  {
					begin = sec();
					set(target, val);
					end = sec();
					printf("set-time in micro: %lf\n", end - begin);
				} else if (target != -1) {
					begin = sec();
					gg = get(target);
					end = sec();
					printf("%d, get-time in micro: %lf\n", gg, end - begin);
					
				} else  {
					fprintf(stderr, "bad input\n");
					reset(&target, &val, &se);
				}

				val = 0;		
				se = -1;
				target = -1;
				break;
			case ' ':
				break;
			default:
				if (isdigit(c)) {
					val = 10*val + c - '0';
				}  else {
					printf("Expected an integer, it was not\n");
					reset(&target, &val, &se);
				}
		}


	}

	destroy();
	return 0;
}
