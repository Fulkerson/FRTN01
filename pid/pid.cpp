
#include "pid.h"
#include <iostream>

PID::PID(double k, double Ti, double Td, double r, double min, double max){
	Kp = k;
	Kd = Td;
	Ki = 1/Ti;
	ref = r;
	umin = min;
	umax = max;
    I = 0;
    eo = 0;
}

double PID::next(double y){
	double e = ref - y;
	I = I + e;
	double u = Kp*e + Ki*I + Kd*(e-eo);
	if (u > umax)
		u = umax;
	if (u < umin)
		u = umin;
	eo = e;
	std::cout << u;
	return u;
}



