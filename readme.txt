School assignment
	for CMPT300, SFU
< Build a simple shell >
Functionalities:
1. The internal shell command "exit" which terminates the shell.
2. A command with no arguments
3. A command with arguments
4. A command, with or without arguments, executed in the background using &
5. A command, with or without arguments, whose output is redirected to a file
6. A command, with or without arguments, whose input is redirected from a file
==========================
C, Linux
==========================
v1.0 Oct 1, 2013
==========================
TODOs:
- pipe operation
==========================
< Compile >
reference: http://users.soe.ucsc.edu/~sbrandt/111/assignment1.html
To compile lex.c, you have to use flex: "flex lex.c". 
This will produce a file called lex.yy.c. 
lex.yy.c and shell.c must then be compiled and linked in order to get a running program. 
To compile, do "gcc -c lex.yy.c" and "gcc -c shell.c". 
To link, use this command, "gcc -o shell lex.yy.o shell.o -lfl".