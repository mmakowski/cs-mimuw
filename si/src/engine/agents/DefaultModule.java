package engine.agents;

import java.util.*;
import engine.Relation;
import engine.Tuple;

/**
 * A module grouping the default agents responsible for the same relation
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

public class DefaultModule extends Agent {

    /** additional value for poll result */
    protected final static int TRUE_UNKNOWN = -1;

    /** the agents contained in the module */
    protected Hashtable agents;

    public DefaultModule(Diagram d, String n, Relation r) {
	super(d, n, r);
	agents = new Hashtable();
    }

    /**
     * add new default agent to this module
     */
    public void addAgent(DefaultAgent a) {
	Set s = null;
	Enumeration en = agents.keys();
	Integer k;

	// find existing set of agents corresponding to this priority
	while (en.hasMoreElements()) {
	    k = (Integer)en.nextElement();
	    if (k.intValue() == a.getPriority())
		s = (Set)agents.get(k);
	}
	// no set found --- create new one
	if (s == null) {
	    s = new HashSet();
	    agents.put(new Integer(a.getPriority()), s);
	}

	s.add(a);
    }

    public String toString() {
	return name + ":" + agents.toString();
    }

    /**
     * perform the prioritized voting
     */
    public int query(Tuple t) {
	Integer max = getMaxPriority(-1);
	int result;

	while (max.intValue() > -1) {
	    result = querySubset((Set)agents.get(max), t);
	    if (result != TRUE_UNKNOWN) 
		return result;
	    max = getMaxPriority(max.intValue());
	}

	return UNKNOWN;
    }

    /**
     * perform voting with agents sharing the same priority
     */
    protected int querySubset(Set s, Tuple t) {
	int result = UNKNOWN, tmpres;
	Iterator iter = s.iterator();

	while (iter.hasNext()) {
	    Agent a = (Agent)iter.next(); 
	    tmpres = a.query(t);
	    //System.err.println(a + ": " + tmpres);
	    if (tmpres != UNKNOWN && tmpres != result && result != UNKNOWN)
		return UNKNOWN;
	    else if (tmpres != UNKNOWN)
		result = tmpres;
	}
	if (result == UNKNOWN)
	    result = TRUE_UNKNOWN;
	return result;
    }
    
    /**
     * find max priority less than ubound
     */
    protected Integer getMaxPriority(int ubound) {
	Enumeration e = agents.keys();
	Integer cur;
	Integer max = new Integer(-1);

	while (e.hasMoreElements()) {
	    cur = (Integer)e.nextElement();
	    if (cur.intValue() > max.intValue())
		if (ubound == -1 || cur.intValue() < ubound)
		    max = cur;
	}

	return max;
    }

    public int applyRules() {
	int applied = 0;
	Enumeration en = agents.keys();
	

	// call applyRules for all agents in this module
	while (en.hasMoreElements()) {
	    Integer k = (Integer)en.nextElement();
	    Set s = (Set)agents.get(k);
	    Iterator iter = s.iterator();
	    while (iter.hasNext())
		applied += ((DefaultAgent)iter.next()).applyRules();
	}

	return applied;
    }
}
