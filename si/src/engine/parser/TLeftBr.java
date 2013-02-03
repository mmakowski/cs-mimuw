package engine.parser;

/**
 * Token - left barcket
 *
 * @author Jerzy Szczepkowski
 * @version 1.1.1
 */

public class TLeftBr extends Token {
	public TLeftBr(int i) {
		super(i);
	}

	public boolean isLBR() {
		return true;
	}

	public String toString() {
		return "(";
	}

	public Token clone(int i) {
		return (Token) new TLeftBr(i);
	}
}
