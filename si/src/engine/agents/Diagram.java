package engine.agents;

import java.util.*;
import engine.*;
import engine.theory.*;

/**
 * The main CAKE diagram.
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

public class Diagram {

    /* the theory, for which the diagram is generated */
    protected Theory theory;
    /* the agents */
    protected Set agents = new HashSet();

    
    public Diagram(Theory th) {
	theory = th;
	generateFromTheory();
    }

    public Theory getTheory() {
	return theory;
    }

    public void generateFromTheory() {
	Iterator iter;
	int dbcount = 0, defcount = 0, defmodcount = 0;

	theory.unify();

	// generate database agents
	System.err.println("Generating database agents...");
	Hashtable ags = new Hashtable();
	// create DB agents for all relations in the theory
	iter = theory.getRelations().values().iterator();
	while (iter.hasNext()) {
	    Relation rel = (Relation)iter.next();
	    DatabaseAgent ag = new DatabaseAgent(this, "DB_" + rel.getName(), rel);
	    ags.put(rel, ag);
	    rel.setDatabaseAgent(ag);
	    rel.setDefaultAgent(ag); // so far we don't have a true default agent...
	    dbcount++;
	}
	// fill in the DB agents
	iter = theory.getAxioms().iterator();
	while (iter.hasNext()) {
	    DatabaseAgent ag;
	    Axiom ax = (Axiom)iter.next();
	    ag = (DatabaseAgent)ags.get(ax.getRel());
	    if (ax.getPos()) 
		ag.addPos(ax.getArgs());
	    else
		ag.addNeg(ax.getArgs());
	}
	// copy the hashtable values to global agents set
	agents.addAll(ags.values());
	System.err.println(ags.values());

	// generate default agents and modules
	System.err.println("Generating default agents and modules...");
	ags = new Hashtable();
	iter = theory.getDefaults().iterator();
	while (iter.hasNext()) {
	    Default d = (Default)iter.next();
	    Relation rel = d.getCons().getRel();
	    DefaultAgent ag = new DefaultAgent(this, "DEF_" + d.getCons().getRelName() + "_" + defcount, d);
	    defcount++;
	    if (ags.containsKey(rel)) {
		DefaultModule mod;
		if (ags.get(rel).getClass().getName().equals("engine.agents.DefaultModule"))
		    mod = (DefaultModule)ags.get(rel);
		else { // create new module
		    mod = new DefaultModule(this, "MOD_" + d.getCons().getRelName(), rel);
		    rel.setDefaultAgent(mod);
		    // move existing agent to the new module
		    mod.addAgent((DefaultAgent)ags.get(rel));
		    ags.put(rel, mod);
		    defmodcount++;
		}
		mod.addAgent(ag);
	    } else { // no default agents for this relation yet
		ags.put(rel, ag);
		rel.setDefaultAgent(ag);
	    }
	}
	// copy the hashtable values to global agents set
	agents.addAll(ags.values());
	System.err.println(ags.values());

	System.err.println("CAKE diagram generated: " + dbcount + 
			   " database agents, " + defcount + 
			   " default agents, " + defmodcount + " default modules.");
	System.err.println();
    }

    public void computeExtension() {
	int applied, tot = 0;
	Iterator iter;

	System.err.println("Computing extension...");
	do {
	    applied = 0;
	    iter = agents.iterator();
	    while (iter.hasNext()) {
		Agent a = (Agent)iter.next();
		applied += a.applyRules();
	    }
	    tot += applied;
	} while (applied > 0);
	System.err.println("Extension computed: " + tot + " elements added.");
    }

    public int query(Agent a, Relation r, Tuple t) {
	if (a == null) {
	    a = r.getDefaultAgent();
	}
	return a.query(t);
    }
}
