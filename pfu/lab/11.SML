(* 11 - funktory *)

signature TREE = 
sig
    type 'a tree
    val empty: 'a tree
    val isempty: 'a tree -> bool
    val root: 'a tree -> 'a
    val left: 'a tree -> 'a tree
    val right: 'a tree -> 'a tree
    val leftinsert: 'a tree * 'a -> 'a tree
    val rightinsert: 'a tree * 'a -> 'a tree
    val remove: 'a tree -> 'a tree * 'a tree
end;

signature TP =
sig
    type elem
    val leq: elem * elem -> bool
end;

functor PQasHeap (structure TotPre: TP structure Tree: TREE) :>
sig
    structure Item: TP
    type queue
    val empty: queue
    val insert: queue * Item.elem -> queue
    val max: queue -> Item.elem * queue
end where type Item.elem = TotPre.elem = 
struct
    structure Item = TotPre
    type queue = Item.elem Tree.tree
    val empty = Tree.empty
    fun insert (q, i) = 
	if Tree.isempty q then Tree.leftinsert (q, i) 
	else
	    if Item.leq ((Tree.root q), i) then insert (Tree.left q, i) 
	    else Tree.leftinsert (q, i) 
    val max = fn q => let val v = Tree.root q in (v, #1(Tree.remove q)) end
end;    

structure Tree :> TREE =
struct 
    datatype 'a tree = NODE of 'a * 'a tree * 'a tree | LEAF
    val empty = LEAF
    fun isempty LEAF = true
      | isempty n = false
    fun root (NODE(v, l, r)) = v
    fun left (NODE(v, l, r)) = l
    fun right (NODE(v, l, r)) = r
    fun leftinsert (n, v) = NODE(v, n, LEAF)
    fun rightinsert (n, v) = NODE(v, LEAF, n)
    fun remove n = (left n, right n)
end;
structure IntTP : TP =
struct
    type elem = int
    fun leq (a, b) = a <= b
end;

structure IPQ = PQasHeap (structure TotPre = IntTP structure Tree = Tree);