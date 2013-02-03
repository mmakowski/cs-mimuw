(* 
 * SEMANTYKA PROSTEGO, OBIEKTOWEGO JÊZYKA PROGRAMOWANIA (soopl)
 *
 * SWP - projekt zaliczeniowy 
 * autor: Maciek Makowski
 * 12.01.2002
 *)


load "TextIO";
load "Int";

(*-------------- dziedziny syntaktyczne --------------*)

datatype Id = mkId of string;
datatype Const = mkConst of int;
datatype Arguments = mkArguments of Expression list 
and      Expression = mkIdentifier of Id
		    | mkConstant of Const
		    | mkMessage of Expression * Id * Arguments
		    | mkCreation of Id;
datatype Statement = mkAssignment of Id * Expression
		   | mkReturn of Expression
		   | mkEvaluation of Expression;
datatype FormalArguments = mkFormalArguments of Id list;
datatype MethodBody = mkMethodBody of Statement list;
datatype MethodDecl = mkMethodDecl of Id * FormalArguments * MethodBody;
datatype MethodMap = mkMethodMap of MethodDecl list;
datatype Attributes = mkAttributes of Id list
datatype ClassBody = mkClassBody of Id * Attributes * MethodMap * Attributes * MethodMap; 
datatype ClassDecl = mkClassDecl of Id * ClassBody;
datatype ClassMap = mkClassMap of ClassDecl list;
datatype Initializer = mkInitializer of Expression;
datatype Program = mkProgram of ClassMap * Initializer
		 | mkErrProg;



(*----------------- dziedziny semantyczne -----------------*)

datatype Ide = mkIde of string;
datatype Loc = mkLoc of int;
datatype Primitive = mkPrimitive of int;
datatype EV = mkEVLoc of Loc
	    | mkEVPrimitive of Primitive;
datatype Env = mkEnv of Ide -> DV
and DV = mkDVLoc of Loc
       | mkDVCExp of CExp
       | mkDVMethod of Method * Ide list
       | mkDVUnbound
and Method = mkMethod of Loc -> Env -> CExp -> CSt
           | mkMethodBot
and CExp = mkCExp of EV -> CSt
and CSt = mkCSt of S -> S
and S = mkS of (Loc -> SV) * Loc
      | mkError 
and SV = mkSVClass of Class
       | mkSVObject of Object
       | mkSVPrimitive of Primitive
       | mkSVLoc of Loc
       | mkSVEmpty
and Class = mkClass of Loc * Env * Env * Ide list * Env
and Object = mkObject of Loc * Env;


(*----------------------- wyj±tki ------------------------*)
exception ValueNotInEV;
exception ValueNotInLoc;
exception BadProgram;
exception UnboundIde of string;
exception RuntimeError of string;
exception HIT;

fun cannotInterpret () = raise BadProgram;
fun runtimeError s = raise RuntimeError(s);
fun unboundIde (mkIde s) = raise UnboundIde(s);
fun printRuntimeError s = TextIO.output(TextIO.stdOut, "Runtime error " ^ s ^ "\n");
fun printUnboundIde s = TextIO.output(TextIO.stdOut, "Unbound identifier: " ^ s ^ "\n");

(*----------------- pomocnicze operacje ------------------*)

(* subst: ('a -> 'b) -> 'b -> 'a -> ('a -> 'b) *)
fun subst f v a = fn x => if x = a then v else f x;

(* append: 'a list -> 'a list -> 'a list *)
fun append [] l = l
  | append (h::t) l = h::(append t l);

(* inlist: 'a -> 'a list -> bool *)
fun inlist e [] = false
  | inlist e (h::t) = if e = h then true else inlist e t;

(* alloc: S -> Loc * S *)
fun alloc mkError = (mkLoc(~1), mkError)
  | alloc (mkS (s, fl)) = 
    let val incloc = fn (mkLoc(i)) => mkLoc(i + 1)
    in (fl, mkS(s, incloc fl)) 
    end;

(* makeenv: Ide list -> S -> Env * S *)
fun makeenv idl s = 
    if idl = [] then (mkEnv(fn id: Ide => unboundIde id), s)
    else let val (l, s') = alloc s
	     val (r', s'') = makeenv (tl idl) s'
	     val (mkEnv(rf)) = r' 
	 in (mkEnv (subst rf (mkDVLoc(l)) (hd idl)), s'')
	 end;

(* envcast: Env -> Env -> Env *)
fun envcast (mkEnv r1) (mkEnv r2) = 
    mkEnv(fn id: Ide => let val u = r2 id in r1 id end);

(* envidcast: Env -> Ide list -> Env *)
fun envidcast (mkEnv r) idl =
    mkEnv(fn id: Ide => if inlist id idl then r id
			else unboundIde id);

(* envcomb: Env -> Env -> Env *)
fun envcomb (mkEnv r1) (mkEnv r2) =
    mkEnv(fn id: Ide => let val u = r2 id in u end handle UnboundIde(s) => r1 id);

(* evtosv: EV -> SV *)
fun evtosv (mkEVLoc(loc)) = mkSVLoc(loc)
  | evtosv (mkEVPrimitive(p)) = mkSVPrimitive(p);

(* svtoev: SV -> EV *)
fun svtoev (mkSVLoc(loc)) = mkEVLoc(loc)
  | svtoev (mkSVPrimitive(p)) = mkEVPrimitive(p)
  | svtoev _ = raise ValueNotInEV;

(* dvtoev: DV -> EV *)
fun dvtoev (mkDVLoc(loc)) = mkEVLoc(loc)
  | dvtoev _ = raise ValueNotInEV;

(* dvtoloc: DV -> Loc *)
fun dvtoloc (mkDVLoc(loc)) = loc
  | dvtoloc _ = raise ValueNotInLoc;

(* evtoloc: EV -> Loc *)
fun evtoloc (mkEVLoc(loc)) = loc
  | evtoloc _ = raise ValueNotInLoc;

(* isobject: SV -> bool *)
fun isobject (v: SV) = let val mkSVObject(ob) = v in true end handle Bind => false;

(* fix: (Method -> Method) -> Method *)
(*TODO*) fun fix F = F mkMethodBot;

(*---------------- pomocnicze warto¶ci -----------------*)

val emptyEnv = mkEnv(fn id: Ide => unboundIde id);
val emptyS = mkS(fn l: Loc => mkSVEmpty, mkLoc(0));
val emptyCSt = mkCSt(fn s: S => s);
val return = mkIde("return");
val self = mkIde("self");

(* standardowa biblioteka *)
val libSize = 2;
(* Root *)
val rootClass = mkClass(mkLoc(0), emptyEnv, emptyEnv, [], emptyEnv);
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
val textIOClass = mkClass(mkLoc(0), 
			  emptyEnv, 
			  mkEnv(fn id: Ide => if id = mkIde("outputInt") then outputIntMethod else unboundIde id ), 
			  [], 
			  emptyEnv);
(* wej¶ciowy stan *)
val initS = mkS(fn l: Loc => 
		   case l of 
		       mkLoc(0) => mkSVClass(rootClass) 
		     | mkLoc(1) => mkSVClass(textIOClass)
		     | _ => mkSVEmpty, mkLoc(libSize));
(* wej¶ciowe ¶rodowisko *)
val initEnv = mkEnv(fn id: Ide => 
		       case id of
			   mkIde("Root") => mkDVLoc(mkLoc(0))
			 | mkIde("TextIO") => mkDVLoc(mkLoc(1)) 
			 | _ => unboundIde id );

(*---------------- funkcje semantyczne -----------------*)

(* MId: Id -> Ide *)
fun MId (mkId(str)) = mkIde(str)
    handle RuntimeError(s) => runtimeError s
	 | UnboundIde(s) => unboundIde (mkIde s)
	 | _ => runtimeError "MId";

(* MConst: Const -> Primitive *)
fun MConst (mkConst(c)) = mkPrimitive(c)
    handle RuntimeError(s) => runtimeError s
	 | UnboundIde(s) => unboundIde (mkIde s)
	 | _ => runtimeError "MConst";

(* MFormalArguments: FormalArguments -> Ide list *)
fun MFormalArguments (mkFormalArguments([])) = 
    ([] handle RuntimeError(s) => runtimeError s
	     | UnboundIde(s) => unboundIde (mkIde s)
	     | _ => runtimeError "MFormalArguments")
  | MFormalArguments (mkFormalArguments(idl)) = 
    (MId(hd idl)::(MFormalArguments(mkFormalArguments(tl idl)))
     handle RuntimeError(s) => runtimeError s
	  | UnboundIde(s) => unboundIde (mkIde s)
	  | _ => runtimeError "MFormalArguments");

(* MAttributes: Attributes -> Ide list *)
fun MAttributes (mkAttributes([])) = 
    ([] handle RuntimeError(s) => runtimeError s
	     | UnboundIde(s) => unboundIde (mkIde s)
	     |_ => runtimeError "MAttributes")
  | MAttributes (mkAttributes(idl)) = 
    (MId(hd idl)::(MAttributes(mkAttributes(tl idl)))
     handle RuntimeError(s) => runtimeError s
	  | UnboundIde(s) => unboundIde (mkIde s)
	  | _ => runtimeError "MAttributes");

(* MArguments: Expression list -> Env -> Env -> Env -> Env -> Ide list -> S -> S *)
fun MArguments (mkArguments([])) = 
    (fn r: Env => 
	fn r_a: Env => 
	   fn r_m: Env => 
	      fn r_loc: Env =>
		 fn ids: Ide list =>
		    mkCSt(fn s: S => s)
		    handle RuntimeError(s) => runtimeError s
			 | UnboundIde(s) => unboundIde (mkIde s)
			 |_ => runtimeError "MArguments")
  | MArguments (mkArguments(expl)) = 
    (fn r: Env => 
	fn r_a: Env => 
	   fn r_m: Env => 
	      fn r_loc: Env =>
		 fn ids: Ide list =>
		    if ids = [] then mkCSt(fn s: S => s)
		    else 
			let 
			    val (mkCSt(k)) = MArguments(mkArguments(tl expl)) r r_a r_m r_loc (tl ids) 
			    val (mkEnv(rf_loc)) = r_loc
			    val (mkDVLoc(loc)) = rf_loc (hd ids)
			in MExpression(hd expl) r r_a r_m 
				      (mkCExp(fn v: EV => mkCSt(fn (mkS(s, l)): S => k (mkS(subst s (evtosv v) loc, l )))))
			end handle RuntimeError(s) => runtimeError s
				 | UnboundIde(s) => unboundIde (mkIde s)
				 |_ => runtimeError "MArguments")

(* MExpression: Expression -> Env -> Env -> Env -> CExp -> CSt *)
and MExpression (mkIdentifier(id)) = 
    (fn (mkEnv(r)): Env => 
	fn (mkEnv(r_a)): Env =>
	   fn (mkEnv(r_m)): Env =>
	      fn (mkCExp(k)): CExp =>
		 mkCSt(fn (mkS(s, ll)): S =>
			  let 
			      val i = MId(id)
			      val mkCSt(ks) = let val u = r_a i 
					      in k (svtoev (s (dvtoloc (r_a i)))) 
					      end handle UnboundIde(id) =>k (dvtoev (r i))
			  in ks (mkS(s, ll))
			  end)
     handle RuntimeError(s) => runtimeError s
	  | UnboundIde(s) => unboundIde (mkIde s)
	  | _ => runtimeError "MExpression (identifier)")
  | MExpression (mkConstant(c)) = 
    (fn (mkEnv(r)): Env => 
	fn (mkEnv(r_a)): Env =>
	   fn (mkEnv(r_m)): Env =>
	      fn (mkCExp(k)): CExp =>
		 k (mkEVPrimitive(MConst(c)))
     handle RuntimeError(s) => runtimeError s
	  | UnboundIde(s) => unboundIde (mkIde s)
	  | _ => runtimeError "MExpression (constant)")
  | MExpression (mkMessage(exp, id, args)) = 
    (fn r: Env => 
	fn r_a: Env =>
	   fn r_m: Env =>
	      fn k: CExp =>
		 MExpression(exp) r r_a r_m
			    (mkCExp(fn t: EV =>
				       mkCSt(fn (mkS(s, ll)): S =>
						let 
						    val tloc = evtoloc t
						    val tar = s tloc
						in
						    if isobject tar then
							let val mkSVObject(mkObject(c, r_a')) = tar
							    val mkSVClass(mkClass(p, r_ca, r_cm, al, mkEnv(r_m'))) = s c
							    val mkDVMethod(mkMethod(m), ids) = (r_m' (MId(id)))
							    val (r_loc, s') = makeenv ids (mkS(s, ll))
							    val mkCSt(cs) = MArguments(args) r r_a r_m r_loc ids 
							    val s'' = cs s'
							    val mkCSt(cs') = m (evtoloc t) r_loc k 
							in cs' s''
							end
						    else 
							let val mkSVClass(mkClass(p, r_ca, mkEnv(r_cm), al, r_m')) = tar
							    val mkDVMethod(mkMethod(m), ids) = (r_cm (MId(id)))
							    val (r_loc, s') = makeenv ids (mkS(s, ll))
							    val mkCSt(cs) = MArguments(args) r r_a r_m r_loc ids 
							    val s'' = cs s'
							    val mkCSt(cs') = m tloc r_loc k
							in cs' s''
							end
						end)))
			    handle RuntimeError(s) => runtimeError s
				 | UnboundIde(s) => unboundIde (mkIde s)
				 | _ => runtimeError "MExpression (message)")
  | MExpression(mkCreation(id)) = 
    (fn (mkEnv(r)): Env => 
	fn r_a: Env =>
	   fn r_m: Env =>
	      fn (mkCExp(k)): CExp =>
		 mkCSt(fn s: S =>
			  let val (l, mkS(s', ll')) = alloc s
			      val mkSVClass(mkClass(p, r_ca, r_cm, al, r_m')) = s' (dvtoloc (r (MId(id))))
			      val (r_a, mkS(s'', ll'')) = makeenv al (mkS(s', ll'))
			      val mkCSt(cs) = k (mkEVLoc(l))
			  in cs (mkS(subst s'' (mkSVObject(mkObject(dvtoloc (r (MId(id))), r_a))) l, ll''))
			  end)
		 handle RuntimeError(s) => runtimeError s
		      | UnboundIde(s) => unboundIde (mkIde s)
		      | _ => runtimeError "MExpression (creation)");

(* MStatement: Statement -> Env -> Env -> Env -> CSt -> CSt *)
fun MStatement(mkAssignment(id, exp)) = 
    (fn r: Env =>
	fn (mkEnv(r_a)): Env =>
	   fn r_m: Env =>
	      fn (mkCSt(k)): CSt =>
		 MExpression(exp) r (mkEnv(r_a)) r_m 
			    (mkCExp(fn v: EV =>
				       mkCSt(
					     fn (mkS(s, ll)): S =>
						k (mkS(subst s (evtosv v) (dvtoloc (r_a (MId(id)))), ll)))))
			    handle RuntimeError(s) => runtimeError s
				 | UnboundIde(s) => unboundIde (mkIde s)
				 | _ => runtimeError "MStatement (assignment)")
  | MStatement(mkReturn(exp)) = 
    (fn r: Env =>
	fn (mkEnv(r_a)): Env =>
	   fn r_m: Env =>
	      fn k: CSt =>
		 MExpression(exp) r (mkEnv(r_a)) r_m 
			    (mkCExp(fn v: EV =>
				       mkCSt(fn (mkS(s, ll)): S =>
						let val mkDVCExp(mkCExp(ce)) = r_a return 
						    val mkCSt(cs) = ce v 
						in cs (mkS(s, ll))
						end))) 
			    handle RuntimeError(s) => runtimeError s
				 | UnboundIde(s) => unboundIde (mkIde s)
				 |_ => runtimeError "MStatement (return)")
  | MStatement(mkEvaluation(exp)) = 
    (fn r: Env =>
	fn r_a: Env =>
	   fn r_m: Env =>
	      fn k: CSt =>
		 MExpression(exp) r r_a r_m (mkCExp(fn v: EV => k))
		 handle RuntimeError(s) => runtimeError s
		      | UnboundIde(s) => unboundIde (mkIde s)
		      | _ => runtimeError "MStatement (evaluation)");

(* MMethodBody: MethodBody -> Env -> Env -> Env -> CSt *)
fun MMethodBody(mkMethodBody([])) = 
    (fn r: Env =>
	fn (mkEnv(r_a)): Env =>
	   fn r_m: Env =>
	      mkCSt(fn s: S =>
		       let 
			   val mkDVCExp(mkCExp(ce)) = r_a return
			   val mkDVLoc(l) = r_a self
			   val mkCSt(cs) = ce (mkEVLoc(l))
		       in cs s
		       end)
	      handle RuntimeError(s) => runtimeError s
		   | UnboundIde(s) => unboundIde (mkIde s)
		   | _ => runtimeError "MMethodBody")
  | MMethodBody(mkMethodBody(stl)) = 
    (fn r: Env =>
	fn r_a: Env =>
	   fn r_m: Env =>
	      MStatement(hd stl) r r_a r_m (MMethodBody(mkMethodBody(tl stl)) r r_a r_m)
	      handle RuntimeError(s) => runtimeError s
		   | UnboundIde(s) => unboundIde (mkIde s)
		   | _ => runtimeError "MMethodBody");

(* MMethodDecl: MethodDecl -> Env -> Env -> Env -> Ide list -> Env -> S -> Env *)
fun MMethodDecl(mkMethodDecl(id, args, mb)) = 
    (fn r: Env =>
	fn r_ca: Env =>
	   fn r_cm: Env =>
	      fn al: Ide list =>
		 fn (mkEnv(r_m)): Env =>
		    fn (mkS(s, ll)): S =>
		       let 
			   val fa = MFormalArguments(args)
			   val F = 
			       fn m: Method =>
				  mkMethod(fn t: Loc =>
					      fn (mkEnv(r_loc)): Env =>
						 fn k: CExp =>
						    mkCSt(fn (mkS(s, ll)): S =>
							     let val mkSVObject(mkObject(c, r_a)) = s t
								 val mkSVClass(mkClass(p, r_ca', r_cm', al', r_m')) = s c
								 val mkEnv(r_t) = envcast r_m' (mkEnv(r_m))
								 val mkCSt(cs) = MMethodBody(mb) r 
											    (envcomb (envidcast r_a al) 
												     (mkEnv(subst (subst r_loc (mkDVCExp(k)) return)
														  (mkDVLoc(t)) 
														  self)))
											    (mkEnv(subst r_t (mkDVMethod(m, fa)) (MId(id))))
							     in cs (mkS(s, ll))
							     end ))
		       in mkEnv(subst r_m (mkDVMethod(fix F, fa)) (MId(id)))
		       end handle RuntimeError(s) => runtimeError s
				| UnboundIde(s) => unboundIde (mkIde s)
				|_ => runtimeError "MMethodDecl");

(* MMethodMap: MethodMap -> Env -> Env -> Env -> Ide list -> Env -> S -> Env *)
fun MMethodMap(mkMethodMap([])) = 
    (fn r: Env =>
	fn r_ca: Env =>
	   fn r_cm: Env =>
	      fn al: Ide list =>
		 fn r_m: Env =>
		    fn s: S =>
		       r_m
		       handle RuntimeError(s) => runtimeError s
			    | UnboundIde(s) => unboundIde (mkIde s)
			    | _ => runtimeError "MMethodMap")
  | MMethodMap(mkMethodMap(mdl)) = 
    (fn r: Env =>
	fn r_ca: Env =>
	   fn r_cm: Env =>
	      fn al: Ide list =>
		 fn r_m: Env =>
		    fn s: S =>
		       MMethodMap(mkMethodMap(tl mdl)) r r_ca r_cm al (MMethodDecl(hd mdl) r r_ca r_cm al r_m s) s
		       handle RuntimeError(s) => runtimeError s
			    | UnboundIde(s) => unboundIde (mkIde s)
			    | _ => runtimeError "MMethodMap");

(* MClMethodDecl: MethodDecl -> Env -> Env -> Env -> S -> Env *)
fun MClMethodDecl(mkMethodDecl(id, args, mb)) = 
    (fn r: Env =>
	fn r_ca: Env =>
	   fn (mkEnv(r_cm)): Env =>
	      fn (mkS(s, ll)): S =>
		 let 
		     val fa = MFormalArguments(args)
		     val F = 
			 fn m: Method =>
			    mkMethod(fn t: Loc =>
					fn (mkEnv(r_loc)): Env =>
					   fn k: CExp =>
					      mkCSt(fn (mkS(s, ll)): S =>
						       let 
							   val mkSVClass(mkClass(p, r_ca', r_cm', al, r_m)) = s t
							   val mkEnv(r_t) = envcast r_cm' (mkEnv(r_cm))
							   val mkCSt(cs) = MMethodBody(mb) r 
										      (envcomb (envcast r_ca' r_ca) 
											       (mkEnv(subst (subst r_loc (mkDVCExp(k)) return)
													    (mkDVLoc(t)) 
													    self)))
										      (mkEnv(subst r_t (mkDVMethod(m, fa)) (MId(id))))
						       in cs (mkS(s, ll))
						       end))
		 in mkEnv(subst r_cm (mkDVMethod(fix F, fa)) (MId(id)))
		 end handle RuntimeError(s) => runtimeError s
			  | UnboundIde(s) => unboundIde (mkIde s)
			  | _ => runtimeError "MClMethodDecl");

(* MClMethodMap: MethodMap -> Env -> Env -> Env -> S -> Env *)
fun MClMethodMap(mkMethodMap([])) = 
    (fn r: Env =>
	fn r_ca: Env =>
	   fn r_cm: Env =>
	      fn s: S =>
		 r_cm
		 handle RuntimeError(s) => runtimeError s
		      | UnboundIde(s) => unboundIde (mkIde s)
		      | _ => runtimeError "MClMethodMap")
  | MClMethodMap(mkMethodMap(mdl)) = 
    (fn r: Env =>
	fn r_ca: Env =>
	   fn r_cm: Env =>
	      fn s: S =>
		 MClMethodMap(mkMethodMap(tl mdl)) r r_ca (MClMethodDecl(hd mdl) r r_ca r_cm s) s
		 handle RuntimeError(s) => runtimeError s
		      | UnboundIde(s) => unboundIde (mkIde s)
		      | _ => runtimeError "MClMethodMap");

(* MClassBody: ClassBody -> Env -> S -> Loc * S *)
fun MClassBody (mkClassBody(id, catr, cmm, atr, mm)) = 
    (fn (mkEnv(r)): Env =>
	fn (mkS(s, ll)): S =>
	   let val mkSVClass(mkClass(p, r_ca, r_cm, al, r_m)) = s (dvtoloc (r (MId(id))))
	       val (r_ca', s') = makeenv (MAttributes(catr)) (mkS(s, ll)) 
	       val r_ca'' = envcomb r_ca r_ca' 
	       val r_cm' = MClMethodMap(cmm) (mkEnv(r)) r_ca'' r_cm s'
	       val al' = append (MAttributes(atr)) al 
	       val r_m' = MMethodMap(mm) (mkEnv(r)) r_ca'' r_cm' al' r_m s'
	       val (l, mkS(s'', ll'')) = alloc s' 
	   in (l, 
	       mkS(subst s'' (mkSVClass(mkClass(dvtoloc (r (MId(id))), 
						r_ca'', 
						r_cm', 
						al', 
						r_m'))) l, ll''))
	   end handle RuntimeError(s) => runtimeError s
		    | UnboundIde(s) => unboundIde (mkIde s)
		    | _ => runtimeError "MClassBody");

(* MClassDecl: ClassDecl -> Env -> S -> Env * S *)
fun MClassDecl (mkClassDecl(id, cb)) = 
    (fn (mkEnv(r)): Env =>
	fn s: S =>
	   let val (l, s') = MClassBody(cb) (mkEnv(r)) s
	   in (mkEnv(subst r (mkDVLoc(l)) (MId(id))), s')
	   end handle RuntimeError(s) => runtimeError s
		    | UnboundIde(s) => unboundIde (mkIde s)
		    | _ => runtimeError "MClassDecl");

(* MInitializer: Initializer -> Env -> S -> S *)
fun MInitializer (mkInitializer(expr)) = 
    (fn r: Env =>
	fn s: S =>
	   let val mkCSt(k) = MStatement(mkEvaluation(expr)) r emptyEnv emptyEnv emptyCSt 
	   in k s
	   end handle RuntimeError(s) => runtimeError s
		    | UnboundIde(s) => unboundIde (mkIde s)
		    | _ => runtimeError "in initializer: probably message not understood");

(* MClassMap: ClassMap -> Env -> S -> Env * S *)
fun MClassMap (mkClassMap([])) = 
    (fn r: Env => 
	fn s: S => 
	   (r, s)
	   handle RuntimeError(s) => runtimeError s
		| UnboundIde(s) => unboundIde (mkIde s)
		| _ => runtimeError "MClassMap")
  | MClassMap (mkClassMap(cdl)) = 
    (fn r: Env => 
	fn s: S => 
	   let val (r', s') = MClassDecl(hd cdl) r s 
	   in MClassMap(mkClassMap(tl cdl)) r' s'
	   end handle RuntimeError(s) => runtimeError s
		    | UnboundIde(s) => unboundIde (mkIde s)
		    | _ => runtimeError "MClassMap");

(* MProgram: Program -> Env -> S -> Env * S *)
fun MProgram (mkErrProg) = cannotInterpret ()
  | MProgram (mkProgram(clm, init)) = 
    (fn r: Env => 
	 fn s: S => 
	    let val (r', s') = MClassMap(clm) r s 
	    in  (r', MInitializer(init) r' s')
	    end handle RuntimeError(s) => runtimeError s
		     | UnboundIde(s) => unboundIde (mkIde s)
		     | _ => runtimeError "MProgram");


val errorResult = (emptyEnv, mkError);
fun interpret prog = MProgram prog initEnv initS
    handle RuntimeError(s) => let val u = printRuntimeError s in errorResult end
	 | UnboundIde(s)   => let val u = printUnboundIde s in errorResult end
	 | _               => errorResult;

