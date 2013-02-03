package engine.agents;

import java.util.*;
import engine.Relation;
import engine.Tuple;
import engine.theory.Default;

/**
 * An agent that is responsible for a default
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

public class DefaultAgent extends Agent {

    /** set of positive rules for the relation the agent is responsible for */
    protected Set posRules;
    /** set of negative rules for the relation the agent is responsible for */
    protected Set negRules;
    /** the priority of the default the agent is responsible for */
    protected int priority;

    /**
     * initializes the agent and fills the rule sets based on the Default d
     */
    public DefaultAgent(Diagram di, String n, Default d) {
	super(di, n, d.getCons().getRel());
	posRules = new HashSet();
	negRules = new HashSet();
	processDefault(d);
    }

    protected void processDefault(Default d) {
	Relation rel = d.getCons().getRel();
	AgentFormula af;
	AgentAtom at;

	// set priority
	priority = d.getPriority();

	// add standard pos and neg formulae
	af = new AgentFormula(diag, d.getCons().getArgs());
	at = new AgentAtom(diag, rel, d.getCons().getArgs(), AgentAtom.POS, true);
	af.addAtom(at);
	posRules.add(af);
	af = new AgentFormula(diag, d.getCons().getArgs());
	at = new AgentAtom(diag, rel, d.getCons().getArgs(), AgentAtom.NEG, true);
	af.addAtom(at);
	negRules.add(af);
	
	// add formula derived from the default
	af = new AgentFormula(diag, d);
	if (d.getCons().getPos()) 
	    posRules.add(af);
	else
	    negRules.add(af);
    }

    public int getPriority() {
	return priority;
    }

    public String dumpRules() {
	String s = "";
	Iterator iter;

	// pos
	s = "POS:\n";
	iter = posRules.iterator();
	while (iter.hasNext())
	    s += iter.next().toString() + "\n";
	s += "----------------\n";
	// next
	s += "NEG:\n";
	iter = negRules.iterator();
	while (iter.hasNext())
	    s += iter.next().toString() + "\n";
	s += "----------------";

	return s;
    }

    /**
     * apply rules
     */
    public int applyRules() {
	Iterator iter;
	int applied = 0;

	iter = posRules.iterator();
	while (iter.hasNext()) {
	    AgentFormula af = (AgentFormula)iter.next();
	    // REM: System.err.println("applying formula (POS)" + af);
	    applied += af.apply(this, AgentFormula.POS);
	}

	iter = negRules.iterator();
	while (iter.hasNext()) {
	    AgentFormula af = (AgentFormula)iter.next();
	    // REM: System.err.println("applying formula (NEG)" + af);
	    applied += af.apply(this, AgentFormula.NEG);
	}

	return applied;
    }

}
