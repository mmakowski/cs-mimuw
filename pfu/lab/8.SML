(* imperatyw *)

(*
val a = ref 0; utworzenie nowej kom�rki z 0
val b = ref 0; utworzenie drugiej kom�rki z 0
a := 1; - wpisanie do kom�rki (-> ())
!a; - warto�� kom�rki
ref `a -> `a ref
ref a = b - a pokazuje an t� sam� kom�rk� co b
*)
(* 
" a i b s� r�ne, je�li [E]fcja ,kt�ra na a i na b daj� r�ne rzeczy (def. Leibniza)"
czy istnieje fcja do odr�nienia a = ref 0 i b = ref 0?
tak: a = b da false.
fcja odr�niacz: b�dzie postaci _; _
np:
fun odr x = x := 5; !a

1. zdefiniowa� typ listy
datatype `a list = NIL | CONS of `a * `a list
ale to nie mo�e zawiera� p�tli (to jest term)
zrobi� tak, �eby lista mog�a si� p�tli� i na tym typie danych zrobi� list�
pust�
1-2
1-2-3->2...

2. zrobi� f p�tla kt�ra bierze list� niep�tl�c� si� i zwraca t� sam� list� zap�tlon�

3. zdef. d� na listach (ilo�� r�nych element�w) (ilo�� w�z��w w postaci kanonicznej)
alg dwuwska�nikowy - jeden skacze o 2, jeden o 1. trzeba wy�apa� wyprzedzenie.
*)

datatype 'a clist = CNIL | CCONS of 'a * 'a clist ref; 

val l1 = CNIL;


val l1 = ref CNIL;
val l2 = ref (CCONS(3, l1));
val l3 = ref (CCONS(2, l2));
val l4 = ref (CCONS(1, l3));
val (CCONS(x, y)) = !l2;
y := !l3;

(* 2 *)
val l2 = CCONS(1, ref (CCONS(2, ref CNIL)));
	
fun clconc (CCONS(v, n)) p = 
    if !n = CNIL then n := p
    else clconc (!n) p;

(* 3 *)

fun meet a b = 
    let 
	val (CCONS(v1, n1)) = !a;
	    val (CCONS(v2, ref (CCONS(v3, n3)))) = !b
    in
	a := !n1; b := !n3;
	if !a = !b then () else meet a b
    end;
    

fun cnt a b n = 
    let
	val (CCONS(v1, n1)) = !a
    in
	a := !n1; 
	if !a = !b then n + 1 else cnt a b (n + 1)
    end;
    

fun cllen CNIL = 0
  | cllen e = 
    let 
	val a = ref e
	val b = ref e
    in
	meet a b;
	cnt a b 0
    end;

