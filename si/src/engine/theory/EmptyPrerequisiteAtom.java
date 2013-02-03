package engine.theory;

import engine.*;
import engine.parser.ParserSettings;

/**
 * Default's prerequisite, when empty
 *
 * @author Jerzy Szczepkowski
 * @version 1.1.1
 */

public class EmptyPrerequisiteAtom extends PrerequisiteAtom {
    /** is prerequisite empty */
    public boolean isEmpty() {
        return true;
    }

    public EmptyPrerequisiteAtom() {
        super("", true, new Tuple(0), false);
    }

    public String toString() {
        return "";
    }
}
