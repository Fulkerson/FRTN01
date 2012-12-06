
#include "pid.h"
#include <iostream>
#include <assert.h>
#include <math.h>

int main(){

	/* Test water tank input output
	 * output should decrease */


	double level = 0;
	double vin = 0;
	double vout = 0;
	double ref = 500;
	int period = 100;

	PIDParameters pp(1,2,1,0,0.1,period,ref,0,255,false);
	PID pid;
	pid.updateParameters(pp);
	std::cout << pp << "\n";
	for (int i = 0; i < 7000; i++){
		vin = 0.2* pid.next(level);
		pid.updateStates();
		vout = 0.1 * sqrt(2*9.82*level);
		level = level + (vin - vout) * period/1000;
		if (level < 0) level = 0;
		std::cout << ref - level << "\n";
	}
}

