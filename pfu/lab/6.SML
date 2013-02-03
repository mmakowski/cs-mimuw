datatype nat = S of nat | ZERO;

(*
plus
mnozenie
silnia
n-ta liczba pierwsza
^ nierekurencyjnie: zrobiæ jedn± ogóln±, koduj±c± rekursjê i wywo³ywaæ j± z powy¿szych
*)

fun rek ZERO y fd vz = vz y
  | rek (S(x)) y fd vz =
	let 
		val c = rek x y fd vz 
	in 
		fd c x y 
	end;

val dd = fn a => fn b => fn c => S(a);
val plus = fn a => fn b => rek a b dd (fn a => a);
val mn = fn a => fn b => fn c => plus a c;
val razy = fn a => fn b => rek a b mn (fn a => ZERO);


fun kod 0 = ZERO
  | kod a = S(kod (a - 1));

fun dekod ZERO = 0
  | dekod (S(x)) = (dekod x) + 1;

fun mz f s = 
	let 
		fun it f i = if (f i) = ZERO then i else it f (S(i)) 
	in 
		it f s
	end;
	
fun tp ZERO = false
  | tp (S ZERO) = false
  | tp (S (S ZERO)) = true
  | tp (S(a)) =
	let 
		fun itp a (S(S(ZERO))) = 
			if (dekod a) mod 2 = 0 then false 
			else true
		  | itp a (S(i)) = 
			if (dekod a) mod (dekod (S(i))) = 0 then false
			else itp a i
	in
		itp (S(a)) a
	end;
	
val nast = fn a => mz (fn b => if (tp b) andalso (dekod b) > (dekod a) then ZERO else S(ZERO)) a;

val np = fn n => rek n ZERO (fn a => fn b => fn c => nast a) nast;
