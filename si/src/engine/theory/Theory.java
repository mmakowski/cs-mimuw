package engine.theory;

import java.util.*;
import java.lang.Math;
import engine.event.*;
import engine.*;
import java.io.*;

/**
 * Whole CAKE theory
 *
 * @author Jerzy Szczepkowski
 * @version 1.1.1
 */

public class Theory extends ModelEventSource implements java.io.Serializable {
	/** Theorty's axioms */
	protected Set axioms;
	/** Theorty's defaults */
	protected Set defaults;
	/** Theorty's relations */
	protected Map relations;
	/** Theorty's values */
	protected Map values;

	public Theory() {
		axioms = new HashSet();
		defaults = new HashSet();
	}

	/** Writes the theory to given file */
	public void writeToFile(String file_name) throws java.io.IOException {
		FileOutputStream fs = new FileOutputStream(file_name);
		ObjectOutputStream os = new ObjectOutputStream(fs);
		os.writeObject(this);
		os.close();
		fs.close();
	} 

	/** Makes new theory form the file */
	public Theory readFromFile(String file_name) 
	  throws java.io.FileNotFoundException, java.io.IOException, java.lang.ClassNotFoundException {
		FileInputStream fs = new FileInputStream(file_name);
		ObjectInputStream os = new ObjectInputStream(fs);
		Theory th = (Theory) os.readObject();
		os.close();
		fs.close();
		return th;
	} 

	/** Adds new axiom to the theory */
	public Axiom addAxiom(Axiom a) {
		axioms.add(a);
		fireModelChange(new ModelEvent(this, a, ModelEvent.OBJECT_ADDED));
		return a;
	}

	/** Adds new axiom to the theory - parse it from the string */
	public Axiom addAxiom(String str) {
		Axiom a = new Axiom(this, str);
		return addAxiom(a);
	}

	/** Remove the axiom from the theory */
	public void removeAxiom(Axiom a) {
		axioms.remove(a);
		fireModelChange(new ModelEvent(this, a, ModelEvent.OBJECT_REMOVED));
	}

	/** Modify axiom 'a' - new value is parsed form string 'str' */
	public Axiom modifyAxiom(Axiom a, String str) {
		removeAxiom(a);
		Axiom new_a = addAxiom(str);
		return new_a;
	}

	/** Adds new default to the theory */
	public Default addDefault(Default d) {
		defaults.add(d);
		fireModelChange(new ModelEvent(this, d, ModelEvent.OBJECT_ADDED));
		return d;
	}

	/** Generates all valuations for variables */
	public Set generateValuations(Set vars) {
		Set res = new HashSet();
		Map elem;
		List var_ = new ArrayList(vars); 
		int var_size = var_.size();
		List val_ = new ArrayList(values.values());
		int val_size = val_.size();
		if (var_size == 0)
			return res;
		int[] v = new int[var_size];
		for (int i = 1; i < var_size; i++) {
			v[i] = 0;
		}
		v[0] = -1;	/* will be incremented in the first iteration */
		double max_steps = Math.pow(val_size, var_size);
		for (double steps = 0; steps < max_steps; steps++) {
			int i = 0;
			v[i]++;
			while (v[i] == val_size) {
				v[i] = 0;
				i++;
				v[i]++;
			} 
			elem = new Hashtable();
			for (i = 0; i < var_size; i++) {
				elem.put(var_.get(i), ((Value)val_.get(v[i])).getValue());
			}
			// REM: System.err.println(elem);
			res.add(elem);
		}
		return res;
	}

	public void applyValuation(Hashtable val) {
	    Enumeration k = val.keys();
	    while (k.hasMoreElements()) {
		Variable v = (Variable)k.nextElement();
		v.setValue((String)val.get(v));
	    }
	}

	/** Adds new default to the theory - parse it from three strings */
	public Default addDefault(String pre, String just, String cons, int p) {
		Default d = new Default(this, pre, just, cons, p);
		return addDefault(d);
	}

	/** Remove the default from the theory */
	public void removeDefault(Default d) {
		defaults.remove(d);
		fireModelChange(new ModelEvent(this, d, ModelEvent.OBJECT_REMOVED));
	}

	/** Modify default 'd' - new value is parsed form three strings */
	public Default modifyDefault(Default d, String pre, String just, String cons, int p) {
		removeDefault(d);
		Default new_d = addDefault(pre, just, cons, p);
		return new_d;
	}

	public Set getAxioms() {
		return axioms;
	}

	public Set getDefaults() {
		return defaults;
	}

	public Map getRelations() {
		return relations;
	}
    
    public Map getValues() {
	return values;
    }

        protected void unifyValuesFromAtom(Atom at, Map values) {
		Iterator val_it = at.getArgs().getValues().iterator();
		while (val_it.hasNext()) {
			Value val = (Value) val_it.next();
			if (!values.containsKey(val.getValue())) 
				values.put(val.getValue(), new Value(val));
			// In 'values' we have copies only...
		}
        }

	protected void unifyValues() {
	    System.err.println("Unifying values...");
		values = new HashMap();
		// Unifying axioms:
		Iterator ax_it = axioms.iterator();
		while (ax_it.hasNext()) {
			Axiom ax = (Axiom) ax_it.next();
			unifyValuesFromAtom((Atom) ax, values);
		}
		// Unifying defaults:
		Iterator df_it = defaults.iterator();
		while (df_it.hasNext()) {
			Default df = (Default) df_it.next();
			unifyValuesFromAtom((Atom) df.getJust(), values);
			unifyValuesFromAtom((Atom) df.getCons(), values);
			Set pre = df.getPre();
			Iterator pre_it = pre.iterator();
			while (pre_it.hasNext()) {
				PrerequisiteAtom pa = (PrerequisiteAtom) pre_it.next();
				unifyValuesFromAtom((Atom) pa, values);
			}
		}
		System.err.println(values.values());
	}

        protected void unifyRelationsFromAtom(Atom at, Map values, boolean set) {
		Relation rel = (Relation) relations.get(at.getRelName());
		if (rel == null) {
			rel = new Relation(at.getRelName(), at.getArgs().getRank());
			relations.put(at.getRelName(), rel);
		}
		at.setRel(rel);
		if (set) {
			if (at.getPos()) 
				rel.addElementPos(at.getArgs());
			else
				rel.addElementNeg(at.getArgs());
		}
	}

	protected void unifyRelations() {
	    System.err.println("Unifying relations...");
		relations = new HashMap();
		// Unifying axioms:
		Iterator ax_it = axioms.iterator();
		while (ax_it.hasNext()) {
			Axiom ax = (Axiom) ax_it.next();
			unifyRelationsFromAtom((Atom) ax, relations, true);
		}
		// Unifying defaults:
		Iterator df_it = defaults.iterator();
		while (df_it.hasNext()) {
			Default df = (Default) df_it.next();
			unifyRelationsFromAtom((Atom) df.getJust(), relations, false);
			unifyRelationsFromAtom((Atom) df.getCons(), relations, false);
			Set pre = df.getPre();
			Iterator pre_it = pre.iterator();
			while (pre_it.hasNext()) {
				PrerequisiteAtom pa = (PrerequisiteAtom) pre_it.next();
				unifyRelationsFromAtom((Atom) pa, relations, false);
			}
		}
		System.err.println(relations.values());
	}

	public void unifyTheory() {
		unifyValues();
		unifyRelations();
	}

	public void unify() {
		unifyTheory();
	}

}
