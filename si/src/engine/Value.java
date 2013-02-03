package engine;
import java.io.*;

/**
 * A constant value passed as an argument to a relation.
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

public class Value implements Argument, java.io.Serializable {

    protected String val;

    public Value(String str) {
	val = new String(str);
    }

    public Value(Value v) {
        val = v.val;
    }

    public String getValue() {
	return val;
    }

    public boolean isVariable() {
	return false;
    }

    public String toString() {
	return val;
    }

}
