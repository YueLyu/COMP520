#ifndef SYMBOL_H
#define SYMBOL_H

#include "tree.h"


SYMBOL * makeSymbol_var(char * name, Type * type, int lineno);
SYMBOL * makeSymbol_const(char * name, Type * type);
SYMBOL * makeSymbol_function(char * name, SYMBOLLIST * func_params, Type *returnType, int lineno);
SYMBOL * makeSymbol_base(char* name, Type * type);
SYMBOL * makeSymbol_type(char * name, Type * type, int lineno);
SYMBOL * makeSymbol_struct(char * name, SYMBOLLIST * fieldList, Type * structtype);
SYMBOL * makeSymbol_array(char * name, SYMBOL * entrytype, Type * arraytype);
SYMBOL * makeSymbol_slice(char * name, SYMBOL * entrytype, Type * slicetype);
SYMBOLLIST * makeSymbolList(Decl *func_params);
int Hash(char *str);
SymbolTable *initSymbolTable();
SymbolTable* scopeSymbolTable(SymbolTable* parent);
SymbolTable * unscopeSymbolTable(SymbolTable *t);
SYMBOL *putSymbol(SymbolTable *cur, SYMBOL * sym);
void symProg(Prog *n, bool printTable);
void symDecl(Decl *n, SymbolTable* cur);
void symStmt(Stmt *n, SymbolTable* cur, SYMBOLLIST *paramList);
void symExp(Exp *n, SymbolTable* cur);
void printSymTable(SymbolTable* t);
#endif 
