package ui;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import engine.theory.Axiom;

/**
 * the pane that displays the axioms
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

class AxiomPane extends JPanel {

    /** the actual view for the axioms */
    protected AxiomView view = new AxiomView();
    /** the set of axioms to be displayed */
    protected Set model;

    public AxiomPane(String t) {
	super();
	initComponents(t);
    }

    protected void initComponents(String t) {
	setLayout(new BorderLayout());
	
	JPanel titleP = new JPanel();
	titleP.setPreferredSize(new Dimension(0, 20));
	titleP.setLayout(new BorderLayout());
	JLabel title = new JLabel(t);
	//title.setFont();
	titleP.add(title, BorderLayout.CENTER);

	add(titleP, BorderLayout.NORTH);

	JScrollPane scrl = new JScrollPane();
	scrl.getViewport().add(view, null);
	add(scrl, BorderLayout.CENTER);
    }

    public void setModel(Set s) {
	model = s;
	updateView();
    }

    public void updateView() {
	view.setListData(new Vector(model));
    }

    /**
     * return the selected axiom
     */
    public Axiom getSelected() {
	return view.getSelected();
    }
    
    public void addMouseListener(MouseListener ml) {
	view.addMouseListener(ml);
    }
}
