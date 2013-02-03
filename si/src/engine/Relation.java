package engine;

import java.lang.IllegalArgumentException;
import java.util.*;
import engine.agents.Agent;
import java.io.*;

/**
 * A logical relation
 *
 * @author Maciek Makowski, Jurek Szczepkowski
 * @version 1.1.1
 */

public class Relation implements java.io.Serializable {

    /** the name of the relation */
    protected String name;
    /** the number of arguments for the relation */
    protected int rank;
    /** the set of tuples belnging to the relation */
    protected Set pos_elements;
    /** the set of tuples NOT belnging to the relation */
    protected Set neg_elements;
    /** the main default agent for this relation */
    protected transient Agent def;
    /** the main database agent for this relation */
    protected transient Agent datab;


    public Relation(String n, int r) {
	name = new String(n);
	rank = r;
	pos_elements = new HashSet();
	neg_elements = new HashSet();
    }

    public String getName() {
        return name;
    }

    public int getRank() {
	return rank;
    }

    public Agent getDefaultAgent() {
	return def;
    }

    public void setDefaultAgent(Agent a) {
	def = a;
    }

    public Agent getDatabaseAgent() {
	return datab;
    }

    public void setDatabaseAgent(Agent a) {
	datab = a;
    }

    /**
     * adds an positive element to the relation - the Tuple t has to have correct rank
     */
    public void addElementPos(Tuple t) {
	if (t.getRank() != rank)
            throw new IllegalArgumentException();
	pos_elements.add(t);
    }

    /**
     * adds an negative element to the relation - the Tuple t has to have correct rank
     */
    public void addElementNeg(Tuple t) {
	if (t.getRank() != rank)
            throw new IllegalArgumentException();
	neg_elements.add(t);
    }

    /**
     * remove an element from the relation - the Tuple t has to have correct rank
     */
//    public void removeElement(Tuple t) {
//	if (t.getRank() != rank)
//	    throw new IllegalArgumentException();
//	elements.remove(t);
//    }

    /**
     * checks, whether t is an element of the relation - t has to have correct rank
     */
//    public boolean isElement(Tuple t) {
//	if (t.getRank() != rank)
//	    throw new IllegalArgumentException();
//	return elements.contains(t);
//    }

    /**
     * checks, whether the relation has any arguments
     */
//    public boolean isEmpty() {
//        return elements.isEmpty();
//    }

    public String toString() {
	return name;
    }

}
