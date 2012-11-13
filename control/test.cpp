
#include <pid.h>
#include <iostream>
#include <assert.h>
#include <math.h>

int main(){

	/* Test water tank input output
	 * output should decrease */


	double level = 0;
	double vin = 0;
	double vout = 0;
	double ref = 5;

	PID* pid = new PID(1,0.1,0.5,ref,0,10);
	for (int i = 0; i < 100; i++){
		vin = 0.2 * pid->next(level);
		vout = 0.1 * sqrt(2*9.82*level);
		level = level - vout + vin;
		if (level < 0) level = 0;
		std::cout << ref - level << "\n";
	}
}

