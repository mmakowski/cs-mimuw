package ui;

import java.awt.Font;
import java.util.*;

/**
 * Produces the custom font initialized from the properties file
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

class FontFactory {

    /**
     * create the default font based on the properties file
     */
    public static Font createDefaultFont() {
	ResourceBundle res = ResourceBundle.getBundle("HardCAKE");
	String tk[] = tokenize(res.getString("WINDOW_FONT"));
	return createFont(tk[0], tk[1], tk[2]);
    }

    /**
     * create the font based on the properties file
     */
    public static Font createFontType(String t) {
	ResourceBundle res = ResourceBundle.getBundle("HardCAKE");
	String tk[] = tokenize(res.getString("WINDOW_FONT_" + t));
	return createFont(tk[0], tk[1], tk[2]);
    }

    /**
     *  chop string to an array of tokens 
     */
    protected static String[] tokenize(String input) {
        Vector v = new Vector();
	StringTokenizer t = new StringTokenizer(input);
	String tk[];

	while (t.hasMoreTokens())
	    v.addElement(t.nextToken());
    	tk = new String[v.size()];
    	for (int i = 0; i < tk.length; i++)
    	    tk[i] = (String)v.elementAt(i);
	
    	return tk;
    } 

    /**
     * create the font based on given parameters
     */
    protected static Font createFont(String name, String style, String size) {
	int st;

	if (style.equals("BOLD"))
	    st = Font.BOLD;
	else if (style.equals("BOLDITALIC"))
	    st = Font.BOLD | Font.ITALIC;
	else if (style.equals("ITALIC"))
	    st = Font.ITALIC;
	else
	    st = Font.PLAIN;

	return new Font(name, st, Integer.parseInt(size));
    }

}
