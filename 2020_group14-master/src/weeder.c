#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "weeder.h"

int default_case = 0;
int weedDefault(Node* e) {
	if (e == NULL) {
		return 0;
	}
	switch (e->kind) {
		case k_NodeKindSwitchDec:
			return weedDefault(e->val.switch_dec.switch_cases);
		case k_NodeKindSwitchCases:
			return weedDefault(e->val.switch_cases.cases);
		case k_NodeKindDefault:
			default_case ++;
			if (default_case > 1) {
				return 1;
			}
			return weedDefault(e->val.switch_cases.cases);
	}
}

int continue_flag = 0;
int break_flag = 0;
int res = 0;
int weedContinueBreak(Node *e)
{
    	if (e == NULL) return 0;

	switch (e->kind) {

		case k_NodeKindFuncDec:
			return weedContinueBreak(e->val.func_dec.block_body);
			
		case k_NodeKindStatements:
			return weedContinueBreak(e->val.statements.statements)
			 + weedContinueBreak(e->val.statements.statement);
			
		case k_NodeKindIfStmt:
			if(e->val.if_stmt.simple_statement_dec==NULL)
			{
				return weedContinueBreak(e->val.if_stmt.block_body)
				+ weedContinueBreak(e->val.if_stmt.else_stmt);
			}
			else
			{
				return weedContinueBreak(e->val.if_stmt.simple_statement_dec) 
				+ weedContinueBreak(e->val.if_stmt.block_body) 
				+ weedContinueBreak(e->val.if_stmt.else_stmt);
			}
			
		case k_NodeKindElseStmt:
			if(e->val.else_stmt.if_stmt==NULL)
			{
				return weedContinueBreak(e->val.else_stmt.block_body);
			}
			else
			{
				return weedContinueBreak(e->val.else_stmt.if_stmt);
			}
			
		case k_NodeKindForDec:
			break_flag ++;
			continue_flag ++;
			res = weedContinueBreak(e->val.for_dec.for_condition)
			+ weedContinueBreak(e->val.for_dec.block_body);
			break_flag --;
			continue_flag --;
			return res;
			
		case k_NodeKindForCondition:
			return weedContinueBreak(e->val.for_condition.left) 
			+ weedContinueBreak(e->val.for_condition.right);
			
		case k_NodeKindSwitchDec:
			break_flag ++;
			res = weedContinueBreak(e->val.switch_dec.switch_def)
			 + weedContinueBreak(e->val.switch_dec.switch_cases);
			break_flag --;
			return res;
			
		case k_NodeKindSwitchDef:
			return weedContinueBreak(e->val.switch_def.simple_statement);
			
		case k_NodeKindSwitchCases:
			return weedContinueBreak(e->val.switch_cases.cases) 
			+ weedContinueBreak(e->val.switch_cases.statements);
			
		case k_NodeKindDefault:
			return weedContinueBreak(e->val.switch_cases.cases) 
			+ weedContinueBreak(e->val.switch_cases.statements);
			
		case k_NodeKindBlockDec:
			return weedContinueBreak(e->val.block_dec.block_body);
			
		case k_NodeKindBlockBody:
			return weedContinueBreak(e->val.block_body.statements);
		
		case k_NodeKindStatement:
			return weedContinueBreak(e->val.statement.stmt);
			
		case k_NodeKindSimpleStatementDec:
			return weedContinueBreak(e->val.simple_statement_dec.statement);
			
		case k_NodeKindBreak:
			if (break_flag > 0) {
				return 0;
			} else if (break_flag == 0) {
				printf("Error: < break statement at invalid location> line number: %d>", e->lineno);
				return 1;
			}
			
		case k_NodeKindContinue:
			if (continue_flag > 0) {
				return 0;
			} else if (continue_flag == 0) {
				printf("Error: < continue statement at invalid location> line number: %d>", e->lineno);
				return 1;
			}
			
		default:
			return 0;
		
	}
}



