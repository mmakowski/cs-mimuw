import java.awt.Toolkit;
import java.util.ResourceBundle;
import javax.swing.*;
import engine.*;
import engine.agents.*;
import engine.theory.*;
import engine.parser.*;
import ui.*;

/**
 * the HardCAKE application
 *
 * @author Maciek Makowski
 * @author Jurek Szczepkowski
 * @version 1.1.1
 */

class HardCAKE {

    /** the resources for this application */
    protected static ResourceBundle resources = ResourceBundle.getBundle("HardCAKE");

    public static void main(String args[]) {
	try {
	    // REQUIRE 1.4
	    JDialog.setDefaultLookAndFeelDecorated(true);
	    // REQUIRE 1.4
	    JFrame.setDefaultLookAndFeelDecorated(true);
	    // REQUIRE 1.4
	    Toolkit.getDefaultToolkit().setDynamicLayout(true); 	
	    UIManager.setLookAndFeel(resources.getString("WINDOW_LOOK_AND_FEEL"));
	} catch (Throwable t) {
	    t.printStackTrace();
	}
	new HardCAKE();
    }

    public HardCAKE() {
	MainFrame frame = new MainFrame();

	frame.validate();
	frame.setVisible(true);
    }
}
