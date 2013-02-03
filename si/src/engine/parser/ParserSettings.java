package engine.parser;

import java.util.ResourceBundle;

/**
 * The customization settings for the parser.
 *
 * @author Maciek Makowski, Jerzy Szczepkowski
 * @version 1.1.1
 */

public class ParserSettings {
    /** the settings resources */
    protected static ResourceBundle res = ResourceBundle.getBundle("Parser");
    /** the negation symbol */
    public static final String NEG = res.getString("SYMBOL_NEG");
    /** the conjunction symbol */
    public static final String CON = res.getString("SYMBOL_CON");
    /** the left bracket symbol */
    public static final String LBR = res.getString("SYMBOL_LBR");
    /** the right bracket symbol */
    public static final String RBR = res.getString("SYMBOL_RBR");
    /** the value separator symbol */
    public static final String SEP = res.getString("SYMBOL_SEP");
    /** the value separator symbol */
    public static final String VAR = res.getString("SYMBOL_VAR");
    /** the strong prerequisite marker */
    public static final String STG = res.getString("SYMBOL_STG");
}
