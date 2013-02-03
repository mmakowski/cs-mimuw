fun mul a b = a * b;
fun add a b = a + b;
fun curry f = fn a => fn b => f (a, b);

fun foldr f pocz [] = pocz
  | foldr f pocz (a::b) = f a (foldr f pocz b);

(*
zrobic za pomoca foldr:
len - dlugosc listy
append - laczenie list
max - max z listy
map - aplikuje f do elementow
filter - zwraca elty spelniajace warunek
*)

fun len l = foldr (fn a => fn b => b + 1) 0 l;
fun append l1 l2 = foldr (fn a => fn l => a::l) l2 l1;
fun max l = foldr (fn a => fn b => if a > b then a else b) 0 l;
fun map f l = foldr (fn a => fn l => (f a)::l) [] l;
fun filter f l = foldr (fn a => fn l => if f a then a::l else l) [] l; 

(*
foldl
foldl za pomoca foldr
*)

fun foldl f pocz [] = pocz 
  | foldl f pocz (a::b) = foldl f (f a pocz) b; 

