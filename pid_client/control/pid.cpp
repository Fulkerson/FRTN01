
#include "pid.h"
#include <iostream>



PIDParameters::PIDParameters(double K, double Ti, double Td, double Tr,
				double period, double ref, double min, double max, bool inverted) :
		h(period/1000), Kp(K), Ki(Ti==0 ? 0 : K*period/1000/Ti),
		Kd(-K*Td/period*1000), track(Tr==0 ? 0 : period/1000/Tr),
		r(ref), umin(min), umax(max), inverted(inverted) {}

PIDParameters::PIDParameters() :
		Kp(0), Ki(0), Kd(0), h(1), r(0), umin(0), umax(0),
		inverted(false) {}


std::ostream& operator <<(std::ostream& os, const PIDParameters& p)
{
	os << "Kp=" << p.Kp << " Ki=" << p.Ki << " Kd=" << p.Kd;
	os << " h=" << p.h;
	os << " r="<< p.r << " umin=" << p.umin << " umax=" << p.umax;
	os << " inverted=" << p.inverted;
	return os;
}

PID::PID() : I(0), e(0), u(0), v(0), yo(0) {}

/*
 * Update parameters.
 */
void PID::updateParameters(const PIDParameters& params)
{
	p = params;
}

/*
 * Limits the double x between min and max.
 */
double PID::limit(double x, double min, double max)
{
	if (x < min)
		return min;
	if (x > max)
		return max;
	return x;
}

/*
 * Calculate the next control signal u based
 * on the new level y.
 */
double PID::next(double y)
{
	e = p.r - y;
	if (p.inverted) {
		e = -e;
	}
	v = p.Kp*e + I + p.Kd*(y-yo);
	u = limit(v, p.umin, p.umax);
	yo = y;
	return u;
}

/*
 * Update integral part.
 */
void PID::updateStates()
{
	I = I + p.Ki*e; + p.track*(u-v);
	I = limit(I,p.umin,p.umax);
}

