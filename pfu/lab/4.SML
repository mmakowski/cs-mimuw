(* ALGEBRAICZNE TYPY DANYCH *)

datatype 'a drzewo = LISC
		| WEZEL of 'a drzewo * 'a * 'a drzewo;


fun wstaw_bst LISC e =
	WEZEL(LISC, e, LISC)
  | wstaw_bst (WEZEL(l, v, r)) e = 
	if v < e then WEZEL(l, v, wstaw_bst r e)
	else WEZEL(wstaw_bst l e, v, r);

fun ls2tr [] = LISC
  | ls2tr (h::t) = wstaw_bst (ls2tr t) h;

val ls = [5, 4, 6, 1, 2, 8];
val tr = ls2tr ls;



(* "KOMPILATOR" *)

datatype WYR = S of int 
             | Z of string
             | MNOZ of WYR * WYR
             | DODAJ of WYR * WYR
             | APP of PROGR * WYR
and PROGR = P of string * WYR;

type ENV = (string * int) list;

exception unbound;
exception empty;

val w = MNOZ(Z("a"), S(5));
val pr = P("a", w);
val app = APP(pr, S(5));

fun wart z [] = raise unbound
  | wart z ((s, i)::t) = if z = s then i else wart z t;
	
fun wk (S(i)) e = i
  | wk (Z(s)) e = wart s e
  | wk (MNOZ(w1, w2)) e = (wk w1 e) * (wk w2 e)
  | wk (DODAJ(w1, w2)) e = (wk w1 e) + (wk w2 e)
  | wk (APP(P(z, wyr), arg)) e = 
	let val env = (z, wk arg e)::e in wk wyr env end;

fun komp (P(zm, wyr)) = fn x => wk wyr [(zm, x)];

fun append [] e = [e]
  | append (h::t) e = h::(append t e);

(*
fx.fx.+xx+xx
*)
datatype token = T_PR | T_ZM of char | T_ADD | T_MUL;

fun whattoken #"f" = T_FUN
  | whattoken #"+" = T_ADD
  | whattoken #"*" = T_MUL
  | whattoken c = T_ZM(c); 


fun tk [] = []
  | tk (h::t) = whattoken(h)::(tk t);

fun tokenize s = tk (explode s);


