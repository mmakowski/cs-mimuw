package engine.agents;

import java.util.*;
import engine.*;

/**
 * An abstract CAKE agent
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

public abstract class Agent {
    
    /** the possible responses to a query */
    public final static int FALSE = 0;
    public final static int TRUE = 1;
    public final static int UNKNOWN = 2;

    /** the diagram this agent belongs to */
    protected Diagram diag;
    /** name of the agent */
    protected String name;
    /** the relation the agent is responsible for */
    protected Relation rel;
    /** the set of r-positive tuples */
    protected Set pos = new HashSet();
    /** the set of r-negative tuples */
    protected Set neg = new HashSet();

    public Agent(Diagram d, String n, Relation r) {
	diag = d;
	name = new String(n);
	rel = r;
    }

    public String getName() {
	return name;
    }
    
    public Relation getRelation() {
	return rel;
    }

    public boolean addPos(Tuple t) {
	// check whether same tuple is already in the set
	// REM: System.err.println("adding " + t + " to pos for " + toString());
	Iterator iter;
	iter = pos.iterator();
	while (iter.hasNext())
	    if (t.equals((Tuple)iter.next()))
		return false;

	pos.add(t);
	return true;
    }

    public boolean addNeg(Tuple t) {
	// check whether same tuple is already in the set
	Iterator iter;
	iter = neg.iterator();
	while (iter.hasNext())
	    if (t.equals((Tuple)iter.next()))
		return false;

	neg.add(t);
	return true;
    }

    public String toString() {
	return getName();
    }

    /**
     * check whether given tuple is in positive or negative set
     */
    public int query(Tuple t) {
	Iterator iter;
	
	// REM: System.err.println("querying for " + t.valuesToString());

	iter = pos.iterator();
	while (iter.hasNext())
	    if (t.equals((Tuple)iter.next()))
		return TRUE;
	iter = neg.iterator();
	while (iter.hasNext())
	    if (t.equals((Tuple)iter.next()))
		return FALSE;
	return UNKNOWN;
    }

    /**
     * apply default rules --- no rules here, so empty
     */
    public int applyRules() {
	return 0;
    }

}
