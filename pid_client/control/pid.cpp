
#include "pid.h"
#include <iostream>



PIDParameters::PIDParameters(double K, double Ti, double Td, double Tr,
	int period,	double ref, double min, double max) :
	h(period/1000), Kp(K), Ki(K*period/1000/Ti), Kd(K*Td/period*1000), 
	track(period/1000/Tr), r(ref), umin(min), umax(max) {}
PIDParameters::PIDParameters() :
	Kp(0), Ki(0), Kd(0), h(1), r(0), umin(0), umax(0) {}


std::ostream& operator <<(std::ostream& os, const PIDParameters& p) {
	os << "Kp=" << p.Kp << " Ki=" << p.Ki << " Kd=" << p.Kd;
	os << " h=" << p.h;
	os << " r="<< p.r << " umin=" << p.umin << " umax=" << p.umax;
	return os;
}

PID::PID() : I(0), eo(0), u(0), v(0) {}

void PID::updateParameters(const PIDParameters& params) {
	p = params;
}

double PID::next(double y){
	double e = p.r - y;
	u = v = p.Kp*e + I + p.Kd*(e-eo);
	if (u > p.umax)
		u = p.umax;
	if (u < p.umin)
		u = p.umin;
	eo = e;
	return u;
}

void PID::updateStates() {
	double Tr = 10;
	I = I + p.Ki*eo + p.track*(u-v);
}

