#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "symbol.h"
#include "tree.h"
#include "type.h"



SymbolTable *t; 
SymbolTable *cur = t;
TYPE * resolveType(SymbolTable * t, SYMBOL * sym) {
	if (sym == NULL) {
		fprintf(stderr, "Error: Trying to resolve type of NULL.");
		exit(1);
	}
	switch (sym->kind) {
	case symkind_var:
		fprintf(stderr, "Error: Cannot resolve type for a variable.");
		exit(1);
		break;
	case symkind_func:
		fprintf(stderr, "Error: Cannot resolve type for a function.");
		exit(1);
		break;
	case symkind_const:
		fprintf(stderr, "Error: Cannot resolve type for a constant.");
		exit(1);
		break;
	case symkind_type:
		return resolveType(t, sym->typelit.defined.underlying); break;
	case symkind_struct:
		return sym->typelit.structdec.structtype; break;
	case symkind_array:
		return sym->typelit.arraydec.arraytype; break;
	case symkind_slice:
		return sym->typelit.slicedec.slicetype; break;
	case symkind_base: return sym->typelit.type; break;
	}
	return NULL;
}

TYPE * checkValidType(SymbolTable * t, TYPE * type, int lineno){
	if(type) {
		SYMBOL * s;
		List * fields;
		switch (type->kind) {
		case unknownType:
			s = getSymbol(t, type->val.ident);
			if(!s || (s->kind != symkind_type && s->kind != symkind_base)) {
				fprintf(stderr, "Error: (line %d) %s is not a valid type\n", lineno, type->val.ident);
				exit(1);
			}
			else{
				free(type);
				if (s->kind == symkind_type) {
					if(!s->typelit.defined.type->rt)
						s->typelit.defined.type->rt = resolve_type_with_TYPE(t, s->typelit.defined.type);
					return s->typelit.defined.type;
				}
				else {
					s->typelit.type->rt = resolve_type_with_TYPE(t, s->typelit.type);
					return s->typelit.type;
				}
			}
			break;
		case nullType:    fprintf(stderr, "Error: (line %d) Nulltype, should not happen, something has gone wrong\n", lineno);
			exit(1);
		case structType:
			fields = type->val.fields;
			while (fields) {
				fields->current.param.type = checkValidType(t, fields->current.param.type, lineno);
				if(fields->current.param.type->rt == NULL)
					fields->current.param.type->rt = resolve_type_with_TYPE(t, fields->current.param.type);
				fields = fields->nextli;
			}
			type-> rt = type;
			addStruct(type);
			break;
		case arrayType:
			type->val.arrayData.entryType = checkValidType(t, type->val.arrayData.entryType, lineno);
			if (!type->val.arrayData.entryType->rt)
				type->val.arrayData.entryType->rt = resolve_type_with_TYPE(t, type->val.arrayData.entryType);
			checkValidType(t, type->val.arrayData.entryType->rt, lineno);
			type-> rt = type;
			break;
		case sliceType:
			type->val.sliceEntryType = checkValidType(t, type->val.sliceEntryType, lineno);
			type-> rt = type;
			break;
		case intType:
		case floatType:
		case runeType:
		case stringType:
		case boolType:
			type-> rt = type;
			break;
		}
	}
	return type;
}



bool compareType(Type* a, Type* b) {//A very good method
	if (a == NULL && b == NULL) {
		return true;
	}
	if ((a == NULL && b != NULL) || (a != NULL && b == NULL)) {
		return false;
	}
	if(a->kind!=b->kind){
		return false;
	}
	
	switch(a->kind){
		k_NodeKindArrayType:
			if(a->val.identifier_type.size!=b->val.identifier_type.size){
				return false;
			}else{
				return compareType(a->val.identifier_type.identifier_type,b->val.identifier_type.identifier_type);
			}
			
		k_NodeKindParType:
		k_NodeKindSliceType:
			return compareType(a->val.identifier_type.identifier_type,b->val.identifier_type.identifier_type);
			
		k_NodeKindIdType:
			if(strcmp(a->val.identifier,b->val.identifier)==0){
				return true;
			} else{
				return false;
			}
			
		k_NodeKindStructType:
			Type *aBody=a->val.identifier_type.identifier_type;
			Type *bBody=b->val.identifier_type.identifier_type;
			return compareType(aBody,bBody);
				
		k_NodeKindStructBody:
			Exp *aFieldNames=a->val.struct_body.identifiers;
			Exp *bFieldNames=b->val.struct_body.identifiers;
			Type *aFieldType=a->val.struct_body.type;
			Type *bFieldType=b->val.struct_body.type;
			if(!compareIdList(aFieldNames,bFieldNames)){
				return false;
			} else if(!compareType(aFieldType,bFieldType)){
				return false;
			} else{
				return compareType(a->val.struct_body.struct_body,b->val.struct_body.struct_body);
			}
		}

}

bool compareIdList(Exp *ids1,Exp *ids2){
	if(ids1==NULL && ids2!=NULL){
		return false;
	}else if(ids1!=NULL&&ids2==NULL){
		return false;
	}else if(ids1==NULL&&ids2==NULL){
		return true;
	}else if(strcmp(ids1->val.identifiers.identifier->val.identifier,ids2->val.identifiers.identifier->val.identifier)!=0){
		return false;
	}else{
		return compareIdList(ids1->val.identifiers.identifiers,ids2->val.identifiers.identifiers);
	}
	
}

void typecheck_prog(SymbolTable *t, Prog* root){
	typecheck_Decl(t, root->top_decs);
}

void typecheck_Decl(SymbolTable* t, Decl* n){
	if(n!=NULL){
		switch(n->kind){
			k_NodeKindPackageDec:
				break;
			k_NodeKindTopDecs:
				typecheck_Decl(SymbolTable* t, n->val.top_decs.top_decs);
				typecheck_Decl(SymbolTable* t, n->val.top_decs.top_dec);
				break;
			k_NodeKindVarDecLine:
			k_NodeKindVarDecPar:
				typecheck_Decl(SymbolTable* t, n->val.var_dec.def);
			k_NodeKindVarDefs:
				typecheck_Decl(SymbolTable* t, n->val.var_defs.var_defs);
				typecheck_Decl(SymbolTable* t, n->val.var_defs.var_def);
				break;
			k_NodeKindVarDef:
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
					for (int i = 0; i < len - 1; i ++) {
						if (!compareType(arr[i], arr[i + 1])) {
							fprintf(stderr, "Error: (line %d) The types of expressions in the variable declaration doesn't match.\n", n->lineno);
							exit(1);
						}
					}
					Exp* temp_id = n->val.var_def.identifiers;
					while (temp_id != NULL) {
						temp_id->val.identifiers.identifier->sym->typelit.type = arr[0];
						temp_id = temp_id->val.identifiers.identifiers;
					}
				} else if (n->val.var_def.identifier_type != NULL && n->val.var_def.expressions != NULL) { // var x, y, z int = 1, 2, true
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
						temp_id->val.identifiers.identifier->sym->typelit.type = arr[0];
						temp_id = temp_id->val.identifiers.identifiers;
					}
				}
				
				
				break;
			k_NodeKindTypeDecPar:
			k_NodeKindTypeDecLine:
			k_NodeKindTypeDefs:
			k_NodeKindTypeDef:
				break;
			k_NodeKindFuncDec:
				typecheck_Stmt(SymbolTable* t, n->val.func_dec.block_body);
				break;
			k_NodeKindFuncParams:
			k_NodeKindFuncType:
				break;
				
		}
	}
}

void checkVarDef(Exp* id, Exp* exp){
	
}

void typecheck_Stmt(SymbolTable* t, Stmt* n){
	if(n!=NULL){
		switch(n.kind){
			k_NodeKindStatements:
				typecheck_Stmt(SymbolTable* t, n->val.statements.statements);
				typecheck_Stmt(SymbolTable* t, n->val.statements.statement);
				break;
			k_NodeKindStatement:
				typecheck_Stmt(SymbolTable* t, n->val.statements.statement);
				break;
			k_NodeKindStatementDec:
				typecheck_Decl(SymbolTable* t, n->val.statement_dec.decl);
				break;
			k_NodeKindBlockBody:
				typecheck_Stmt(SymbolTable* t, n->val.block_body.statements);
				break;
			k_NodeKindSimpleStatementDec:
				typecheck_Stmt(SymbolTable* t, n->val.simple_statement_dec.statement);
				break;
			k_NodeKindSimpleStatementExp,
			k_NodeKindSimpleStatementInc,
			k_NodeKindSimpleStatementDecrease,
			k_NodeKindSimpleStatementEqual,
			k_NodeKindSimpleStatementPlusEqual,
			k_NodeKindSimpleStatementMinEqual,
			k_NodeKindSimpleStatementMulEqual,
			k_NodeKindSimpleStatementDivEqual,
			k_NodeKindSimpleStatementModEqual,
			k_NodeKindSimpleStatementBitAndEqual,
			k_NodeKindSimpleStatementBitOrEqual,
			k_NodeKindSimpleStatementBitXorEqual,
			k_NodeKindSimpleStatementLeftShiftEqual,
			k_NodeKindSimpleStatementRightShiftEqual,
			k_NodeKindSimpleStatementBitClearEqual,
			k_NodeKindSimpleStatementDeclEqual,
				Type *varType=typecheck_Exp(SymbolTable* t, n->val.simple_statement.lhs);
				Type *expType=typecheck_Exp(SymbolTable* t, n->val.simple_statement.rhs);
				if(varType==NULL||expType==NULL||strcmp(getType(varType),getType(expType))!=0){fprintf(stderr, "Declaration Error: variable and expressions not the same type\n";}//check assignability
				
				break;
			k_NodeKindPrintDec:
			k_NodeKindPrintlnDec:
				typecheck_Exp(SymbolTable* t, n->val.print_dec.expression_opt);
				break;
			k_NodeKindReturn:
				typecheck_Exp(SymbolTable* t, n->val.return_dec.expression_opt);
				break;
			k_NodeKindIfStmt:
				typecheck_Stmt(SymbolTable* t, n->val.if_stmt.simple_statement_dec);
				Type *ifCondition=typecheck_Exp(SymbolTable* t, n->val.if_stmt.expression);
				if(strcmp(getType(ifCondition),"bool")!=0){fprintf(stderr,"If condition not bool");}
				typecheck_Stmt(SymbolTable* t, n->val.if_stmt.block_body);
				typecheck_Stmt(SymbolTable* t, n->val.if_stmt.else_stmt);
				break;
			k_NodeKindElseStmt:
				typecheck_Stmt(SymbolTable* t, n->val.else_stmt.if_stmt);
				typecheck_Stmt(SymbolTable* t, n->val.else_stmt.block_body);
				break;
			k_NodeKindForDec:
				typecheck_Stmt(SymbolTable* t, n->val.for_dec.for_condition);
				typecheck_Stmt(SymbolTable* t, n->val.for_dec.block_body);
				break;
			k_NodeKindForCondition:
				typecheck_Stmt(SymbolTable* t, n->val.for_condition.left);
				Type *forCondition=typecheck_Exp(SymbolTable* t, n->val.for_condition.expression);
				if(strcmp(getType(forCondition),"bool")!=0){fprintf(stderr,"For condition not bool");}
				typecheck_Stmt(SymbolTable* t, n->val.for_condition.right);
				break;
			k_NodeKindSwitchDec:
				typecheck_Stmt(SymbolTable* t, n->val.switch_dec.switch_def);
				typecheck_Stmt(SymbolTable* t, n->val.switch_dec.switch_cases);
				break;
			k_NodeKindSwitchDef:
				typecheck_Stmt(SymbolTable* t, n->val.switch_def.simple_statement);
				Type *switchExpType=typecheck_Exp(SymbolTable* t, n->val.switch_def.expression_opt);
				break;
			k_NodeKindSwitchCases:
			k_NodeKindDefault:
				typecheck_Stmt(SymbolTable* t, n->val.switch_cases.cases);
				Type *caseExpType=typecheck_Exp(SymbolTable* t, n->val.switch_cases.expressions);
				if(strcmp(getType(switchExpType),getType(caseExpType))!=0){fprintf(stderr,"case expr doesn't match switch expr");}
				typecheck_Stmt(SymbolTable* t, n->val.switch_cases.statements);
				break;
			
			k_NodeKindBreak:
			k_NodeKindContinue:
				break;
		}
	}
}

Type *inferType_Exp(SymbolTable* t, Exp* n){
	if(n!=NULL){
		switch(n->kind){
			k_NodeKindIdentifiers:
				typecheck_Exp(SymbolTable* t, n->val.identifiers.identifiers);
				typecheck_Exp(SymbolTable* t, n->val.identifiers.identifier);
				break;
			k_NodeKindIdentifier,
				typecheck_Exp()
				break;
			k_NodeKindExpressions,
			k_NodeKindExpressionOpt,
			k_NodeKindExpressionsOpt,
			k_NodeKindExpression,
			
			k_NodeKindExpressionBinaryPlus,
			k_NodeKindExpressionBinaryMinus,
			k_NodeKindExpressionBinaryMultiply,
			k_NodeKindExpressionBinaryDivide,
			k_NodeKindExpressionBinaryModulo,
			k_NodeKindExpressionBinaryBitAnd,
			k_NodeKindExpressionBinaryBitOr,
			k_NodeKindExpressionBinaryBitXor,
			k_NodeKindExpressionBinarybitClear,
			k_NodeKindExpressionBinaryLeftShift,
			k_NodeKindExpressionBinaryRightShift,
			k_NodeKindExpressionBinaryIsEqual,
			k_NodeKindExpressionBinaryIsNotEqual,
			k_NodeKindExpressionBinaryLessThan,
			k_NodeKindExpressionBinaryGreaterThan,
			k_NodeKindExpressionBinaryLessThanEqual,
			k_NodeKindExpressionBinaryGreaterThanEqual,
			k_NodeKindExpressionBinaryAnd,
			k_NodeKindExpressionBinaryOr,
			
			k_NodeKindUMinus,
			k_NodeKindUPlus,
			k_NodeKindUNot,
			k_NodeKindUXor,
			
			k_NodeKindIntLiteral,
			k_NodeKindRuneLiteral,
			k_NodeKindFloatLiteral,
			k_NodeKindStringLiteral,
			
			k_NodeKindExpressionPrimary,
			k_NodeKindSelector,
			k_NodeKindIndex,
			
			k_NodeKindAppend,
			k_NodeKindLen,
			k_NodeKindCap,
			k_NodeKindFuncCall,
		}
	}
}





