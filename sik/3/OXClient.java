import java.io.*;
import OX.*;
import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;
import org.omg.PortableServer.*;
import org.omg.PortableServer.POA;

class Exiter extends Thread {
    public void run() {
	System.exit(0);
    }
}


class MoveReader extends Thread {
    private Game game;
    private int symbol;

    public MoveReader(Game g, int s) {
	game = g;
	symbol = s;
    }

    protected void endGame() {
	game.unregister(symbol);
    }

    protected void restartGame() {
	game.start_new();
    }

    public void run() {
	BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
	int x = -1, y = -1;
	
	System.out.println("enter your move (enter 'n' for new game or 'q' for quit game as x)");
	// TODO: n, e

	while (x == -1) {
	    System.out.print("x: ");
	    try {
		String s = in.readLine();
		if (s.compareToIgnoreCase("n") == 0) {
		    restartGame();
		    return;
		}
		if (s.compareToIgnoreCase("q") == 0) {
		    endGame();
		    return;
		}
		x = Integer.parseInt(s);
	    } catch (Exception ex) {}
	}
	while (y == -1) {
	    System.out.print("y: ");
	    try {y = Integer.parseInt(in.readLine());} catch (Exception ex) {}
	}
	game.make_move(x, y, symbol);
    }

}


class GamePresenterImpl extends GamePresenterPOA {
    private ORB orb;
    private Game game;
    private Board board;
    private int symbol;

    public void setORB(ORB o) {
	orb = o;
    }

    public void setGame(Game g) {
	game = g;
    }

    public void setSymbol(int s) {
	symbol = s;
    }

    public void setBoard(Board b) {
	board = b;
    }

    public void updateBoard(Board brd, String msg, boolean move) {
	brd = board;

	for (int j = 0; j < brd.size_y(); j++) {
	    for (int i = 0; i < brd.size_x(); i++)
		switch (brd.get_cell(i, j)) {
		case BoardConsts.CELL_EMPTY:
		    System.out.print(".");
		    break;
		case BoardConsts.CELL_O:
		    System.out.print("o");
		    break;
		case BoardConsts.CELL_X:
		    System.out.print("x");
		    break;
		}
	    System.out.println();
	}

	update(msg, move);
    }

    public void update(String msg, boolean move) {
	System.out.println("> " + msg);
	if (move) {
	    MoveReader mr = new MoveReader(game, symbol);
	    mr.start();
	}
    }

    public void disconnect() {
	System.out.println("You have been disconnected");
	Exiter e = new Exiter();
	e.start();
    }

    public GamePresenter getRef(POA rootpoa) {
	GamePresenterPOATie tie = new GamePresenterPOATie(this, rootpoa);
	return tie._this(orb);
    }
}


public class OXClient {
    static Game game;
    static Board board;
    static GamePresenterImpl presenter;

    public static void main(String args[]) {
	try {
	    // create and initialize the ORB
	    ORB orb = ORB.init(args, null);
	    
	    // get reference to rootpoa & activate the POAManager
	    POA rootpoa = POAHelper.narrow(orb.resolve_initial_references("RootPOA"));
	    rootpoa.the_POAManager().activate();

	    // initialize the presenter
	    presenter = new GamePresenterImpl();
	    presenter.setORB(orb);
	    GamePresenter gpref = presenter.getRef(rootpoa);

	    // get the root naming context
	    org.omg.CORBA.Object objRef = orb.resolve_initial_references("NameService");
	    // Use NamingContextExt instead of NamingContext. This is 
	    // part of the Interoperable naming Service.  
	    NamingContextExt ncRef = NamingContextExtHelper.narrow(objRef);
	    
	    // resolve the Object References in Naming
	    NameComponent[] cn = new NameComponent[1];
	    cn[0] = new NameComponent("ox", "game_type_context");
	    NamingContext oxNC = NamingContextHelper.narrow(ncRef.resolve(cn));
	    cn[0] = new NameComponent("3x3w3", "game_variant_context");
	    NamingContext v33NC = NamingContextHelper.narrow(oxNC.resolve(cn));
	    cn[0] = new NameComponent("game_1", "game_instance");
	    game = GameHelper.narrow(v33NC.resolve(cn));
	    cn[0] = new NameComponent("board_1", "board_instance");
	    board = BoardHelper.narrow(v33NC.resolve(cn));

	    presenter.setGame(game);
	    presenter.setBoard(board);
	    int symb = game.register(gpref);
	    if (symb == 0) {
		System.err.println("There are already two players");
		System.exit(0);
	    }
	    presenter.setSymbol(symb);
	    // TODO: gramy (orb.run()?)
	    orb.run();

	    //game.unregister(symbol);

	} catch (Exception e) {
	    System.out.println("ERROR : " + e) ;
	    e.printStackTrace(System.out);
	}
    }

}
