#include "stdio.h"
#include <string.h>
#include "main.h"
#include "tree.h"
#include "symbol.h"
//#include "pretty.h"

void yyparse();
int yylex();
Prog* root;

int main(int argc, char **argv) {
	if (strcmp(argv[1], "scan") == 0) {
		mode = scan;
		while (yylex()) {
		}
		printf("OK\n");
	} else if (strcmp(argv[1], "tokens") == 0) {
		mode = tokens;
		while (yylex()) {
		}
	} else if (strcmp(argv[1], "parse") == 0) {
		mode = parse;
		yyparse();
		printf("OK\n");
	} else if (strcmp(argv[1], "pretty") == 0) {
		mode = pretty;
		yyparse();
		//prettyNode(root);
	}
	else if (strcmp(argv[1], "symbols") == 0) {
		mode = symbols;
		yyparse();
		symProg(root, true);
	}
	return 0;
}


