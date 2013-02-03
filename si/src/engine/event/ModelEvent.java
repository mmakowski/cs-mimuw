package engine.event;

import java.util.*;

/**
 * The event occuring when model gets changed.
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

public class ModelEvent extends EventObject {
    
    /** an object has been added */
    public static final int OBJECT_ADDED = 0;
    /** an object has been removed */
    public static final int OBJECT_REMOVED = 1;
    /** an object has been changed */
    public static final int OBJECT_CHANGED = 2;

    /** the object that was actually modified */
    protected Object mod;
    /** what has happened? */
    protected int evType = OBJECT_CHANGED;
    
    public ModelEvent(ModelEventSource s) {
	super(s);
	mod = s;
    }

    public ModelEvent(ModelEventSource s, Object o) {
	super(s);
	mod = o;
    }

    public ModelEvent(ModelEventSource s, Object o, int et) {
	super(s);
	mod = o;
	evType = et;
    }

    public Object getModifiedObject() {
	return mod;
    }

    public int getEventType() {
	return evType;
    }
}
