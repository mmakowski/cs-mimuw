package engine.parser;


public abstract class Token {
	protected int pos;
	/** token's starting position in parsed string */

	public Token(int i) {
		pos = i;
	}

	public Token clone(int i) {
		throw new IllegalArgumentException();
	}

	public void setPos(int i) {
		pos = i;
	}

	public int getPos() {
		return pos;
	}

	public boolean isNEG() {
		return false;
	}

	public boolean isCON(){
		return false;
	}

	public boolean isSEP(){
		return false;
	}

	public boolean isLBR(){
		return false;
	}

	public boolean isRBR(){
		return false;
	}

	public boolean isVAR(){
		return false;
	}

	public boolean isSTG(){
		return false;
	}

	public boolean isID(){
		return false;
	}

	public String getId() {
		throw new IllegalArgumentException();
	}

	public abstract String toString();
}
