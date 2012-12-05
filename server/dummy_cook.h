#ifndef DUMMY_COOK_H
#define DUMMY_COOK_H
#include <iostream>
#include <ctime>

static int level;
static int in_pump;
static int out_pump;
static timespec timestate;

static void update_state()
{

    timespec cur_t;
    clock_gettime(CLOCK_MONOTONIC, &cur_t);

    int secdiff = cur_t.tv_sec - timestate.tv_sec;
    int nanodiff = cur_t.tv_nsec - timestate.tv_nsec;

    double delta = secdiff + nanodiff / 1000000000.0;
   
    double vin = 0.2 * in_pump;
    double vout = 0.2 * out_pump;
    level = level + (vin - vout) * delta;
    if (level < 0) {
        level = 0;
    }
    timestate = cur_t;
}

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


/* Get current value of target channel*/
int get(enum read_target target)
{
    update_state();

    switch (target) {
        case LEVEL:
            return level;
            break;
        default:
            return 0;
    }
}

/* Set a value to target channel */
int set(enum set_target target, int value)
{
    update_state();
    switch (target) {
        case IN_PUMP:
            in_pump = value;
            break;
        case OUT_PUMP:
            out_pump = value;
            break;
    }
    return 0;
}

/* Setup communication */
int init(const char* tty)
{
    std::cout << "Dummy communication setup on " << tty << std::endl;
    clock_gettime(CLOCK_MONOTONIC, &timestate);
    return 0;
}

/* Close communication */
int destroy(void)
{
    std::cout << "Dummy connection destroyed" << std::endl;
    return 0;
}

#endif
