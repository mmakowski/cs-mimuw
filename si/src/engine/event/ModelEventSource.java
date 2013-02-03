package engine.event;

import java.util.*;

/**
 * The base class for all the model classes
 * which should notify the listeners about
 * content changes.
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

public class ModelEventSource {

    /** the listener collection */
    protected Vector listeners = new Vector();

    public void addModelEventListener(ModelEventListener l) {
	listeners.add(l);
    }

    public void removeModelEventListener(ModelEventListener l) {
	listeners.remove(l);
    }

    public void fireModelChange(ModelEvent ev) {
	for (Enumeration e = listeners.elements(); e.hasMoreElements(); )
	    ((ModelEventListener)e.nextElement()).modelChanged(ev);
    }

}
