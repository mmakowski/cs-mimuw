package ui;

import java.awt.*;
import java.awt.event.*;
import java.util.ResourceBundle;
import javax.swing.*;

/**
 * The About HardCAKE dialog.
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

class AboutDialog extends JDialog {

    /** the resources for this dialog */
    protected static ResourceBundle resources = ResourceBundle.getBundle("HardCAKE");

    public AboutDialog(JFrame fr) {
	super(fr, resources.getString("STR_CAP_DLG_ABOUT"), true);
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
	JPanel content = (JPanel)getContentPane();
	JPanel info = new JPanel();
	JPanel control = new JPanel();

	content.setLayout(new BorderLayout());
	content.add(info, BorderLayout.CENTER);
	content.add(control, BorderLayout.SOUTH);

	// info panel
	info.setLayout(new BoxLayout(info, BoxLayout.Y_AXIS));
	JLabel title = new JLabel("HardCAKE");
	JLabel version = new JLabel(resources.getString("STR_PROG_VERSION"));
	JLabel copyright = new JLabel(resources.getString("STR_PROG_COPYRIGHT"));
	title.setAlignmentX(JLabel.CENTER_ALIGNMENT);
	title.setFont(FontFactory.createFontType("BIG"));
	version.setAlignmentX(JLabel.CENTER_ALIGNMENT);
	version.setFont(FontFactory.createFontType("LABEL"));
	copyright.setAlignmentX(JLabel.CENTER_ALIGNMENT);
	copyright.setFont(FontFactory.createFontType("LABEL"));
	info.add(title);
	info.add(version);
	info.add(copyright);

	// control panel
	JButton btn = new JButton(resources.getString("STR_CAP_BTN_OK"));
	btn.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent ev) { dispose(); }
	    });
	control.add(btn);
	control.getRootPane().setDefaultButton(btn);

	setResizable(false);
    }

}
