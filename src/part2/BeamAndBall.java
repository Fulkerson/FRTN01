package part2;
import javax.swing.*;
import java.awt.*;
import java.awt.geom.*;
import java.awt.event.*;
import SimEnvironment.*;

public class BeamAndBall extends VirtualProcess {

    private static final int stateNbr=3;  //antal tillst�nd
    private static final int inputNbr=2;  //antal ing�ngar
    private static final int outputNbr=2; //antal utg�ngar

    private double kPhi=4.4; //processkonstant f�r vinkel
    private double kX = 7.0; //processkonstant f�r kulan

    private double scale = 100.0;
    private RoundRectangle2D box = new RoundRectangle2D.Double(100.0, 80.0, 100.0, 50.0, 10.0, 10.0);
    private Ellipse2D axis = new Ellipse2D.Double(144, 109.0, 12.0, 12.0);
    private Rectangle2D beam = new Rectangle2D.Double(50.0, 120.0, 200.0, 5.0);
    private Ellipse2D ball = new Ellipse2D.Double(145, 112.0, 10.0, 10.0);
    private boolean init = false;

    public BeamAndBall() {
	super(stateNbr, inputNbr, outputNbr);
	Plotter plotter = new Plotter(3,100,10,-10);
	getSource(0).setPlotter(plotter,0);
	getSink(0).setPlotter(plotter,1);
	getSink(1).setPlotter(plotter,2);
	JFrame frame = new JFrame("Virtual Beam and Ball");
	frame.getContentPane().setLayout(new GridBagLayout());
	JPanel jp = getAnimationPanel();
        jp.setPreferredSize(new Dimension(300,200));
        frame.getContentPane().add(jp); // l�gg till animeringspanel
	frame.getContentPane().add(plotter.getPanel());
        frame.pack();
        frame.setVisible(true);
    }
    
    public double[] computeOutput(double[] state, double[] input) {
	double[] output = new double[outputNbr];
	output[0] = state[0];                          //uppdatera kulans l�ge
	output[1] = state[2];                          //uppdatera bommens vinkel
	return output;
    }

    private double limit(double v, double min, double max) {
	if (v < min) {
	    v = min;
	} else {
	    if (v > max) {
		v = max;
	    }
	}
	return v;
    }

    public double[] updateState(double[] state, double[] input, double h) {
	double[] newState = new double[stateNbr];
	double ulim;
	ulim = limit(input[0],-10,10);
	newState[0] = state[0] + h*state[1];        //uppdatera kulans l�ge
	newState[1] = state[1] - kX*h*state[2];     //uppdatera kulans hastighet
	newState[2] = state[2] + kPhi*h*ulim;   //uppdatara bommens vinkel
	return newState;
    }

    public void draw(Graphics2D g2, JPanel jp, double[] state, 
		     double[] input, double[] output) {
	if (!init) {
	    jp.addMouseListener(new MouseListener() {
		public void mouseClicked(MouseEvent e) {
		    resetProcess();
		    init = true;
		}
		public void mousePressed(MouseEvent e){}
		public void mouseEntered(MouseEvent e){}
		public void mouseReleased(MouseEvent e){}
		public void mouseExited(MouseEvent e){}

	    });
	}
		
	scale = Math.min(jp.getWidth()/300.0, jp.getHeight()/200.0);
	
	g2.scale(scale, scale);                       //skala f�ljande animeringen
	g2.setColor(Color.gray);
	g2.fill(box);                                 //animera bakgrund
	g2.setColor(Color.black);
	g2.drawString("Ball&Beam",122,95);
	g2.draw(box); 
	g2.rotate(-1*output[1]*Math.PI/40,150,115);   //rotera f�ljande med bommens vinkel
	g2.setColor(Color.darkGray);
	g2.fill(axis);                                //animera axeln
	g2.fill(beam);                                //animera bommen
	g2.setColor(Color.blue);
	g2.translate(output[0]*10.0, 0.0);            //flytta f�ljande med kulans l�ge
	g2.fill(ball);                                //animera kulan
    }




} // BeamAndBall
