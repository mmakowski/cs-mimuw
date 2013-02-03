(* 7 *)
(*
parser:
	TERM ::= fn ZM.TERM | TERM TERM | (TERM) | ZM

lexer: string -> lex list
parser: lex list -> string

*)

(* datatype lex = T_LN | T_PN | T_FN | T_KR | T_ZM of string; *)
(* datatype wyr = E_APP of wyr * wyr | E_ZM of string | E_FUN of string * wyr; *)
(* type env = (string * wyr) list; *)

val prg = E_APP(E_FUN("xa", E_ZM("xa")), E_APP(E_FUN("xa", E_ZM("y")), E_ZM "super"));

(* fun value z [] = E_ZM z
  | value z ((s, v)::t) = if z = s then v else value z t; *)

fun subst (E_ZM d) z s = if d = z then s else E_ZM d
  | subst (E_FUN (a, E_ZM d)) z s = subst (E_ZM d) z s
  | subst (E_FUN (a, b)) z s = subst b z s;

fun reduce (E_ZM a) = E_ZM a
  | reduce (E_FUN(a, b)) = E_FUN (a, reduce b)
  | reduce (E_APP(E_FUN(a, b), e2)) = 
	let 
		val (E_FUN(ar, br)) = reduce (E_FUN(a, b))
	in
		reduce (subst br ar e2)
	end;


 
(* fun parser [] =  *)
