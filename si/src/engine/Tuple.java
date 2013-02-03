package engine;

import java.util.*;
import engine.parser.ParserSettings;
import java.io.*;

/**
 * A tuple of arguments for a relation.
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

public class Tuple extends Vector implements java.io.Serializable {
    /** the number of the elements in the tuple */
    private int rank;

    /**
     * creates new tuple with rank r
     */
    public Tuple(int r) {
	super(r);
	rank = r;
    }

    public int getRank() {
	return rank;
    }

    public void setRank(int r) {
        rank = r;
    }

    /**
     * create tuple containing the values of given variables
     */
    public static Tuple createFromVars(Tuple t) {
	Tuple vt = new Tuple(t.getRank());
	
	for (int i = 0; i < t.getRank(); i++)
	    vt.add(i, new Value(((Argument)t.get(i)).getValue()));

	return vt;
    }

    /**
     * return all the variables in the tuple
     */
    public Set getVariables() {
	HashSet s = new HashSet();
	for (Enumeration e = elements(); e.hasMoreElements(); ) {
	    Argument a = (Argument)e.nextElement();
	    if (a.isVariable())
		s.add(a);
	}
	return s;
    }


    /**
     * return all the values in the tuple
     */
    public Set getValues() {
	HashSet s = new HashSet();
	for (Enumeration e = elements(); e.hasMoreElements(); ) {
	    Argument a = (Argument)e.nextElement();
	    if (!a.isVariable())
		s.add(a);
	}
	return s;
    }

    /**
     * return the string representation of this tuple
     */
    public String toString() {
	String s = new String(ParserSettings.LBR);
	boolean first = true;

	for (Enumeration e = elements(); e.hasMoreElements(); ) {
	    Argument a = (Argument)e.nextElement();
	    if (first)
		first = false;
	    else
		s = s + ParserSettings.SEP + " ";
	    s = s + a.toString();
	}
	s = s + ParserSettings.RBR;

	return s;
    }

    public String valuesToString() {
	String s = new String(ParserSettings.LBR);
	boolean first = true;

	for (Enumeration e = elements(); e.hasMoreElements(); ) {
	    Argument a = (Argument)e.nextElement();
	    if (first)
		first = false;
	    else
		s = s + ParserSettings.SEP + " ";
	    s = s + a.getValue();
	}
	s = s + ParserSettings.RBR;

	return s;
    }

    /**
     * check whether the values equal these in src
     */
    public boolean equals(Tuple src) {
	// REM: System.err.println("comparing " + valuesToString() + " with " + src.valuesToString());
	if (rank != src.rank)
	    return false;
	// REM: System.err.println("rank ok");
	Enumeration e = elements();
	Enumeration esrc = src.elements();

	while (e.hasMoreElements()) {
	    Argument a = (Argument)e.nextElement();
	    Argument asrc = (Argument)esrc.nextElement();
	    if (a.getValue().compareTo(asrc.getValue()) != 0)
		return false;
	}

	return true;
    }
}

