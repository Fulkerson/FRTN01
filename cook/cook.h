
enum read_target {
	NILL, TEMP, LEVEL, IN_PUMP_RATE, OUT_PUMP_RATE, HEATER_RATE, 
	MIXER_RATE, COOLER_RATE
};

enum set_target {
	HEATER, COOLER, IN_PUMP, OUT_PUMP, MIXER
};

/* Methods returns false if they fail */

int get(enum channel);				/* Get current value of target channel */

/* char value may not use the most significant bit */	
int set(enum set_target, char value);		/* Set a value to target channel */

int init(const char*);				/* Setup communication */

int destroy(void);				/* Close communication */


