package engine.theory;

import java.util.*;

import engine.*;
import engine.event.ModelEvent;
import engine.parser.*;
import java.io.*;

/**
 * Therory's default
 *
 * @author Jerzy Szczepkowski
 * @version 1.1.1
 */

public class Default implements java.io.Serializable {
	/** Theory which default belongs to */
	protected Theory theory;
	/** priority */
	protected int priority;
	/** free variables */
	protected Set vars;
	/** prerequisite */
	protected Set pre;
	/** justification */
	protected Atom just;
	/** consequent */
	protected Consequent cons;

//	public void addVar(String name) {
//	}

	public void initialize(Set p, Atom j, Consequent c, Set v) {
		pre = p;
		just = j;
		cons = c;
		vars = v;
	}

	public int getPriority() {
		return priority;
	}

	public Default(Theory t) {
		super();
		theory = t;
	}

	public Default(String p, String j, String c) {
		super();
		parseFromStr(p, j, c);
	}

	public Default(Theory t, String p, String j, String c) {
		super();
		theory = t;
		parseFromStr(p, j, c);
	}

	public Default(Theory t, String p, String j, String c, int prio) {
		super();
		theory = t;
		priority = prio;
		parseFromStr(p, j, c);
	}

	public Theory getTheory () {
		return theory;
	}

	public Set getPre() {
		return pre;
	}

	public Atom getJust() {
		return just;
	}

	public Consequent getCons() {
		return cons;
	}

	public String getPreString() {
		String str = "";
		Iterator it = pre.iterator();
		if (it.hasNext()) {
			str += it.next();
		}
		while (it.hasNext()) {
			str += " " + ParserSettings.CON + " " + it.next();
		}
		return str;
	}

	public String getJustString() {
		return just.toString();
	}

	public String getConsString() {
		return cons.toString();
	}

	public String toString() {
		return "[" + priority + "] " + getPreString() + " : " + getJustString() + " / " + getConsString();
	}

	private void parseFromStr(String p, String j, String c) {
		PDefault pars = new PDefault(p, j, c);
		pars.parsDefault(this);
//		th.fireModelChange(new ModelEvent(th, this, ModelEvent.OBJECT_CHANGED));
	}

}
