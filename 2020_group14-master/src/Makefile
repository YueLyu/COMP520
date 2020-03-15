all: tiny

y.tab.c y.tab.h: parser.y
	bison -d --yacc parser.y
lex.yy.c: scanner.l y.tab.h
	flex scanner.l
tiny: lex.yy.c y.tab.c y.tab.h tree.c symbol.c
	gcc main.c main.h tree.c tree.h symbol.c symbol.h y.tab.c lex.yy.c y.tab.h -o tiny -lfl

clean: 
	rm lex.yy.c y.tab.c y.tab.h; rm tiny
