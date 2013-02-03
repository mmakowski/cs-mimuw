package engine;
import engine.parser.*;
import java.io.*;

/**
 * A variable that can be passed as an argument to a relation.
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

public class Variable implements Argument, java.io.Serializable {

    protected String name;
    protected String val;

    public Variable(String n) {
	name = new String(n);
    }

    public String getValue() {
	return val;
    }
    
    public void setValue(String v) {
	val = new String(v);
    }

    public String getName() {
	return name;
    }

    public boolean isVariable() {
	return true;
    }

    public String toString() {
	return ParserSettings.VAR + name;
    }

}
