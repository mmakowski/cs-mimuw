(*
 *    SOOPL -- SIMPLE OBJECT-ORIENTED PROGRAMMING LANGUAGE
 *    A programming language interpreter
 * 
 *    Functional Programming Course Assignment
 *
 *    author: Maciek Makowski (189188) 
 *    version: 0.1.2
 *    modified on: 07.05.2002
 *
 *    Running programs: in the SML console type:
 *      SOOPL.run <progcode>
 *    where <progcode> is the string containing program source code.
 *    Useful command for debugging:
 *      SOOPL.dump <result>
 *    where <result> is the value returned by SOOPL.run. It prints
 *    the global environment and object memory (program state) the
 *    <result> contains.
 *)


(* ======================================================================= *)
(*                              specification                              *)


(* miscellaneous functions *)
signature UtilitiesSIG = 
sig
    val subst: (''a -> 'b) -> 'b -> ''a -> (''a -> 'b)
(*    axiom forall f =>
	      forall v => 
                  forall a =>
                      (subst f v a a = v) andalso
		      (forall b => (b <> a) implies (subst f v a b = f b))*)

    val inlist: ''a -> ''a list -> bool
(*    axiom forall e => not (inlist e [])
    axiom forall e =>
	      forall l => 
                  (inlist e l) implies (hd l = e orelse inlist e (tl l))*)

    val append: 'a list -> 'a list -> 'a list
(*    axiom forall l1 =>
              forall l2 =>
                  forall e =>
                      ((inlist e l1) implies (inlist e (append l1 l2))) andalso
		      ((inlist e l2) implies (inlist e (append l1 l2)))*)
end



(* tokens *)
signature TokensSIG =
sig
    datatype token = IDtkn of string
	           | NUMtkn of int
		   | BLKENDtkn
		   | BLKSTARTtkn
		   | CLASStkn
		   | EVALtkn
		   | EXECtkn
		   | EXTENDStkn
		   | OBJVARStkn
		   | CLASSVARStkn
		   | OBJMETHODtkn
		   | CLASSMETHODtkn
		   | ARGStkn
		   | IStkn
		   | ASSIGNtkn
		   | MESSAGEtkn
		   | NEWtkn
		   | RETURNtkn
		   | SENDtkn
		   | DOTtkn
		   | INITtkn
		   | WITHtkn

    val tokenName: token -> string		
end



(* syntactic domains *)
signature SyntacticDomainsSIG =
sig
    datatype Id = mkId of string;
    datatype Const = mkConst of int
    datatype Arguments = mkArguments of Expression list 
    and      Expression = mkIdentifier of Id
                        | mkConstant of Const
			| mkBlockDecl of Statement list
			| mkMessage of Expression * Id * Arguments
			| mkCreation of Id
    and      Statement = mkAssignment of Id * Expression
		       | mkReturn of Expression
		       | mkEvaluation of Expression
		       | mkExecution of Expression
    datatype FormalArguments = mkFormalArguments of Id list
    datatype MethodBody = mkMethodBody of Statement list
    datatype MethodDecl = mkMethodDecl of Id * FormalArguments * MethodBody
    datatype MethodMap = mkMethodMap of MethodDecl list
    datatype Attributes = mkAttributes of Id list
    datatype ClassBody = mkClassBody of Id * Attributes * MethodMap * Attributes * MethodMap
    datatype ClassDecl = mkClassDecl of Id * ClassBody
    datatype ClassMap = mkClassMap of ClassDecl list
    datatype Initializer = mkInitializer of Expression
    datatype Program = mkProgram of ClassMap * Initializer
		     | mkErrProg
end



(* semantic domains *)
signature SemanticDomainsSIG =
sig
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

    exception ValueNotInEV
    exception ValueNotInLoc
    exception UnboundIde of string
    val unboundIde: Ide -> 'a

    (* allocating the new location in memory *)
    val alloc: S -> Loc * S
(*    axiom forall (s, ll) =>
              forall (l, s') => 
                  alloc (mkS(s, ll)) == (l, s') implies s l == mkSVEmpty*)

    (* creating the environment based on identifier list *)
    val makeenv: Ide list -> S -> Env * S
(*    local fun is_in (e, []) = false
              | is_in (e, x::l) = (e = x) orelse is_in (e, l)
    in axiom forall il =>
                 forall s => 
                     forall (mkEnv r, s') =>
                         makeenv il s == (mkEnv r, s') implies 
			 (forall id => 
                              exists l =>
                                  is_in id il implies r id = mkDVLoc l)
    end*)

    (* narrowing the environment so that only the identifiers bound in second environment
       are bound in the resulting environment *)
    val envcast: Env -> Env -> Env
(*    axiom forall r1 =>
              forall r2 => 
                  forall id =>
                      (forall str =>
                           (r2 id raises UnboundIde(str) => true implies
                           forall r =>
                               mkEnv r == envcast (mkEnv r1) (mkEnv r2) implies
                               (r id raises UnboundIde(str) => true))) andalso
                      (forall dv =>
                          r2 id == dv implies 
                          forall r => 
                              mkEnv r == envcast (mkEnv r1) (mkEnv r2) implies r id == r1 id)*)

    (* narrowing the environment so that only the identifiers present in the identifier
       list are bound in the resulting environment *)
    val envidcast: Env -> Ide list -> Env
(*    local fun is_in (e, []) = false
              | is_in (e, x::l) = (e = x) orelse is_in (e, l)
    in axiom forall r1 =>
                 forall il => 
                     forall id =>
                         (exists str =>
                              (not (is_in id il) implies
                              envidcast (mkEnv r1) il id raises UnboundIde(str))) andalso
                         (is_in id il implies envidcast (mkEnv r1) il id = r1 id)
    end*)

    (* combining two environments *)
    val envcomb: Env -> Env -> Env 
(*    axiom forall r1 =>
              forall r2 =>
                  forall id =>
                      (forall dv =>
                           r2 id == dv implies 
                           forall r => 
                               mkEnv r == envcomb (mkEnv r1) (mkEnv r2) implies r id == dv) andalso
                      (forall str => 
                           (r2 id raises UnboundIde(str) => true) implies 
                           forall r =>
                               mkEnv r == envcomb (mkEnv r1) (mkEnv r2) implies r id == r1 id)*)

    val evtosv: EV -> SV
    val svtoev: SV -> EV
    val dvtoev: DV -> EV
    val dvtoloc: DV -> Loc
    val isobject: SV -> bool
    val evtoloc: EV -> Loc 

    (* computing the fixpoint of an operator *)
    val fix: (Method -> Method) -> Method 
(*    axiom forall F =>
              fix F == F (fix F) *)

end



(* lexical analyzer *)
signature LexerSIG = 
sig
    type progtext
    type token

    val lex: progtext -> token list

    exception LexerError of string * string
end 



(* parser *)
signature ParserSIG =
sig
    type token
    type Program

    val parse: token list -> Program

    val ParserError: string * token list -> exn
end



(* class library *)
signature ClassLibrarySIG =
sig
    type S
    type Env

    val libEnv: Env
    val libS: S
end



(* interpreter *)
signature InterpreterSIG = 
sig
    type Program
    type progresult

    val interpret: Program -> progresult

    val RuntimeError: string -> exn
end



(* dumper *)
signature DumperSIG =
sig
    type progresult

    val symbols: string list
    val dump: progresult -> unit
end




(* program code *)
signature ProgramCodeSIG =
sig
    type progtext
end



(* The SOOPL *)
signature SOOPLSIG = 
sig
    type progtext = string
    type progresult

    val run: progtext -> progresult
    val dump: progresult -> unit

    exception SOOPLError
end;




(* ======================================================================= *)
(*                             implementation                              *)


(* required modules from the SML library *)
load "Int";
load "TextIO";

(* the program modules *)
use "Utilities.sml";
use "Tokens.sml";
use "SyntacticDomains.sml";
use "SemanticDomains.sml";
use "ProgramCode.sml";
use "Lexer.sml";
use "Parser.sml";
use "ClassLibrary.sml";
use "Interpreter.sml";
use "Dumper.sml";


(* ----------------------------------------------------------------------- *)
(*                                SOOPLFUN                                 *)
functor SOOPLFUN (structure Lexer: LexerSIG where type progtext = string
		  structure Parser: ParserSIG where type token = Lexer.token
		  structure Interpreter: InterpreterSIG where type Program = Parser.Program
		  structure Dumper: DumperSIG where type progresult = Interpreter.progresult) :> SOOPLSIG =
struct

(*--------------- types ---------------*)

type progtext = string
type progresult = Interpreter.progresult


(*------------- exceptions ------------*)

exception SOOPLError


(*------------- functions -------------*)

(* running the program *)
fun run p = Interpreter.interpret (Parser.parse (Lexer.lex p))

(* printing out the program result *)
fun dump pr = Dumper.dump pr


end (* SOOPLFUN *)


(* structures that already exist:
 *  Utilities
 *  ProgramCode
 *  Tokens
 *  SyntacticDomains
 *)

structure SemanticDomains = SemanticDomainsFUN (structure Utils = Utilities)

structure Lexer = LexerFUN (structure ProgCode = ProgramCode 
			    structure Tok = Tokens)

structure Parser = ParserFUN (structure Tok = Tokens
			      structure SyntDomains = SyntacticDomains)

structure Interpreter = InterpreterFUN (structure SyntDomains = SyntacticDomains
					structure SemDomains = SemanticDomains)

structure Dumper = DumperFUN (structure SemDomains = SemanticDomains)

structure SOOPL = SOOPLFUN (structure Lexer = Lexer
			    structure Parser = Parser
			    structure Interpreter = Interpreter
			    structure Dumper = Dumper);
