
signature SSIG = 
sig
    type 'a stack
    exception Empty
    val push: 'a * 'a stack -> 'a stack
    val pop: 'a stack -> 'a * 'a stack
    val empty: 'a stack
end;

structure S :> SSIG = 
struct
    type 'a stack = 'a list
    exception Empty
    fun push (a, s) = a::s
    fun pop [] = raise Empty
      | pop (a::s) = (a, s)
    val empty = []
end;

signature KPSIG = 
sig
    type elm
    val le: elm * elm -> bool
    type queue
    val insert: elm * queue -> queue
    val rmmax: queue -> elm * queue
    val empty: queue
end;

signature KPINTSIG = KPSIG where type elm = int;

(* jakby tego nie by³o, to nie mogliby¶my nic w³o¿yæ do kolejki - 
   nie mieliby¶my nic typu elm *)
(*
structure KPINT :> KPINTSIG =
struct
    type elm = int
    fun le (a, b) = a <= b
    datatype queue = HNODE of elm * queue * queue | HLEAF

    fun heapify HLEAF = HLEAF
      | heapify (HNODE(e, HLEAF, HLEAF)) = (HNODE(e, HLEAF, HLEAF))
      | heapify (HNODE(e1, HLEAF, HNODE(e2, q1, q2))) =
	let
	    val (HNODE(e2', q1', q2')) = heapify (HNODE(e2, q1, q2))
	in
	    if lt(e1, e2') then 
		(HNODE(e2', HLEAF, HNODE(e1, q1', q2')))
	    else
		(HNODE(e1, HLEAF, HNODE(e2', q1', q2')))
	end    
      | heapify (HNODE(e1, HNODE(e2, q1, q2), HNODE(e3, q3, q4)) =
	let
	    val (HNODE(e2', q1', q2')) = heapify (HNODE(e2, q1, q2))
	    val (HNODE(e3', q3', q4')) = heapify (HNODE(e3, q3, q4))
	in
	    if lt(e1, e2') then 
		(HNODE(e2', HNODE(e1, q1', q2'), HNODE(e3', q3', q4'))
	    else
		if lt(e1, e3') then
		    (HNODE(e3', HNODE(e2' q1', q2'), HNODE(e1, q3', q4')))    
		else
		    (HNODE(e1, HNODE(e2', q1', q2'), HNODE(e3', q3', q4'))
	end    

    fun inslast (e, HLEAF) = HNODE(e, HLEAF, HLEAF)
      | inslast 
	
    val empty = HLEAF
end;
*)

signature SETSIG = 
sig
    type 'a set
    exception Empty
    val sum: 'a set * 'a set -> 'a set
    val diff: 'a set * 'a set -> 'a set
(*    val get_elm: 'a set -> 'a*)
    val rm_elm: 'a * 'a set -> 'a set
    val insert: 'a * 'a set -> 'a set
    val in_set: 'a * 'a set -> bool
    val empty: 'a set
end;

structure SET :> SETSIG =
struct
    type 'a set = 
end;