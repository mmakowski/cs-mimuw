package engine.parser;

/**
 * Token - negation
 *
 * @author Jerzy Szczepkowski
 * @version 1.1.1
 */

public class TNegation extends Token {
	public TNegation(int i) {
		super(i);
	}

	public boolean isNEG() {
		return true;
	}

	public String toString() {
		return "~";
	}

	public Token clone(int i) {
		return (Token) new TNegation(i);
	}
}
