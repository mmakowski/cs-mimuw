(*
 *    SOOPL -- SIMPLE OBJECT-ORIENTED PROGRAMMING LANGUAGE
 *    A few sample programs
 * 
 *    Functional Programming Course Assignment
 *
 *    author: Maciek Makowski (189188) 01.05.2002
 *
 *)

val prog0 = "\
	    \ class MyInteger extends Integer\n\
            \   objvars\n\ 
            \   classvars\n\
	    \   objmethod mySetValue args newVal is\n\
	    \     return send self message setValue with newVal\n\
	    \\
	    \ class Main extends Root\n\
	    \   objvars\n\
	    \   classvars\n\
	    \   classmethod run args int is\n\
	    \     assign int new MyInteger\n\
	    \     eval send int message mySetValue with 15\n\
	    \     eval send TextIO message outputInt with send int message multiply with 10\n\
	    \     eval send TextIO message outputEOL\n\
	    \     eval send TextIO message outputInt with send int message equals with 15\n\
	    \     eval send TextIO message outputEOL\n\
	    \\
	    \ init send Main message run";


val prog1 = "\
	    \ class Returner extends Root\n\
	    \   objvars\n\
	    \   classvars\n\
	    \   objmethod doReturn args ret is\n\
	    \     assign ret [\n\
	    \       eval send TextIO message outputInt with 2\n\
	    \       return 3\n\
	    \     ]\n\
	    \     eval send TextIO message outputInt with 1\n\
	    \     exec ret\n\
	    \\
            \ class Main extends Root\n\
            \   objvars\n\
            \   classvars\n\
            \   classmethod run args r is\n\
	    \     assign r new Returner\n\
	    \     eval send TextIO message outputInt with send r message doReturn\n\
	    \     eval send TextIO message outputEOL\n\
	    \\
            \ init send Main message run";

val prog2 = "\
	    \ class Main extends Root\n\
	    \   objvars\n\
	    \   classvars\n\
	    \   classmethod run args b t1 t2 is\n\
	    \     assign b new Boolean\n\
	    \     assign t1 new Integer\n\
	    \     assign t2 new Integer\n\
	    \     eval send b message setValue with 1\n\
	    \     eval send t1 message setValue with 1\n\
	    \     eval send t2 message setValue with 2\n\
	    \     eval send TextIO\n\
	    \          message outputInt\n\
	    \          with send send b\n\
	    \                    message ifElse\n\
	    \                    with t1 t2\n\
	    \               message getValue\n\
	    \\
	    \ init send Main message run";

(* prints out first n Fibonacci numbers *)
val fib = "\
	    \ class Fibonacci extends Root\n\
	    \   objvars\n\
	    \   classvars\n\
	    \   classmethod fib args p1 p2 cit tot \n\
            \                        tmp1 tmp2 newV b is\n\
	    \     assign tmp1 new Integer\n\
	    \     assign tmp2 new Integer\n\
	    \     assign newV new Integer\n\
	    \     eval send tmp1 message setValue with p1 \n\
	    \     eval send tmp2 message setValue with cit \n\
	    \     eval send newV message setValue \n\
	    \          with send tmp1 message add with p2 \n\
	    \     eval send send TextIO message outputInt \n\
	    \               with send newV message getValue \n\
	    \          message outputEOL \n\
	    \     assign b new Boolean \n\
	    \     eval send b message setValue \n\
	    \          with send tmp2 message equals with tot\n\
	    \     exec send b message ifElse with [ \n\
	    \       return self \n\
	    \     ] [ \n\
	    \       eval send self message fib \n\
	    \            with p2 \n\
	    \                 send newV message getValue \n\
	    \                 send tmp2 message add with 1. \n\
	    \                 tot \n\
	    \     ] \n\
	    \\
	    \   classmethod run args it is \n\
	    \     eval send self message fib with 0 1 1 it \n\
	    \\
	    \ init send Fibonacci message run with 40.";

