
#include "pid.h"
#include <iostream>



PIDParameters::PIDParameters(double K, double Ti, double Td, double Tr,
				double N, double period, double ref,
				double min, double max, bool inverted) :
		h(period/1000), Kp(K), Ki(Ti==0 ? 0 : K*period/1000/Ti),
		track(Tr==0 ? 0 : period/1000/Tr),
		r(ref), umin(min), umax(max), inverted(inverted)
{
		aKd = Td == 0 ? 0 : Td/(Td + N * h);
		bKd = K*aKd*N;
}

PIDParameters::PIDParameters() :
		Kp(0), Ki(0), aKd(0), bKd(0), h(1), r(0), umin(0), umax(0),
		inverted(false) {}


std::ostream& operator <<(std::ostream& os, const PIDParameters& p)
{
	os << "Kp=" << p.Kp << " Ki=" << p.Ki << " aKd=" << p.aKd;
	os << " bKd=" << p.bKd << " h=" << p.h;
	os << " r="<< p.r << " umin=" << p.umin << " umax=" << p.umax;
	os << " inverted=" << p.inverted;
	return os;
}

PID::PID() : I(0), e(0), u(0), v(0), yold(0) {}

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
	double D = p.aKd*D - p.bKd*(y - yold);
	v = p.Kp*e + I + D;
	u = limit(v, p.umin, p.umax);
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

