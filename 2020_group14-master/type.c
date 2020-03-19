#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "symbol.h"
#include "tree.h"
#include "type.h"

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

Type *inferType_Exp(SymbolTable* t, Exp* n){//TODO:
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
	return ;
}






