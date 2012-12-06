
#include <iostream>

class PIDParameters
{
public:
	double Kp;
	double Ki;
	double aKd;
	double bKd;
	double track;
	double h;
	double r;
	double umin;
	double umax;
	bool inverted;
	PIDParameters();
	PIDParameters(double,double,double,double,double,
				double,double,double,double,bool);

	friend std::ostream& operator <<(std::ostream&, const PIDParameters&);
};

class PID
{
private:
	double I, u, v, e, yold;
	PIDParameters p;
	double limit(double,double,double);
public:
	PID();
	void updateParameters(const PIDParameters&);
	double next(double);
	void updateStates();
};

