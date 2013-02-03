package engine.theory;

import java.util.*;
import engine.*;
import engine.parser.*;

/**
 * Default's consequent
 *
 * @author Jerzy Szczepkowski
 * @version 1.1.1
 */

public class Consequent extends Atom {
//    Consequent(Relation r, boolean p, Tuple a) {
//       super();
//   }

	public Consequent(String str) {
		super();
		parseFromStr(str);
	}

	public Consequent(String str, Map vars) {
		super();
		parseFromStr(str, vars);
	}

	private void parseFromStr(String str) {
		PAtom pars = new PAtom(str, true);
		pars.parsAtom(this);
	}


	private void parseFromStr(String str, Map vars) {
		PAtom pars = new PAtom(str, true);
		pars.parsAtom(this, vars);
	}



}
