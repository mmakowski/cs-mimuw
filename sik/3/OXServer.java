import java.util.*;
import java.awt.Point;
import OX.*;
import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;
import org.omg.PortableServer.*;
import org.omg.PortableServer.POA;


/* the board */
class BoardImpl extends BoardPOA {
    private ORB orb;
    private POA rootpoa;
    private Map cells = new HashMap();
    private int size_x, size_y;
	
    static int getOpponent(int ox) {
	if (ox == BoardConsts.CELL_O)
	    return BoardConsts.CELL_X;
	else
	    return BoardConsts.CELL_O;
    }
    
    public BoardImpl(ORB o, POA rp, int x, int y) {
	setORB(o);
	setRootPOA(rp);
	size_x = x;
	size_y = y;
	clear();
    }

    public void clear() {
	for (int i = 0; i < size_y; i++)
	    for (int j = 0; j < size_x; j++)
		cells.put(new Point(i, j), new Integer(BoardConsts.CELL_EMPTY));
    }

    public void setORB(ORB o) {
	orb = o;
    }
    
    public void setRootPOA(POA rp) {
	rootpoa = rp;
    }

    public int size_x() {
	return size_x;
    }

    public int size_y() {
	return size_y;
    }

    public int get_cell(int x, int y) {
	if (x >= size_x || y >= size_y || x < 0 || y < 0)
	    return -1;
	return ((Integer)cells.get(new Point(x, y))).intValue();
    }

    public int set_cell(int x, int y, int val) {
	if (x >= size_x || y >= size_y || x < 0 || y < 0)
	    return -1;
	Point pt = new Point(x, y);
	int cval = ((Integer)cells.get(pt)).intValue();
	if (cval != BoardConsts.CELL_EMPTY)
	    return cval;
	else
	    cells.put(pt, new Integer(val));
	return val;
    }

    public Board getRef() {
	BoardPOATie tie = new BoardPOATie(this, rootpoa);
	return tie._this(orb);
    }
}


/* the game */
class GameImpl extends GamePOA {
    private ORB orb;
    private POA rootpoa;
    private BoardImpl board;
    private Map presenters = new HashMap();
    private int lastRegistered = BoardConsts.CELL_EMPTY;

    public GameImpl(ORB o, POA rp, BoardImpl b) {
	setORB(o);
	setRootPOA(rp);
	board = b;
    }

    public void setORB(ORB o) {
	orb = o;
    }
    
    public void setRootPOA(POA rp) {
	rootpoa = rp;
    }

    public int register(GamePresenter pr) {
	if (lastRegistered >= BoardConsts.CELL_X) {
	    System.err.println("Can't register another player");
	    return 0;
	}
	int ox = ++lastRegistered;
	presenters.put(new Integer(ox), pr);
	System.err.println("presenter registered (" + String.valueOf(ox) + ")");
	if (lastRegistered == BoardConsts.CELL_O)
	    pr.updateBoard(board.getRef(), "You play O. Wait for your opponent to join", false);
	else {
	    pr.updateBoard(board.getRef(), "You play X. Opponent move", false);
	    ((GamePresenter)presenters.get(new Integer(BoardConsts.CELL_O))).update("Opponent joined the game. Your move",
										    true);
	}

	return ox;
    }

    public void unregister(int ox) {
	((GamePresenter)presenters.get(new Integer(BoardConsts.CELL_O))).disconnect();
	System.err.println("unregistered o");
	((GamePresenter)presenters.get(new Integer(BoardConsts.CELL_X))).disconnect();
	System.err.println("unregistered x");
	presenters.clear();
	System.err.println("Presenters unregistered");
	board.clear();
	System.err.println("Board cleared");
	lastRegistered = BoardConsts.CELL_EMPTY;
	System.err.println("Ready for new game");
    }

    public int make_move(int x, int y, int ox) {
	GamePresenter prc = (GamePresenter)presenters.get(new Integer(ox));
	GamePresenter pro = (GamePresenter)presenters.get(new Integer(BoardImpl.getOpponent(ox)));
	int rv = board.set_cell(x, y, ox);
	if (rv != ox) {
	    prc.update("This move is not allowed", true);
	} else {
	    prc.updateBoard(board.getRef(), "Opponent move", false);
	    pro.updateBoard(board.getRef(), "Your move", true);
	}
	return rv;
    }

    public int start_new() {
	board.clear();
	GamePresenter pro = (GamePresenter)presenters.get(new Integer(BoardConsts.CELL_O));
	GamePresenter prx = (GamePresenter)presenters.get(new Integer(BoardConsts.CELL_X));
	prx.updateBoard(board.getRef(), "New game started. Opponent move", false);
	pro.updateBoard(board.getRef(), "New game started. Your move", true);
	System.err.println("New game started");

	return 0;
    }

    public Game getRef() {
	GamePOATie tie = new GamePOATie(this, rootpoa);
	return tie._this(orb);
    }
}


public class OXServer {
    static GameImpl game;
    static BoardImpl board;

    public static void main(String args[]) {
	try {
	    // create and initialize the ORB
	    ORB orb = ORB.init(args, null);
	    
	    // get reference to rootpoa & activate the POAManager
	    POA rootpoa = POAHelper.narrow(orb.resolve_initial_references("RootPOA"));
	    rootpoa.the_POAManager().activate();

	    // initialize the presenter
	    board = new BoardImpl(orb, rootpoa, 3, 3);
	    game = new GameImpl(orb, rootpoa, board);
	    Game gref = game.getRef();
	    Board bref = board.getRef();

	    // get the root naming context
	    org.omg.CORBA.Object objRef = orb.resolve_initial_references("NameService");
	    // Use NamingContextExt instead of NamingContext. This is 
	    // part of the Interoperable naming Service.  
	    NamingContextExt ncRef = NamingContextExtHelper.narrow(objRef);
	    
	    // bind the Object References in Naming
	    NameComponent[] cn = new NameComponent[1];
	    cn[0] = new NameComponent("ox", "game_type_context");
	    try { ncRef.unbind(cn); } catch (Throwable t) {}
	    NamingContext oxNC = ncRef.bind_new_context(cn);
	    cn[0] = new NameComponent("3x3w3", "game_variant_context");
	    NamingContext v33NC = oxNC.bind_new_context(cn);
	    cn[0] = new NameComponent("game_1", "game_instance");
	    v33NC.rebind(cn, gref);
	    cn[0] = new NameComponent("board_1", "board_instance");
	    v33NC.rebind(cn, bref);

	    System.out.println("Server ready and waiting...");

	    // wait for invocations from clients
	    orb.run();
	    
	} catch (Exception e) {
	    System.out.println("ERROR : " + e) ;
	    e.printStackTrace(System.out);
	}
    }

}
