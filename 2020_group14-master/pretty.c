#include <stdio.h>
#include "pretty.h"

void prettyNode(Node *e)
{
    	if (e == NULL) return;

	switch (e->kind) {

		case k_NodeKindProgram:
		    prettyNode(e->val.program.package_dec);
		    prettyNode(e->val.program.top_decs);
		    break;

		case k_NodeKindPackageDec:
			printf("package ");
			prettyNode(e->val.package_dec.identifier);
			printf("\n");
			break;
			
		case k_NodeKindIdentifier:
			printf("%s", e->val.identifier);
			printf(" ");
			break;
			
		case k_NodeKindTopDecs:
			prettyNode(e->val.top_decs.top_decs);
			prettyNode(e->val.top_decs.dec);
			break;
			
		case k_NodeKindVarDecLine:
			printf("var ");
			prettyNode(e->val.var_dec.def);
			break;
			
		case k_NodeKindVarDecPar:
			printf("var (\n");
			prettyNode(e->val.var_dec.def);
			printf(")\n");
			break;
		case k_NodeKindVarDefs:
			prettyNode(e->val.var_defs.var_defs);
			prettyNode(e->val.var_defs.var_def);
			break;
		case k_NodeKindVarDef:
			if (e->val.var_def.expressions == NULL) {
				prettyNode(e->val.var_def.identifiers);
				printf(" ");
				prettyNode(e->val.var_def.identifier_type);
				printf(" ");
			} else if (e->val.var_def.identifier_type == NULL) {
				prettyNode(e->val.var_def.identifiers);
				printf("=");
				prettyNode(e->val.var_def.expressions);
			} else {
				prettyNode(e->val.var_def.identifiers);
				prettyNode(e->val.var_def.identifier_type);
				printf("=");
				prettyNode(e->val.var_def.expressions);
			}
			break;
		case k_NodeKindTypeDecLine:
			printf("type ");
			prettyNode(e->val.type_dec.def);
			printf("\n");
			break;
		case k_NodeKindTypeDecPar:
			printf("type (");
			prettyNode(e->val.type_dec.def);
			printf(")\n");
			break;
		case k_NodeKindTypeDefs:
			prettyNode(e->val.type_defs.type_defs);
			prettyNode(e->val.type_defs.type_def);
			break;
		case k_NodeKindTypeDef:
			prettyNode(e->val.type_def.identifier);
			prettyNode(e->val.type_def.identifier_type);
			break;
			
		case k_NodeKindArrayType:
			printf("[");
			printf("%d", e->val.identifier_type.size);
			printf("]");
			prettyNode(e->val.identifier_type.identifier_type);
			break;
		case k_NodeKindSliceType:
			printf("[");
			printf("]");
			prettyNode(e->val.identifier_type.identifier_type);
			break;
		case k_NodeKindStructType:
			printf("struct {");
			prettyNode(e->val.identifier_type.struct_body);
			printf("}");
			break;
		case k_NodeKindIdType:
			prettyNode(e->val.identifier_type.identifier);
			break;
		case k_NodeKindParType:
			printf("(");
			prettyNode(e->val.identifier_type.identifier_type);
			printf(")");
			break;
		case k_NodeKindStructBody:
			prettyNode(e->val.struct_body.struct_body);
			prettyNode(e->val.struct_body.identifiers);
			printf(" ");
			prettyNode(e->val.struct_body.identifier_type);
			printf("\n");
			break;
		case k_NodeKindFuncDec:
			if (e->val.func_dec.func_params == NULL) {
				printf("func");
				printf(" ");
				prettyNode(e->val.func_dec.identifier);
				printf("()");
				prettyNode(e->val.func_dec.func_type);
				prettyNode(e->val.func_dec.block_body);
			} else {
				printf("func");
				printf(" ");
				prettyNode(e->val.func_dec.identifier);
				printf("(");
				prettyNode(e->val.func_dec.func_params);
				printf(")");
				prettyNode(e->val.func_dec.func_type);
				prettyNode(e->val.func_dec.block_body);
			}
			break;
		case k_NodeKindFuncParams:
			if(e->val.func_params.func_params==NULL)
			{
				prettyNode(e->val.func_params.identifiers);
				prettyNode(e->val.func_params.identifier_type);
			}
			else
			{
				prettyNode(e->val.func_params.func_params);
				printf(", ");
				prettyNode(e->val.func_params.identifiers);
				prettyNode(e->val.func_params.identifier_type);
			}
			break;
			
		case k_NodeKindIdentifiers:
			if(e->val.identifiers.identifiers==NULL)
			{
				prettyNode(e->val.identifiers.identifier);
			}
			else
			{
				prettyNode(e->val.identifiers.identifiers);
				printf(",");
				prettyNode(e->val.identifiers.identifier);
			}
			break;
			
		case k_NodeKindFuncType:
			prettyNode(e->val.func_type.identifier_type);
			break;
			
		case k_NodeKindStatements:
			prettyNode(e->val.statements.statements);
			prettyNode(e->val.statements.statement);
			printf("\n");
			break;
			
		case k_NodeKindIfStmt:
			if(e->val.if_stmt.simple_statement_dec==NULL)
			{
				printf("if ");
				prettyNode(e->val.if_stmt.expression);
				prettyNode(e->val.if_stmt.block_body);
				prettyNode(e->val.if_stmt.else_stmt);
			}
			else
			{
				printf("if ");
				prettyNode(e->val.if_stmt.simple_statement_dec);
				printf(";");
				prettyNode(e->val.if_stmt.expression);
				prettyNode(e->val.if_stmt.block_body);
				prettyNode(e->val.if_stmt.else_stmt);
			}
			break;
			
		case k_NodeKindElseStmt:
			printf("else ");
			if(e->val.else_stmt.if_stmt==NULL)
			{
				prettyNode(e->val.else_stmt.block_body);
			}
			else
			{
				prettyNode(e->val.else_stmt.if_stmt);
			}
			break;
			
		case k_NodeKindForDec:
			printf("for ");
			prettyNode(e->val.for_dec.for_condition);
			prettyNode(e->val.for_dec.block_body);
			break;
			
		case k_NodeKindForCondition:
			if(e->val.for_condition.left==NULL)
			{
				prettyNode(e->val.for_condition.expression);
			}
			else
			{
				prettyNode(e->val.for_condition.left);
				printf(";");
				prettyNode(e->val.for_condition.expression);
				printf(";");
				prettyNode(e->val.for_condition.right);
			}
			break;
			
		case k_NodeKindSwitchDec:
			printf("switch ");
			prettyNode(e->val.switch_dec.switch_def);
			printf("{");
			prettyNode(e->val.switch_dec.switch_cases);
			printf("}");
			break;
			
		case k_NodeKindSwitchDef:
			if(e->val.switch_def.simple_statement==NULL)
			{
				prettyNode(e->val.switch_def.expression_opt);
			}
			else
			{
				prettyNode(e->val.switch_def.simple_statement);
				printf(";");
				prettyNode(e->val.switch_def.expression_opt);
			}
			break;
			
		case k_NodeKindSwitchCases:
			prettyNode(e->val.switch_cases.cases);
			printf("case ");
			prettyNode(e->val.switch_cases.expressions);
			printf(":\n");
			prettyNode(e->val.switch_cases.statements);
			break;
			
		case k_NodeKindDefault:
			prettyNode(e->val.switch_cases.cases);
			printf("case ");
			printf("default:\n");
			prettyNode(e->val.switch_cases.statements);
			break;
			
		case k_NodeKindBlockDec:
			prettyNode(e->val.block_dec.block_body);
			break;
			
		case k_NodeKindBlockBody:
			printf("{\n");
			prettyNode(e->val.block_body.statements);
			printf("\n}\n");
			break;
		
		case k_NodeKindStatement:
			prettyNode(e->val.statement.stmt);
			printf("\n");
			break;
			
		case k_NodeKindSimpleStatementDec:
			prettyNode(e->val.simple_statement_dec.statement);
			break;
			
		case k_NodeKindPrintDec:
			printf("print(");
			prettyNode(e->val.print_dec.expression_opt);
			printf(")");
			break;
			
		case k_NodeKindPrintlnDec:
			printf("println(");
			prettyNode(e->val.print_dec.expression_opt);
			printf(")");
			break;
		
		case k_NodeKindBreak:
			printf("break;\n");
			break;
			
		case k_NodeKindContinue:
			printf("continue;\n");
			break;
		
		case k_NodeKindSimpleStatementExp:
			prettyNode(e->val.simple_statement.lhs);
			break;
			
		case k_NodeKindSimpleStatementInc:
			prettyNode(e->val.simple_statement.lhs);
			printf("++");
			break;
			
		case k_NodeKindSimpleStatementDecrease:
			prettyNode(e->val.simple_statement.lhs);
			printf("--");
			break;
			
		case k_NodeKindSimpleStatementEqual:
			prettyNode(e->val.simple_statement.lhs);
			printf("=");
			prettyNode(e->val.simple_statement.rhs);
			break;
			
		case k_NodeKindSimpleStatementPlusEqual:
			prettyNode(e->val.simple_statement.lhs);
			printf("+=");
			prettyNode(e->val.simple_statement.rhs);
			break;
			
		case k_NodeKindSimpleStatementMinEqual:
			prettyNode(e->val.simple_statement.lhs);
			printf("-=");
			prettyNode(e->val.simple_statement.rhs);
			break;
			
		case k_NodeKindSimpleStatementMulEqual:
			prettyNode(e->val.simple_statement.lhs);
			printf("*=");
			prettyNode(e->val.simple_statement.rhs);
			break;
			
		case k_NodeKindSimpleStatementDivEqual:
			prettyNode(e->val.simple_statement.lhs);
			printf("/=");
			prettyNode(e->val.simple_statement.rhs);
			break;
			
		case k_NodeKindSimpleStatementModEqual:
			prettyNode(e->val.simple_statement.lhs);
			printf("%%");
			printf("=");
			prettyNode(e->val.simple_statement.rhs);
			break;
			
		case k_NodeKindSimpleStatementBitAndEqual:
			prettyNode(e->val.simple_statement.lhs);
			printf("&=");
			prettyNode(e->val.simple_statement.rhs);
			break;
			
		case k_NodeKindSimpleStatementBitOrEqual:
			prettyNode(e->val.simple_statement.lhs);
			printf("|=");
			prettyNode(e->val.simple_statement.rhs);
			break;
		
		case k_NodeKindSimpleStatementBitXorEqual:
			prettyNode(e->val.simple_statement.lhs);
			printf("^=");
			prettyNode(e->val.simple_statement.rhs);
			break;
		
		case k_NodeKindSimpleStatementLeftShiftEqual:
			prettyNode(e->val.simple_statement.lhs);
			printf("<<=");
			prettyNode(e->val.simple_statement.rhs);
			break;
			
		case k_NodeKindSimpleStatementRightShiftEqual:
			prettyNode(e->val.simple_statement.lhs);
			printf(">>=");
			prettyNode(e->val.simple_statement.rhs);
			break;
			
		case k_NodeKindSimpleStatementBitClearEqual:
			prettyNode(e->val.simple_statement.lhs);
			printf("&^=");
			prettyNode(e->val.simple_statement.rhs);
			break;
			
		case k_NodeKindSimpleStatementDeclEqual:
			prettyNode(e->val.simple_statement.lhs);
			printf(":=");
			prettyNode(e->val.simple_statement.rhs);
			break;
			
		case k_NodeKindReturn:
			printf("return ");
			prettyNode(e->val.return_dec.expressions_opt);
			printf("\n");
			break;
			
		case k_NodeKindExpressions:
			if(e->val.expressions.expressions ==NULL){
				prettyNode(e->val.expressions.expression);
			}else{
				prettyNode(e->val.expressions.expressions);
				printf(", ");
				prettyNode(e->val.expressions.expression);
			}
			break;
		
		case k_NodeKindExpressionOpt:
			prettyNode(e->val.expression.expression);
			break;
			
		case k_NodeKindExpression:
			prettyNode(e->val.expression.expression);
			break;
			
		case k_NodeKindExpressionBinaryPlus:
			prettyNode(e->val.binary.lhs);
			printf("+");
			prettyNode(e->val.binary.rhs);
			break;
			
		case k_NodeKindExpressionBinaryMinus:
			prettyNode(e->val.binary.lhs);
			printf("-");
			prettyNode(e->val.binary.rhs);
			break;
			
		case k_NodeKindExpressionBinaryMultiply:
			prettyNode(e->val.binary.lhs);
			printf("*");
			prettyNode(e->val.binary.rhs);
			break;
			
		case k_NodeKindExpressionBinaryDivide:
			prettyNode(e->val.binary.lhs);
			printf("/");
			prettyNode(e->val.binary.rhs);
			break;
			
		case k_NodeKindExpressionBinaryModulo:
			prettyNode(e->val.binary.lhs);
			printf("%%");
			prettyNode(e->val.binary.rhs);
			break;
			
		case k_NodeKindExpressionBinaryBitAnd:
			prettyNode(e->val.binary.lhs);
			printf("&");
			prettyNode(e->val.binary.rhs);
			break;
			
		case k_NodeKindExpressionBinaryBitOr:
			prettyNode(e->val.binary.lhs);
			printf("|");
			prettyNode(e->val.binary.rhs);
			break;
		
		case k_NodeKindExpressionBinaryBitXor:
			prettyNode(e->val.binary.lhs);
			printf("^");
			prettyNode(e->val.binary.rhs);
			break;
			
		case k_NodeKindExpressionBinarybitClear:
			prettyNode(e->val.binary.lhs);
			printf("&^");
			prettyNode(e->val.binary.rhs);
			break;
			
		case k_NodeKindExpressionBinaryLeftShift:
			prettyNode(e->val.binary.lhs);
			printf("<<");
			prettyNode(e->val.binary.rhs);
			break;
		
		case k_NodeKindExpressionBinaryRightShift:
			prettyNode(e->val.binary.lhs);
			printf(">>");
			prettyNode(e->val.binary.rhs);
			break;
			
		case k_NodeKindExpressionBinaryIsEqual:
			prettyNode(e->val.binary.lhs);
			printf("==");
			prettyNode(e->val.binary.rhs);
			break;
		
		case k_NodeKindExpressionBinaryIsNotEqual:
			prettyNode(e->val.binary.lhs);
			printf("!=");
			prettyNode(e->val.binary.rhs);
			break;
		
		case k_NodeKindExpressionBinaryLessThan:
			prettyNode(e->val.binary.lhs);
			printf("<");
			prettyNode(e->val.binary.rhs);
			break;
			
		case k_NodeKindExpressionBinaryGreaterThan:
			prettyNode(e->val.binary.lhs);
			printf(">");
			prettyNode(e->val.binary.rhs);
			break;
			
		case k_NodeKindExpressionBinaryLessThanEqual:
			prettyNode(e->val.binary.lhs);
			printf("<=");
			prettyNode(e->val.binary.rhs);
			break;
			
		case k_NodeKindExpressionBinaryGreaterThanEqual:
			prettyNode(e->val.binary.lhs);
			printf(">=");
			prettyNode(e->val.binary.rhs);
			break;
		
		case k_NodeKindExpressionBinaryAnd:
			prettyNode(e->val.binary.lhs);
			printf("&&");
			prettyNode(e->val.binary.rhs);
			break;
			
		case k_NodeKindExpressionBinaryOr:
			prettyNode(e->val.binary.lhs);
			printf("||");
			prettyNode(e->val.binary.rhs);
			break;
			
		case k_NodeKindUMinus:
			printf("-");
			prettyNode(e->val.unary.operand);
			break;
			
		case k_NodeKindUPlus:
			printf("+");
			prettyNode(e->val.unary.operand);
			break;
		
		case k_NodeKindUNot:
			printf("!");
			prettyNode(e->val.unary.operand);
			break;
		
		case k_NodeKindUXor:
			printf("^");
			prettyNode(e->val.unary.operand);
			break;
			
		case k_NodeKindIntLiteral:
			printf("%i", e->val.intLiteral);
			break;
			
		case k_NodeKindFloatLiteral:
			printf("%f", e->val.floatLiteral);
			break;
			
		case k_NodeKindStringLiteral:
			printf("%s", e->val.stringLiteral);
			break;
			
		case k_NodeKindRuneLiteral:
			printf("%s", e->val.runeLiteral);
			break;
			
		case k_NodeKindAppend:
			printf("append(");
			prettyNode(e->val.builtins.expression1);
			printf(", ");
			prettyNode(e->val.builtins.expression2);
			printf(")");
			break;
		
		case k_NodeKindLen:
			printf("Len(");
			prettyNode(e->val.builtins.expression1);
			printf(")");
			break;
			
		case k_NodeKindCap:
			printf("Len(");
			prettyNode(e->val.builtins.expression1);
			printf(")");
			break;
			
		case k_NodeKindFuncCall:
			prettyNode(e->val.func_call.identifier);
			printf("(");
			prettyNode(e->val.func_call.expressions_opt);
			printf(")");
			break;
			
		case k_NodeKindExpressionPrimary:
			if(e->val.primary_expression.selector!=NULL)
			{
				prettyNode(e->val.primary_expression.primary_expression);
				prettyNode(e->val.primary_expression.selector);
			}
			else if(e->val.primary_expression.index!=NULL)
			{
				prettyNode(e->val.primary_expression.primary_expression);
				prettyNode(e->val.primary_expression.index);
			}
			else
			{
				prettyNode(e->val.primary_expression.identifier);
			}
			break;
			
		case k_NodeKindSelector:
			printf(".");
			prettyNode(e->val.selector.identifier);
			break;
			
		case k_NodeKindIndex:
			printf("[");
			prettyNode(e->val.index.expression);
			printf("]");
			break;
			
		
	}
}
