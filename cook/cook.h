
enum read_target {
	TEMP , 
	LEVEL, 
	IN_PUMP_RATE, 
	OUT_PUMP_RATE, 
	HEATER_RATE, 
	MIXER_RATE, 
	COOLER_RATE
};

enum set_target {
	HEATER, 
	COOLER, 
	IN_PUMP, 
	OUT_PUMP, 	
	MIXER
};


int get(enum read_target);			/* Get current value of target channel */

int set(enum set_target, int value);		/* Set a value to target channel */

int init(const char*);				/* Setup communication */

int destroy(void);				/* Close communication */


