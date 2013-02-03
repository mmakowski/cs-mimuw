package ui;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;
import javax.swing.*;
import javax.swing.event.*;
import engine.agents.*;
import engine.event.*;
import engine.theory.*;

/**
 * The main window of the application.
 *
 * @author Maciek Makowski
 * @version 1.1.1
 */

public class MainFrame extends JFrame {

    /**
     * A command that can be issued by user 
     */
    protected class Command {
	/** the command description */
	protected String desc;
	/** the listener for the command */
	protected ActionListener listener;

	public Command(String d, ActionListener al) {
	    desc = new String(d);
	    listener = al;
	}

	public String getDescription() {
	    return desc;
	}

	public ActionListener getActionListener() {
	    return listener;
	}
    }

    /** the resources for this frame */
    protected static ResourceBundle resources = ResourceBundle.getBundle("HardCAKE");
    /** the hashtable conaining all the commands user can issue using menu, toolbar or accelerator keys */
    protected Hashtable commands = new Hashtable();
    /** the menu bar */
    protected JMenuBar menu = new JMenuBar();
    /** the theory view */
    protected TheoryPane theoryPane;
    /** the diagnostic view */
    protected TranscriptPane transView = new TranscriptPane();
    /** the diagnostic stream */
    protected Transcript trans = new Transcript();
    /** the CAKE diagram */
    protected Diagram diagram = null;
    /** current file */
    protected File currentFile = null;
    /** were there any changes since the last save */
    protected boolean needSave = false;

    public MainFrame() {
	try {
	    jbInit();
	} catch (Throwable t) {
	    t.printStackTrace();
	}
    }

    protected void jbInit() {
	initComponents();
    }

    protected void initComponents() {
	setDefaultCloseOperation(3);
	updateTitle();
	String tk[] = tokenize(resources.getString("WINDOW_SIZE"));
	setSize(new Dimension(Integer.parseInt(tk[0]), Integer.parseInt(tk[1])));

	//Center the window
	Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
	Dimension frameSize = getSize();
	if (frameSize.height > screenSize.height) {
	    frameSize.height = screenSize.height;
	}
	if (frameSize.width > screenSize.width) {
	    frameSize.width = screenSize.width;
	}
	setLocation((screenSize.width - frameSize.width) / 2, (screenSize.height - frameSize.height) / 2);

	prepareCommands();
	initMenu();
	setJMenuBar(menu);
	
	JPanel content = (JPanel)getContentPane();
	content.setLayout(new BorderLayout());
	
	JSplitPane splt = new JSplitPane();
	content.add(splt, BorderLayout.CENTER);
	splt.setOrientation(JSplitPane.VERTICAL_SPLIT);
	splt.setOneTouchExpandable(true);
	splt.setDividerLocation(0.5);
	splt.setResizeWeight(1);
	theoryPane = new TheoryPane(resources.getString("STR_CAP_PAN_AXIOM"), 
				    resources.getString("STR_CAP_PAN_DEFAULT"));
	theoryPane.addAxiomMouseListener(new MouseInputAdapter() {
		public void mouseClicked(MouseEvent ev) { axiomsMouseClicked(ev); }
	    });
	theoryPane.addDefaultMouseListener(new MouseInputAdapter() {
		public void mouseClicked(MouseEvent ev) { defaultsMouseClicked(ev); }
	    });
	//	content.add(theoryPane, BorderLayout.CENTER);
	splt.add(theoryPane, JSplitPane.TOP);

	JScrollPane scrl = new JScrollPane();
	scrl.getViewport().add(transView, null);
	splt.add(scrl, JSplitPane.BOTTOM);

	trans.setView(transView);
	System.setErr(trans);

	// create new document
	cmdNew(null);

	content.revalidate();
	
	System.err.println("---------------------------------------------------------");
	System.err.println("HardCAKE " + resources.getString("STR_PROG_VERSION"));
	System.err.println(resources.getString("STR_PROG_COPYRIGHT"));
	System.err.println("---------------------------------------------------------");
	System.err.println();

	pack();
    }

    /**
     * create all the commands available for the user to issue
     */
    protected void prepareCommands() {
	Command cmd;

	commands.clear();

	// about
	cmd = new Command(resources.getString("STR_CMD_ABOUT"),
			  new ActionListener() { 
			      public void actionPerformed(ActionEvent ev) { cmdAbout(ev); } 
			  });
	commands.put("cmdAbout", cmd);

	// add axiom
	cmd = new Command(resources.getString("STR_CMD_ADD_AXIOM"),
			  new ActionListener() { 
			      public void actionPerformed(ActionEvent ev) { cmdAddAxiom(ev); } 
			  });
	commands.put("cmdAddAxiom", cmd);

	// edit default
	cmd = new Command(resources.getString("STR_CMD_ADD_DEFAULT"),
			  new ActionListener() { 
			      public void actionPerformed(ActionEvent ev) { cmdAddDefault(ev); } 
			  });
	commands.put("cmdAddDefault", cmd);

	// compute extension
	cmd = new Command(resources.getString("STR_CMD_COMP_EXT"),
			  new ActionListener() { 
			      public void actionPerformed(ActionEvent ev) { cmdCompExt(ev); } 
			  });
	commands.put("cmdCompExt", cmd);

	// delete axiom
	cmd = new Command(resources.getString("STR_CMD_DEL_AXIOM"),
			  new ActionListener() { 
			      public void actionPerformed(ActionEvent ev) { cmdDelAxiom(ev); } 
			  });
	commands.put("cmdDelAxiom", cmd);

	// delete default
	cmd = new Command(resources.getString("STR_CMD_DEL_DEFAULT"),
			  new ActionListener() { 
			      public void actionPerformed(ActionEvent ev) { cmdDelDefault(ev); } 
			  });
	commands.put("cmdDelDefault", cmd);

	// edit axiom
	cmd = new Command(resources.getString("STR_CMD_EDIT_AXIOM"),
			  new ActionListener() { 
			      public void actionPerformed(ActionEvent ev) { cmdEditAxiom(ev); } 
			  });
	commands.put("cmdEditAxiom", cmd);

	// edit default
	cmd = new Command(resources.getString("STR_CMD_EDIT_DEFAULT"),
			  new ActionListener() { 
			      public void actionPerformed(ActionEvent ev) { cmdEditDefault(ev); } 
			  });
	commands.put("cmdEditDefault", cmd);

	// exit
	cmd = new Command(resources.getString("STR_CMD_EXIT"),
			  new ActionListener() { 
			      public void actionPerformed(ActionEvent ev) { cmdExit(ev); } 
			  });

	commands.put("cmdExit", cmd);

	// generate CAKE diagram
	cmd = new Command(resources.getString("STR_CMD_GEN_DIAGRAM"),
			  new ActionListener() { 
			      public void actionPerformed(ActionEvent ev) { cmdGenDiagram(ev); } 
			  });
	commands.put("cmdGenDiagram", cmd);

	// new
	cmd = new Command(resources.getString("STR_CMD_NEW"),
			  new ActionListener() { 
			      public void actionPerformed(ActionEvent ev) { cmdNew(ev); } 
			  });
	commands.put("cmdNew", cmd);

	// open
	cmd = new Command(resources.getString("STR_CMD_OPEN"),
			  new ActionListener() { 
			      public void actionPerformed(ActionEvent ev) { cmdOpen(ev); } 
			  });
	commands.put("cmdOpen", cmd);

	// query
	cmd = new Command(resources.getString("STR_CMD_QUERY"),
			  new ActionListener() { 
			      public void actionPerformed(ActionEvent ev) { cmdQuery(ev); } 
			  });
	commands.put("cmdQuery", cmd);

	// save
	cmd = new Command(resources.getString("STR_CMD_SAVE"),
			  new ActionListener() { 
			      public void actionPerformed(ActionEvent ev) { cmdSave(ev); } 
			  });
	commands.put("cmdSave", cmd);

	// save as
	cmd = new Command(resources.getString("STR_CMD_SAVE_AS"),
			  new ActionListener() { 
			      public void actionPerformed(ActionEvent ev) { cmdSaveAs(ev); } 
			  });
	commands.put("cmdSaveAs", cmd);

    }

    /**
     *  chop string to an array of tokens 
     */
    protected static String[] tokenize(String input) {
        Vector v = new Vector();
	StringTokenizer t = new StringTokenizer(input);
	String cmd[];

	while (t.hasMoreTokens())
	    v.addElement(t.nextToken());
    	cmd = new String[v.size()];
    	for (int i = 0; i < cmd.length; i++)
    	    cmd[i] = (String)v.elementAt(i);
	
    	return cmd;
    } 

    /**
     * create the menu bar based on user preferences 
     */
    protected void initMenu() {
	menu.setDoubleBuffered(true);
	menu.removeAll();

	Stack keyStack = new Stack();
	String[] mainKeys = tokenize(resources.getString("MENU"));

	for (int i = 0; i < mainKeys.length; i++) {
	    JMenu m = new JMenu(resources.getString("STR_MENU_" + mainKeys[i]));

	    menu.add(m);
	    String[] menuKeys = tokenize(resources.getString("MENU_" + mainKeys[i]));
	    keyStack.push(menuKeys);
	    keyStack.push(new Integer(0));
	    keyStack.push(m);

	    while (!keyStack.empty()) {
		JMenu pm = (JMenu)keyStack.pop();
		int ci = ((Integer)keyStack.pop()).intValue();
		String cmk[] = (String[])keyStack.pop();

		for (; ci < cmk.length; ci++) {
		    if (cmk[ci].equals("-")) { // separator
			pm.addSeparator();
		    } else if (commands.containsKey(cmk[ci])) { // command
			JMenuItem mi = new JMenuItem();
			Command cmd = (Command)commands.get(cmk[ci]);
			
			mi.setText(cmd.getDescription());
			mi.addActionListener(cmd.getActionListener());
			pm.add(mi);
		    } else { // submenu
			JMenu nm = new JMenu(resources.getString("STR_MENU_" + cmk[ci]));
			String[] nmk = tokenize(resources.getString("MENU_" + cmk[ci]));

			pm.add(nm);
			keyStack.push(cmk);
			keyStack.push(new Integer(++ci));
			keyStack.push(pm);
			keyStack.push(nmk);
			keyStack.push(new Integer(0));
			keyStack.push(nm);
			break;
		    }
		}
	    }
	}
    }


    /*
     * update the frame caption
     */
    protected void updateTitle() {
	// this method might have been called from anywhere, so 
	// run the GUI update in the event dispatching thread
	SwingUtilities.invokeLater(new Runnable() {
		public void run() {
		    if (currentFile != null)
			setTitle("HardCAKE - " + currentFile.getName());
		    else
			setTitle("HardCAKE");
		}
	    });
    }

    /*
     * the current file is about to close - is it ok to close it?
     */
    protected boolean closeCurrent() {
	if (!needSave)
	    return true;

	switch (JOptionPane.showConfirmDialog(this, 
					      resources.getString("STR_MSG_SAVE_CURRENT"),
					      resources.getString("STR_MSG_SAVE_CURRENT_TITLE"),
					      JOptionPane.YES_NO_CANCEL_OPTION,
					      JOptionPane.QUESTION_MESSAGE)) {
	case JOptionPane.YES_OPTION:
	    return cmdSave(null);
	case JOptionPane.NO_OPTION:
	    return true;
	case JOptionPane.CANCEL_OPTION:
	    return false;
	}
	return true;
    }


    /*-------- the event handlers --------*/
    protected void axiomsMouseClicked(MouseEvent ev) {
	if (ev.getClickCount() > 1) {
	    if (theoryPane.getSelectedAxiom() != null)
		cmdEditAxiom(null);
	    else
		cmdAddAxiom(null);
	}
    }

    protected void defaultsMouseClicked(MouseEvent ev) {
	if (ev.getClickCount() > 1) {
	    if (theoryPane.getSelectedDefault() != null)
		cmdEditDefault(null);
	    else
		cmdAddDefault(null);
	}
    }

    /*------- the command handlers -------*/

    /** handle the about command */
    protected void cmdAbout(ActionEvent ev) {
	AboutDialog dlg = new AboutDialog(this);
	dlg.pack();
	dlg.show();
	// the dialog itself performs all the necessary updates,
	// so we don't have to
    }

    /** handle the add axiom command */
    protected void cmdAddAxiom(ActionEvent ev) {
	AxiomDialog dlg = new AxiomDialog(this, null, theoryPane.getModel());
	dlg.pack();
	dlg.show();
    }

    /** handle the add default command */
    protected void cmdAddDefault(ActionEvent ev) {
	DefaultDialog dlg = new DefaultDialog(this, null, theoryPane.getModel());
	dlg.pack();
	dlg.show();
    }

    /** handle the compute extension command */
    protected void cmdCompExt(ActionEvent ev) {
	if (diagram != null)
	    diagram.computeExtension();
    }

    /** handle the delete axiom command */
    protected void cmdDelAxiom(ActionEvent ev) {
	Axiom a = theoryPane.getSelectedAxiom();
	if (a == null) 
	    JOptionPane.showMessageDialog(this, resources.getString("STR_MSG_NO_AXIOM_SELECTED"),
					  resources.getString("STR_MSG_NO_AXIOM_SELECTED_TITLE"),
					  JOptionPane.INFORMATION_MESSAGE);
	else
	    theoryPane.getModel().removeAxiom(a);
    }

    /** handle the delete default command */
    protected void cmdDelDefault(ActionEvent ev) {
	Default d = theoryPane.getSelectedDefault();
	if (d == null) 
	    JOptionPane.showMessageDialog(this, resources.getString("STR_MSG_NO_DEFAULT_SELECTED"),
					  resources.getString("STR_MSG_NO_DEFAULT_SELECTED_TITLE"),
					  JOptionPane.INFORMATION_MESSAGE);
	else {
	    theoryPane.getModel().removeDefault(d);
	}
    }

    /** handle the edit axiom command */
    protected void cmdEditAxiom(ActionEvent ev) {
	Axiom a = theoryPane.getSelectedAxiom();
	if (a == null) 
	    JOptionPane.showMessageDialog(this, resources.getString("STR_MSG_NO_AXIOM_SELECTED"),
					  resources.getString("STR_MSG_NO_AXIOM_SELECTED_TITLE"),
					  JOptionPane.INFORMATION_MESSAGE);
	else {
	    AxiomDialog dlg = new AxiomDialog(this, a, theoryPane.getModel());
	    dlg.pack();
	    dlg.show();
	    // the dialog itself performs all the necessary updates,
	    // so we don't have to
	}
    }

    /** handle the edit default command */
    protected void cmdEditDefault(ActionEvent ev) {
	Default d = theoryPane.getSelectedDefault();
	if (d == null) 
	    JOptionPane.showMessageDialog(this, resources.getString("STR_MSG_NO_DEFAULT_SELECTED"),
					  resources.getString("STR_MSG_NO_DEFAULT_SELECTED_TITLE"),
					  JOptionPane.INFORMATION_MESSAGE);
	else {
	    DefaultDialog dlg = new DefaultDialog(this, d, theoryPane.getModel());
	    dlg.pack();
	    dlg.show();
	    // the dialog itself performs all the necessary updates,
	    // so we don't have to
	}
    }

    /** handle the exit command */
    protected void cmdExit(ActionEvent ev) {
	if (closeCurrent())
	    System.exit(0);
    }

    /** handle the generate CAKE diagram command */
    protected void cmdGenDiagram(ActionEvent ev) {
	diagram = new Diagram(theoryPane.getModel());
    }

    /** handle the new command */
    protected void cmdNew(ActionEvent ev) {
	if (!closeCurrent())
	    return;
	currentFile = null;
	needSave = false;
	theoryPane.setModel(new Theory());
	theoryPane.getModel().addModelEventListener(new ModelEventListener() {
		public void modelChanged(ModelEvent ev) { needSave = true; }
	    });
	updateTitle();
    }

    /** handle the open command */
    protected void cmdOpen(ActionEvent ev) {
	JFileChooser chooser = new JFileChooser();
	ExampleFileFilter filter = new ExampleFileFilter();
	filter.addExtension("dth");
	filter.setDescription("Default theory files");
	chooser.setFileFilter(filter);
	int rv = chooser.showOpenDialog(this);

	if (rv == JFileChooser.APPROVE_OPTION) {
	    if (!chooser.getSelectedFile().exists()){
		JOptionPane.showMessageDialog(this, 
					      resources.getString("STR_MSG_FILE_NOT_FOUND").replaceAll("%1", chooser.getSelectedFile().getName()),
					      resources.getString("STR_MSG_FILE_NOT_FOUND_TITLE"),
					      JOptionPane.WARNING_MESSAGE);
		return;
	    }
	    if (!chooser.getSelectedFile().canRead()){
		JOptionPane.showMessageDialog(this, 
					      resources.getString("STR_MSG_FILE_NOT_READABLE").replaceAll("%1", chooser.getSelectedFile().getName()),
					      resources.getString("STR_MSG_FILE_NOT_READABLE_TITLE"),
					      JOptionPane.WARNING_MESSAGE);
		return;
	    }

	    File prev = currentFile;
	    currentFile = chooser.getSelectedFile();
	    try {
		FileInputStream in = new FileInputStream(currentFile);
		ObjectInputStream s = new ObjectInputStream(in);
		theoryPane.setModel((Theory)s.readObject());
	    } catch (Throwable th) {
		JOptionPane.showMessageDialog(this, resources.getString("STR_MSG_ERROR_OPENING").replaceAll("%1", th.getMessage()),
					      resources.getString("STR_MSG_ERROR_OPENING_TITLE"),
					      JOptionPane.WARNING_MESSAGE);
		currentFile = prev;
		return;
	    }

	    updateTitle();
	}
    }

    /** handle the query command */
    protected void cmdQuery(ActionEvent ev) {
	if (diagram == null) {
	    JOptionPane.showMessageDialog(this, resources.getString("STR_MSG_NO_DIAGRAM"),
					  resources.getString("STR_MSG_NO_DIAGRAM_TITLE"),
					  JOptionPane.WARNING_MESSAGE);
	} else {
	    QueryDialog dlg = new QueryDialog(this, diagram);
	    dlg.pack();
	    dlg.show();
	}
    }

    /** handle the save command */
    protected boolean cmdSave(ActionEvent ev) {
	if (currentFile == null)
	    return cmdSaveAs(ev);
	else {
	    try {
		FileOutputStream out = new FileOutputStream(currentFile);
		ObjectOutputStream s = new ObjectOutputStream(out);
		s.writeObject(theoryPane.getModel());
		s.flush();
	    } catch (IOException ex) {
		JOptionPane.showMessageDialog(this, resources.getString("STR_MSG_ERROR_SAVING").replaceAll("%1", ex.getMessage()),
					      resources.getString("STR_MSG_ERROR_SAVING_TITLE"),
					      JOptionPane.WARNING_MESSAGE);
		return false;
	    }
      	    needSave = false;
	    updateTitle();
	    return true;
	}
    }

    /** handle the save as command */
    protected boolean cmdSaveAs(ActionEvent ev) {
	JFileChooser chooser = new JFileChooser();
	ExampleFileFilter filter = new ExampleFileFilter();
	filter.addExtension("dth");
	filter.setDescription("Default theory files");
	chooser.setFileFilter(filter);
	if (currentFile != null)
	    chooser.setSelectedFile(currentFile);
	
	if (chooser.showSaveDialog(this) == JFileChooser.APPROVE_OPTION) {
	    if (chooser.getSelectedFile().exists()) {
		if (JOptionPane.showConfirmDialog(this, 
						  resources.getString("STR_MSG_FILE_EXISTS").replaceAll("%1", chooser.getSelectedFile().getName()),
						  resources.getString("STR_MSG_FILE_EXISTS_TITLE"),
						  JOptionPane.YES_NO_OPTION,
						  JOptionPane.QUESTION_MESSAGE) == JOptionPane.NO_OPTION)
		    return false;
	    }
	    currentFile = chooser.getSelectedFile();
	    return cmdSave(ev);
	} 
	return false;
    }

}
