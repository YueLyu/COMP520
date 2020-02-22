#include <stdlib.h>
// #include <stdbool.h>
#include "tree.h"

extern int yylineno;

Node *newNode(NodeKind k, int lineno){		// Create new node with specified kind.
    Node *n = malloc(sizeof(Node));
    n->lineno = lineno;
    n->kind = k;
    return n;
}

Node *newProg(Node* line, Node* program, int lineno){	// Create new program node
    Node *n = newNode(k_NodeKindProg, lineno);
    n->val.prog.line = line;
    n->val.prog.program = program;
    return n;
}

Node* newType(NodeKind type, int lineno){	// Create variable type node
    Node *n = newNode(type, lineno);
    return n;
}

Node* newLine(Node* node, int lineno){	// Create variable type node
    Node *n = newNode(k_NodeKindLine, lineno);
    n->val.node = node;
    return n;
}

Node* newDeclaration(Node* ident, Node* exp, int lineno){	// Create new decleration with type
    Node *n = newNode(k_NodeKindDeclaration, lineno);
    n->val.declaration.ident = ident;
	n->val.declaration.varType = NULL;
    n->val.declaration.exp = exp;
	return n;
}

Node* newDeclarationType(Node* ident, Node* varType, Node* exp, int lineno){	// Create new decleration
    Node *n = newNode(k_NodeKindDeclarationType, lineno);
    n->val.declaration.ident = ident;
    n->val.declaration.varType = varType;
    n->val.declaration.exp = exp;
	return n;
}

Node *newStatementRead(Node* ident, int lineno){	// Create new read statement node
    Node *n = newNode(k_NodeKindStatementRead, lineno);
    n->val.node = ident;
    return n;
}

Node *newStatementPrint(Node* exp, int lineno){		// Create new print statement node
    Node *n = newNode(k_NodeKindStatementPrint, lineno);
    n->val.node = exp;
    return n;
}

Node *newStatementAssign(Node* ident, Node* exp, int lineno){	// Create new assignment node
    Node *n = newNode(k_NodeKindStatementAssign, lineno);
    n->val.assignStatement.ident = ident;
    n->val.assignStatement.exp = exp;
    return n;
}

Node* newStatementIfElse(Node* expression, Node* program, Node* elseIf, Node* program2, int lineno) {	// Create new ifElse node
    Node *n = newNode(k_NodeKindStatementIfElse, lineno);
    n->val.ifElseStatement.expression = expression;
    n->val.ifElseStatement.program = program;
    n->val.ifElseStatement.elseIf = elseIf;
    n->val.ifElseStatement.program2 = program2;
    return n;
}

Node* newStatementIfElseIf(Node* expression, Node* program, Node* elseIf, int lineno) {	// Create new ifElseIf node
    Node *n = newNode(k_NodeKindStatementIfElseIf, lineno);
    n->val.ifElseIfStatement.expression = expression;
    n->val.ifElseIfStatement.program = program;
    n->val.ifElseIfStatement.elseIf = elseIf;
    return n;
}

Node* newStatementElseIf(Node* elseIf, Node* expression, Node* program, int lineno) {	// Create new elseIf node
    Node *n = newNode(k_NodeKindStatementElseIf, lineno);
    n->val.elseIfStatement.elseIf = elseIf;
    n->val.elseIfStatement.expression = expression;
    n->val.elseIfStatement.program = program;
    return n;
}

Node *newStatementWhile(Node* exp, Node* program, int lineno){	// Create while statement node
    Node *n = newNode(k_NodeKindStatementWhile, lineno);
    n->val.whileStatement.exp = exp;
    n->val.whileStatement.program = program;
    return n;
}


// expression
Node* expressionIdentifier(char* val, int lineno){	// Create identifier node
    Node *n = newNode(k_NodeKindExpIdentifier, lineno);
    n->val.identifier = val;
    return n;
}

Node* expressionIntLiteral(int val, int lineno){	// Create int node
    Node *n = newNode(k_NodeKindExpIntLiteral, lineno);
    n->val.intLiteral = val;
    return n;
}
Node* expressionFloatLiteral(float val, int lineno){	// Create float node
    Node *n = newNode(k_NodeKindExpFloatLiteral, lineno);
    n->val.floatLiteral = val;
    return n;
}
Node* expressionStringLiteral(char* val, int lineno){	// Create string node
    Node *n = newNode(k_NodeKindExpStringLiteral, lineno);
    n->val.stringLiteral = val;
    return n;
}
Node* expressionBoolLiteral(bool val, int lineno){	// Create bool node
    Node *n = newNode(k_NodeKindExpBoolLiteral, lineno);
    n->val.boolLiteral = val;
    return n;
}
Node *expressionBinary(NodeKind op, Node *lhs, Node *rhs, int lineno){	// Create binary expression node
    Node *n = newNode(op, lineno);
    n->val.binary.lhs = lhs;
    n->val.binary.rhs = rhs;
    return n;
}
Node *expressionUnary(NodeKind op, Node *node, int lineno){	// Create unary expression node
    Node *n = newNode(op, lineno);
    n->val.node = node;
    return n;
}
