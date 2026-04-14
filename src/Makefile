#
#-------------------------------------------------------------------------
# Project   - Checkpoint #7 / while, exit, modulo, arrays
# Course    - CS4223
# Developer - Hayden Berry
# 
# Description - This is the Makefile for the compiler
# Due - 02.04.2025
#-------------------------------------------------------------------------
#
ifeq ($(shell uname),Darwin)
	# For Macintosh
	FLEXLIB = -ll
else
	# For Linux, Cygwin, etc.
	FLEXLIB = -lfl
endif

a.out: scanner.l parser.y main.c symbolTable.c symbolTable.h ast.h
	bison -y -d parser.y
	flex scanner.l
	gcc main.c lex.yy.c y.tab.c symbolTable.c ${FLEXLIB}
clean:
	rm -f a.out y.tab.c y.tab.h lex.yy.c symbolTable.o
