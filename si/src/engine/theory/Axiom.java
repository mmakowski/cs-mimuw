package engine.theory;

import engine.*;
import engine.event.ModelEvent;
import engine.parser.*;

/**
 * Theory's axiom
 *
 * @author Jerzy Szczepkowski
 * @version 1.1.1
 */

public class Axiom extends Atom {
	/** Theory which axiom belongs to */
	protected Theory theory;

	public Axiom(Theory t) {
		super();
	}

	public Axiom(Theory t, String str) {
		super();
		theory = t;
		parseFromStr(str);
	}

	public Theory getTheory () {
		return theory;
	}

	private void parseFromStr(String str) {
		PAtom pars = new PAtom(str, false);
		pars.parsAtom(this);
	}

	public void printAxiom() {	// TODO: remove
		System.out.println(rel.getName()+" "+pos+" "+args);
		System.out.println(rel.getName()+" "+pos+" "+ ((Value) args.firstElement()).getValue());
	}

}
