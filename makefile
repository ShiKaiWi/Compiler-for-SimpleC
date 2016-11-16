CINC	 = -Iinclude
CONF	 = config
SRC	 = src
BIN	 = bin
UTILSRC  = $(SRC)/list.c $(SRC)/error.c $(SRC)/ast.c

CC       = gcc -g -w $(CINC)
LEX      = flex -i -I 
YACC     = bison -d -y

c1c:	$(CONF)/C1.y $(CONF)/C1.lex
		$(YACC) -b c1c -o $(SRC)/c1c.tab.c $(CONF)/C1.y
		$(LEX) -o $(SRC)/c1c.lex.c $(CONF)/C1.lex
		$(CC) -DDEBUG -o $(BIN)/c1c $(SRC)/c1c.lex.c $(SRC)/symtab.c $(SRC)/c1c.tab.c $(UTILSRC) -ll -lm

clean:
	rm -rf ./bin/c1c
	rm -rf ./src/c1c.tab.c
	rm -rf ./src/c1c.tab.h
	rm -rf ./test/*.out
	rm -rf ./src/c1c.lex.c
	rm -rf ./test/build/*
