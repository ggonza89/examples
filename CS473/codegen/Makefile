# Makefile for cminus

CC=gcc
CC1=clang++
LEX=flex
YACC=bison
MV=/bin/mv
RM=/bin/rm
CP=/bin/cp
CG=./codegen

LIBRARIES=  -ll -ldl -lpthread

CFLAGS=-g
YFLAGS=-dv

PROGRAM=codegen

OBJECTS=codegen.o cmlexer.o cmparser.o symbolTable.o ast.o util.o typecheck.o

SOURCES=cmlexer.l cmparser.y symbolTable.c ast.c util.c typecheck.c

CTEMPS=cmlexer.c cmparser.c cmparser.tab.h cmparser.tab.c cmparser.output

$(PROGRAM): $(OBJECTS)
	$(CC1) `llvm-config --ldflags` -g -o codegen codegen.o cmlexer.o cmparser.o symbolTable.o ast.o util.o typecheck.o `llvm-config --libs` -ll -ldl -lpthread -lcurses -lz -v

typecheck.o: typecheck.c
	$(CC) $(FLAGS) -c typecheck.c

util.o: util.c
	$(CC) $(CFLAGS) -c util.c

ast.o:  ast.c
	$(CC) $(CFLAGS) -c ast.c

symbolTable.o: symbolTable.c
	$(CC) $(CFLAGS) -c symbolTable.c

cmparser.o: cmparser.c
	$(CC) $(CFLAGS) -c cmparser.c

cmparser.c: cmparser.y
	$(YACC)  cmparser.y -o cmparser.c

cmlexer.o: cmlexer.c cmparser.tab.h globals.h
	$(CC) $(CFLAGS) -c cmlexer.c

cmparser.tab.h: cmparser.y
	$(YACC) $(YFLAGS)  cmparser.y

cmlexer.c: cmlexer.l
	$(LEX) -ocmlexer.c  cmlexer.l

codegen.o: codegen.cpp
	$(CC1) `llvm-config --cxxflags` -g -o codegen.o -c codegen.cpp

clean:
	$(RM) $(PROGRAM) $(OBJECTS) $(CTEMPS) codegen codegen.o

run:
	$(CG) test.c
	llc -march=x86-64 -o outx86.s out.s
	clang outx86.s -o out.exe
	./out.exe
	echo $$?
