(* 
 * SOOPL - The class library
 *
 * 2002 Maciek Makowski
 *)


(* Library contents:


-----------------
Boolean: Root - the class representing boolean values
  class attributes: 

  class methods: 

  object attributes: 
    val                              - stores the value of this boolean

  object methods: 
    and (primitive op)               - if if both val and op are true returns true
                                       otherwise returns false.
    ifElse (Object/block blt, blf)   - if val is true returns blt, otherwise blf.
    or (primitive op)                - if at least one of val, op is true returns true
                                       otherwise returns false.
    getValue ()                      - returns val
    setValue (primitive newVal)      - sets the val to newVal


-----------------
Integer: Root - the wrapper class for integers
  class attributes:

  class methods: 

  object attributes:
    val                          - stores the value of this Integer

  object methods: 
    add (primitive operand)      - adds the operand to val and returns the result
    equals (primitive value)     - returns true if val = value, false otherwise
    getValue ()                  - returns val
    multiply (primitive operand) - multiplies the operand by val and returns the result    
    setValue (primitive newVal)  - sets the val to newVal


-----------------
Root: Root - the base class
  class attributes: 

  class methods: 

  object attributes: 

  object methods: 


-----------------
TextIO: Root - the class for text input/output
  class attributes:

  class methods:  
    outputEOL ()              - prints out the newline character
    outputInt (primitive int) - prints out the argument

  object attributes:

  object methods:



*)

(* ----------------------------------------------------------------------- *)
(*                           ClassLibraryFUN                               *)
functor ClassLibraryFUN (structure SemDomains: SemanticDomainsSIG) : ClassLibrarySIG =
struct

type Env = SemDomains.Env
type S = SemDomains.S

open SemDomains

(*---------------- pomocnicze warto¶ci -----------------*)

val emptyEnv = mkEnv(fn id: Ide => unboundIde id);
val emptyS = mkS(fn l: Loc => mkSVEmpty, mkLoc(0));
val emptyCSt = mkCSt(fn s: S => s);
val emptyMethod = mkDVMethod(mkMethod(fn p: Loc =>
					 fn mkEnv(lr): Env =>
					    fn mkCExp(ce): CExp =>
					       mkCSt(fn mkS(s, ml): S =>
							let val mkCSt(cs) = ce (mkEVLoc p)
							in cs (mkS(s, ml))
							end)), 
			     []);

val return = mkIde("return");
val self = mkIde("self");

(* ------- standard library ------- *)
val libSize = 4;


(* Root *)
val rootClass = mkClass(mkLoc(0), emptyEnv, emptyEnv, [], emptyEnv);


(* Boolean *)
val andMethod = mkDVMethod(mkMethod(fn t: Loc =>
				       fn mkEnv(lr): Env =>
					  fn mkCExp(ce): CExp =>
					     mkCSt(fn mkS(s, ml): S =>
						      let val mkDVLoc(l1) = lr (mkIde "val")
							  val mkDVLoc(l2) = lr (mkIde "op")
							  val mkSVPrimitive(mkPrimitive(i1)) = s l1
							  val mkSVPrimitive(mkPrimitive(i2)) = s l2
							  val mkCSt(cs) = ce (mkEVPrimitive(mkPrimitive(if i1 <> 0 andalso i2 <> 0 then 1 
													else 0)))
						      in cs (mkS(s, ml))
						      end)),
			   [mkIde("op")]);
val ifElseMethod = mkDVMethod(mkMethod(fn t: Loc =>
					  fn mkEnv(lr): Env =>
					     fn mkCExp(ce): CExp =>
						mkCSt(fn mkS(s, ml): S =>
							 let val mkDVLoc(l) = lr (mkIde "val")
							     val mkSVPrimitive(mkPrimitive(v)) = s l
							     val mkDVLoc(rl) = (if v = 0 then lr (mkIde "blf")
										else lr (mkIde "blt"))
							     val mkCSt(cs) = ce (svtoev (s rl))
							 in cs (mkS(s, ml))
							 end)),
			      [mkIde("blt"), mkIde("blf")]);
val orMethod = mkDVMethod(mkMethod(fn t: Loc =>
				      fn mkEnv(lr): Env =>
					 fn mkCExp(ce): CExp =>
					    mkCSt(fn mkS(s, ml): S =>
						     let val mkDVLoc(l1) = lr (mkIde "val")
							 val mkDVLoc(l2) = lr (mkIde "op")
							 val mkSVPrimitive(mkPrimitive(i1)) = s l1
							 val mkSVPrimitive(mkPrimitive(i2)) = s l2
							 val mkCSt(cs) = ce (mkEVPrimitive(mkPrimitive(if i1 <> 0 orelse i2 <> 0 then 1 
												       else 0)))
						     in cs (mkS(s, ml))
						     end)),
			  [mkIde("op")]);
val getValueMethod = mkDVMethod(mkMethod(fn t: Loc =>
					    fn mkEnv(lr): Env =>
					       fn mkCExp(ce): CExp =>
						  mkCSt(fn mkS(s, ml): S =>
							   let val mkDVLoc(l) = lr (mkIde "val")
							       val mkSVPrimitive(pi) = s l
							       val mkCSt(cs) = ce (mkEVPrimitive pi)
							   in cs (mkS(s, ml))
							   end)),
				[]);
val setValueMethod = mkDVMethod(mkMethod(fn t: Loc =>
					    fn mkEnv(lr): Env =>
					       fn mkCExp(ce): CExp =>
						  mkCSt(fn mkS(s, ml): S =>
							   let val mkDVLoc(l1) = lr (mkIde "val")
							       val mkDVLoc(l2) = lr (mkIde "newVal")
							       val mkSVPrimitive(pi) = s l2
							       val mkCSt(cs) = ce (mkEVPrimitive pi)
							   in cs (mkS(fn al: Loc => if al = l1 then mkSVPrimitive(pi)
										    else s al,
								      ml))
							   end)),
				[mkIde("newVal")]);
val booleanClass = mkClass(mkLoc(0),
			   emptyEnv,
			   emptyEnv,
			   [mkIde("val")],
			   mkEnv(fn id: Ide => case id of
						   mkIde("and") => andMethod
						 | mkIde("ifElse") => ifElseMethod
						 | mkIde("or") => orMethod
						 | mkIde("getValue") => getValueMethod
						 | mkIde("setValue") => setValueMethod
						 | _ => unboundIde id));


(* TextIO *)
val outputIntMethod = mkDVMethod(mkMethod(fn p: Loc =>
					     fn mkEnv(lr): Env =>
						fn mkCExp(ce): CExp =>
						   mkCSt(fn mkS(s, ml): S =>
							    let val mkDVLoc(l) = lr (mkIde "int")
								val mkSVPrimitive(mkPrimitive(i)) = s l
								val u = TextIO.output(TextIO.stdOut, Int.toString i)
								val mkCSt(cs) = ce (mkEVLoc p)
							    in cs (mkS(s, ml))
							    end)), 
				 [mkIde("int")]);
val outputEOLMethod = mkDVMethod(mkMethod(fn p: Loc =>
					     fn mkEnv(lr): Env =>
						fn mkCExp(ce): CExp =>
						   mkCSt(fn mkS(s, ml): S =>
							    let	val u = TextIO.output(TextIO.stdOut, "\n")
								val mkCSt(cs) = ce (mkEVLoc p)
							    in cs (mkS(s, ml))
							    end)), 
				 []);
val textIOClass = mkClass(mkLoc(0), 
			  emptyEnv, 
			  mkEnv(fn id: Ide => case id of
						  mkIde("outputEOL") => outputEOLMethod
						| mkIde("outputInt") => outputIntMethod 
						| _ => unboundIde id), 
			  [], 
			  emptyEnv);


(* Integer *)
val getValueMethod = mkDVMethod(mkMethod(fn t: Loc =>
					    fn mkEnv(lr): Env =>
					       fn mkCExp(ce): CExp =>
						  mkCSt(fn mkS(s, ml): S =>
							   let val mkDVLoc(l) = lr (mkIde "val")
							       val mkSVPrimitive(pi) = s l
							       val mkCSt(cs) = ce (mkEVPrimitive pi)
							   in cs (mkS(s, ml))
							   end)),
			     []);
val setValueMethod = mkDVMethod(mkMethod(fn t: Loc =>
					    fn mkEnv(lr): Env =>
					       fn mkCExp(ce): CExp =>
						  mkCSt(fn mkS(s, ml): S =>
							   let val mkDVLoc(l1) = lr (mkIde "val")
							       val mkDVLoc(l2) = lr (mkIde "newVal")
							       val mkSVPrimitive(pi) = s l2
							       val mkCSt(cs) = ce (mkEVPrimitive pi)
							   in cs (mkS(fn al: Loc => if al = l1 then mkSVPrimitive(pi)
										    else s al,
								      ml))
							   end)),
				[mkIde("newVal")]);
val addMethod = mkDVMethod(mkMethod(fn t: Loc =>
				       fn mkEnv(lr): Env =>
					  fn mkCExp(ce): CExp =>
					     mkCSt(fn mkS(s, ml): S =>
						      let val mkDVLoc(l1) = lr (mkIde "val")
							  val mkDVLoc(l2) = lr (mkIde "operand")
							  val mkSVPrimitive(mkPrimitive(i1)) = s l1
							  val mkSVPrimitive(mkPrimitive(i2)) = s l2
							  val mkCSt(cs) = ce (mkEVPrimitive(mkPrimitive(i1 + i2)))
						      in cs (mkS(s, ml))
						      end)),
			   [mkIde("operand")]);
val multiplyMethod = mkDVMethod(mkMethod(fn t: Loc =>
					    fn mkEnv(lr): Env =>
					       fn mkCExp(ce): CExp =>
						  mkCSt(fn mkS(s, ml): S =>
							   let val mkDVLoc(l1) = lr (mkIde "val")
							       val mkDVLoc(l2) = lr (mkIde "operand")
							       val mkSVPrimitive(mkPrimitive(i1)) = s l1
							       val mkSVPrimitive(mkPrimitive(i2)) = s l2
							       val mkCSt(cs) = ce (mkEVPrimitive(mkPrimitive(i1 * i2)))
							   in cs (mkS(s, ml))
							   end)),
				[mkIde("operand")]);
val equalsMethod = mkDVMethod(mkMethod(fn t: Loc =>
					  fn mkEnv(lr): Env =>
					     fn mkCExp(ce): CExp =>
						mkCSt(fn mkS(s, ml): S =>
							 let val mkDVLoc(l1) = lr (mkIde "val")
							     val mkDVLoc(l2) = lr (mkIde "value")
							     val mkSVPrimitive(mkPrimitive(i1)) = s l1
							     val mkSVPrimitive(mkPrimitive(i2)) = s l2
							     val mkCSt(cs) = ce (if i1 = i2 then 	
										     mkEVPrimitive(mkPrimitive(1))
										 else
										     mkEVPrimitive(mkPrimitive(0)))
							 in cs (mkS(s, ml))
							 end)),
			      [mkIde("value")]);
val integerClass = mkClass(mkLoc(0),
			   emptyEnv,
			   emptyEnv,
			   [mkIde("val")],
			   mkEnv(fn id: Ide => case id of
						   mkIde("getValue") => getValueMethod
						 | mkIde("setValue") => setValueMethod
						 | mkIde("add") => addMethod
						 | mkIde("multiply") => multiplyMethod
						 | mkIde("equals") => equalsMethod
						 | _ => unboundIde id));


(* initial state *)
val libS = mkS(fn l: Loc => 
		  case l of 
		      mkLoc(0) => mkSVClass(rootClass) 
		    | mkLoc(1) => mkSVClass(textIOClass)
		    | mkLoc(2) => mkSVClass(integerClass)
		    | mkLoc(3) => mkSVClass(booleanClass)
		    | _ => mkSVEmpty, mkLoc(libSize));
    
(* initial environment *)
val libEnv = mkEnv(fn id: Ide => 
		      case id of
			  mkIde("Root") => mkDVLoc(mkLoc(0))
			| mkIde("TextIO") => mkDVLoc(mkLoc(1)) 
			| mkIde("Integer") => mkDVLoc(mkLoc(2))
			| mkIde("Boolean") => mkDVLoc(mkLoc(3))
			| _ => unboundIde id );
    
end (* ClassLibraryFUN *)

