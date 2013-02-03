package ui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import engine.event.*;
import engine.theory.*;


/**
 * the pane that displays the contents of a default theory
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

public class TheoryPane extends JSplitPane {

    /** pane displaying axioms */
    protected AxiomPane axioms;
    /** pane displaying defaults */
    protected DefaultPane defaults;
    /** the theory to be displayed */
    protected Theory model;

    public TheoryPane(String at, String dt) {
	super();
	axioms = new AxiomPane(at);
	defaults = new DefaultPane(dt);
	initComponents();
    }

    protected void initComponents() {
	setOrientation(VERTICAL_SPLIT);
	setOneTouchExpandable(true);
	setDividerLocation(0.5);
	setResizeWeight(0.5);

	add(axioms, TOP);
	add(defaults, BOTTOM);
    }

    public Theory getModel() {
	return model;
    }

    public void setModel(Theory t) {
	model = t;
	axioms.setModel(model.getAxioms());
	defaults.setModel(model.getDefaults());
	model.addModelEventListener(new ModelEventListener() {
		public void modelChanged(ModelEvent ev) { handleModelChange(ev); }
	    });    
    }

    /**
     * return the selected axiom
     */
    public Axiom getSelectedAxiom() {
	return axioms.getSelected();
    }

    /**
     * return the selected default
     */
    public Default getSelectedDefault() {
	return defaults.getSelected();
    }

    /**
     * handle a change in the model
     */
    protected void handleModelChange(ModelEvent ev) {
	if (ev.getModifiedObject().getClass().getName().equals("engine.theory.Axiom"))
	    updateAxioms();
	else if (ev.getModifiedObject().getClass().getName().equals("engine.theory.Default"))
	    updateDefaults();
    }

    public void updateAxioms() {
	// this method might have been called from anywhere, so 
	// run the GUI update in the event dispatching thread
	SwingUtilities.invokeLater(new Runnable() {
		public void run() { axioms.updateView(); }
	    });
    }	

    public void updateDefaults() {
	// this method might have been called from anywhere, so 
	// run the GUI update in the event dispatching thread
	SwingUtilities.invokeLater(new Runnable() {
		public void run() { defaults.updateView(); }
	    });
    }	

    public void addAxiomMouseListener(MouseListener ml) {
	axioms.addMouseListener(ml);
    }

    public void addDefaultMouseListener(MouseListener ml) {
	defaults.addMouseListener(ml);
    }
}

