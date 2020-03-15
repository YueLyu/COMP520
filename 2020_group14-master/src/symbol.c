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

SYMBOL * makeSymbol_function(char * name, SYMBOLLIST * paramList, Type *returnType, int lineno){
	SYMBOL * s = malloc(sizeof(SYMBOL));
	s->kind = symkind_func;
	s->name = name;
	s->typelit.functiondec.paramList = paramList;
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
					for (int i = 0; i < len; i ++) {
						if (!compareType(arr[i], type)) {
							fprintf(stderr, "Error: (line %d) The types of expressions in the variable declaration doesn't match.\n", n->lineno);
							exit(1);
						}
					}
					Exp* temp_id = n->val.var_def.identifiers;
					while (temp_id != NULL) {
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
		    case k_NodeKindFuncDec:
		    case k_NodeKindTypeDef:
		    	//type check type definition
		    	;
		    	checkType(cur, n->val.type_def.identifier_type);
		    	putSymbol(cur, makeSymbol_type(n->val.type_def.identifier->val.identifier, n->val.type_def.identifier_type, n->lineno));
				break;
		    case k_NodeKindFuncDec:
		    	if(n->val.func_dec.func_type->val.func_type.identifier_type!=NULL){
		    		checkType(cur, n->val.func_dec.func_type->val.func_type.identifier_type);
		    	}
		    	SYMBOLLIST* paramList = makeSymbolList(n->val.func_dec.func_params);
		    	if (n->val.func_dec.func_type == NULL) {
		    		putSymbol(cur, makeSymbol_function(n->val.func_dec.identifier->val.identifier, paramList, NULL, n->lineno));
		    	} else {
		    		putSymbol(cur, makeSymbol_function(n->val.func_dec.identifier->val.identifier, paramList, n->val.func_dec.func_type->val.func_type.identifier_type, n->lineno));
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

void checkType(SymbolTable* t, Type* n){
	if (n!=NULL){
		switch (n->kind){
			case k_NodeKindArrayType:
			case k_NodeKindSliceType:
			case k_NodeKindParType:
			case k_NodeKindStructType:
				checkType(t, n->val.identifier_type.identifier_type);
				break;
			case k_NodeKindIdType:
				getSymbol(t, n->val.identifier);
				break;
			case k_NodeKindStructBody:
				checkType(t, n->val.struct_body.struct_body);
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
			case k_NodeKindStatementDec:
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
			case k_NodeKindSimpleStatementExp:
				
			case k_NodeKindSimpleStatementInc:
			case k_NodeKindSimpleStatementDecrease:
			case k_NodeKindSimpleStatementEqual:
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
				symExp(n->val.simple_statement.lhs,cur);
				if(n->val.simple_statement.rhs!=NULL){symExp(n->val.simple_statement.rhs,cur);}
				break;
			case k_NodeKindSimpleStatementDeclEqual:
				//short hand declarations
			
			case k_NodeKindPrintDec:
				symExp(n->val.print_dec.expression_opt,cur);
				break;
			case k_NodeKindPrintlnDec:
				symExp(n->val.print_dec.expression_opt,cur);
				break;
			
			case k_NodeKindReturn:
				symExp(n->val.return_dec.expressions_opt,cur);
				break;
			
			case k_NodeKindIfStmt:
				symStmt(n->val.if_stmt.simple_statement_dec,cur, NULL);
				symExp(n->val.if_stmt.expression,cur);
				symStmt(n->val.if_stmt.block_body,cur, NULL);
				symStmt(n->val.if_stmt.else_stmt,cur, NULL);
				break;
			case k_NodeKindElseStmt:
				symStmt(n->val.else_stmt.if_stmt,cur, NULL);
				symStmt(n->val.else_stmt.block_body,cur, NULL);
				break;
			case k_NodeKindForDec:
				symStmt(n->val.for_dec.for_condition,cur, NULL);
				symStmt(n->val.for_dec.block_body,cur, NULL);
				break;
			case k_NodeKindForCondition:
				symStmt(n->val.for_condition.left,cur, NULL);
				symExp(n->val.for_condition.expression,cur);
				symStmt(n->val.for_condition.right,cur, NULL);
				break;
			case k_NodeKindSwitchDec:
				symStmt(n->val.switch_dec.switch_def,cur, NULL);
				symStmt(n->val.switch_dec.switch_cases,cur, NULL);
				break;
			case k_NodeKindSwitchDef:
				symStmt(n->val.switch_def.simple_statement,cur, NULL);
				symExp(n->val.switch_def.expression_opt,cur);
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


void symExp(Exp *n, SymbolTable* cur){
	if (n != NULL){
		switch (n->kind) {
			case k_NodeKindIdentifiers:
				symExp(n->val.identifiers.identifiers,cur);
				symExp(n->val.identifiers.identifier,cur);
				break;
			case k_NodeKindIdentifier:
				getSymbol(cur,n->val.identifier);
				break;
			case k_NodeKindExpressionsOpt:
			case k_NodeKindExpressions:
				symExp(n->val.expressions.expressions,cur);
				symExp(n->val.expressions.expression,cur);
				break;
			case k_NodeKindExpressionOpt:
			case k_NodeKindExpression:
				symExp(n->val.expression.expression,cur);
				break;
			
			case k_NodeKindExpressionBinaryPlus:
			case k_NodeKindExpressionBinaryMinus:
			case k_NodeKindExpressionBinaryMultiply:
			case k_NodeKindExpressionBinaryDivide:
			case k_NodeKindExpressionBinaryModulo:
			case k_NodeKindExpressionBinaryBitAnd:
			case k_NodeKindExpressionBinaryBitOr:
			case k_NodeKindExpressionBinaryBitXor:
			case k_NodeKindExpressionBinarybitClear:
			case k_NodeKindExpressionBinaryLeftShift:
			case k_NodeKindExpressionBinaryRightShift:
			case k_NodeKindExpressionBinaryIsEqual:
			case k_NodeKindExpressionBinaryIsNotEqual:
			case k_NodeKindExpressionBinaryLessThan:
			case k_NodeKindExpressionBinaryGreaterThan:
			case k_NodeKindExpressionBinaryLessThanEqual:
			case k_NodeKindExpressionBinaryGreaterThanEqual:
			case k_NodeKindExpressionBinaryAnd:
			case k_NodeKindExpressionBinaryOr:
				symExp(n->val.binary.lhs,cur);
				symExp(n->val.binary.rhs,cur);
				break;
			case k_NodeKindUMinus:
			case k_NodeKindUPlus:
			case k_NodeKindUNot:
			case k_NodeKindUXor:
				symExp(n->val.unary.operand,cur);
				break;
			
			case k_NodeKindIntLiteral:
			case k_NodeKindRuneLiteral:
			case k_NodeKindFloatLiteral:
			case k_NodeKindStringLiteral:
				break;
			
			case k_NodeKindExpressionPrimary:
				symExp(n->val.primary_expression.primary_expression,cur);
				symExp(n->val.primary_expression.selector,cur);
				symExp(n->val.primary_expression.index,cur);
				symExp(n->val.primary_expression.expression,cur);
			case k_NodeKindSelector:
				break;
			case k_NodeKindIndex:
				symExp(n->val.index.expression,cur);
				break;
			case k_NodeKindAppend:
			case k_NodeKindLen:
			case k_NodeKindCap:
				symExp(n->val.builtins.expression1,cur);
				symExp(n->val.builtins.expression2,cur);
				break;
			case k_NodeKindFuncCall:
				symExp(n->val.func_call.identifier,cur);
				symExp(n->val.func_call.expressions_opt,cur);
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

