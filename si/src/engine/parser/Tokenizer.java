package engine.parser;

import java.util.*;
import engine.theory.*;
import engine.*;

/**
 * Tokenizer
 *
 * @author Jerzy Szczepkowski
 * @version 1.1.1
 */



public class Tokenizer {
	/** source string before tokenization */
	protected String str;
	/** string after tokenization */
	protected LinkedList tokens;

	protected HashMap settings;

	public String parseError(int i) {
		return (str.substring(0, i) + " <<<<< " + str.substring(i));
	}

	public Tokenizer(String s) {
		str = s + " ";	// last BLANK - Guard
		settings = new HashMap();
		settings.put((String) ParserSettings.SEP, new TSeparator(0));
		settings.put((String) ParserSettings.NEG, new TNegation(0));
		settings.put((String) ParserSettings.CON, new TConjunction(0));
		settings.put((String) ParserSettings.LBR, new TLeftBr(0));
		settings.put((String) ParserSettings.RBR, new TRightBr(0));
		settings.put((String) ParserSettings.VAR, new TVariable(0));
		settings.put((String) ParserSettings.STG, new TStrong(0));
		tokenize();
	}

	public Token getToken() {
		if (tokens.isEmpty()) 
			return null;
		else 
			return (Token) tokens.removeFirst();
	}

	private int str_pos = 0;

	protected String getTokenStr() {
		String str_tok = "";
	        int last = str.length();
	        while (str_pos < last && str.charAt(str_pos) == ' ')
	            str_pos++;
	        if (str_pos == last)
	            return "";
	        last = str.indexOf(" ", str_pos);
	        for (int l = last; l > str_pos; l--) {
	            str_tok = str.substring(str_pos, l);
	            if (settings.containsKey(str_tok)) {
	                str_pos = l;
	                return str_tok;
	            }
	        }
	        for (int j = str_pos; j < last; j++) {
	            for (int l = last; l > j; l--) {
	                str_tok = str.substring(j, l);
	                if (settings.containsKey(str_tok)) {
	                    str_tok = str.substring(str_pos, j);
	                    str_pos = j;
	                    return str_tok;
	                }
	            }
        	}
	        str_tok = str.substring(str_pos, last);
	        str_pos = last;
	        return str_tok;
	}


	/** translates source string to tokenized form */ 
	protected void tokenize() {
		tokens = new LinkedList();
		String str_tok;
		while ((str_tok = getTokenStr()) != "") {
		  	Token token = (Token) settings.get(str_tok);
		  	if (token == null) 
		  		tokens.add(new TIdentifier(str_tok, str_pos));
		  	else 
		  		tokens.add(token.clone(str_pos));
		}
	}



	/** Prints tokens. This method is for debuging only. TODO: remove */
	public void printTokens () {
		System.out.println("After tokenization:");
		Iterator it = tokens.iterator();
		Token tok;
		while (it.hasNext()) {
			tok = (Token) it.next();
			System.out.println("  : " + tok.toString());
		}
	}

}
