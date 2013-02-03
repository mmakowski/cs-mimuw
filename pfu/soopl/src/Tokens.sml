(* ----------------------------------------------------------------------- *)
(*                                  Tokens                                 *)
structure Tokens :> TokensSIG =
struct

datatype token = IDtkn of string
	       | NUMtkn of int
	       | BLKENDtkn
	       | BLKSTARTtkn
	       | CLASStkn
	       | EVALtkn
	       | EXECtkn
	       | EXTENDStkn
	       | OBJVARStkn
	       | CLASSVARStkn
	       | OBJMETHODtkn
	       | CLASSMETHODtkn
	       | ARGStkn
	       | IStkn
	       | ASSIGNtkn
	       | MESSAGEtkn
	       | NEWtkn
	       | RETURNtkn
	       | SENDtkn
	       | DOTtkn
	       | INITtkn
	       | WITHtkn

(* name of the token *)
fun tokenName tk =
    case tk of
	BLKENDtkn => "BLOCK END"
      | BLKSTARTtkn => "BLOCK START"
      |	CLASStkn => "CLASS"
      | EVALtkn => "EVAL"
      | EXECtkn => "EXEC"
      | EXTENDStkn => "EXTENDS"
      | OBJVARStkn => "OBJVARS"
      | CLASSVARStkn => "CLASSVARS"
      | OBJMETHODtkn => "OBJMETHOD"
      | CLASSMETHODtkn => "CLASSMETHOD"
      | ARGStkn => "ARGS"
      | IStkn => "IS"
      | ASSIGNtkn => "ASSIGN"
      | MESSAGEtkn => "MESSAGE"
      | NEWtkn => "NEW"
      | RETURNtkn => "RETURN"
      | SENDtkn => "SEND"
      | INITtkn => "INIT"
      | WITHtkn => "WITH"
      | IDtkn(sym) =>"ID '" ^ sym ^ "'"
      | NUMtkn(n) => "NUM '" ^ (Int.toString n) ^ "'";

end (* Tokens *)
