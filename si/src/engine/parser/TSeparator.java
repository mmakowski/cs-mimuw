package engine.parser;

/**
 * Token - separator
 *
 * @author Jerzy Szczepkowski
 * @version 1.1.1
 */

public class TSeparator extends Token {
	public TSeparator(int i) {
		super(i);
	}

	public boolean isSEP() {
		return true;
	}

	public String toString() {
		return "&";
	}

	public Token clone(int i) {
		return (Token) new TSeparator(i);
	}
}
