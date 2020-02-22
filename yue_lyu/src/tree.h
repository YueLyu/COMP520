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
	k_NodeKindProg,
	k_NodeKindLine,
	k_NodeKindDeclaration,
	k_NodeKindDeclarationType,

	// expression kind:
	k_NodeKindExpIdentifier,
	k_NodeKindExpIntLiteral,
	k_NodeKindExpFloatLiteral,
	k_NodeKindExpStringLiteral,
	k_NodeKindExpBoolLiteral,
	k_NodeKindExpAddition,
	k_NodeKindExpSubtraction,
	k_NodeKindExpMultiplication,
	k_NodeKindExpDivision,
	k_NodeKindExpEqual,
	k_NodeKindExpNotEqual,
	k_NodeKindExpAnd,
	k_NodeKindExpOr,
	k_NodeKindExpUMinus,
	k_NodeKindExpNeg,
	k_NodeKindExpGreater,
	k_NodeKindExpGreaterEqual,
	k_NodeKindExpSmaller,
	k_NodeKindExpSmallerEqual,

	k_NodeKindTypeInt,
	k_NodeKindTypeBool,
	k_NodeKindTypeFloat,
	k_NodeKindTypeString,
	
	k_NodeKindStatementRead,
	k_NodeKindStatementPrint,
	k_NodeKindStatementAssign,
	k_NodeKindStatementElseIf,
	k_NodeKindStatementIfElseIf,
	k_NodeKindStatementIfElse,
	k_NodeKindStatementWhile,
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
		bool boolLiteral;

		Node* node;
		struct { Node
 *line; Node *program; } prog;
		struct { Node *lhs; Node *rhs; } binary;
		struct { Node *ident; Node *varType; Node *exp;} declaration;
		struct { Node *ident; Node *exp;} assignStatement;
		struct { Node * expression; Node *program; Node *elseIf; Node* program2;} ifElseStatement;
		struct { Node * expression; Node *program; Node *elseIf;} ifElseIfStatement;
		struct { Node *elseIf; Node *expression; Node *program;} elseIfStatement;
		struct { Node *exp; Node *program;} whileStatement;
	} val;
};

// Function declerations

Node* newNode(NodeKind k, int lineno);
Node* newProg(Node* line, Node* program, int lineno);
Node* newType(NodeKind type, int lineno);
Node* newLine(Node* node, int lineno);
Node* newDeclaration(Node* ident, Node* exp, int lineno);
Node* newDeclarationType(Node* ident, Node* varType, Node* exp, int lineno);
Node* newStatementRead(Node* ident, int lineno);
Node* newStatementPrint(Node* exp, int lineno);
Node* newStatementAssign(Node* ident, Node* exp, int lineno);
Node* newStatementIfElse(Node* expression, Node* program, Node* elseIf, Node* program2, int lineno) ;
Node* newStatementIfElseIf(Node* expression, Node* program, Node* elseIf, int lineno) ;
Node* newStatementElseIf(Node* elseIf, Node* expression, Node* program, int lineno);
Node *newStatementWhile(Node* exp, Node* statements, int lineno);

// expression
Node* expressionIdentifier(char* val, int lineno);
Node* expressionIntLiteral(int val, int lineno);
Node* expressionFloatLiteral(float val, int lineno);
Node* expressionStringLiteral(char* val, int lineno);
Node* expressionBoolLiteral(bool val, int lineno);
Node *expressionBinary(NodeKind op, Node *lhs, Node *rhs, int lineno);
Node *expressionUnary(NodeKind op, Node *node, int lineno);

#endif /* !TREE_H */
