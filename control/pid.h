

class PID {
	private:
		double Kp, Kd, Ki, ref, umin, umax, I, eo;
	public:
		PID (double, double, double, double, double, double);
		double next(double);
};

