package ui;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import engine.theory.Default;

/**
 * the pane that displays the defaults
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

class DefaultPane extends JPanel {

    /** the actual view for the defaults */
    protected DefaultView view = new DefaultView();
    /** the set of defaults to be displayed */
    protected Set model;

    public DefaultPane(String t) {
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
     * return the selected default
     */
    public Default getSelected() {
	return view.getSelected();
    }

    public void addMouseListener(MouseListener ml) {
	view.addMouseListener(ml);
    }
}
