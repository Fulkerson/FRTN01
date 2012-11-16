#include <assert.h>
#include "cook.c"

int main(void)
{
	assert(1 == init("/dev/ttyUSB0"));

	printf("tmp: %d", get(TEMP));
	
	assert(-1 != destroy());
	return 0;
}
