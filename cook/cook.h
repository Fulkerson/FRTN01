
enum channel {
	NILL, TEMP, LEVEL, IN_PUMP_RATE, OUT_PUMP_RATE, HEATER_RATE, 
	MIXER_RATE, COOLER_RATE
};

/* Methods returns false if they fail */

int get(enum channel);				/* Get current value of target channel */
	
int set(enum channel, int value);		/* Set a value to target channel */

int init(const char*);				/* Setup communication */

int destroy(void);				/* Close communication */


