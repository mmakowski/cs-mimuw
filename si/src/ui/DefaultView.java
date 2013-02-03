package ui;

import java.util.*;
import javax.swing.*;
import engine.*;
import engine.theory.Default;

/**
 * A view containing the defaults.
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

class DefaultView extends JList {

    public DefaultView() {
	super();
	initComponents();
    }

    protected void initComponents() {
	setFont(FontFactory.createFontType("THEORY"));

	//Vector v = new Vector(16);
	//v.add("Bird($x) : Fly($x) / Fly($x)");
	//v.add(": Insect($x) / Insect($x)");
	//setListData(v);
    }

    /**
     * return the selected default
     */
    public Default getSelected() {
	return (Default)getSelectedValue();
    }

}
