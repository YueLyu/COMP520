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
				break;
				
			k_NodeKindIdentifier:// 		TODO: doesn't support _ (blank identifier)
				SYMBOL* symbol = getSymbol(t, n->val.identifier);
				Type* inferred_type = symbol->typelit.type;
				n->type = inferred_type;
				return inferred_type;
				
			k_NodeKindExpressions:
			k_NodeKindExpressionsOpt:
				
			k_NodeKindExpressionOpt:
			k_NodeKindExpression:
				return inferType_Exp(t, n->val.expression.expression);
				
			/* --------------------------- Binary expressions: ---------------------------*/
			k_NodeKindExpressionBinaryPlus:		// +
				Type* lhs = inferType_Exp(t, n->val.binary.lhs);
				Type* rhs = inferType_Exp(t, n->val.binary.rhs);
				if (isNumeric(lhs) && isNumeric(rhs)) {
					if (lhs->inferType == Float64Type || rhs->inferType == Float64Type) {
						Type* inferred_type = newIdType("float64", n->lineno);
						inferred_type->inferType = Float64Type;
						n->type = inferred_type;
						return inferred_type;
					} else {
						Type* inferred_type = newIdType("int", n->lineno);
						inferred_type->inferType = IntType;
						n->type = inferred_type;
						return inferred_type;
					}
				} else if (lhs->inferType == StringType && rhs->inferType == StringType) {
					Type* inferred_type = newIdType("string", n->lineno);
					inferred_type->inferType = StringType;
					n->type = inferred_type;
					return inferred_type;
				} else {
					fprintf(stderr, "Error: (line %d) The operands in binary expression don't type check.", n->lineno);
					exit(1);
				}
					
			k_NodeKindExpressionBinaryMinus:	// -
			k_NodeKindExpressionBinaryMultiply:		// *
			k_NodeKindExpressionBinaryDivide:	// /		
				Type* lhs = inferType_Exp(t, n->val.binary.lhs);
				Type* rhs = inferType_Exp(t, n->val.binary.rhs);
				if (isNumeric(lhs) && isNumeric(rhs)) {
					if (lhs->inferType == Float64Type || rhs->inferType == Float64Type) {
						Type* inferred_type = newIdType("float64", n->lineno);
						inferred_type->inferType = Float64Type;
						n->type = inferred_type;
						return inferred_type;
					} else {
						Type* inferred_type = newIdType("int", n->lineno);
						inferred_type->inferType = IntType;
						n->type = inferred_type;
						return inferred_type;
					}
				} else {
					fprintf(stderr, "Error: (line %d) The operands in binary expression don't type check.", n->lineno);
					exit(1);
				}
					
			k_NodeKindExpressionBinaryModulo:	// %
			k_NodeKindExpressionBinaryBitAnd:	// &
			k_NodeKindExpressionBinaryBitOr:	// |
			k_NodeKindExpressionBinaryBitXor:	// ^
			k_NodeKindExpressionBinarybitClear:		  // &^
			k_NodeKindExpressionBinaryLeftShift:	//  <<
			k_NodeKindExpressionBinaryRightShift:	// >>
				Type* lhs = inferType_Exp(t, n->val.binary.lhs);
				Type* rhs = inferType_Exp(t, n->val.binary.rhs);
				if (isInteger(lhs) && isInteger(rhs)) {
					Type* inferred_type = newIdType("int", n->lineno);
					inferred_type->inferType = IntType;
					n->type = inferred_type;
					return inferred_type;
				} else {
					fprintf(stderr, "Error: (line %d) The operands in binary expression don't type check.", n->lineno);
					exit(1);
				}
				
			k_NodeKindExpressionBinaryIsEqual:	// ==
			k_NodeKindExpressionBinaryIsNotEqual:	// !=		
				Type* lhs = inferType_Exp(t, n->val.binary.lhs);
				Type* rhs = inferType_Exp(t, n->val.binary.rhs);
				if (isComparable(lhs) && isComparable(rhs)) {
					Type* inferred_type = newIdType("bool", n->lineno);
					inferred_type->inferType = BoolType;
					n->type = inferred_type;
					return inferred_type;
				} else {
					fprintf(stderr, "Error: (line %d) The operands in binary expression don't type check.", n->lineno);
					exit(1);
				}
				
			k_NodeKindExpressionBinaryLessThan:		// <
			k_NodeKindExpressionBinaryGreaterThan:		// >
			k_NodeKindExpressionBinaryLessThanEqual:	// <=
			k_NodeKindExpressionBinaryGreaterThanEqual:		// >=	
				Type* lhs = inferType_Exp(t, n->val.binary.lhs);
				Type* rhs = inferType_Exp(t, n->val.binary.rhs);
				if (isOrdered(lhs) && isOrdered(rhs)) {
					Type* inferred_type = newIdType("bool", n->lineno);
					inferred_type->inferType = BoolType;
					n->type = inferred_type;
					return inferred_type;
				} else {
					fprintf(stderr, "Error: (line %d) The operands in binary expression don't type check.", n->lineno);
					exit(1);
				}
				
			k_NodeKindExpressionBinaryAnd:		// ||		bool || bool = bool
			k_NodeKindExpressionBinaryOr:		// &&		bool && bool = bool
				Type* lhs = inferType_Exp(t, n->val.binary.lhs);
				Type* rhs = inferType_Exp(t, n->val.binary.rhs);
				if (lhs->inferType == BoolType && rhs->inferType == BoolType) {
					n->type = lhs;
					return lhs;
				} else {
					fprintf(stderr, "Error: (line %d) The operands in binary expression don't type check.", n->lineno);
					exit(1);
				}
			
			/* --------------------------- Unary expressions: ---------------------------*/
			k_NodeKindUMinus:
			k_NodeKindUPlus:	// -, + : numeric type
				Type* operand_type = inferType_Exp(t, n->val.unary.operand);
				if(operand_type->inferType == IntType
					|| operand_type->inferType == Float64Type
				 	|| operand_type->inferType == RuneType) {
					n->type = operand_type;
					return operand_type;
				} else {
					fprintf(stderr, "Error: (line %d) The operand in unary expression doesn't type check.", n->lineno);
					exit(1);
				}
			k_NodeKindUNot:		// ! : bool type
				Type* operand_type = inferType_Exp(t, n->val.unary.operand);
				if(operand_type->inferType == BoolType) {
					n->type = operand_type;
					return operand_type;
				} else {
					fprintf(stderr, "Error: (line %d) The operand in unary expression doesn't type check.", n->lineno);
					exit(1);
				}
			k_NodeKindUXor:		// ^ : int, rune
				Type* operand_type = inferType_Exp(t, n->val.unary.operand);
				if(operand_type->inferType == IntType
					|| operand_type->inferType == RuneType) {
					n->type = operand_type;
					return operand_type;
				} else {
					fprintf(stderr, "Error: (line %d) The operand in unary expression doesn't type check.", n->lineno);
					exit(1);
				}
			
			/* --------------------------- Literals: ---------------------------*/
			k_NodeKindIntLiteral:
				Type* inferred_type = newIdType("int", n->lineno);
				inferred_type->inferType = IntType;
				n->type = inferred_type
				return inferred_type;
			k_NodeKindRuneLiteral:
				Type* inferred_type = newIdType("rune", n->lineno);
				inferred_type->inferType = RuneType;
				n->type = inferred_type
				return inferred_type;
			k_NodeKindFloatLiteral:
				Type* inferred_type = newIdType("float64", n->lineno);
				inferred_type->inferType = Float64Type;
				n->type = inferred_type
				return inferred_type;
			k_NodeKindStringLiteral:
				Type* inferred_type = newIdType("string", n->lineno);
				inferred_type->inferType = StringType;
				n->type = inferred_type
				return inferred_type;
			k_NodeKindExpressionPrimary:
				
			k_NodeKindSelector:
			k_NodeKindIndex:
				
			k_NodeKindAppend://append []T T = []T
				return 
			k_NodeKindLen://len []T/[N]T = int
				return 
			k_NodeKindCap://cap []T/[N]T = int
				return 
			k_NodeKindFuncCall://check func_type
				return 
		}
	}
	return NULL;
}

// Find the underlying type 
resolveType(SymbolTable* t, Type* type) {
	
} 


bool isNumeric(Type* type) {
	return type->inferType == IntType || type->inferType == Float64Type
		|| type->inferType == RuneType;
}

bool isInteger(Type* type) {
	return type->inferType == IntType || type->inferType == RuneType;
}
/*	TODO: not sure what is pointer???
	comparable type: bool, int, float64, string, pointer, 
					 struct(if all their fields are comparable), 
					 array(if values of array element type are comparable)
*/
bool isComparable(Type* type) {
	if (type == NULL) {
		return true;
	}
	if (type->inferType == BoolType || type->inferType == IntType 
		|| type->inferType == Float64Type
		|| type->inferType == StringType) {
		return true;
	} else if (type->inferType == StructType) {
		return isComparable(type->val.identifier_type.identifier_type);
	} else if (type->inferType == ArrayType) {
		return isComparable(type->val.identifier_type.identifier_type);
	} else if (type->inferType == UnknownType && type->kind == k_NodeKindStructBody) {
		return isComparable(type->val.struct_body.type) 
			&& isComparable(type->val.struct_body.struct_body);
	} else {
		return false;
	}
}
/*  ordered type: int, float64, string  */
bool isOrdered(Type* type) {
	return type->inferType == IntType || type->inferType == Float64Type
		|| type->inferType == StringType;
}


