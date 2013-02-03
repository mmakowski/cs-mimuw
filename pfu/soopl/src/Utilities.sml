(* ----------------------------------------------------------------------- *)
(*                                Utilities                                *)
structure Utilities :> UtilitiesSIG =
struct

(* subst: ('a -> 'b) -> 'b -> 'a -> ('a -> 'b) *)
fun subst f v a = fn x => if x = a then v else f x;

(* append: 'a list -> 'a list -> 'a list *)
fun append [] l = l
  | append (h::t) l = h::(append t l);

(* inlist: 'a -> 'a list -> bool *)
fun inlist e [] = false
  | inlist e (h::t) = if e = h then true else inlist e t;

end (* Utilities *)
