#ifndef TREE_H
#define TREE_H

#include <stdbool.h>

typedef enum{ st_INT, st_FLOAT, st_STRING, st_BOOL, st_UNDEFINED} SymbolType;
typedef union typeData{
    char* stringLiteral;
    int intLiteral;
    float floatLiteral;
    bool boolLiteral;
} typeData;
typedef struct Node Node;

typedef struct SYMBOL {
    char *name;
    SymbolType type;
    Node *val;
    typeData evalValue;
    struct SYMBOL *next;
} SYMBOL;

typedef enum {
		//toplevel
		k_NodeKindProgram,
		k_NodeKindPackage,
		k_NodeKindTopDecs,
		//identifiers
		k_NodeKindIdentifiers,
		k_NodeKindIdentifier,
		//var dec
		k_NodeKindVarDec,
		k_NodeKindVarDefs,
		k_NodeKindVarDef,
		//type dec
		k_NodeKindTypeDec,
		k_NodeKindTypeDefs,
		k_NodeKindTypeDef,
		//func dec
		k_NodeKindFuncDec,
		k_NodeKindFuncParams,
		k_NodeKindFuncType,
		//id type
		k_NodeKindArrayType,
		k_NodeKindSliceType,
		k_NodeKindStructType,
		k_NodeKindIdType,
		k_NodeKindParType,
		k_NodeKindStructBody,
		//statements
		k_NodeKindStatements,
		k_NodeKindStatement,
		k_NodeKindBlockDec,
		k_NodeKindBlockBody,
		k_NodeKindSimpleStatementDec,
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
		
		k_NodeKindPrintStatement,
		k_NodeKindPrintlnStatement,
		
		k_NodeKindReturn,
		
		k_NodeKindIfStmt,
		k_NodeKindElseStmt,
		k_NodeKindForDec,
		k_NodeKindForCondition,
		k_NodeKindSwitchDec,
		k_NodeKindSwitchDef,
		k_NodeKindSwitchCases,
		k_NodeKindDefault,
		
		k_NodeKindBreak,
		k_NodeKindContinue,
		
		//expressions
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
		k_NodeKindExpressionBinaryLeftShit,
		k_NodeKindExpressionBinaryRightShift,
		k_NodeKindExpressionBinaryisEqual,
		k_NodeKindExpressionBinaryisNotEqual,
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
} NodeKind;

struct Node {
	int lineno;
	NodeKind kind;
	SYMBOL *symbol;		// optional
	SymbolType type;	// optional
    	typeData evalValue;
	union {
		char *identifier;
		char* stringLiteral;
		int intLiteral;
		double floatLiteral;
		//bool boolLiteral;
		char* runeLiteral;

		Node* node;
		struct { Node *package_dec; Node *top_decs; } program;
		struct { Node *identifier;} package_dec;
		struct { Node *top_decs; Node *dec;} top_decs;
		struct { Node *identifiers; Node *identifier;} identifiers;
		struct { Node *def;} var_dec;
		struct { Node *var_defs; Node *var_def;} var_defs;
		struct { Node *identifiers; Node *identifier_type; Node *expressions;} var_def;
		struct { Node *def;}type_dec;
		struct { Node *type_defs; Node *type_def;}type_defs;
		struct { Node *identifier; Node *identifier_type;}type_def;
		struct { Node *identifier; Node *func_params; Node *func_type; Node *block_body;}func_dec;
		struct { Node *func_params; Node *identifiers; Node *identifier_type;}func_params;
		struct { Node *identifier_type;}func_type;
		struct { int size; Node *identifier_type; Node *struct_body; Node *identifier;}identifier_type;
		struct { Node *struct_body; Node *identifiers; Node *identifier_type;}struct_body;
		struct { Node *statements; Node *statement;}statements;
		struct { Node *stmt;} statement;
		struct { Node *block_body;}block_dec;
		struct { Node *statements;}block_body;
		struct { Node *statement;}simple_statement_dec;
		struct { Node *lhs; Node *rhs;}simple_statement;
		struct { Node *expression_opt;}print_dec;
		struct { Node *expression_opt;}return_dec;
		struct { Node *simple_statement_dec; Node *expression; Node *block_body; Node *else_stmt;}if_stmt;
		struct { Node *if_stmt; Node *block_body;}else_stmt;
		struct { Node *for_condition; Node *block_body;}for_dec;
		struct { Node *left; Node *expression; Node *right;}for_condition;
		struct { Node *switch_def; Node *switch_cases;}switch_dec;
		struct { Node *simple_statement; Node *expression_opt;}switch_def;
		struct { Node *cases; Node *expressions; Node *statements;}switch_cases;
		struct { Node *expressions; Node *expression;}expressions;
		struct { Node *expression;}expression;
		struct { Node *lhs; Node *rhs;}binary;
		struct { Node *operand;}unary;
		struct { Node *primary_expression; Node *selector; Node *index; Node *identifier;}primary_expression;
		struct { Node *identifier;}selector;
		struct { Node *expression;}index;
		struct { Node *expression1; Node *expression2;}builtins;
		struct { Node *identifier; Node *expressions_opt;}func_call;
		
		
	} val;
};

// Function declerations

Node* newNode(NodeKind k, int lineno);
Node *newIdentifiers(Node* ids, Node* id, int lineno);
Node *newIdentifier(char* id, int lineno);
Node *newProgram(Node* package_dec, Node* top_decs, int lineno);
Node *newPackage_dec(Node* identifier, int lineno);
Node *newTop_decs(Node* top_decs, Node* dec, int lineno);
Node *newVarDec(Node* def, int lineno);
Node *newVarDefs(Node* defs, Node* def, int lineno);
Node *newVarDef(Node* identifiers, Node* identifier_type, Node* expressions, int lineno);
Node *newTypeDec(Node* def, int lineno);
Node *newTypeDefs(Node* type_defs, Node* type_def, int lineno);
Node *newTypeDef(Node* identifier, Node* identifier_type, int lineno);
Node *newFuncDec(Node* id, Node* params, Node* id_type, Node* block, int lineno);
Node *newFuncParams(Node* func_params, Node* ids, Node* id_type, int lineno);
Node *newFuncType(Node* id_type, int lineno);
Node *newIdentifierType(int size, Node *identifier_type, Node* struct_body, Node* identifier, NodeKind kind, int lineno);
Node *newStructBody(Node *struct_body, Node *ids, Node *identifier_type, int lineno);
Node *newStatements(Node *statements, Node *statement, int lineno);
Node *newStatement(Node *stmt, int lineno);
Node *newBlockDec(Node *block_body, int lineno);
Node *newBlockBody(Node *statements, int lineno);
Node *newSimpleStatementDec(Node *statement, int lineno);
Node *newSimpleStatement(Node *lhs, Node *rhs, NodeKind kind, int lineno);
Node *newPrintStatement(Node *expression_opt, NodeKind kind, int lineno);
Node *newReturnStatement(Node *expression_opt, int lineno);
Node *newIfStmt(Node* simple_statement_dec, Node *expression, Node* block_body, Node* else_stmt, int lineno);
Node *newElseStmt(Node *if_stmt, Node* block_body, int lineno);
Node *newForDec(Node *for_condition, Node* block_body, int lineno);
Node *newForCondition(Node *left, Node *expression, Node *right, int lineno);
Node *newSwitchDec(Node *switch_def, Node *switch_cases, int lineno);
Node *newSwitchDef(Node *simple_statement, Node *expression_opt, int lineno);
Node *newSwitchCases(Node *cases, Node *expressions, Node *statements, NodeKind kind, int lineno);
Node *newBreak(int lineno);
Node *newContinue(int lineno);



// expression
Node *newExpressions(Node *expressions, Node *expression, int lineno);
Node *newExpressionOpt(Node *expression, int lineno);
Node *newExpressionsOpt(Node *expressions, int lineno);
Node *newExpression(Node *expression, int lineno);
Node *newExpressionBinary(Node *lhs, Node *rhs, NodeKind kind, int lineno);
Node *newExpressionUnary(Node *operand, NodeKind kind, int lineno);
Node *newIntLiteral(int literal, int lineno);
Node *newRuneLiteral(char* literal, int lineno);
Node *newFloatLiteral(double literal, int lineno);
Node *newStringLiteral(char* literal, int lineno);
Node *newExpressionPrimary(Node *primary_expression, Node *selector, Node *index, Node *identifier, int lineno);
Node *newSelector(Node *identifier, int lineno);
Node *newIndex(Node *expression, int lineno);
Node *newBuiltin(Node *expression1, Node *expression2, NodeKind kind, int lineno);
Node *newFuncCall(Node *identifier, Node *expressions_opt, int lineno);

#endif /* !TREE_H */
