(*
1. napisac kod i dekod
2. napisac fcje przerabiajaca listy cyfr na liczby ([1, 2, 3] -> 123)
*)

(*
val a = fn x => x + 1;
a(2);
val b = fn x => (x, x);
b(b ());
*)
(*
fun mn x y = 
    if y = 0 then 0 
    else (mn x (y - 1)) + x;
fun fib2 n = 
    if n = 2 then (1, 1)
    else let val x = fib2(n - 1) in (#2(x), #1(x) + #2(x)) end;    
val fib = fn n => #2(fib2 n);
*)

(**** KOD / DEKOD ****)
(*
fun kod (m, n) =    
    (m + n) + (m + n - 1) * (m + n) div 2 + m;
fun dekod k = 1;
*)

(**** STOS ****)
(*
val empty = ();
fun push e s = (e, s);
fun pop (e, s) = s;
fun top (e, s) = e;    
*)

(**** LISTY ****)

val l = 0::[1, 2, 3, 4];
fun length l =
	if null l then 0 else length (tl l) + 1;

fun sum l =
	if null l then 0 else hd l + sum (tl l);

fun 	suma [] = 0 | 
    	suma (h::l) = h + suma l;

fun 	ost [k] = k |
    	ost (h::l) = ost l;

fun 	append [] l = l |
    	append (h::t) l = h::(append t l);

fun 	elmn l 1 = hd l |
	elmn l n = elmn (tl l) n - 1;

exception wrongarg;

val p = [[1, 2], [3], [4, 5, 6]];

fun	flatten [h::t] = h::t |
	flatten (h::t) = append h (flatten t) |
	flatten _ = raise wrongarg;

(**** SORT ****)
val t = [4, 3, 18, 2, 4, 10, 1]

fun	lt a b = (a < b);

fun	lpart cmpfn a [] = [] |
	lpart cmpfn a (h::t) = 
		if (cmpfn h a) then
			h::(lpart cmpfn a t)
		else
			lpart cmpfn a t;

fun	rpart cmpfn a [] = [] |
	rpart cmpfn a (h::t) = 
		if not (cmpfn h a) then
			h::(rpart cmpfn a t)
		else
			rpart cmpfn a t;

fun	part cmpfn a l = 
		(lpart cmpfn a l, rpart cmpfn a l);
	
fun	sort cmpfn [] = [] |
	sort cmpfn [e] = [e] |
	sort cmpfn (h::t) = 
		let 
			val (p1, p2) = (part cmpfn h t) 
		in
			append (sort cmpfn p1) (h::(sort cmpfn p2))
		end;