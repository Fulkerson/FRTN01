#include <assert.h>
#include "cook.c"

int main(void)
{
	assert(1 == init("/dev/ttyUSB0"));

	printf("tmp: %d\n", get(TEMP));
//	set(IN_PUMP_RATE, 17);
	
	assert(-1 != destroy());
	return 0;
}
