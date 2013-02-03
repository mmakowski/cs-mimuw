package engine.parser;

/**
 * Token - right barcket
 *
 * @author Jerzy Szczepkowski
 * @version 1.1.1
 */

public class TRightBr extends Token {
	public TRightBr(int i) {
		super(i);
	}

	public boolean isRBR() {
		return true;
	}

	public String toString() {
		return ")";
	}

	public Token clone(int i) {
		return (Token) new TRightBr(i);
	}
}
