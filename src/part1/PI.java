package part1;

import part2.PIDParameters;

// PI class to be written by you
public class PI {

	// Private attribute containing a reference to the GUI
	private PIGUI GUI;
	// Private attribute containing a reference to the PIParameters
	// currently used
	private PIParameters p;
	
	private double I;
	private double v;
	private double e;

	// Additional attributes

	// Constructor
	public PI(String name) {
		PIParameters p = new PIParameters();
		p.Beta = 1.0;
		p.H = 0.1;
		p.integratorOn = false;
		p.K = 1.0;
		p.Ti = 0.0;
		p.Tr = 10.0;
		GUI = new PIGUI(this, p, name);
		
		this.I = 0.0;
		this.v = 0.0;
		this.e = 0.0;
		setParameters(p);
	}

	// Calculates the control signal v. Called from BeamRegul.
	public synchronized double calculateOutput(double y, double yref) {
		this.e = yref - y;
		this.v = p.K * (p.Beta * yref - y) + I;
		return this.v;
	}

	// Updates the controller state. Should use tracking-based anti-windup
	// Called from BeamRegul.
	public synchronized void updateState(double u) {
		if (p.integratorOn) {
			I = I + (p.K * p.H / p.Ti) * e + (p.H / p.Tr) * (u - v);
		} else {
			I = 0.0;
		}
	}

	// Returns the sampling interval expressed as a long. Explicit type casting
	// needed
	public synchronized long getHMillis() {
		return (long) (p.H * 1000.0);
	}

	// Sets the PIParameters. Called from PIGUI. Must clone newParameters.
	public synchronized void setParameters(PIParameters newParameters) {
		p = (PIParameters) newParameters.clone();
		if (!p.integratorOn) {
			I = 0.0;
		}
	}

}