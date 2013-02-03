package engine.theory;

import engine.*;
import engine.parser.ParserSettings;

/**
 * Default's prerequisite
 *
 * @author Jerzy Szczepkowski
 * @version 1.1.1
 */

public class PrerequisiteAtom extends Atom {
    /** is prerequisite storng (does it need "sure knowlage") */
    protected boolean strong;

    /** is prerequisite empty */
    public boolean isEmpty() {
        return false;
    }

    public PrerequisiteAtom(Relation r, boolean p, Tuple t, boolean s) {
    }

    public PrerequisiteAtom(String r_name, boolean p, Tuple t, boolean s) {
        super.initialize(r_name, p, t);
        strong = s;
    }

    public String toString() {
	if (strong)
            return ParserSettings.STG + super.toString();
        else
            return super.toString();
    }

    public boolean getStrong() {
	return strong;
    }
}
