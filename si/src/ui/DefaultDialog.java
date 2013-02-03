package ui;

import java.awt.*;
import java.awt.event.*;
import java.util.ResourceBundle;
import javax.swing.*;
import javax.swing.event.*;
import engine.*;
import engine.theory.*;

/**
 * A dialog for editing default properties.
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

class DefaultDialog extends JDialog {

    /** no button has been pressed yet */
    public static final int NONE = -1;
    /** OK was pressed */
    public static final int OK = 0;
    /** Cancel was pressed */
    public static final int CANCEL = 1;

    /** the resources for this dialog */
    protected static ResourceBundle resources = ResourceBundle.getBundle("HardCAKE");

    /** the axiom that is requested to be edited */
    protected Default model;
    /** the theory to which the axiom belongs */
    protected Theory theory;
    /** indicates, which button did user press */
    protected int btnPressed;
    /** the box with prerequisites text */
    protected JTextField txtPre = new JTextField();
    /** the box with justification text */
    protected JTextField txtJust = new JTextField();
    /** the box with consequent text */
    protected JTextField txtCons = new JTextField();
    /** the box with priority */
    protected JTextField txtPriority = new JTextField();

    public DefaultDialog(JFrame fr, Default d, Theory t) {
	super(fr, resources.getString("STR_CAP_DLG_DEFAULT"), true);
	btnPressed = NONE;

	theory = t;
	model = d;

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
	// prerequisite
	Box b = Box.createHorizontalBox();
	JLabel lbl = new JLabel(resources.getString("STR_CAP_TXT_PRE"));
	lbl.setFont(FontFactory.createFontType("LABEL"));
	b.add(lbl);
	txtPre.setFont(FontFactory.createFontType("THEORY_INPUT"));
	if (model != null)
	    txtPre.setText(model.getPreString());
	b.add(txtPre);
	main.add(b);
	// justification
	b = Box.createHorizontalBox();
	lbl = new JLabel(resources.getString("STR_CAP_TXT_JUST"));
	lbl.setFont(FontFactory.createFontType("LABEL"));
	b.add(lbl);
	txtJust.setFont(FontFactory.createFontType("THEORY_INPUT"));
	if (model != null)
	    txtJust.setText(model.getJustString());
	b.add(txtJust);
	main.add(b);
	// listener that copies characters from justification to consequent
	txtJust.getDocument().addDocumentListener(new DocumentListener() {
		public void changedUpdate(DocumentEvent ev) {
		    txtCons.setText(txtJust.getText());
		}
		public void insertUpdate(DocumentEvent ev) {
		    changedUpdate(ev);
		}
		public void removeUpdate(DocumentEvent ev) {
		    changedUpdate(ev);
		}
	    });

	// consequent - disabled
	b = Box.createHorizontalBox();
	lbl = new JLabel(resources.getString("STR_CAP_TXT_CONS"));
	lbl.setFont(FontFactory.createFontType("LABEL"));
	b.add(lbl);
	txtCons.setFont(FontFactory.createFontType("THEORY_INPUT"));
	if (model != null)
	    txtCons.setText(model.getJustString());
	b.add(txtCons);
	main.add(b);
	txtCons.setEnabled(false);
	// priority
	b = Box.createHorizontalBox();
	lbl = new JLabel(resources.getString("STR_CAP_TXT_PRIORITY"));
	lbl.setFont(FontFactory.createFontType("LABEL"));
	b.add(lbl);
	txtPriority.setFont(FontFactory.createFontType("THEORY_INPUT"));
	if (model != null)
	    txtPriority.setText(String.valueOf(model.getPriority()));
	b.add(txtPriority);
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

    public Default getModel() {
	return model;
    }

    /**
     * user pressed the OK button
     */
    protected void okPressed() {
	int p = 0;
	
	// try to parse priority as integer
	try {
	    p = Integer.parseInt(txtPriority.getText());
	} catch (Exception ex) {}
	if (p < 0)
	    p = 0;    

	if (model == null)
	    model = theory.addDefault(txtPre.getText(), txtJust.getText(), txtCons.getText(), p);
	else 
	    model = theory.modifyDefault(model, txtPre.getText(), txtJust.getText(), txtCons.getText(), p);
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
