package engine.parser;

/**
 * Token - identifier
 *
 * @author Jerzy Szczepkowski
 * @version 1.1.1
 */

public class TIdentifier extends Token {
	private String id;

	public TIdentifier(String s, int i) {
		super(i);
		id = s;
	}

	public boolean isID() {
		return true;
	}

	public String getId() {
		return id;
	}

	public String toString() {
		return "`"+id+"'";
	}

//	public Token clone(int i) {
//		return (Token) new TIdentifier(i);
//	}
}
