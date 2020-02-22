#include "tree.h"
#define HashSize 317

typedef struct SymbolTable {
    SYMBOL *table[HashSize];
	struct SymbolTable* parent;
	struct SymbolTable* child[1000];
} SymbolTable;  // Implement scoped symbol table.


int Hash(char *str);
SymbolTable *initSymbolTable();
SymbolTable* scopeSymbolTable(SymbolTable* parent);
SYMBOL *putSymbol(Node* declaration, SymbolTable* cur);
SYMBOL *getSymbol(char *name, SymbolTable* cur);
//bool symbolNotDefined(char* name);
void symProgram(Node *n, bool printTable);
void printSymTable(SymbolTable* t);
void buildSymbolTable(Node *n, SymbolTable* cur);
