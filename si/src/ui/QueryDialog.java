package ui;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import javax.swing.event.*;
import engine.*;
import engine.theory.*;
import engine.agents.Diagram;
import engine.agents.Agent;
import engine.parser.ParserSettings;

/**
 * A dialog for querying the diagram
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

class QueryDialog extends JDialog {


    /** the resources for this dialog */
    protected static ResourceBundle resources = ResourceBundle.getBundle("HardCAKE");

    /** the diagram which we query */
    protected Diagram model;
    /** the box with agent name */
    protected JComboBox lstAgent = new JComboBox();
    /** the box with relation name */
    protected JComboBox lstRelation;
    /** the argument combo boxes */
    protected Vector argLists;
    /** argument combos root */
    protected JPanel pnlArgs = new JPanel();
    /** response panel */
    protected JPanel pnlResp = new JPanel();
    /** response label */
    protected JLabel lblResponse = new JLabel();
    /** relation to be queried */
    protected Relation relation = null;
    /** arguments */
    protected Tuple arguments = null;

    public QueryDialog(JFrame fr, Diagram d) {
	super(fr, resources.getString("STR_CAP_DLG_QUERY"), true);

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

	Box b = Box.createHorizontalBox();
	// agent
	lstAgent.setFont(FontFactory.createFontType("THEORY_INPUT"));
	lstAgent.setEnabled(false);
	b.add(lstAgent);

	// relation
	lstRelation = new JComboBox(new Vector(model.getTheory().getRelations().keySet()));
	lstRelation.setFont(FontFactory.createFontType("THEORY_INPUT"));
	lstRelation.setEditable(true);
	lstRelation.addItemListener(new ItemListener() {
		public void itemStateChanged(ItemEvent ev) { 
		    if (ev.getStateChange() == ItemEvent.SELECTED)
			relationChanged(); 
		}
	    });
	lstRelation.setSelectedItem(null);
	b.add(lstRelation);

	// args panel
	JLabel lbl = new JLabel(ParserSettings.LBR);
	lbl.setFont(FontFactory.createFontType("THEORY_INPUT"));
	b.add(lbl);
	b.add(pnlArgs);
	lbl = new JLabel(ParserSettings.RBR);
	lbl.setFont(FontFactory.createFontType("THEORY_INPUT"));
	b.add(lbl);
	
	// query button
	JButton btn = new JButton(resources.getString("STR_CAP_BTN_QUERY"));
	btn.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent ev) { queryPressed(); }
	    });
	b.add(btn);
	main.add(b);
	main.getRootPane().setDefaultButton(btn);

	b = Box.createHorizontalBox();
	lbl = new JLabel(resources.getString("STR_CAP_LBL_RESPONSE"));
	lbl.setFont(FontFactory.createFontType("LABEL"));
	pnlResp.add(lbl);
	lblResponse.setFont(FontFactory.createFontType("LABEL"));	
	pnlResp.add(lblResponse);
	b.add(pnlResp);
	main.add(b);

	// control panel
	btn = new JButton(resources.getString("STR_CAP_BTN_CLOSE"));
	btn.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent ev) { closePressed(); }
	    });
	control.add(btn);

	setResizable(false);
    }

    
    protected void updateArgumentControls() {
	boolean first = true;
	argLists = new Vector();

	pnlArgs.removeAll();
	for (int i = 0; i < relation.getRank(); i++) {
	    JComboBox cbx = new JComboBox(new Vector(model.getTheory().getValues().keySet()));
	    cbx.setFont(FontFactory.createFontType("THEORY_INPUT"));
	    cbx.setEditable(true);
	    argLists.add(cbx);
	    // display
	    if (first) 
		first = false;
	    else {
		JLabel lbl = new JLabel(ParserSettings.SEP);
		lbl.setFont(FontFactory.createFontType("THEORY_INPUT"));
		pnlArgs.add(lbl);
	    }
	    pnlArgs.add(cbx);
	}
	pnlArgs.invalidate();
	super.pack();
    }

    /**
     * user pressed the close button
     */
    protected void closePressed() {
	dispose();
    }

    /**
     * user pressed the query button
     */
    protected void queryPressed() {
	SwingUtilities.invokeLater(new Runnable() {
		public void run() { doQuery(); }
	    });
    }

    protected void doQuery() {
	Enumeration e = argLists.elements();
	String resp;
	int qr;
	arguments = new Tuple(relation.getRank());

	while (e.hasMoreElements())
	    arguments.add(new Value((String)((JComboBox)e.nextElement()).getSelectedItem()));

	qr = model.query(null, relation, arguments);
	switch(qr) {
	case Agent.TRUE:
	    resp = resources.getString("STR_CAP_LBL_TRUE");
	    break;
	case Agent.FALSE:
	    resp = resources.getString("STR_CAP_LBL_FALSE");
	    break;
	case Agent.UNKNOWN:
	    resp = resources.getString("STR_CAP_LBL_UNKNOWN");
	    break;
	default:
	    System.err.println("Error: unexpected response: " + qr);
	    resp = "";
	}
	lblResponse.setText(resp);
	lblResponse.invalidate();
    }

    /**
     * selected element chas changed
     */
    protected void relationChanged() {
	relation = (Relation)model.getTheory().getRelations().get(lstRelation.getSelectedItem());
	if (relation != null)
	    SwingUtilities.invokeLater(new Runnable() {
		    public void run() { updateArgumentControls(); }
		});
    }
}
