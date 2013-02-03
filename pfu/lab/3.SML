(* LAB 3 *)

(* rekurencja ogonowa *)
val l = [1, 2, 3, 4, 5];

fun silnia (n, s) = 
	if n = 0 then s
	else silnia (n - 1, s * n);


fun len (i, l) = 
	if l = [] then i
	else len (i + 1, tl l);

(*
fun rev (ls, ld) =
	if ls = [] then ld
	else rev (tl ld, *)

(* lista -> funkcja *)
exception NoVal;
val fl = [(1, 1), (2, 2), (3, 3)];

fun 	l2f [] = (fn k => raise NoVal) 
|	l2f ((a, v)::t) = (fn k => (if k = a then v else (l2f t) k)); 
	

(* automaty *)
type automat = ((int * char * int) list * int list);
exception NoMove;

val a1 = ([(0, #"a", 1), (0, #"b", 1), (1, #"a", 0), (1, #"b", 0)], [0]);

fun acc ((p, s) :automat, str: string) = 
	let 
		fun accs (ch :(int*char*int) list, cl :char list, st :int) = 
			let 
				fun 	mv [] s l = (raise NoMove)
				|	mv ((ss, lm, sd)::t) s l  = 
						if lm = l andalso ss = s then sd
						else mv t s l
			in
				if cl = [] then st
				else accs (ch, (tl cl), (mv ch st (hd cl))) 
			end
	in
		let 
			val s = explode str 
		in 
			(accs (p s 0)) = 0 
		end
	end; 
 

