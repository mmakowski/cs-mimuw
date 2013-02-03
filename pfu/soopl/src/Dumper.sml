(* ----------------------------------------------------------------------- *)
(*                               DumperFUN                                 *)
functor DumperFUN (structure SemDomains: SemanticDomainsSIG) : DumperSIG =
struct

(*--------------- types ---------------*)

type progresult = SemDomains.progresult

(*------------- functions -------------*)

open SemDomains

(* printing out the environment and program state *)

val symbols = ["Root", "TextIO", "outputInt", "Integer", "val", "getValue", "setValue", "add", "Main", "run",
	       "self", "return", "Block", "Boolean", "multiply", "equals", "do", "ifElse", "outputEOL"]

fun p s = TextIO.output(TextIO.stdOut, s)
fun pe i s = p ("\t\t" ^ i ^ " -> " ^ s)
fun printDLoc i (mkLoc l) = pe i ("loc: " ^ (Int.toString l) ^ "\n")
fun printDCExp i ce = pe i "<CExp>\n"
fun printDMethod i m = pe i "<Method>\n"
fun printLEnv (mkEnv e) (h::t) = (let val u = case e (mkIde h) of
						  mkDVLoc(l) => printDLoc h l
						| mkDVCExp(ce) => printDCExp h ce
						| mkDVMethod(m) => printDMethod h m
				  in printLEnv (mkEnv e) t
				  end handle _ => printLEnv (mkEnv e) t)
  | printLEnv (mkEnv e) [] = ()
fun printEnv (mkEnv e) = printLEnv (mkEnv e) symbols
fun ps i s = p ("\n" ^ (Int.toString i) ^ " => " ^ s)
fun printSLoc i (mkLoc l) = ps i ("\tloc: " ^ (Int.toString l) ^ "\n")
fun printSPrimitive i (mkPrimitive l) = ps i ("\tprimitive: " ^ (Int.toString l) ^ "\n")
fun printSEmpty i = ps i ("\tempty\n")
fun printSObject i (mkObject(mkLoc(l), e)) = let val u = ps i ("\tobject: class at " ^ (Int.toString l) ^ ",\n\tattributes {\n")
						 val u = printEnv e
					     in p "\t}\n" 
					     end
fun printAttrList ((mkIde s)::t) = let val u = p (s ^ " ") in printAttrList t end
  | printAttrList [] = ()
fun printSClass i (mkClass(mkLoc(l), ca, cm, oa, om)) = let val u = ps i ("class: parent at " ^ (Int.toString l) ^ ",\n\tclass attributes {\n")
							    val u = printEnv ca
							    val u = p ("\t}\n\tclass methods {\n")
							    val u = printEnv cm
							    val u = p ("\t}\n\tobject attributes: [")
							    val u = printAttrList oa
							    val u = p ("]\n\tobject methods {\n")

							    val u = printEnv om
							    val u =  p "\t}\n"
							in u
							end
fun printSBlock i (mkBlock(r, r_a, r_m, bl)) = ps i ("<Block>\n")
fun printS (mkS(s, mkLoc(lm))) = if lm < 0 then ()
				 else let val u = printS (mkS(s, mkLoc(lm - 1)))
					  val u = case s (mkLoc lm) of
					     mkSVLoc(l) => printSLoc lm l
					   | mkSVPrimitive(p) => printSPrimitive lm p
					   | mkSVEmpty => printSEmpty lm
					   | mkSVObject(ob) => printSObject lm ob
					   | mkSVClass(cl) => printSClass lm cl
					   | mkSVBlock(bl) => printSBlock lm bl
				      in u
				      end
  | printS mkError = p ("Error!\n")


fun dump (e, s) = let val u = p "\n***** Global environment *****\n\n"
		      val u = printEnv e
		      val u = p "\n******* Object memory ********\n\n"
		      val u = printS s 
		   in u 
		   end


end (* DumperFUN *)