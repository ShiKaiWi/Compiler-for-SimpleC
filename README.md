## Compiler-for-Simple-C

This is a compliler for a simple C language named after `C1` below (which is a simple form of real C and does not contain the pointer, struct and multi-dimension array features).

## Dependency

**bison** and **flex** should be installed

```
$ sudo apt install bison
$ sudo apt install flex
```

## Instructions of running this Compiler

**MUST BE RUNNED UNDER LINUX OS Environment**

#### Introduction

There are two source files in this project, which are named as `C1.y` and `C1.lex` and placed in the "config" directory and four source files, named as `ast.c`,`error.c`,`list.c` and `symtab.c`, placed in the "src" directory.

And after `make` there will be three more source files -- `c1c.lex.cc`, `c1c.tab.h` and `c1c.tab.c` and they are lexical analyzer and syntax analyzer. Also there will be one executable file named `c1c`, which can analyze programme written in `C1` language, create `AST` and then translate the source code into `AT&T` assembly language that can be `ld`ed and executed under Ubuntu OS.

#### Source Tree

This project includes the following files and directories:

```
README.md	this file
makefile	a makefile to build c1c.lex.cc, c1c.tab.h, c1c.tab.c and c1c
config		directory storing C1.y and C1.lex
src			directory storing source files
include		directory storing header files
doc			directory storing documents
test		directory storing three test files
bin			directory storing execution/objective files
	--run	a shell script for automatically making and running
```

**NOTE: YOU CAN JUST RUN "run" IN DIRECTORY CALLED "bin" INSTEAD OF FOLLOWING THE GUIDE BELOW AND THE PROGRAM WILL AUTOMATICALLY TESTS THE FILE IN THE DIRECTORY NAMED "test".**

#### Building

```bash
# Enter: ./
make
```

#### Running
```bash
# Enter: ./
bin/c1c "infilename" # infilename means file written in C1 language
```
or
```
bin.c1c -o "outfilename" "infilename" # outputfile means file to be stored for results.if not existing, it will be created)
```
And then you can use `as` and `ld`(**run `as` with --32 and `ld` with -melf_i386 if your OS is 64-bit Ubuntu**) to create an executable file and then execute it.

