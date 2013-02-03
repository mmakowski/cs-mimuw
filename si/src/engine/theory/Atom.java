package engine.theory;

import engine.*;
import engine.parser.ParserSettings;
import java.io.*;

/**
 * Single relation in formula
 *
 * @author Jerzy Szczepkowski
 * @version 1.1.1
 */

public class Atom implements java.io.Serializable {

	/** relation, which the atom references */
	protected Relation rel;
	/** is relation value taken in positive?  */
	protected boolean pos;
	/** arguments for the referenced relation */
	protected Tuple args;
	
	public Atom () {
	}

	public Relation getRel() {
		return rel;
	}

	public void setRel(Relation r) {
		rel = r;
	}

	public boolean getPos() {
		return pos;
	}

	public void setPos(boolean p) {
		pos = p;
	}

	public Tuple getArgs() {
		return args;
	}

	public void setArgs(Tuple a) {
		args = a;
	}

	public void initialize(String r_name, boolean p, Tuple a) {
		pos = p;
		args = a;
		rel = new Relation(r_name, a.getRank());
	}

	public String getRelName() {
		return rel.getName();
	}

	
	/**
	 * return the string representation of this atom
	 */
	public String toString() {
		String s = new String("");
		if (!pos)
			s += ParserSettings.NEG;
		s += rel.getName() + args.toString();
		return s;
	}
}
