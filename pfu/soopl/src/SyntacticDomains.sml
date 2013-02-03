(* ----------------------------------------------------------------------- *)
(*                            SyntacticDomains                             *)
structure SyntacticDomains :> SyntacticDomainsSIG =
struct

datatype Id = mkId of string
datatype Const = mkConst of int
datatype Arguments = mkArguments of Expression list 
and      Expression = mkIdentifier of Id
                    | mkConstant of Const
		    | mkBlockDecl of Statement list
		    | mkMessage of Expression * Id * Arguments
		    | mkCreation of Id
and      Statement = mkAssignment of Id * Expression
                   | mkReturn of Expression
		   | mkEvaluation of Expression
		   | mkExecution of Expression
datatype FormalArguments = mkFormalArguments of Id list
datatype MethodBody = mkMethodBody of Statement list
datatype MethodDecl = mkMethodDecl of Id * FormalArguments * MethodBody;
datatype MethodMap = mkMethodMap of MethodDecl list
datatype Attributes = mkAttributes of Id list
datatype ClassBody = mkClassBody of Id * Attributes * MethodMap * Attributes * MethodMap;
datatype ClassDecl = mkClassDecl of Id * ClassBody
datatype ClassMap = mkClassMap of ClassDecl list
datatype Initializer = mkInitializer of Expression
datatype Program = mkProgram of ClassMap * Initializer
		 | mkErrProg

end (* SyntacticDomains *)

