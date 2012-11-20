#include <assert.h>
#include "cook.c"

int main(void)
{
	int	i;

	assert(1 == init("/dev/ttyUSB0"));

	assert(-1 != set(IN_PUMP, 10));
	assert(-1 != set(OUT_PUMP, 10));
	assert(-1 != set(MIXER, 10));
	assert(-1 != set(HEATER, 10));
//	assert(-1 != set(COOLER, 1));

	//assert(5 == get(HEATER_RATE));
	//assert(5 == get(COOLER_RATE));
	
	//assert(10 == get(MIXER_RATE));
	//assert(10 == get(OUT_PUMP_RATE));
	//assert(10 == get(IN_PUMP_RATE));
	

	assert(-1 != set(IN_PUMP, 0));
	assert(-1 != set(OUT_PUMP, 0));
	assert(-1 != set(MIXER, 0));
	assert(-1 != set(HEATER, 0));
	
	assert(-1 != destroy());

	return 0;
}
