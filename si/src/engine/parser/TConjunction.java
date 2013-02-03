package engine.parser;

/**
 * Token - conjunction
 *
 * @author Jerzy Szczepkowski
 * @version 1.1.1
 */

public class TConjunction extends Token {
	public TConjunction(int i) {
		super(i);
	}

	public boolean isCON() {
		return true;
	}

	public String toString() {
		return "&";
	}

	public Token clone(int i) {
		return (Token) new TConjunction(i);
	}
}
