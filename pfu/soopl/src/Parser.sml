(* ----------------------------------------------------------------------- *)
(*                              ParserFUN                                  *)
functor ParserFUN (structure Tok: TokensSIG
		   structure SyntDomains: SyntacticDomainsSIG) : ParserSIG =
struct

(*--------------- types ---------------*)

type token = Tok.token
type Program = SyntDomains.Program


(*------------- exceptions ------------*)

open Tok
open SyntDomains

exception ParserError of (string * token list);
val error = mkErrProg
val anyNum = ~123123;
fun parseError m t = raise ParserError (m, t);
fun wrongToken e t = let val es = case e of
				      IDtkn("") => "ID" 
				    | NUMtkn(anyNum) => "NUM"
				    | _ => tokenName e
		       in parseError (es ^ " expected; " ^ (tokenName (hd t)) ^ " found instead") t
		       end;
fun unexpectedEnd s = if s = "" then
			  parseError "unexpected end of program" []
		      else
			  parseError ("unexpected end of program; expected " ^ s) [];
fun printParserError s = TextIO.output(TextIO.stdOut, "Parse error: " ^ s ^ "\n");


(*------------- functions -------------*)

(* does this token end method body? *)
fun endsMethodBody tk = tk = CLASStkn orelse 
			tk = OBJMETHODtkn orelse
			tk = CLASSMETHODtkn orelse
			tk = DOTtkn orelse
			tk = INITtkn;

(* does this token end object method map?  *)
fun endsObjMethodMap tk = tk = CLASStkn orelse
			  tk = CLASSMETHODtkn orelse
			  tk = INITtkn;

(* does this token end class method map?  *)
fun endsClassMethodMap tk = tk = CLASStkn orelse
			    tk = INITtkn;

(* does this token end argument list? *)
fun endsArguments tk = tk = ASSIGNtkn orelse
		       tk = RETURNtkn orelse
		       tk = EVALtkn orelse
		       tk = EXECtkn orelse
		       tk = MESSAGEtkn orelse
		       tk = BLKENDtkn orelse
		       endsMethodBody tk;

(* integer constant *)
fun parseConst [] = unexpectedEnd "constant"
  | parseConst (h::t) = let val NUMtkn(n) = h
			in (mkConst(n), t)
			end handle Bind => wrongToken (NUMtkn anyNum) (h::t);


(* identifier *)
fun parseId [] = unexpectedEnd "identifier"
  | parseId (h::t) = let val IDtkn(s) = h
		     in (mkId(s), t)
		     end handle Bind => wrongToken (IDtkn "") (h::t);


(* attributes *)
fun parseAttributes [] = unexpectedEnd "attributes"
  | parseAttributes (h::t) = let val IDtkn(s) = h
				 val (mkAttributes(al), t') = parseAttributes t
			     in (mkAttributes((mkId s)::al), t')
			     end handle Bind => (mkAttributes([]), (h::t));


(* object attributes *)
fun parseObjAttributes [] = unexpectedEnd "object attributes"
  | parseObjAttributes (h::t) = if h <> OBJVARStkn then
				    wrongToken OBJVARStkn (h::t)
				else
				    parseAttributes t;


(* class attributes *)
fun parseClassAttributes [] = unexpectedEnd "class attributes"
  | parseClassAttributes (h::t) = if h <> CLASSVARStkn then
				      wrongToken CLASSVARStkn (h::t)
				  else
				      parseAttributes t;


(* formal arguments *)
fun parseFormalArguments [] = unexpectedEnd "formal arguments"
  | parseFormalArguments (h::t) = if h = IStkn then
				      (mkFormalArguments([]), t)
				  else
				      let val (a, tl0) = parseId (h::t)
					  val (mkFormalArguments(fa), tl1) = parseFormalArguments tl0
				      in (mkFormalArguments(a::fa), tl1)
				      end;


(* expression - constant *)
fun parseConstant tkl = let val (c, tl0) = parseConst tkl
			in (mkConstant(c), tl0)
			end;

(* expression - identifier *)
fun parseIdentifier tkl = let val (id, tl0) = parseId tkl
			  in (mkIdentifier(id), tl0)
			  end;

(* expression - creation *)
fun parseCreation tkl = let val (id, tl0) = parseId tkl
			in (mkCreation(id), tl0)
			end;

(* actual arguments *)
fun parseArguments [] = unexpectedEnd "actual arguments"
  | parseArguments (h::t) = if endsArguments h then
				if h = DOTtkn then (mkArguments([]), t) else (mkArguments([]), (h::t))
			    else
				let val (expr, tl0) = parseExpression (h::t) 
				    val (mkArguments(al), tl1) = parseArguments tl0
				in (mkArguments(expr::al), tl1)
				end

(* expression - message *)
and parseMessage tkl = let val (target, (h::tl0)) = parseExpression tkl
		       in if h <> MESSAGEtkn then
			      wrongToken MESSAGEtkn (h::tl0)
			  else
			      let val (id, tl1) = parseId tl0
			      in if tl1 = [] orelse hd tl1 <> WITHtkn then
				     (mkMessage(target, id, mkArguments([])), tl1)
				 else
				     let val (args, tl2) = parseArguments (tl tl1)
				     in (mkMessage(target, id, args), tl2)
				     end
			      end 
		       end handle Bind => unexpectedEnd "message"

(* block *)
and parseBlock [] = unexpectedEnd "block"
  | parseBlock (h::t) = if h = BLKENDtkn then (mkBlockDecl([]), t)
			else
			    let val (stat, tl0) = parseStatement (h::t)
				val (mkBlockDecl(stl), tl1) = parseBlock tl0
			    in (mkBlockDecl(stat::stl), tl1)
			    end

(* expression *)
and parseExpression [] = unexpectedEnd "expression"
  | parseExpression (h::t) =  case h of 
				 NEWtkn => parseCreation t
			       | SENDtkn => parseMessage t
			       | NUMtkn(n) => parseConstant (h::t)
			       | IDtkn(s) => parseIdentifier (h::t)
			       | BLKSTARTtkn => parseBlock t
			       | _ => parseError ("expression (NEW or SEND or NUM or ID) expected; " ^ (tokenName h) ^ " found instead") (h::t)


(* assignment *)
and parseAssignment tkl = let val (id, tl0) = parseId tkl
			      val (expr, tl1) = parseExpression tl0
			  in (mkAssignment(id, expr), tl1)
			  end

(* evaluation *)
and parseEvaluation tkl = let val (expr, tl0) = parseExpression tkl
			  in (mkEvaluation(expr), tl0)
			  end

(* block execution *)
and parseExecution tkl = let val (expr, tl0) = parseExpression tkl
			 in (mkExecution(expr), tl0)
			 end

(* return *)
and parseReturn tkl = let val (expr, tl0) = parseExpression tkl
		      in (mkReturn(expr), tl0)
		      end

(* statement *)
and parseStatement [] = unexpectedEnd "statement"
  | parseStatement (h::t) = case h of
				ASSIGNtkn => parseAssignment t
			      | EVALtkn => parseEvaluation t
			      | EXECtkn => parseExecution t
			      | RETURNtkn => parseReturn t
			      | _ => parseError ("statement (ASSIGN or EVAL or RETURN) expected; " ^ (tokenName h) ^ " found instead") (h::t)
(* method body *)
fun parseMethodBody [] = unexpectedEnd "method body"
  | parseMethodBody (h::t) = if endsMethodBody h then
				 (mkMethodBody([]), (h::t))
			     else
				 let val (st, tl0) = parseStatement (h::t)
				     val (mkMethodBody(stl), tl1) = parseMethodBody tl0
				 in (mkMethodBody(st::stl), tl1)
				 end;

(* common for class and object methods *)
fun parseMethodDecl tkl = let val (id, (h::tl0)) = parseId tkl
			  in if h <> ARGStkn then
				 wrongToken ARGStkn (h::tl0)
			     else
				 let val (args, tl1) = parseFormalArguments tl0
				     val (mb, tl2) = parseMethodBody tl1
				 in  (mkMethodDecl(id, args, mb), tl2)
				 end
			  end handle Bind => unexpectedEnd "method declaration";

(* object method declaration *)
fun parseObjMethodDecl [] = unexpectedEnd "object method declaration"
  | parseObjMethodDecl (h::t) = if h <> OBJMETHODtkn then
				    wrongToken OBJMETHODtkn (h::t)
				else
				    parseMethodDecl t;

(* class method declaration *)
fun parseClassMethodDecl [] = unexpectedEnd "class method declaration"
  | parseClassMethodDecl (h::t) = if h <> CLASSMETHODtkn then
				      wrongToken CLASSMETHODtkn (h::t)
				  else
				      parseMethodDecl t;

(* object method map *)
fun parseObjMethodMap [] = unexpectedEnd "object method map"
  | parseObjMethodMap (h::t) = if endsObjMethodMap h then
				   (mkMethodMap([]), (h::t))
			       else
				   let val (md, tl0) = parseObjMethodDecl (h::t)
				       val (mkMethodMap(mdl), tl1) = parseObjMethodMap tl0
				   in (mkMethodMap(md::mdl), tl1)
				   end;

(* class method map *)
fun parseClassMethodMap [] = unexpectedEnd "class method map"
  | parseClassMethodMap (h::t) = if endsClassMethodMap h then
				     (mkMethodMap([]), (h::t))
				 else
				     let val (md, tl0) = parseClassMethodDecl (h::t)
					 val (mkMethodMap(mdl), tl1) = parseClassMethodMap tl0
				     in (mkMethodMap(md::mdl), tl1)
				     end;

(* superclass identifier *)
fun parseParent [] = unexpectedEnd "superclass"
  | parseParent (h::t) = if h <> EXTENDStkn then
			     wrongToken EXTENDStkn (h::t)
			 else
			     parseId t;

(* class body *)
fun parseClassBody [] = unexpectedEnd "class body"
  | parseClassBody (h::t) = let val (id, tl0) = parseParent (h::t)
				val (oa, tl1) = parseObjAttributes tl0
				val (ca, tl2) = parseClassAttributes tl1
				val (om, tl3) = parseObjMethodMap tl2
				val (cm, tl4) = parseClassMethodMap tl3
			    in (mkClassBody(id, ca, cm, oa, om), tl4)
                            end;

(* class declaration *)
fun parseClassDecl [] = unexpectedEnd "class declaration"
  | parseClassDecl (h::t) = if h <> CLASStkn then
				wrongToken CLASStkn (h::t)
			    else 
				let val (id, tl0) = parseId t
				    val (cb, tl1) = parseClassBody tl0
				in (mkClassDecl(id, cb), tl1)
				end;

(* class map *)
fun parseClassMap [] = unexpectedEnd "class map"
  | parseClassMap (h::t) = if h <> INITtkn then 
			       let val (c, tl) = parseClassDecl (h::t)
				   val (mkClassMap(cl), tl') = parseClassMap tl
			       in
				   (mkClassMap(c::cl), tl')
			       end 
			   else
			       (mkClassMap([]), (h::t));

(* initializer *)
fun parseInitializer [] = unexpectedEnd "initializer"
  | parseInitializer (h::t) = if h <> INITtkn then
				  wrongToken INITtkn (h::t)
			      else
				  let val (expr, tl0) = parseExpression t
				  in (mkInitializer(expr), tl0)
				  end;


(* program *)
fun parseProgram t = 
    let val (clm, tl0) = parseClassMap t
	val (init, tl1) = parseInitializer tl0
    in if tl1 <> [] then
	   parseError ("unexpected token after initializer: " ^ (tokenName (hd tl1))) tl1 
       else
	   mkProgram(clm, init)
    end;

(* complete parser *)
fun parse tkl = parseProgram tkl
    handle ParserError(s, t) => let val u = printParserError s in error end;

end (* ParserFUN *)

