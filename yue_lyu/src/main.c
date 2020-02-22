#include "stdio.h"
#include <string.h>
#include "main.h"
#include "tree2.h"

void yyparse();
int yylex();
Node* root;

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
	}
	return 0;
}


