package ui;

import java.awt.*;
import java.awt.event.*;
import java.util.ResourceBundle;
import javax.swing.*;
import engine.*;
import engine.theory.*;

/**
 * A dialog for editing axiom properties.
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

class AxiomDialog extends JDialog {

    /** no button has been pressed yet */
    public static final int NONE = -1;
    /** OK was pressed */
    public static final int OK = 0;
    /** Cancel was pressed */
    public static final int CANCEL = 1;

    /** the resources for this dialog */
    protected static ResourceBundle resources = ResourceBundle.getBundle("HardCAKE");

    /** the axiom that is requested to be edited */
    protected Axiom model;
    /** the theory to which the axiom belongs */
    protected Theory theory;
    /** indicates, which button did user press */
    protected int btnPressed;
    /** the box with axiom text */
    protected JTextField txt = new JTextField();

    public AxiomDialog(JFrame fr, Axiom a, Theory t) {
	super(fr, resources.getString("STR_CAP_DLG_AXIOM"), true);
	btnPressed = NONE;

	theory = t;
	model = a;

	initComponents();
    }

    public void pack() {
	super.pack();

	//Center the window
	Point parentLoc = getOwner().getLocation();
	Dimension parentSize = getOwner().getSize();
	Dimension frameSize = getSize();
	if (frameSize.height > parentSize.height)
	    frameSize.height = parentSize.height;
	if (frameSize.width > parentSize.width)
	    frameSize.width = parentSize.width;
	setLocation(parentLoc.x + (parentSize.width - frameSize.width) / 2, 
		    parentLoc.y + (parentSize.height - frameSize.height) / 2);
    } 

    protected void initComponents() {
	Container content = getContentPane();
	JPanel main = new JPanel();
	JPanel control = new JPanel();

	content.setLayout(new BorderLayout());
	content.add(main, BorderLayout.CENTER);
	content.add(control, BorderLayout.SOUTH);

	// main panel
	main.setLayout(new BoxLayout(main, BoxLayout.Y_AXIS));
	Box b = Box.createHorizontalBox();
	JLabel lbl = new JLabel(resources.getString("STR_CAP_TXT_AXIOM"));
	lbl.setFont(FontFactory.createFontType("LABEL"));
	b.add(lbl);
	txt.setFont(FontFactory.createFontType("THEORY_INPUT"));
	if (model != null)
	    txt.setText(model.toString());
	b.add(txt);
	main.add(b);

	// control panel
	JButton btn = new JButton(resources.getString("STR_CAP_BTN_OK"));
	btn.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent ev) { okPressed(); }
	    });
	control.add(btn);
	control.getRootPane().setDefaultButton(btn);
	btn = new JButton(resources.getString("STR_CAP_BTN_CANCEL"));
	btn.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent ev) { cancelPressed(); }
	    });
	control.add(btn);

	setResizable(false);
    }

    /**
     * which button has been pressed
     */
    public int getButtonPressed() {
	return btnPressed;
    }

    public Axiom getModel() {
	return model;
    }

    /**
     * user pressed the OK button
     */
    protected void okPressed() {
	if (model == null)
	    model = theory.addAxiom(txt.getText());
	else 
	    model = theory.modifyAxiom(model, txt.getText());
	btnPressed = OK;
	dispose();
    }

    /**
     * user pressed the Cancel button
     */
    protected void cancelPressed() {
	btnPressed = CANCEL;
	dispose();
    }
}
