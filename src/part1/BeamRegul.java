package part1;
import SimEnvironment.AnalogSink;
import SimEnvironment.AnalogSource;

// BeamRegul class to be written by you
public class BeamRegul extends Thread {
	private ReferenceGenerator referenceGenerator;
	private PI controller;

	// IO interface declarations
	private AnalogSource beamAngle;
	private AnalogSink analogOut;
	private AnalogSink analogRef;

	private double uMin = -10.0;
	private double uMax = 10.0;

	public BeamRegul(ReferenceGenerator ref, Beam beam, int pri) {
		referenceGenerator = ref;
		controller = new PI("PI");
		beamAngle = beam.getSource(0);
		analogOut = beam.getSink(0);
		analogRef = beam.getSink(1);
		setPriority(pri);
	}

	private double limit(double u, double umin, double umax) {
		if (u < umin) {
			u = umin;
		} else if (u > umax) {
			u = umax;
		}

		return u;
	}

	public void run() {
		long t = System.currentTimeMillis();
		
		/* 
		 * Main regulator loop.
		 * 
		 * 1. Get value of beam.
		 * 2. Get reference value.
		 * 3. Keep control signal within bounds.
		 * 4. 
		 * */
		while (true) {
			double y = beamAngle.get();
			double ref = referenceGenerator.getRef();

			synchronized (controller) {
				double u = limit(controller.calculateOutput(y, ref), uMin, uMax);
				analogOut.set(u);
				controller.updateState(u);
			}
			analogRef.set(ref);
			
			t = t + controller.getHMillis();
			long duration = t - System.currentTimeMillis();
			if (duration > 0) {
				try {
					sleep(duration);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		}
	}
}
