(*
Leniwo¶æ:


*)
(* datatype 'a nlist = L of unit -> 'a * 'a nlist; *)
(* zrobiæ element tego typu*)

fun il n = L(fn () => (n, il (n + 1)));

(* teraz: hd, tl, map, filter *)

fun nhd (L(l)) = #1(l());
fun ntl (L(l)) = #2(l());
fun nmap l f = L(fn () => (f (nhd l), nmap (ntl l) f));
fun nffirst l f = if f (nhd l) then l else nffirst (ntl l) f;
fun nfilter l f = L(fn () => 
			 let val g = nffirst l f in
			     (nhd g, nfilter (ntl g) f)
			 end);

(* lista liczb pierwszych *)(*
fun ndok e l f = 
    if e = nhd l then 
	true 
    else 
	if f e (nhd l) then ndok e (ntl l) f else false;
  
fun ndffirst l1 l2 f = if ndok (nhd l1) l2 f then l1 else ndffirst (ntl l1) l2 f; 
fun ndfilter l1 l2 f = L(fn () =>
			    let val g = ndffirst l1 l2 f in
				(nhd g, ndfilter (ntl g) l2 f)
			    end);
val p = ndfilter (il 1) (il 1) (fn x => fn y => if x mod y = 0 then false else true);

*)
fun nltf l 1 = nhd l
  | nltf l n = nltf (ntl l) (n - 1);
 