School assignment
	for CMPT300, SFU
< Build a simple shell >
Requirements see:
http://www.cs.sfu.ca/CourseCentral/300/stevenp/HW/HW2.html
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