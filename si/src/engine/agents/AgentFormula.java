package engine.agents;

import java.util.*;
import engine.Variable;
import engine.Tuple;
import engine.theory.*;

/**
 * A rule in agent that can decide whether certain tuple is an
 * element of a relation that agent is responsible for. Each
 * AgentFormula is a conjunction of atoms.
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

public class AgentFormula {

    public static final int POS = 0;
    public static final int NEG = 1;

    /** the diagram this formula belongs to */
    protected Diagram diag;
    /** the atoms that are conjugated in this formula */
    protected Set atoms;
    /** the variables from atoms */
    protected Set vars;
    /** the tuple of variables that is added to appropriate (pos/neg) set in the agent */
    protected Tuple resultVars;

    public AgentFormula(Diagram d, Tuple rv) {
	diag = d;
	atoms = new HashSet();
	vars = new HashSet();
	resultVars = rv;
    }

    public AgentFormula(Diagram di, Default d) {
	diag = di;
	atoms = new HashSet();
	vars = new HashSet();
	resultVars = d.getCons().getArgs();
	vars.addAll(d.getCons().getArgs().getVariables());
	
	Set pre;
	Iterator iter;

	// process prerequisite
	pre = d.getPre();
	iter = pre.iterator();
	while (iter.hasNext()) {
	    PrerequisiteAtom pa = (PrerequisiteAtom)iter.next();
	    if (!pa.isEmpty()) {
		AgentAtom at = new AgentAtom(diag, pa);
		addAtom(at);
	    }
	}
	// add justification atom; 
	// NB: can't use AgentAtom(theory.Atom) because of different pos/neg handling
	addAtom(new AgentAtom(diag, d.getJust().getRel(), d.getJust().getArgs(),
			      (d.getJust().getPos()) ? AgentAtom.NONNEG : AgentAtom.NONPOS, 
			      true));
    }

    public void addAtom(AgentAtom a) {
	vars.addAll(a.getVariables());
	atoms.add(a);
    }

    public Set getAtoms() {
	return atoms;
    }

    public Set getVariables() {
	return vars;
    }

    public Tuple getResultVars() {
	return resultVars;
    }

    public String toString() {
	String s = "";
	Iterator iter = atoms.iterator();
	while (iter.hasNext()) {
	    if (s.length() > 0)
		s += " & ";
	    s += iter.next().toString();
	}
	s += " => " + resultVars.toString();
	return s;
    }

    /**
     * try to apply this formula
     */
    public int apply(DefaultAgent ag, int group) {
	// find all the possible valuations of free variables
	// and for each check whether atoms conjunction is true
	
	int applied = 0; 

	// no variables --- try to apply and exit
	if (getVariables().isEmpty()) {
	    if (prerequisiteTrue()) {
		if (addTuple(ag, group))
		    return 1;
		else
		    return 0;
	    }
	}

	// handle variables
	Set valuations = diag.getTheory().generateValuations(getVariables());
	Iterator iter = valuations.iterator();
	

	while (iter.hasNext()) {
	    diag.getTheory().applyValuation((Hashtable)iter.next());
	    if (prerequisiteTrue())
		if (addTuple(ag, group))
		    applied++;
	}
			 
	return applied;
    }

    protected boolean prerequisiteTrue() {
	Iterator iter = atoms.iterator();
	while (iter.hasNext()) 
	    if (!((AgentAtom)iter.next()).isTrue())
		return false;
	return true;
    }

    protected boolean addTuple(DefaultAgent ag, int group) {
	boolean rv = false;
	Tuple t = Tuple.createFromVars(resultVars);
	
	if (group == POS)
	    rv = ag.addPos(t);
	else
	    rv = ag.addNeg(t);

	if (rv)
	    System.err.println(ag.getName() + 
			       ((group == POS) ? "[POS]: " : "[NEG]: ") +
			       ag.getRelation().getName() + t.toString());

	return rv;
    }

}
