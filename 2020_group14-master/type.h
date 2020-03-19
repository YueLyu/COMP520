ifndef TYPECHECK_H
#define TYPECHECK_H
#include "tree.h"
#include "symbol.h"

TYPE * resolveType(SymbolTable * t, SYMBOL * sym);
TYPE * checkValidType(SymbolTable * t, TYPE * type, int lineno);
bool compareType(Type* a, Type* b);
bool compareIdList(Exp *ids1,Exp *ids2);
Type *inferType_Exp(SymbolTable* t, Exp* n);


#endif /* !TYPECHECK_H */
