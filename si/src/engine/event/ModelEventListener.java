package engine.event;

import java.util.*;

/**
 * The interface for classes interested in handling
 * model change events.
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

public interface ModelEventListener {
    /** the method that gets called when the model changes */
    public void modelChanged(ModelEvent ev);
}
