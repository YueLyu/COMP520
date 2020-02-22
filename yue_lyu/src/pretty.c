#include <stdio.h>
#include "pretty.h"

void prettyNode(Node *e)
{
    	if (e == NULL) return;

	switch (e->kind) {

		case k_NodeKindProg:
		    prettyNode(e->val.prog.line);
		    prettyNode(e->val.prog.program);
		    break;

		case k_NodeKindLine:
			prettyNode(e->val.node);
			break;

		case k_NodeKindExpGreater:
			printf("(");
			prettyNode(e->val.binary.lhs);
			printf(">");
			prettyNode(e->val.binary.rhs);
			printf(")");
			break;

		case k_NodeKindExpGreaterEqual:
			printf("(");
			prettyNode(e->val.binary.lhs);
			printf(">=");
			prettyNode(e->val.binary.rhs);
			printf(")");
			break;
		
		case k_NodeKindExpSmaller:
			printf("(");
			prettyNode(e->val.binary.lhs);
			printf("<");
			prettyNode(e->val.binary.rhs);
			printf(")");
			break;

		case k_NodeKindExpSmallerEqual:
			printf("(");
			prettyNode(e->val.binary.lhs);
			printf("<=");
			prettyNode(e->val.binary.rhs);
			printf(")");
			break;

		case k_NodeKindExpIdentifier:
				printf("%s", e->val.identifier);
				break;

		case k_NodeKindExpIntLiteral:
				printf("%i", e->val.intLiteral);
				break;

		case k_NodeKindExpFloatLiteral:
		    printf("%f", e->val.floatLiteral);
		    break;

		case k_NodeKindExpStringLiteral:
		    printf("%s", e->val.stringLiteral);
		    break;

		case k_NodeKindExpBoolLiteral:
		    printf(e->val.boolLiteral ? "True" : "False");
		    break;

		case k_NodeKindExpAddition:
			printf("(");
			prettyNode(e->val.binary.lhs);
			printf("+");
			prettyNode(e->val.binary.rhs);
			printf(")");
			break;

		case k_NodeKindExpSubtraction:
			printf("(");
			prettyNode(e->val.binary.lhs);
			printf("-");
			prettyNode(e->val.binary.rhs);
			printf(")");
			break;

		case k_NodeKindExpMultiplication:
			printf("(");
			prettyNode(e->val.binary.lhs);
			printf("*");
			prettyNode(e->val.binary.rhs);
			printf(")");
			break;

		case k_NodeKindExpDivision:
			printf("(");
			prettyNode(e->val.binary.lhs);
			printf("/");
			prettyNode(e->val.binary.rhs);
			printf(")");
			break;

		case k_NodeKindExpEqual:
				printf("(");
				prettyNode(e->val.binary.lhs);
				printf("==");
				prettyNode(e->val.binary.rhs);
				printf(")");
				break;

		case k_NodeKindExpNotEqual:
				printf("(");
				prettyNode(e->val.binary.lhs);
				printf("!=");
				prettyNode(e->val.binary.rhs);
				printf(")");
				break;

		case k_NodeKindExpAnd:
		    printf("(");
		    prettyNode(e->val.binary.lhs);
		    printf("&&");
		    prettyNode(e->val.binary.rhs);
		    printf(")");
		    break;

		case k_NodeKindExpOr:
		    printf("(");
		    prettyNode(e->val.binary.lhs);
		    printf("||");
		    prettyNode(e->val.binary.rhs);
		    printf(")");
		    break;

		case k_NodeKindExpUMinus:
		    printf("(-");
		    prettyNode(e->val.node);
		    printf(")");
		    break;

		case k_NodeKindExpNeg:
		    printf("(!");
		    prettyNode(e->val.node);
		    printf(")");
		    break;

		case k_NodeKindTypeInt:
		    printf("int");
		    break;

		case k_NodeKindTypeBool:
		    printf("boolean");
		    break;

		case k_NodeKindTypeFloat:
		    printf("float");
		    break;

		case k_NodeKindTypeString:
		    printf("string");
		    break;

		case k_NodeKindDeclarationType:
		    printf("var ");
		    prettyNode(e->val.declaration.ident);
		    printf(": ");
		    prettyNode(e->val.declaration.varType);
		    printf(" = ");
		    prettyNode(e->val.declaration.exp);
		    printf(";\n");
		    break;

		case k_NodeKindDeclaration:
		    printf("var ");
		    prettyNode(e->val.declaration.ident);
		    printf(" = ");
		    prettyNode(e->val.declaration.exp);
		    printf(";\n");
		    break;

		case k_NodeKindStatementRead:
		    printf("read ");
		    printf("(");
		    prettyNode(e->val.node);
		    printf(")");
		    printf(";\n");
		    break;

		case k_NodeKindStatementPrint:
		    printf("print ");
		    printf("(");
		    prettyNode(e->val.node);
		    printf(")");
		    printf(";\n");
		    break;

		case k_NodeKindStatementAssign:
		    prettyNode(e->val.assignStatement.ident);
		    printf(" = ");
		    prettyNode(e->val.assignStatement.exp);
		    printf(";\n");
		    break;

		case k_NodeKindStatementIfElseIf:
		    printf("if (");
		    prettyNode(e->val.ifElseIfStatement.expression);
		    printf(") {\n");
		    prettyNode(e->val.ifElseIfStatement.program);
		    printf("} ");
		    prettyNode(e->val.ifElseIfStatement.elseIf);
		    printf("\n");
		    break;


		case k_NodeKindStatementIfElse:
		    printf("if (");
		    prettyNode(e->val.ifElseStatement.expression); // expression
		    printf(") {\n");
	 	    prettyNode(e->val.ifElseStatement.program);	// program
		    printf("} ");
		    prettyNode(e->val.ifElseStatement.elseIf); // else if
		    printf("else {\n");
		    prettyNode(e->val.ifElseStatement.program2); // program
		    printf("}\n");
		    break;

		case k_NodeKindStatementElseIf:
		    prettyNode(e->val.elseIfStatement.elseIf);
		    printf("else if (");
		    prettyNode(e->val.elseIfStatement.expression); // expression
		    printf(") {\n");
	 	    prettyNode(e->val.elseIfStatement.program);	// program
		    printf("} ");
		    break;

		case k_NodeKindStatementWhile:
		    printf("while ");
		    printf("(");
		    prettyNode(e->val.whileStatement.exp);
		    printf(") {\n");
		    prettyNode(e->val.whileStatement.program);
		    printf("}\n");
		    break;
	}
}
