package lab;
public class Main { 
    public static void main(String[] argv) { 

        final int regulPriority = 8; 
        final int refGenPriority = 6; 
        final int plotterPriority = 7; 

        ReferenceGenerator refgen = new ReferenceGenerator(refGenPriority); 
        Regul regul = new Regul(regulPriority); 
        OpCom opcom = new OpCom(plotterPriority); 

        regul.setOpCom(opcom); 
        regul.setRefGen(refgen); 
        opcom.setRegul(regul); 

        opcom.initializeGUI(); 

        refgen.start(); 
        opcom.start(); 
        regul.start(); 
    } 
} 
