package engine.parser;

import java.util.*;
import engine.theory.*;
import engine.*;

/**
 * Parser - makes axioms from strings.
 *
 * @author Jerzy Szczepkowski
 * @version 1.1.1
 */


public class PAtom extends Parser {
    protected Tokenizer tok;
    boolean enableVars;

    public PAtom(String s, boolean ev) {
        tok = new Tokenizer(s);
        enableVars = ev;
    }

    /** parse string (Parser.str) to atom (ax) */
    public Atom parsAtom(Atom at) {
	Map vars = new HashMap();
	return parsAtom(at, vars);
    }

    /** parse string (Parser.str) to atom (ax) */
    public Atom parsAtom(Atom at, Map vars) {
        boolean pos = true;
        String rel_name = new String("");
        int state = 0;
        int rank = 0;
        Tuple tuple = new Tuple(0);
        Token t;
        while ((t = tok.getToken()) != null) {
            switch (state) {
              case 0:
                    if (t.isNEG()) {
                        pos = false;
                        state = 1;
                    } else if (t.isID()) {
                        pos = true;
        		rel_name = ((TIdentifier) t).getId();
			state = 2;
                    } else {
                    	// exception
                        throw new ParserException("id or " +
                            ParserSettings.NEG + " excepted" + tok.parseError(t.getPos()));
                    }
                    break;
              case 1: 
                    if (t.isID()) {
        		rel_name = ((TIdentifier) t).getId();
			state = 2;
                    } else {
                    	// exception
                        throw new ParserException("id token: id excepted: "
                            + tok.parseError(t.getPos()));
                    }
                    break;
              case 2:
                    if (t.isLBR()) {
                        state = 3;
                    } else {
                        // exception
                        throw new ParserException(""
                            + ParserSettings.LBR + " excepted: "
                            + tok.parseError(t.getPos()));
                    }
                    break;
              case 3:
                    if (t.isVAR()) {
                        if (!enableVars) {
                            throw new ParserException("Atom can't have variable: "
                                + tok.parseError(t.getPos()));
                        } else {
                            state = 4;
                        }
                    } else if (t.isID()) {
                        rank++;
                        String val_name = ((TIdentifier) t).getId();
                        tuple.add(new Value(val_name));
                        state = 5;
                    } else if (t.isRBR()) {
                        state = 6;
                    } else {
                        // exception
                        throw new ParserException("value or variable excepted: "
                            + tok.parseError(t.getPos()));
                    }
                    break;
              case 4:
                    if (t.isID()) {
                        rank++;
                        String var_name = ((TIdentifier) t).getId();
			Variable v = (Variable) vars.get(var_name);
			if (v == null) {
				v = new Variable(var_name);
				vars.put(var_name, v);
			}
                        tuple.add(v);
                        state = 5;
                    } else {
                        // exception
                        throw new ParserException("id excepted: "
                            + tok.parseError(t.getPos()));
                    }
                    break;
              case 5:
                    if (t.isRBR()) {
                        state = 6;
                    } else if (t.isSEP()) {
                        state = 3;
                    } else {
                        // exception
                        throw new ParserException(""
                            + ParserSettings.RBR + " or " + ParserSettings.SEP + " excepted: "
                            + tok.parseError(t.getPos()));
                    }
                    break;
              case 6:
                    // exception
                        throw new ParserException("Atom has been finished: "
                            + tok.parseError(0));
            }
        }

        if (rel_name.compareTo("") == 0)
//			System.out.println("dupsko");
                        throw new ParserException("Atom cannot be empty!");

        tuple.setRank(rank);
        at.initialize(rel_name, pos, tuple);

        return at;
    }
}
