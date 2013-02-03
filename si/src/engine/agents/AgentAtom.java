package engine.agents;

import java.util.Set;
import engine.*;
import engine.theory.Atom;
import engine.theory.PrerequisiteAtom;

/**
 * The simplest element of a formula
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

public class AgentAtom {

    /** the possible forms of an atom */
    public final static int POS    = 0;
    public final static int NEG    = 1;
    public final static int NONPOS = 2;
    public final static int NONNEG = 3;

    /** the diagram this atom belongs to */
    protected Diagram diag;
    /** relation, which the atom references */
    protected Relation rel;
    /** form in which this atom appears in a formula */
    protected int form;
    /** is this a strong prerequisite? */
    protected boolean strong;
    /** arguments for the referenced relation */
    protected Tuple args;


    public AgentAtom(Diagram d, Relation r, Tuple a, int f, boolean s) {
	diag = d;
	rel = r;
	args = a;
	form = f;
	strong = s;
    }

    public AgentAtom(Diagram d, PrerequisiteAtom a) {
	diag = d;
	rel = a.getRel();
	args = a.getArgs();
	if (a.getPos())
	    form = POS;
	else
	    form = NEG;
	// TODO:
	strong = a.getStrong();
    }

    /**
     * return all the variables passed as arguments to the relation
     */
    public Set getVariables() {
	return args.getVariables();
    }

    public String toString() {
	String s = "";
	s = rel.toString();
	s += args.toString();
	switch (form) {
	case POS:
	    s += "+";
	    break;
	case NEG:
	    s += "-";
	    break;
	case NONPOS:
	    s += "(-)";
	    break;
	case NONNEG:
	    s += "(+)";
	    break;
	}
	return s;
    }

    /**
     * is the atom true?
     */
    public boolean isTrue() {
	Agent a;

	if (strong)
	    a = rel.getDatabaseAgent();
	else 
	    a = rel.getDefaultAgent();

	switch (a.query(args)) {
	case Agent.FALSE:
	    if (form == NEG || form == NONPOS)
		return true;
	    else
		return false;
	case Agent.TRUE:
	    if (form == POS || form == NONNEG)
		return true;
	    else
		return false;
	case Agent.UNKNOWN:
	    if (form == NONPOS || form == NONNEG)
		return true;
	    else
		return false;
	}
	return false;
    }

}
