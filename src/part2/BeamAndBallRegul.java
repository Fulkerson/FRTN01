package part2;

import SimEnvironment.AnalogSink;
import SimEnvironment.AnalogSource;
import part1.ReferenceGenerator;
import part1.PI;

// BeamAndBallRegul class to be written by you
public class BeamAndBallRegul extends Thread {
	private ReferenceGenerator referenceGenerator;
	private PID pidControl;
	private PI piControl;
	
	private AnalogSource beamAngle;
	private AnalogSource ballPos;
	private AnalogSink beamOut;
	private AnalogSink analogRef;
	
	// Input x to the B&B process is |x| < 10 Volt
	private double uMin = -10.0;
	private double uMax = 10.0;
	
	
	// Constructor
	public BeamAndBallRegul(ReferenceGenerator ref, BeamAndBall beam, int pri) {
		referenceGenerator = ref;
		piControl = new PI("PI");
		pidControl = new PID("PID");
		beamAngle = beam.getSource(1);
		ballPos = beam.getSource(0);
		beamOut = beam.getSink(0);
		analogRef = beam.getSink(1);
		setPriority(pri);
	}
	
	/**
	 * Bound signal.
	 * @param u signal to be bounded.
	 * @return bounded signal.
	 */
	private double boundSignal(double u) {
		if (u > this.uMax) {
			return this.uMax;
		} else if (u < this.uMin) {
			return this.uMin;
		} else {
			return u;
		}
	}

	public void run() {
		long t = System.currentTimeMillis();
		
		while (true) {
			// do stuff
			double yBeam = beamAngle.get();
			double refBeam = referenceGenerator.getRef();
			
			synchronized(piControl) {
				double u = boundSignal(piControl.calculateOutput(yBeam,
						refBeam));
				beamOut.set(u);
				piControl.updateState(u);
			}
			analogRef.set(refBeam);
			
			t = t + piControl.getHMillis();
			long sleepFor = t - System.currentTimeMillis();
			if (sleepFor > 0) {
				try {
					sleep(sleepFor);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
		
	}
}
