package ui;

import java.awt.*;
import javax.swing.*;

/**
 * The diagnostic message view.
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

class TranscriptPane extends JTextArea {
    
    public TranscriptPane() {
	super();
	initComponents();
    }

    protected void initComponents() {
	setEditable(false);
	setFont(FontFactory.createFontType("TRANSCRIPT"));
    }

    public void append(String s) {
	super.append(s);
	Dimension d = getSize();
	scrollRectToVisible(new Rectangle(1, d.height, 1, 1));
    }
}
