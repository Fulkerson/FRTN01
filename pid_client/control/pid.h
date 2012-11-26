
#include <iostream>

class PIDParameters {
	public:
		double Kp;
		double Ki;
		double Kd;
		double track;
		double h;
		double r;
		double umin;
		double umax;
		PIDParameters();
		PIDParameters(double,double,double,double,int,double,double,double);
	
		friend std::ostream& operator <<(std::ostream&, const PIDParameters&);
};

class PID {
	private:
		double I, eo, u, v;
		PIDParameters p;
	public:
		PID ();
		void updateParameters(const PIDParameters&);
		double next(double);
		void updateStates();
};

