#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol.h"

SymbolTable *t;

int Hash(char *str){		// hash function
    unsigned int hash = 0;
    while (*str) hash = (hash << 1) + *str++;
    return hash % HashSize;
}


SymbolTable *initSymbolTable(){		// create new symbol table
    SymbolTable *t = malloc(sizeof(SymbolTable));
    for (int i = 0; i < HashSize; i++) {
        t->table[i] = NULL;
    }
	t -> parent = NULL;
    return t;
}

SymbolTable* scopeSymbolTable(SymbolTable* parent){		// create new scoped symbol table
    SymbolTable *t = initSymbolTable();
    t->parent = parent;
    for (int i = 0; i < 1000; i ++) {
    	if (parent->child[i] == NULL) {
    		parent->child[i] = t;
    		return t;
    	}
    }
}



SYMBOL *putSymbol(Node* declaration, SymbolTable* cur) {		// put declaration variable
    char* name = strdup((declaration->val.declaration.ident)->val.identifier);
    int i = Hash(name);
    for (SYMBOL *s = cur->table[i]; s; s = s->next) {
        if (strcmp(s->name, name) == 0) {
        	printSymTable(t);			/*Print the table before exit*/
            fprintf(stderr, "Error: (line %d) \"%s\" is already declared\n", declaration->lineno, name);
            exit(1);
        }
    }
    SYMBOL *s = malloc(sizeof(SYMBOL));
    s->name = name;
    s->val = declaration->val.declaration.exp;   // defined point value
    s->type = st_UNDEFINED;
    if (declaration->val.declaration.varType != NULL) {
		switch ((declaration->val.declaration.varType)->kind) {
		    case k_NodeKindTypeInt:
		        s->type = st_INT;
		        break;

		    case k_NodeKindTypeBool:
		        s->type = st_BOOL;
		        break;

		    case k_NodeKindTypeFloat:
		        s->type = st_FLOAT;
		        break;

		    case k_NodeKindTypeString:
		        s->type = st_STRING;
		        break;
		    default:
		    	printSymTable(t);			/*Print the table before exit*/
		        fprintf(stderr, "Error: (line %d) Putting symbol \"%s\" wrong type\n", declaration->lineno, name);
		        exit(1);
		}
	}
    // s->kind = kind;
    s->next = cur->table[i];
    cur->table[i] = s;
    return s;
}

SYMBOL *getSymbol(char *name, SymbolTable* cur) {	// find the SYMBOL from the current scope to root symbol table.
    int i = Hash(name);
    // Check the current scope
    for (SYMBOL *s = cur->table[i]; s; s = s->next) {
        if (strcmp(s->name, name) == 0) return s;
    }
    if (cur->parent == NULL) {
    	return NULL;
    }
    return getSymbol(name, cur->parent);
}

/*
bool symbolNotDefined(char* name){
    return getSymbol(name)==NULL;
}
*/

void symProgram(Node *n, bool printTable){	// Start building the table for input program
    t = initSymbolTable();
    buildSymbolTable(n, t);
    if (printTable){
        printSymTable(t);
    }
}

void printSymTable(SymbolTable* t){		// Print symbolTable tree
	if (t != NULL) {
		printf("New scope {\n");
		for (int i = 0; i < HashSize; i++) {
		    if (t->table[i] != NULL){
		        for (SYMBOL *s = t->table[i]; s; s = s->next) {
		            printf("%s", s->name);
		            switch (s->type) {
		                case st_INT:
		                    printf(": int\n");
		                    break;
		                case st_FLOAT:
		                    printf(": float\n");
		                    break;
		                case st_STRING:
		                    printf(": string\n");
		                    break;
		                case st_BOOL:
		                    printf(": bool\n");
		                    break;
		                case st_UNDEFINED:
		                	printf("\n");
		                	break;
		            }
		        }
		    }
    	}
    	for (int i = 0; i < 1000; i ++) {
    		if (t->child[i] != NULL){
    			printSymTable(t->child[i]);
    		} else {
    			break;
    		}
    	}
    	printf("} End new scope\n");
	}
}
void buildSymbolTable(Node *n, SymbolTable* cur){
    char* name;
    SYMBOL* s;
    SymbolTable* newTable;
    if (n != NULL){
        switch (n->kind) {
            case k_NodeKindProg:
                buildSymbolTable(n->val.prog.line, cur);
                buildSymbolTable(n->val.prog.program, cur);
                break;
            
            case k_NodeKindLine:
            	buildSymbolTable(n->val.node, cur);
            	break;

            case k_NodeKindDeclaration:
                name = (n->val.declaration.ident)->val.identifier;
                s = putSymbol(n, cur);
				n->val.declaration.ident->type = s->type;
				n->val.declaration.ident->symbol = s;
                break;
                
            case k_NodeKindDeclarationType:
                name = (n->val.declaration.ident)->val.identifier;
                s = putSymbol(n, cur);
                n->val.declaration.ident->type = s->type;
				n->val.declaration.ident->symbol = s;
                break;

            case k_NodeKindExpIdentifier:
                name = n->val.identifier;
                s = getSymbol(name, cur);
                if (s == NULL){
                	printSymTable(t);			/*Print the table before exit*/
                    fprintf(stderr, "Error: (line %d) \"%s\" is not declared\n", n->lineno, name);
            		exit(1);
                }
                n->symbol = s;
                n->type = s->type;
                break;

            case k_NodeKindExpAddition:
            case k_NodeKindExpSubtraction:
            case k_NodeKindExpMultiplication:
            case k_NodeKindExpDivision:
            case k_NodeKindExpEqual:
            case k_NodeKindExpNotEqual:
            case k_NodeKindExpAnd:
            case k_NodeKindExpOr:
            case k_NodeKindExpGreater:
			case k_NodeKindExpGreaterEqual:
			case k_NodeKindExpSmaller:
			case k_NodeKindExpSmallerEqual:
                buildSymbolTable(n->val.binary.lhs, cur);
                buildSymbolTable(n->val.binary.rhs, cur);
                break;

            case k_NodeKindExpUMinus:
            case k_NodeKindExpNeg:
            	buildSymbolTable(n->val.node, cur);
            	break;
            
            case k_NodeKindStatementRead:	
            	buildSymbolTable(n->val.node, cur);
            	break;
            case k_NodeKindStatementPrint:	
            	buildSymbolTable(n->val.node, cur);
            	break;
            case k_NodeKindStatementAssign:
                buildSymbolTable(n->val.assignStatement.ident, cur);
                buildSymbolTable(n->val.assignStatement.exp, cur);
                break;
            case k_NodeKindStatementWhile:
                buildSymbolTable(n->val.whileStatement.exp, cur);
                newTable = scopeSymbolTable(cur);
                buildSymbolTable(n->val.whileStatement.program, newTable);
                break;
            case k_NodeKindStatementIfElseIf:
                buildSymbolTable(n->val.ifElseIfStatement.expression, cur);
                newTable = scopeSymbolTable(cur);
                buildSymbolTable(n->val.ifElseIfStatement.program, newTable);
                buildSymbolTable(n->val.ifElseIfStatement.elseIf, cur);
                break;
            case k_NodeKindStatementElseIf:
                buildSymbolTable(n->val.elseIfStatement.elseIf, cur);
                buildSymbolTable(n->val.elseIfStatement.expression, cur);
                newTable = scopeSymbolTable(cur);
                buildSymbolTable(n->val.elseIfStatement.program, newTable);
                break;
            case k_NodeKindStatementIfElse:
                buildSymbolTable(n->val.ifElseStatement.expression, cur);
                newTable = scopeSymbolTable(cur);
                buildSymbolTable(n->val.ifElseStatement.program, newTable);
                buildSymbolTable(n->val.ifElseStatement.elseIf, cur);
                newTable = scopeSymbolTable(cur);
                buildSymbolTable(n->val.ifElseStatement.program2, newTable);
                break;

            default: break;
        }
    }
}

