(* ----------------------------------------------------------------------- *)
(*                           SemanticDomainsFUN                            *)
functor SemanticDomainsFUN (structure Utils: UtilitiesSIG) : SemanticDomainsSIG =
struct

(*--------------- types ---------------*)

datatype Ide = mkIde of string
datatype Loc = mkLoc of int
datatype Primitive = mkPrimitive of int
datatype EV = mkEVLoc of Loc
	    | mkEVPrimitive of Primitive
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
       | mkSVBlock of Block
       | mkSVPrimitive of Primitive
       | mkSVLoc of Loc
       | mkSVEmpty
and Block = mkBlock of Env * Env * Env * (Env -> Env -> Env -> CSt -> CSt)
and Class = mkClass of Loc * Env * Env * Ide list * Env
and Object = mkObject of Loc * Env
type progresult = Env * S


(*------------- exceptions ------------*)

exception ValueNotInEV
exception ValueNotInLoc
exception UnboundIde of string

fun unboundIde (mkIde s) = raise UnboundIde(s)


(*------------- functions -------------*)

open Utils;

(* alloc: S -> Loc * S *)
fun alloc mkError = (mkLoc(~1), mkError)
  | alloc (mkS (s, fl)) = 
    let val incloc = fn (mkLoc(i)) => mkLoc(i + 1)
    in (fl, mkS(s, incloc fl)) 
    end

(* makeenv: Ide list -> S -> Env * S *)
fun makeenv idl s = 
    if idl = [] then (mkEnv(fn id: Ide => unboundIde id), s)
    else let val (l, s') = alloc s
	     val (r', s'') = makeenv (tl idl) s'
	     val (mkEnv(rf)) = r' 
	 in (mkEnv (subst rf (mkDVLoc(l)) (hd idl)), s'')
	 end

(* envcast: Env -> Env -> Env *)
fun envcast (mkEnv r1) (mkEnv r2) = 
    mkEnv(fn id: Ide => let val u = r2 id in r1 id end)

(* envidcast: Env -> Ide list -> Env *)
fun envidcast (mkEnv r) idl =
    mkEnv(fn id: Ide => if inlist id idl then r id
			else unboundIde id)

(* envcomb: Env -> Env -> Env *)
fun envcomb (mkEnv r1) (mkEnv r2) =
    mkEnv(fn id: Ide => let val u = r2 id in u end handle UnboundIde(s) => r1 id)

(* evtosv: EV -> SV *)
fun evtosv (mkEVLoc(loc)) = mkSVLoc(loc)
  | evtosv (mkEVPrimitive(p)) = mkSVPrimitive(p)

(* svtoev: SV -> EV *)
fun svtoev (mkSVLoc(loc)) = mkEVLoc(loc)
  | svtoev (mkSVPrimitive(p)) = mkEVPrimitive(p)
  | svtoev _ = raise ValueNotInEV

(* dvtoev: DV -> EV *)
fun dvtoev (mkDVLoc(loc)) = mkEVLoc(loc)
  | dvtoev _ = raise ValueNotInEV

(* dvtoloc: DV -> Loc *)
fun dvtoloc (mkDVLoc(loc)) = loc
  | dvtoloc _ = raise ValueNotInLoc

(* evtoloc: EV -> Loc *)
fun evtoloc (mkEVLoc(loc)) = loc
  | evtoloc _ = raise ValueNotInLoc

(* isobject: SV -> bool *)
fun isobject (v: SV) = let val mkSVObject(ob) = v in true end handle Bind => false

(* fix: (Method -> Method) -> Method *)
fun fix F: (Method) = F mkMethodBot


end (* SemanticDomainsFUN *)
