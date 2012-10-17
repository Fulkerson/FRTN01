package part1;
public class Main {
    public static void main(String[] argv) {
		final int regulPriority = 8;
	
		/* First create a beam gui. */
		Beam beam = new Beam();
		
		/* The refgen generates the wanted position of the beam should be. */
	    ReferenceGenerator refgen = new ReferenceGenerator(5.0, 4.0);
	    
	    /* The regulator will move the beam to the position refgen says. */
		BeamRegul regul = new BeamRegul(refgen, beam, regulPriority);
	
		refgen.start();
		/* This is an ugly hack and should never be allowed in teaching code. */
		try {
			Thread.sleep(5000);
		} catch (InterruptedException x) {
		}
		regul.start();
    }
}

