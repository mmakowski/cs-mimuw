package engine.parser;

/**
 * Token - variable
 *
 * @author Jerzy Szczepkowski
 * @version 1.1.1
 */

public class TVariable extends Token {
	public TVariable(int i) {
		super(i);
	}

	public boolean isVAR() {
		return true;
	}

	public String toString() {
		return "$";
	}

	public Token clone(int i) {
		return (Token) new TVariable(i);
	}
}
