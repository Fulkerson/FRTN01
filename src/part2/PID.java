package part2;

import part1.PIGUI;

// PID class to be written by you
public class PID {

  // Private attribute containing a reference to the GUI
  private PIDGUI GUI;
  // Private attribute containing a reference to the PIDParameters
  // currently used
  private PIDParameters p;

  // Additional attributes
  private double e;
  private double value;
  private double D, I;
  private double ad, bd;
  private double y, yold;


  // Constructor
  public PID(String name) {
	  PIDParameters p = new PIDParameters();
	  p.Beta = 1.0;
	  p.H = 0.1;
	  p.integratorOn = false;
	  p.K = -0.05;
	  p.N = 5;
	  p.Td = 1;
	  p.Ti = 0.0;
	  p.Tr = 10.0;
	  GUI = new PIDGUI(this, p, name);
	  
	  this.I = 0.0;
	  this.value = 0.0;
	  this.e = 0.0;
	  
	  setParameters(p);
  }

  // Calculates the control signal v. Called from BeamAndBallRegul.
  public synchronized double calculateOutput(double y, double yref) {
	  this.y = y;
	  this.e = yref - y;
	  double P = p.K*(p.Beta * yref - y);
	  this.D =  ad*D - bd * (y - yold);
	  
	  this.value = P + I + D;
	  return this.value;
  }

  // Updates the controller state. Should use tracking-based anti-windup
  // Called from BeamAndBallRegul.
  public synchronized void updateState(double u) {
		if (p.integratorOn) {
			I = I + (p.K * p.H / p.Ti) * e + (p.H / p.Tr) * (u - value);
		} else {
			I = 0.0;
		}
		yold = y;
  }

  // Returns the sampling interval expressed as a long. Explicit type casting
  // needed
  public synchronized long getHMillis() {
	  return (long) (p.H *1000.0);
  }

  // Sets the PIDParameters. Called from PIDGUI. Must clone newParameters.
  public synchronized void setParameters(PIDParameters newParameters) {
	  p = (PIDParameters) newParameters.clone();
	  ad = p.Td / (p.Td + p.N*p.H);
	  bd = p.K*ad*p.N;

  }

}