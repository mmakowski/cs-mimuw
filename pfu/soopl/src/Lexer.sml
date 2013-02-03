(* ----------------------------------------------------------------------- *)
(*                               LexerFUN                                  *)
functor LexerFUN (structure ProgCode: ProgramCodeSIG where type progtext = string
		  structure Tok: TokensSIG) : LexerSIG =
struct

(*--------------- types ---------------*)
type progtext = ProgCode.progtext;
type token = Tok.token;

val error = [];


(*------------- exceptions ------------*)
exception LexerError of (string * string);
fun lexicalError s t = raise LexerError (s, implode t);


(*------------- functions -------------*)

open Tok;

(* token type recognition *)
fun recognize sym =
    case sym of
	"[" => BLKSTARTtkn
      | "]" => BLKENDtkn
      |	"class" => CLASStkn
      | "eval" => EVALtkn
      | "exec" => EXECtkn
      | "extends" => EXTENDStkn
      | "objvars" => OBJVARStkn
      | "classvars" => CLASSVARStkn
      | "objmethod" => OBJMETHODtkn
      | "classmethod" => CLASSMETHODtkn
      | "args" => ARGStkn
      | "is" => IStkn
      | "assign" => ASSIGNtkn
      | "message" => MESSAGEtkn
      | "new" => NEWtkn
      | "return" => RETURNtkn
      | "send" => SENDtkn
      | "init" => INITtkn
      | "with" => WITHtkn
      | _ => IDtkn sym;
		   
(* convertion of a digit to number *)
fun digtonum dig = (ord dig) - (ord #"0");

(* letter and text characters recognition *)
fun isletter chr = ((#"A" <= chr) andalso (chr <= #"Z")) orelse 
		   ((#"a" <= chr) andalso (chr <= #"z")) orelse
		   (chr = #"[") orelse (chr = #"]");

(* digit recognition *)
fun isdigit chr = (#"0" <= chr) andalso (chr <= #"9");

(* alphanumeric symbols recognition *)
fun isalphanum chr = (isletter chr) orelse
		     (isdigit chr) orelse
		     (chr = #"_");

(* white space recognition *)
fun iswhite chr = (chr = #" ") orelse
		  (chr = #"\n") orelse
		  (chr = #"\t");

(* reading a number *)
fun getnum [] n = (n, [])
  | getnum (h::t) n =  
    if isdigit h then 
	getnum t (10 * n + digtonum h)
    else
	(n, h::t);

(* reading a token *)
fun getalphanum [] s = (s, [])
  | getalphanum (h::t) s = 
    if isalphanum h then 
	getalphanum t (s ^ (str h))
    else
	(s, h::t);

(* the lexer *)
fun lexchrlist [] = []
  | lexchrlist (h::t) = 
    if isletter h then
	let val (s, t1) = getalphanum t (str h)
	in (recognize s)::(lexchrlist t1)
	end
    else
	if isdigit h then
	    let val (n, t1) = getnum t (digtonum h) 
	    in (NUMtkn n)::(lexchrlist t1)
	    end
	else
	    case h of 
		#"." => DOTtkn::(lexchrlist t)
	      | _    => if iswhite h then
			    lexchrlist t
			else
			    lexicalError ("Lexical error: disallowed symbol '" ^ (str h) ^ "'\n") t;


fun lex s = lexchrlist (explode s) 
    handle LexerError(s1, s2) => let val u = TextIO.output(TextIO.stdOut, s1) in [] end;


end (* LexerFUN *)
