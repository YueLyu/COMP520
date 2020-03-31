#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "symbol.h"

SymbolTable *root;
SymbolTable *t;
int scope = 0;

// Make symbol related functions.
SYMBOL * makeSymbol_var(char * name, Type * type, int lineno) {
	SYMBOL * s = malloc(sizeof(SYMBOL));
	s->kind = symkind_var;
	s->name = name;
	s->typelit.type = type;
	s->lineno = lineno;
	return s;
}

SYMBOL * makeSymbol_const(char * name, Type * type) {
	SYMBOL * s = malloc(sizeof(SYMBOL));
	s = makeSymbol_var(name, type, 0);
	s->kind = symkind_const;
	return s;
}

SYMBOL * makeSymbol_function(char * name, TYPELIST * typelist, Type *returnType, int lineno){
	SYMBOL * s = malloc(sizeof(SYMBOL));
	s->kind = symkind_func;
	s->name = name;
	s->typelit.functiondec.typelist = typelist;
	s->typelit.functiondec.returnType = returnType;
	s->lineno = lineno;
	return s;
}

SYMBOL * makeSymbol_base(char* name, Type * type) {
	SYMBOL * s = malloc(sizeof(SYMBOL));
	s->kind = symkind_base;
	s->name = name;
	s->typelit.type = type;
	
	return s;
}

SYMBOL * makeSymbol_definedType(char * name, Type * type, int lineno) {
	SYMBOL * s = malloc(sizeof(SYMBOL));
	s->kind = symkind_definedType;
	s->name = name;
	s->typelit.type = type;
	s->lineno = lineno;
	return s;
}

SYMBOL * makeSymbol_type(char * name, Type * type, int lineno) {
	SYMBOL * s = malloc(sizeof(SYMBOL));
	s->kind = symkind_type;
	s->name = name;
	s->typelit.type = type;
	s->lineno = lineno;
	return s;
}

SYMBOL * makeSymbol_struct(char * name, SYMBOLLIST * fieldList, Type * structtype) {
	SYMBOL * s = malloc(sizeof(SYMBOL));
	s->kind = symkind_struct;
	s->name = name;
	s->typelit.structdec.fieldList = fieldList;
	s->typelit.structdec.structtype = structtype;
	return s;
}

SYMBOL * makeSymbol_array(char * name, SYMBOL * entrytype, Type * arraytype) {
	SYMBOL * s = malloc(sizeof(SYMBOL));
	s->kind = symkind_array;
	s->name = name;
	s->typelit.arraydec.entrytype = entrytype;
	s->typelit.arraydec.arraytype = arraytype;
	return s;
}

SYMBOL * makeSymbol_slice(char * name, SYMBOL * entrytype, Type * slicetype) {
	SYMBOL * s = malloc(sizeof(SYMBOL));
	s->kind = symkind_slice;
	s->name = name;
	s->typelit.slicedec.entrytype = entrytype;
	s->typelit.slicedec.slicetype = slicetype;
	return s;
}

//For structs and functions: makes a list of symbols for each field/parameter respectively
SYMBOLLIST * makeSymbolList(SymbolTable* t, Decl *funcParams) {
	SYMBOLLIST * list = malloc(sizeof(SYMBOLLIST));
	SYMBOLLIST * head = list;
	Decl * temp = funcParams;
	while (temp != NULL) { //loops through list of params
		Exp * curId = temp->val.func_params.identifiers;
		while (curId != NULL) { //loops through idlist of list of params
			SYMBOLLIST * l = head;
			while (l != list) {
				if(strcmp(l->currSym->name, curId->val.identifiers.identifier->val.identifier) == 0 && strcmp(l->currSym->name, "_") != 0) {
					fprintf(stderr, "Error: %s appears more than once in the list\n", l->currSym->name);
					exit(1);
				}
				l = l->next;
			}
			checkType(t, temp->val.func_params.identifier_type);
			list->currSym = makeSymbol_var(curId->val.identifiers.identifier->val.identifier, temp->val.func_params.identifier_type, funcParams->lineno);
			list->next = malloc(sizeof(SYMBOLLIST));
			list = list->next;
			curId =curId->val.identifiers.identifiers;
		}
		
		temp = temp->val.func_params.func_params;
	}
	list->currSym = NULL;

	return head;
}

TYPELIST * makeTypeList(SYMBOLLIST * params) {
	SYMBOLLIST * currS = params;
	TYPELIST * list = malloc(sizeof(TYPELIST));
	TYPELIST * head = list;
	while (currS != NULL && currS->currSym != NULL) {
		s = currS->currSym;
		list->currType = s.typelit.type;
		list->next = malloc(sizeof(TYPELIST));
		list = list->next;
		currS = currS->next;
	}
	return head;
}

//Symbol table related functions.
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

SymbolTable * unscopeSymbolTable(SymbolTable *t) {
	return t->parent;
}

SYMBOL *putSymbol(SymbolTable *cur, SYMBOL * sym) {
	char* name = strdup(sym->name);
	if (sym != NULL && strcmp(sym->name, "_") != 0) {
		int i = Hash(name);
		for (SYMBOL *s = cur->table[i]; s; s = s->next) {
			if (strcmp(s->name, name) == 0 /*&& s->kind == sym->kind*/) {
				printSymTable(cur);			/*Print the table before exit*/
			    fprintf(stderr, "Error: (line %d) \"%s\" is already declared\n", sym->lineno, name);
			    exit(1);
			}
		}

		sym->next = cur->table[i];
		cur->table[i] = sym;
	} else if (sym == NULL) {
		fprintf(stderr, "Error: trying to put a NULL symbol into the table.\n");
		exit(1);
	}
	return sym;
}

SYMBOL *getSymbol(SymbolTable *cur, char *name) {
	int i = Hash(name);
	// Check the current scope
	for (SYMBOL *s = cur->table[i]; s; s = s->next) {
		if (strcmp(s->name, name) == 0) return s;
	}
	// Check for existence of a parent scope
	if (cur->parent == NULL) {
		fprintf(stderr, "Error:  %s has not been declared in this context\n", name);
		exit(1);
	}
	// Check the parent scopes
	return getSymbol(cur->parent, name);
}

void symProg(Prog *n, bool printTable){	// Start building the table for input program
	if (n != NULL) {
		t = initSymbolTable();
		root = t;
		
		putSymbol(t, makeSymbol_base("int", newIdType("int", 0)));
		putSymbol(t, makeSymbol_base("float64", newIdType("float64", 0)));
		putSymbol(t, makeSymbol_base("bool", newIdType("bool", 0)));
		putSymbol(t, makeSymbol_base("rune", newIdType("rune", 0)));
		putSymbol(t, makeSymbol_base("string", newIdType("string", 0)));
		putSymbol(t, makeSymbol_const("true", newIdType("bool", 0)));
		putSymbol(t, makeSymbol_const("false",newIdType("bool", 0)));
		
		//SymbolTable* cur = scopeSymbolTable(t);
		symDecl(n->program.package_dec, t);
		symDecl(n->program.top_decs, t);
		
		if (printTable){
		    printSymTable(root);
		}
	}
}


void symDecl(Decl *n, SymbolTable* cur){
    if (n != NULL){
		switch (n->kind) {
			case k_NodeKindPackageDec:
				putSymbol(cur, makeSymbol_var(n->val.package_dec.identifier->val.identifier, NULL, n->lineno));
				break;		//TODO: Not sure!
		    case k_NodeKindTopDecs:
		    	symDecl(n->val.top_decs.top_decs, cur);
		    	symDecl(n->val.top_decs.dec, cur);
		    	break;
		    case k_NodeKindVarDecLine:
		    case k_NodeKindVarDecPar:
		    	symDecl(n->val.var_dec.def, cur);
		    	break;
		    case k_NodeKindVarDefs:
		    	symDecl(n->val.var_defs.var_defs, cur);
		    	symDecl(n->val.var_defs.var_def, cur);
		    	break;
		    case k_NodeKindVarDef:
		    	if (n->val.var_def.identifier_type == NULL) { // var x, y, z = 1, 2, 3
					int len = 0;
					Exp* temp_exp = n->val.var_def.expressions;
					while (temp_exp != NULL) {
						len ++;
						temp_exp = temp_exp->val.expressions.expressions;
					}
					Type* arr [len]; 
					int pos = 0;
					temp_exp = n->val.var_def.expressions;
					while (temp_exp != NULL) {
						arr[pos] = inferType_Exp(t, temp_exp->val.expressions.expression);
						temp_exp = temp_exp->val.expressions.expressions;
						pos ++;
					}
					/*
					for (int i = 0; i < len - 1; i ++) {
						if (!compareType(arr[i], arr[i + 1])) {
							fprintf(stderr, "Error: (line %d) The types of expressions in the variable declaration doesn't match.\n", n->lineno);
							exit(1);
						}
					}*/
					int len2 = 0;
					Exp* temp_id = n->val.var_def.identifiers;
					while (temp_id != NULL) {
						temp_id = temp_id->val.identifiers.identifiers;
						len2 ++;
					}
					if (len != len2) {
						fprintf(stderr, "Error: (line %d) The number of expressions and the number of variables don't match.\n", n->lineno);
							exit(1);
					}
					pos = 0;
					temp_id = n->val.var_def.identifiers;
					while (temp_id != NULL) {
						SYMBOL* s = makeSymbol_var(temp_id->val.identifiers.identifier->val.identifier, arr[pos], n->lineno);
						putSymbol(cur, s);
						temp_id = temp_id->val.identifiers.identifiers;
						pos ++;
					}
				} else if (n->val.var_def.identifier_type != NULL && n->val.var_def.expressions != NULL) { // var x, y, z int = 1, 2, true
					checkType(cur, n->val.var_def.identifier_type);
					Type* type = n->val.var_def.identifier_type;
					int len = 0;
					Exp* temp_exp = n->val.var_def.expressions;
					while (temp_exp != NULL) {
						len ++;
						temp_exp = temp_exp->val.expressions.expressions;
					}
					Type* arr [len]; 
					int pos = 0;
					temp_exp = n->val.var_def.expressions;
					while (temp_exp != NULL) {
						arr[pos] = inferType_Exp(t, temp_exp->val.expressions.expression);
						temp_exp = temp_exp->val.expressions.expressions;
						pos ++;
					}

					if (len != pos) {
						fprintf(stderr, "Error: (line %d) The number of expressions and the number of variables don't match.\n", n->lineno);
							exit(1);
					}
					for (int i = 0; i < len; i ++) {
						if (!compareType(arr[i], type)) {
							fprintf(stderr, "Error: (line %d) The types of expressions in the variable declaration doesn't match.\n", n->lineno);
							exit(1);
						}
					}
					Exp* temp_id = n->val.var_def.identifiers;
					while (temp_id != NULL) {//didn't check num of exp = num of id;
						SYMBOL* s = makeSymbol_var(temp_id->val.identifiers.identifier->val.identifier, arr[0], n->lineno);
						putSymbol(cur, s);
						temp_id = temp_id->val.identifiers.identifiers;
					}
				} else if (n->val.var_def.expressions == NULL) {	// var x, y, z int
					Exp* temp = n->val.var_def.identifiers;
					while (temp != NULL) {
						SYMBOL* s = makeSymbol_var(temp->val.identifiers.identifier->val.identifier, n->val.var_def.identifier_type, n->lineno);
						//temp->val.identifiers.identifier->sym = s;	// Link symbol to the node in AST.
						putSymbol(cur, s);
						temp = temp->val.identifiers.identifiers;
					}
				}
				break;
		    case k_NodeKindTypeDecPar:
		    case k_NodeKindTypeDecLine:
		    	symDecl(n->val.type_dec.def, cur);
				break;
		    case k_NodeKindTypeDefs:
		    	symDecl(n->val.type_defs.type_defs, cur);
		    	symDecl(n->val.type_defs.type_def, cur);
				break;
		    //case k_NodeKindFuncDec:
		    case k_NodeKindTypeDef:
		    	//type check type definition
		    	;
		    	checkType(cur, n->val.type_def.identifier_type);
		    	putSymbol(cur, makeSymbol_type(n->val.type_def.identifier->val.identifier, n->val.type_def.identifier_type, n->lineno));
				break;
		    case k_NodeKindFuncDec:
		        //check for special functions
		    	if(strcmp(n->val.func_dec.identifier->val.identifier, "main")==0||strcmp(n->val.func_dec.identifier->val.identifier, "main")==0){
					if(n->val.func_dec.func_type!=NULL||n->val.func_dec.func_params!=NULL){
						fprintf(stderr, "Error: (line %d) funcion init or main should have no parameters nor return value.\n", n->lineno);
						exit(1);
					}
		    	}
		    	if(n->val.func_dec.func_type->val.func_type.identifier_type!=NULL){
		    		checkType(cur, n->val.func_dec.func_type->val.func_type.identifier_type);
		    		//update return type
		    		cur_return_type = n->val.func_dec.func_type->val.func_type.identifier_type;
		    	}else{
		    		cur_return_type=NULL;
		    	}

		    	SYMBOLLIST* paramList = makeSymbolList(cur, n->val.func_dec.func_params);
		    	TYPELIST* typelist = makeTypeList(paramList);
		    	if (n->val.func_dec.func_type == NULL) {
		    		putSymbol(cur, makeSymbol_function(n->val.func_dec.identifier->val.identifier, typeList, NULL, n->lineno));
		    	} else {
		    		putSymbol(cur, makeSymbol_function(n->val.func_dec.identifier->val.identifier, typeList, n->val.func_dec.func_type->val.func_type.identifier_type, n->lineno));
		    	}

		    	if (n->val.func_dec.block_body != NULL) {
		    		symStmt(n->val.func_dec.block_body, cur, paramList);
		    	}
		    	break;
		    case k_NodeKindFuncParams:
		    case k_NodeKindFuncType:
				break;
		}
    }
}

void checkType(SymbolTable* t, Type* n){	// check if the type is defined.
	if (n!=NULL){
		switch (n->kind){
			case k_NodeKindArrayType:
			case k_NodeKindSliceType:
			case k_NodeKindParType:
			case k_NodeKindStructType:
				checkType(t, n->val.identifier_type.identifier_type);//[]  check emptyness: slice type with empty type identifier identifier_type is NULL
				break;
			case k_NodeKindIdType:
				getSymbol(t, n->val.identifier);//[]??? check undefined: slice type with undefined type identifier will trigger error in getSymbol()
				break;
			case k_NodeKindStructBody:
				if(n->val.struct_body.struct_body!=NULL){
					checkType(t, n->val.struct_body.struct_body);
				}
				checkType(t, n->val.struct_body.type);
				break;
		}
	}else{
		fprintf(stderr, "Error: (line %d) null type.\n", n->lineno);
		exit(1);
	}
}

void symStmt(Stmt *n, SymbolTable* cur, SYMBOLLIST *paramList){
	if (n != NULL){
		switch (n->kind) {
			//statements
			case k_NodeKindStatements:
				symStmt(n->val.statements.statements,cur, NULL);
				symStmt(n->val.statements.statement,cur, NULL);
				break;
			case k_NodeKindStatement:
				symStmt(n->val.statement.stmt,cur, NULL);
				break;
			case k_NodeKindStatementDec:	// typeDec, varDec
				symDecl(n->val.statement_dec.decl,cur);
				break;
			case k_NodeKindBlockBody:
				;
				SymbolTable *child=scopeSymbolTable(cur);
				if (paramList != NULL) {
					SYMBOLLIST * currS = paramList;
					while (currS != NULL && currS->currSym != NULL) {
						if (strcmp(currS->currSym->name, "_" ) != 0) {
							putSymbol(child, currS->currSym);
						}
						currS = currS->next;
					}
				}
				symStmt(n->val.block_body.statements,child, NULL);
				cur=unscopeSymbolTable(child);
				break;
			case k_NodeKindSimpleStatementDec:
				symStmt(n->val.simple_statement_dec.statement,cur, NULL);
				break;
			case k_NodeKindSimpleStatementExp:  //use inferType_Exp instead of symExp: 1. symExp never updates the symbol table  2. if the type of the expression can be infered, then the identifiers in this expression must already exist in the symbol table
				if(n->val.simple_statement.lhs->kind!=k_NodeKindFuncCall){	// Weeding out illegal expression statements.
					fprintf(stderr, "Error: (line %d)  In GoLite, only function call expressions are allowed to be used as statements.\n", n->lineno);
					exit(1);
				}
				inferType_Exp(cur,n->val.simple_statement.lhs);
				break;

			case k_NodeKindSimpleStatementInc: //TODO:
			{
				Type *varType=inferType_Exp(n->val.simple_statement.lhs);
				if(isNumeric(varType)==false){
					fprintf(stderr, "Error: (line %d)  Variable of increment statement is not numeric.\n", n->lineno);
					exit(1);
				}
				break;
			}
			case k_NodeKindSimpleStatementDecrease: //TODO:
			{
				Type *varType=inferType_Exp(n->val.simple_statement.lhs);
				if(isNumeric(varType)==false){
					fprintf(stderr, "Error: (line %d)  Variable of decrement statement is not numeric.\n", n->lineno);
					exit(1);
				}
				break;
			}

			case k_NodeKindSimpleStatementEqual: // assignment
				{
					;
					int idListLength=0;
					Exp *ids=n->val.simple_statement.lhs;
					while(ids!=NULL){
						idListlength++;
						ids=ids->val.primary_expressions.primary_expressions;
					}
					Type *idList[idListlength];
					ids=n->val.simple_statement.lhs;
					for(int i=0;i<idListlength;i++){
						idList[i]=inferType_Exp(ids->val.primary_expressions.primary_expression);
						ids=ids->val.primary_expressions.primary_expressions;
					}
					int expListLength=0;
					Exp *exps=n->val.simple_statement.rhs;
					while(exps!=NULL){
						expListLength++;
						exps=exps->val.expressions.expressions;
					}
					if(idListlength!=expListLength){
						fprintf(stderr, "Error: (line %d)  The number of identifiers doesn't match the number of expressions.\n", n->lineno);
						exit(1);
					}
					Type *expList[idListlength];
					exps=n->val.simple_statement.rhs;
					for(int i=0;i<expsListlength;i++){
						expList[i]=inferType_Exp(exps->val.expressions.expression);
						exps=exps->val.expressions.expressions;
					}
					for(int i=0;i<idListlength;i++){
						if(!compareType(idList[i],expList[i])){
							fprintf(stderr, "Error: (line %d)  The type of identifier doesn't match the type of expression.\n", n->lineno);
							exit(1);
						}
					}
					break;
				}
			case k_NodeKindSimpleStatementPlusEqual:
			case k_NodeKindSimpleStatementMinEqual:
			case k_NodeKindSimpleStatementMulEqual:
			case k_NodeKindSimpleStatementDivEqual:
			case k_NodeKindSimpleStatementModEqual:
			case k_NodeKindSimpleStatementBitAndEqual:
			case k_NodeKindSimpleStatementBitOrEqual:
			case k_NodeKindSimpleStatementBitXorEqual:
			case k_NodeKindSimpleStatementLeftShiftEqual:
			case k_NodeKindSimpleStatementRightShiftEqual:
			case k_NodeKindSimpleStatementBitClearEqual:
			{
				Exp *exp_lhs = inferType_Exp(n->val.simple_statement.lhs);
				Exp *exp_rhs = inferType_Exp(n->val.simple_statement.rhs);
				if(!compareType(exp_lhs, exp_rhs)){
						fprintf(stderr, "Error: (line %d)  The left hand side variables don't have the same types as the right hand side expressions.\n", n->lineno);
						exit(1);
					}
				}
				n->type = exp_lhs;
				break;
			}
			case k_NodeKindSimpleStatementDeclEqual:
				{
					;
					//short hand declarations
					int idListlength=0;
					Exp *ids=n->val.simple_statement.lhs;
					while(ids!=NULL){//every identifiers that is not null has an identifier and an identifiers
						idListlength++;
						ids=ids->val.identifiers.identifiers;
					}
					Exp *idList[idListlength];
					ids=n->val.simple_statement.lhs;
					for(int i=0;i<idListlength;i++){
						idList[i]=ids->val.identifiers.identifer;
						ids=ids->val.identifiers.identifiers;
					}
					bool existsUndeclaredVar=false;
					for(int i=0;i<idListlength;i++){
						char *id=idList[i]->val.identifier;
						if(checkSymbolExists(cur,id)==NULL){
							existsUndeclaredVar=true;
						}
					}
					if(existsUndeclaredVar=false){
						fprintf(stderr, "Error: (line %d)  All identifiers on the left hand side have been declared.\n", n->lineno);
						exit(1);
					}
					int expListLength=0;
					Exp *exps=n->val.simple_statement.rhs;
					while(exps!=NULL){
						expListLength++;
						exps=exps->val.expressions.expressions;
					}
					if(idListLength!=expListLength){
						fprintf(stderr, "Error: (line %d)  The number of dentifiers doesn't match the number of expressions.\n", n->lineno);
						exit(1);
					}
					Type *expList[expListLength];
					exps=n->val.simple_statement.rhs;
					for(int i=0;i<expListlength;i++){
						expList[i]=inferType_Exp(exps->val.expressions.expression);
						exps=exps->val.expressions.expressions;
					}
					/*for(int i=0;i<expListLength;i++){
						if(compareType(expList[i],expList[i+1])==false){
							fprintf(stderr, "Error: (line %d) The types of expressions in the short declaration doesn't match.\n", n->lineno);
							exit(1);
						}
					}*/
					Type *expsType=expList[0];
					bool allIdsNotDeclared=true;
					for(int i=0;i<idListlength;i++){
						char *id=idList[i]->val.identifier;
						if(checkSymbolExists(cur,id)!=NULL){
							allIdsNotDeclared=false;
							break;
						}
					}
					if(allIdsNotDeclared==true){//if none of the identifiers has been decalred(none of the identifiers has a type)
						for(int i=0;i<idListlength;i++){
							char *id=idList[i]->val.identifier;
							SYMBOL *s=makeSymbol_var(id,expsType,n->lineno);
							putSymbol(cur,s);
							}
						}
					}else{//if some of the identifiers has been decalred
						for(int i=0;i<idListlength;i++){
							char *id=idList[i]->val.identifier;
							if(checkSymbolExists(cur,id)==NULL){//for the undeclared identifiers
								SYMBOL *s=makeSymbol_var(id,expsType,n->lineno);
								putSymbol(cur,s);
							}else{//for the declared identifiers
								Type *idType=getSymbol(cur,id)->typelit.type;
								if(idType!=expsType){
									fprintf(stderr, "Error: (line %d) The types of identifier doesn't match the type of expressions.\n", n->lineno);
									exit(1);
								}else{
									SYMBOL *s=makeSymbol_var(id,expsType,n->lineno);
									putSymbol(cur,s);
								}
							}
						}
					}
					break;
				}

			case k_NodeKindPrintDec:
			case k_NodeKindPrintlnDec:
			{
				Exp *exps=n->val.print_dec.expressions_opt->val.expressions;
				while(exps!=NULL){
					Type *expType=inferType_Exp(exps.expression);
					if(isBool(expType)==false&&isString(expType)==false&&isNumeric(expType)==false){
						fprintf(stderr, "Error: (line %d)  The printed expression is not one of the basic types.\n", n->lineno);
						exit(1);
					}
				}
				break;
			}
			
			case k_NodeKindReturn:
				if(cur_return_type==NULL&&n->val.return_dec.expression_opt!=NULL){//the field of return_dec should be expression_opt instead of expressions_opt
				fprintf(stderr, "Error: (line %d)  The return type of the function is void, but an expression is returned.\n", n->lineno);
				exit(1);
				}else if(cur_return_type==NULL&&n->val.return_dec.expression_opt==NULL){
				break;
				}else if(cur_return_type!=NULL&&n->val.return_dec.expression_opt==NULL){
				fprintf(stderr, "Error: (line %d)  The return type of the function is not void, but an nothing is returned.\n", n->lineno);
				exit(1);
				}else{
					Type *returnType=inferType_Exp(n->val.return_dec.expressions_opt,cur);
					if(compareType(cur_return_type,returnType)==false){
						fprintf(stderr, "Error: (line %d)  The type of the returned expression does not match the type of the funciton.\n", n->lineno);
						exit(1);
					}else{
						break;
					}
				}
			
			case k_NodeKindIfStmt:
			{
				if(n->val.if_stmt.simple_statement_dec!=NULL){
					SymbolTable *child=scopeSymbolTable(cur);
					symStmt(n->val.if_stmt.simple_statement_dec,child,NULL);
					Type *expType=inferType_Exp(n->val.if_stmt.expression);
					if(isBool(expType)==false){
						fprintf(stderr, "Error: (line %d)  if condition is not of type bool.\n", n->lineno);
						exit(1);
					}
					symStmt(n->val.if_stmt.block_body,child,NULL);
					symStmt(n->val.if_stmt.else_stmt,child,NULL);
					cur=unscopeSymbolTable(child);
				}else{
					Type *expType=inferType_Exp(n->val.if_stmt.expression);
					if(isBool(expType)==false){
						fprintf(stderr, "Error: (line %d)  if condition is not of type bool.\n", n->lineno);
						exit(1);
					}
					symStmt(n->val.if_stmt.block_body,child,NULL);
					symStmt(n->val.if_stmt.else_stmt,child,NULL);
					}
				break;
			}
			case k_NodeKindElseStmt:
				symStmt(n->val.else_stmt.if_stmt,cur, NULL);
				symStmt(n->val.else_stmt.block_body,cur, NULL);
				break;
			case k_NodeKindForDec:
			{
				if(n->val.for_dec.for_condition->val.for_condition.left!=NULL){
					SymbolTable *child=scopeSymbolTable(cur);
					symStmt(n->val.for_dec.for_condition->val.for_condition.left,child, NULL);//put variables declared in for loop's init into an independent intermediate symbol table

					symStmt(n->val.for_dec.for_condition,child, NULL);//According to specification, declarations in init can shadow declarations in for loop's scope

					symStmt(n->val.for_dec.block_body,child, NULL);//then traverse the for loop's body with the init's declarations
					cur=unscopeSymbolTable(child);
				}
				else{
					symStmt(n->val.for_dec.for_condition,cur, NULL);
					symStmt(n->val.for_dec.block_body,cur, NULL);
				}
				break;
			}
			case k_NodeKindForCondition:
			{
				Stmt *cond=n->val.for_condition;
				if(cond.left!=NULL&&cond.expression!=NULL&&cond.right!=NULL){
					symStmt(cond.left);
					Type *expType=inferType_Exp(cond.expression);
					if(isBool(expType)==false){
						fprintf(stderr, "Error: (line %d)  for condition is not of type bool.\n", n->lineno);
						exit(1);
					}
					symStmt(cond.right);
				}else if(cond.expression!=NULL){
					Type *expType=inferType_Exp(cond.expression);
					if(isBool(expType)==false){
						fprintf(stderr, "Error: (line %d)  for condition is not of type bool.\n", n->lineno);
						exit(1);
					}
				}else{
					break;
				}
			}
			case k_NodeKindSwitchDec:
				if(n->val.switch_dec.switch_def->val.switch_def.simple_statement!=NULL){
					SymbolTable *child=scopeSymbolTable(cur);
					symStmt(n->val.switch_dec.switch_def->val.switch_def.simple_statement,child,NULL)
					if(n->val.switch_dec.switch_def->val.switch_def.expression_opt!=NULL){//switch_def has both exp and stmt
						symExp(n->val.switch_def.expression_opt,child,NULL);
						symStmt(n->val.switch_dec.switch_cases,child,NULL);
					}else{//switch_def only has stmt
						Stmt *temp=n->val.switch_dec.switch_cases;
						while(temp!=NULL){
							if(isBool(temp->val.switch_cases.expression)==false){
								fprintf(stderr, "Error: (line %d)  Case expression is not bool while switching without expression.\n", n->lineno);
								exit(1);
							}
							symStmt(temp->val.switch_cases.statements,child,NULL);
							temp=temp->val.switch_cases.switch_cases;
						}
					}
					cur=unscopeSymbolTable(child);
				}else{//switch_def has only exp
					if(n->val.switch_dec.switch_def->val.switch_def.expression_opt!=NULL){//switch_def has both exp and stmt
						symExp(n->val.switch_def.expression_opt,cur,NULL);
						symStmt(n->val.switch_dec.switch_cases,cur,NULL);
					}else{//switch_def is empty
						Stmt *temp=n->val.switch_dec.switch_cases;
						while(temp!=NULL){
							if(isBool(temp->val.switch_cases.expression)==false){
							fprintf(stderr, "Error: (line %d)  Case expression is not bool while switching without expression.\n", n->lineno);
							exit(1);
							}
							symStmt(temp->val.switch_cases.statements,cur,NULL);
							temp=temp->val.switch_cases.switch_cases;
						}
					}
				}
				symStmt(n->val.switch_dec.switch_def,cur, NULL);
				symStmt(n->val.switch_dec.switch_cases,cur, NULL);
				break;
			case k_NodeKindSwitchDef:
				break;
			case k_NodeKindDefault:
			case k_NodeKindSwitchCases:
				symStmt(n->val.switch_cases.cases,cur, NULL);
				symExp(n->val.switch_cases.expressions,cur);
				symStmt(n->val.switch_cases.statements,cur, NULL);
				break;
			
			case k_NodeKindBreak:
			case k_NodeKindContinue:
					break;
			}
		}
}

void indent(int x) {
	for (int i = 0; i < x; i ++) {
		printf("    ");
	}
}

char *getType(Type* n) {
	if (n != NULL) {
		switch (n->kind) {
			case k_NodeKindArrayType:
				printf("reach1");
				char *secondHalf=getType(n->val.identifier_type.identifier_type); //the inner layers of the type
				char *leftParen="[";
				char length[10];
				sprintf(length, "%d", n->val.identifier_type.size);
				char *rightParen="]";
				char *result1=(char *) malloc(sizeof(char) * (1 + strlen(length)+ strlen(leftParen)+ strlen(rightParen)+ strlen(secondHalf)));
				strcpy(result1, leftParen); //[
				strcat(result1,length); //[3
				strcat(result1, rightParen); //[3]
				strcat(result1, secondHalf); //[3]int, or [3][4]int
				printf("type1: %s", result1);
				return result1;
			case k_NodeKindSliceType:
				printf("reach2");
				char *second=getType(n->val.identifier_type.identifier_type); //the inner layers of the type
				char *leftPar="[";
				char *rightPar="]";
				char *result2=(char *) malloc(sizeof(char) * (1 + strlen(leftPar)+ strlen(rightPar)+ strlen(secondHalf)));
				strcpy(result2, leftPar); 
				strcat(result2, rightPar); 
				strcat(result2, second); 
				printf("type2: %s", result2);
				return result2;
			case k_NodeKindIdType:
				printf("reach3\n");
			    printf("%s",n->val.identifier);
				char *result4 = (char *) malloc(sizeof(char) * (1+strlen(n->val.identifier)));
				strcpy(result4, n->val.identifier);
				printf("type4: %s", result4);
				return result4;
			case k_NodeKindStructType:
				printf("reach4");
				char* s = "struct {\n";
				char* body = getType(n->val.identifier_type.identifier_type);
				char* rightPar3 = "\n}\n";
				char* result3 = (char *) malloc(sizeof(char) * (1 + strlen(s)+ strlen(body)+ strlen(rightPar3)));
				strcpy(result3, s); 
				strcat(result3, body); 
				strcat(result3, rightPar3); 
				printf("type3: %s", result3);
				return result3;
			case k_NodeKindParType:
				printf("reach5");
				printf("type5: %s", getType(n->val.identifier_type.identifier_type));
				return getType(n->val.identifier_type.identifier_type);
			case k_NodeKindStructBody:
				printf("reach6");
				char* first_half = getType(n->val.struct_body.struct_body);
				char* second_half = getType(n->val.struct_body.type);
				char* id = (char *)malloc(sizeof(char) * (1+strlen(first_half)+strlen(second_half)));
				Exp* temp = n->val.struct_body.identifiers;
				while (temp != NULL) {
					char* cur = temp->val.identifiers.identifier->val.identifier;
					char *buffer=(char *)malloc(sizeof(char) * (1+strlen(id)+strlen(cur)));
					strcpy(buffer, cur);
					strcat(buffer,id);
					id = buffer;
					temp = temp->val.identifiers.identifiers;
				}
				char *finalResult=(char *)malloc(sizeof(char) * (1+strlen(first_half)+strlen(id)+strlen(second_half)));
				strcpy(finalResult,first_half);
				strcat(finalResult, id);
				strcat(finalResult, second_half);
				printf("type6: %s", finalResult);
				return finalResult;
	  	}
	
	}
}

void printType(Type* n) {
	if (n != NULL) {
		switch (n->kind) {
			case k_NodeKindArrayType:
				printType(n->val.identifier_type.identifier_type); //the inner layers of the type
				printf("[");
				printf("%d", n->val.identifier_type.size);
				printf("]");
				break;
			case k_NodeKindSliceType:
				printType(n->val.identifier_type.identifier_type); //the inner layers of the type
				printf("[");
				printf("]");
				break;
			case k_NodeKindIdType:
			    printf("%s",n->val.identifier);
			    break;
			case k_NodeKindStructType:
				printf("struct {\n");
				printType(n->val.identifier_type.identifier_type);
				printf("\n}\n");
				break;
			case k_NodeKindParType:
				printType(n->val.identifier_type.identifier_type);
				break;
			case k_NodeKindStructBody:
				printType(n->val.struct_body.struct_body);
				
				char* id = "";
				Exp* temp = n->val.struct_body.identifiers;
				while (temp != NULL) {
					char* cur = temp->val.identifiers.identifier->val.identifier;
					char buffer [100];
					strcpy(buffer, cur);
					strcat(buffer,id);
					id = buffer;
					temp = temp->val.identifiers.identifiers;
				}
				
				printf("%s", id);
				printType(n->val.struct_body.type);
				break;
	  	}
	
	}
}

void printSymTable(SymbolTable* t){		// Print symbolTable tree
	if (t != NULL) {
		indent(scope);
		printf("New scope {\n");
		scope ++;
		for (int i = 0; i < HashSize; i++) {
		    if (t->table[i] != NULL){
		        for (SYMBOL *s = t->table[i]; s; s = s->next) {
		        	indent(scope);
		            printf("%s ", s->name);
					if (s->typelit.type != NULL) {
						printType(s->typelit.type);
						//getType(s->typelit.type);
						//char* str = getType(s->typelit.type);
						//printf("%c", str[0]);
						/*
						if (str == NULL) {
							printf("error");
						} else {
							printf("%s", str);
						}*/
						
					}
		            printf("\n");
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
    	scope --;
    	indent(scope);
    	printf("} End new scope\n");
	}
}

