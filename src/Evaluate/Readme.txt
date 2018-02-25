		Evaluate.lib Readme

	Designed and Programmed by Sean O'Rourke.
	http://backfrog.digitalrice.com

 - OVERVIEW -

Evaluate.lib is a static library for lcc-win32, a free C compiler available at 
http://www.cs.virginia.edu/~lcc-win32.  Evaluate.lib will parse a string and evaluate
the answer based on order of operation.

I found this useful several times while trying to write applications, so I decided
to create a library for others to use.  

 - INSTRUCTIONS -

All you have to do to parse a string is include the library Evaluate.lib in your
project and then #include the header file, Evaluate.h, in the source file.  For an 
example see the source file test.c.  In order to parse and evaluate a string call 
Evaluate(Str) where Str is the string to be evaluated.  For example,

Evaluate("15/3");

Evaluate returns a double of the evaluated string, in this case 5.  All memory 
management is conducted in the library all you have to worry about is passing 
a valid expression.

If an error occurs, Evaluate returns P_ERR, defined in Evaluate.h.

For an example of this check out test.c and run it's executable test.exe.  Evaluate.lib
is able to use additon (+), subtraction (-), multiplication (*), division (/), and
exponents (^) as well as any amount of brackets.  

 - DEBUG -

If you wish to compile Evaluate.lib in debug mode then define DEBUG in Parse.h.
If DEBUG is defined in parse.h then all steps of the parsing process will be displayed
and for any errors that occur a message will be displayed to the console window 
explaining what the error was.  

 - QUESTIONS AND COMMENTS - 

If there are any questions or comments you can e-mail me at backfrog@digitalrice.com
or visit my web page at http://backfrog.digitalrice.com.  If you find a bug in 
the code you can contact me and I will try to fix it as soon as possible.  