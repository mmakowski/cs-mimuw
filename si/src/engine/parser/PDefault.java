package engine.parser;

import java.util.*;
import engine.theory.*;
import engine.*;

/**
 * Parser - makes defaults form strings.
 *
 * @author Jerzy Szczepkowski
 * @version 1.1.1
 */


public class PDefault extends Parser {
    protected Tokenizer tok;
    protected String pre;
    protected String just;
    protected String cons;


    public PDefault(String s) {
//        super(s);
    }

    public PDefault(String p, String j, String c) {
        pre = p;
        just = j;
        cons = c;
    }

    public Default parsDefault(Default df) {
        tok = new Tokenizer(pre);
	Map vars = new HashMap();

        boolean strong = false;
        boolean pos = true;
        String rel_name = " ";
        int state = -1;
        int rank = 0;
        LinkedList pre_list = new LinkedList();
        PrerequisiteAtom pa;
        Token t;
        Tuple tuple = new Tuple(0);
        while ((t = tok.getToken()) != null) {
            switch (state) {
              case -1:
                    if (t.isSTG()) {
                        strong = true;
                        state = 0;
                    } else if (t.isNEG()) {
                        pos = false;
                        state = 1;
                    } else if (t.isID()) {
                        pos = true;
                        rel_name = ((TIdentifier) t).getId();
                        state = 2;
                    } else {
                        // exception
                        throw new ParserException("id, " + ParserSettings.STG + " or " +
                            ParserSettings.NEG + " excepted: " + tok.parseError(t.getPos()));
                    }
                    break;
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
                            ParserSettings.NEG + " excepted: " + tok.parseError(t.getPos()));
                    }
                    break;
              case 1: 
                    if (t.isID()) {
                        rel_name = ((TIdentifier) t).getId();
                        state = 2;
                    } else {
                        // exception
                        throw new ParserException("id excepted: "
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
                        state = 4;
                    } else if (t.isID()) {
                        String val_name = ((TIdentifier) t).getId();
                        rank++;
                        tuple.add(new Value(val_name));
                        state = 5;
                    } else if (t.isRBR()) {
                        state = 6;
                    } else {
                        // exception
                        throw new ParserException(""
                            + tok.parseError(t.getPos()));
                    }
                    break;
              case 4:
                    if (t.isID()) {
                        String var_name = ((TIdentifier) t).getId();
			Variable v = (Variable) vars.get(var_name);
			if (v == null) {
				v = new Variable(var_name);
				vars.put(var_name, v);
			}
                        rank++;
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
                    if (t.isCON()) {
                        tuple.setRank(rank);
                        pa = new PrerequisiteAtom(rel_name, pos, tuple, strong);
                        pre_list.add(pa);
                        tuple = new Tuple(0);
                        rank = 0;
                        strong = false;
                        pos = true;
                        state = -1;
                    } else {
                        throw new ParserException("Atom has been finished, " 
                            + ParserSettings.CON + " excepted: "
                            + tok.parseError(t.getPos()));
                        // exception
                    }
                    break;
            }
        }  

       	tuple.setRank(rank);
       	if (rel_name.compareTo(" ") == 0)
       	    pa = (PrerequisiteAtom) new EmptyPrerequisiteAtom();
        else 
       	    pa = new PrerequisiteAtom(rel_name, pos, tuple, strong);

        pre_list.add(pa);
	Consequent c = new Consequent(cons, vars);
	Consequent j = new Consequent(just, vars);
	df.initialize(new HashSet((Collection) pre_list), j, c, new HashSet(vars.values()));
        return df;
    }


}
