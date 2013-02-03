package engine.parser;

/**
 * Token - strong prerequisite
 *
 * @author Jerzy Szczepkowski
 * @version 1.1.1
 */

public class TStrong extends Token {
	public TStrong(int i) {
		super(i);
	}

	public boolean isSTG() {
		return true;
	}

	public String toString() {
		return "(";
	}

	public Token clone(int i) {
		return (Token) new TStrong(i);
	}
}
