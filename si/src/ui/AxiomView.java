package ui;

import java.util.*;
import javax.swing.*;
import engine.*;
import engine.theory.Axiom;

/**
 * A view containing the axioms.
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

class AxiomView extends JList {

    public AxiomView() {
	super();
	initComponents();
    }

    protected void initComponents() {
	setFont(FontFactory.createFontType("THEORY"));
    }

    /**
     * return the selected axiom
     */
    public Axiom getSelected() {
	return (Axiom)getSelectedValue();
    }

}
