/* Code section */
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "tree2.h"

extern Node* root;
extern int yylineno;
extern char* yytext;
int yylex();

void yyerror(const char *s) {
	if (mode == parse){
		fprintf(stderr, "Error: (line %d) %s\n", yylineno, s);
		fprintf(stderr,  "%s\n", yytext);
		exit(1);
	}
	else{
		while(yylex()){}
	}
}
%}

%code requires {
	#include "tree.h"
}

%union {
	int int_val;
	int octal_val;
	int hex_val;
	double float_val;
	char* string_val;
	char* rune_val;
	Node* node;
}

%type <node> 	program
%type <node> 	package_dec
%type <node> 	top_decs
%type <node> 	identifiers
%type <node> 	var_def
%type <node> 	var_defs
%type <node>    var_dec
%type <node>    type_def
%type <node>    type_defs
%type <node>    type_dec
%type <node>    identifier_type
%type <node>    expression
%type <node>    expressions
%type <node>    primary_expression
%type <node>	selector
%type <node>	index
%type <node>	func_call
%type <node>	expression_opt
%type <node>	expressions_opt
%type <node>	binary_expression
%type <node>	unary_expression
%type <node> 	literals
%type <node> 	builtins
%type <node>	struct_body
%type <node>	func_dec
%type <node>	block_dec
%type <node>	block_body
%type <node>	func_params
%type <node>	func_type
%type <node>	statement
%type <node>	statements
%type <node>	return_dec
%type <node>	break_dec
%type <node>	continue_dec
%type <node>	print_dec
%type <node>	println_dec
%type <node>	if_stmt
%type <node> 	else_stmt
%type <node>	for_dec
%type <node>	for_condition
%type <node>	simple_statement_dec
%type <node>	simple_statement
%type <node>	switch_dec
%type <node>	switch_def
%type <node>	switch_cases


%token <int_val> tINT
%token <float_val> tFLOAT
%token <string_val> tIDENTIFIER
%token <string_val> tSTRING
%token <hex_val> tHEX
%token <octal_val> tOCTAL
%token <rune_val> tRUNE

%token tNEWLINE

%token tBREAK
%token tCASE
%token tCHAN
%token tCONST
%token tCONTINUE
%token tDEFAULT
%token tDEFER
%token tELSE
%token tFALLTHROUGH
%token tFOR
%token tFUNC
%token tGO
%token tGOTO
%token tIF
%token tIMPORT
%token tINTERFACE
%token tMAP
%token tPACKAGE
%token tRANGE
%token tRETURN
%token tSELECT
%token tSTRUCT
%token tSWITCH
%token tTYPE
%token tVAR
%token tPRINT
%token tPRINTLN
%token tAPPEND
%token tLEN
%token tCAP

%token tPLUS                   "+"
%token tMINUS                  "-"
%token tMULTIPLY               "*"
%token tDIVIDE                 "/"
%token tMODULO                 "%"
%token tBIT_AND                "&"
%token tBIT_OR                 "|"
%token tBIT_XOR                "^"
%token tLEFT_SHIFT             "<<"
%token tRIGHT_SHIFT            ">>"
%token tBIT_CLEAR              "&^"
%token tPLUS_EQUAL             "+="
%token tMINUS_EQUAL            "-="
%token tMULTIPLY_EQUAL         "*="
%token tDIVIDE_EQUAL           "/="
%token tMODULO_EQUAL           "%="
%token tBIT_AND_EQUAL          "&="
%token tBIT_OR_EQUAL           "|="
%token tBIT_XOR_EQUAL          "^="
%token tLEFT_SHIFT_EQUAL       "<<="
%token tRIGHT_SHIFT_EQUAL      ">>="
%token tBIT_CLEAR_EQUAL        "&^="
%token tAND                    "&&"
%token tOR                     "||"
%token tCHANNEL		       "<-"
%token tINC                    "++"
%token tDEC                    "--"
%token tIS_EQUAL               "=="
%token tIS_NOT_EQUAL           "!="
%token tLESS_THAN              "<"
%token tGREATER_THAN           ">"
%token tEQUAL                  "="
%token tNOT                    "!"
%token tLESS_THAN_EQUAL        "<="
%token tGREATER_THAN_EQUAL     ">="
%token tDECLARATION            ":="
%token tELLIPSIS	       "..."
%token tLEFT_PAR	       "("
%token tLEFT_SQUARE		"["
%token tLEFT_CURL		"{"
%token tCOMMA                  ","
%token tDOT                    "."
%token tRIGHT_PAR              ")"
%token tRIGHT_SQUARE           "]"
%token tRIGHT_CURL             "}"
%token tSEMICOLON              ";"
%token tCOLON                  ":"


%left tOR
%left tAND
%left tIS_EQUAL tIS_NOT_EQUAL tLESS_THAN tGREATER_THAN tLESS_THAN_EQUAL tGREATER_THAN_EQUAL
%left tPLUS tMINUS tBIT_OR tBIT_XOR
%left tMULTIPLY tDIVIDE tMODULO tLEFT_SHIFT tRIGHT_SHIFT tBIT_AND tBIT_CLEAR
%left pNEG pPOS pNOT pXOR


%start program

%locations
%error-verbose

%%

/****************************
 *        PROGRAM
 ****************************/

/**
 * Grammar starting point
 **/
program
    : package_dec[package] top_decs[top]
        {
		root = newProgram($package, $top, @package.first_line);            
        }
    ;

/**
 * Package decalaration
 **/
package_dec[root]
    : tPACKAGE[package] tIDENTIFIER[id] tSEMICOLON
        {
            $root = newPackage_dec(newIdentifier($id, @id.first_line), @package.first_line);
        }
    ;

/**
 * Top level delcarations
 **/
top_decs[root]
    : top_decs[top] type_dec[type]
        {
            $root = newTop_decs($top, $type, @top.first_line);
        }
    | top_decs[top] var_dec[var]
        {
            $root = newTop_decs($top, $var, @top.first_line);
        }
    | top_decs[top] func_dec[func]
        {
            $root = newTop_decs($top, $func, @top.first_line);
        }
    | %empty
        {
            $root = NULL;
        }
    ;


/****************************
 *        VARIABLE
 ****************************/

/**
 * Variable delcaration
 **/
var_dec[root]
    : tVAR[var] var_def[variable] tSEMICOLON
        {
            $root = newVarDec($variable, @var.first_line);
        }
    | tVAR[var] tLEFT_PAR var_defs[variables] tRIGHT_PAR tSEMICOLON
        {
            $root = newVarDec($variables, @var.first_line);
        }
    ;

/**
 * Optional variable definitions
 **/
var_defs[root]
    : var_defs[defs] var_def[def] tSEMICOLON
        {
            $root = newVarDefs($defs, $def, @defs.first_line);
        }
    | %empty
        {
            $root = NULL;
        }
    ;

/**
 * Variable definition
 **/
var_def[root]
    : identifiers[ids] identifier_type[id_type] tEQUAL expressions[exprs]
        {
            $root = newVarDef($ids, $id_type, $exprs, @ids.first_line);
        }
	| identifiers[ids] identifier_type[id_type]
		{
			$root = newVarDef($ids, $id_type, NULL, @ids.first_line);
		}
    | identifiers[ids] tEQUAL expressions[exprs]
        {
            $root = newVarDef($ids, NULL, $exprs, @ids.first_line);
        }
    ;

/****************************
 *     TYPE DECLARATION
 ****************************/

/**
 * Type declaration
 **/
type_dec[root]
    : tTYPE[t] type_def[type] tSEMICOLON
        {
            $root = newTypeDec($type, @t.first_line);
        }
    | tTYPE[t] tLEFT_PAR type_defs[types] tRIGHT_PAR tSEMICOLON
        {
            $root = newTypeDec($types, @t.first_line);
        }
    ;

/**
 * Optinal type definitions
 */
type_defs[root]
    : type_defs[def] type_def[type] tSEMICOLON
        {
            $root = newTypeDefs($def, $type, @def.first_line);
        }
    | %empty
        {
            $root = NULL;
        }
    ;

/**
 * Type definition
 **/
type_def[root]
    : tIDENTIFIER[id] identifier_type[id_type]
        {
            $root = newTypeDefs(newIdentifier($id, @id.first_line), $id_type, @id.first_line);
        }
    ;

/****************************
 *     IDENTIFIER TYPE
 ****************************/

/**
 * Type for identifiers
 **/
identifier_type[root]
    : tLEFT_SQUARE[left] tINT[size] tRIGHT_SQUARE identifier_type[p_root]
        {
            $root = newIdentifierType($size, $p_root, NULL, NULL, k_NodeKindArrayType, @left.first_line);
        }
    | tLEFT_SQUARE[left_square] tRIGHT_SQUARE identifier_type[p_root]
        {
            $root = newIdentifierType(-1, $p_root, NULL, NULL, k_NodeKindSliceType, @left_square.first_line);
        }
    | tSTRUCT tLEFT_CURL[left_curl] struct_body[fields] tRIGHT_CURL
        {
            $root = newIdentifierType(-1, NULL, $fields, NULL, k_NodeKindStructType, @left_curl.first_line);
        }
    | tIDENTIFIER[id]
        {
            $root = newIdentifierType(-1, NULL, NULL, newIdentifier($id, @id.first_line), k_NodeKindIdType, @id.first_line);
        }
    | tLEFT_PAR[left] identifier_type[id_type] tRIGHT_PAR
        {
            $root = newIdentifierType(-1, $id_type, NULL, NULL, k_NodeKindParType, @left.first_line);
        }
    ;

/**
 * 'struct' body
 **/
struct_body[root]
    : struct_body[sb] identifiers[ids] identifier_type[id_type] tSEMICOLON
        {
           $root = newStructBody($sb, $ids, $id_type, @sb.first_line);
        }
    | %empty
        {
            $root = NULL;
        }
    ;

/****************************
 *        FUNCTION
 ****************************/

/**
 * Function declaration
 **/
func_dec[root]
    : tFUNC[func] tIDENTIFIER[id] tLEFT_PAR func_params[params] tRIGHT_PAR func_type[id_type] block_body[block] tSEMICOLON
        {
            $root = newFuncDec(newIdentifier($id, @id.first_line), $params, $id_type, $block, @func.first_line);
        }
    | tFUNC[func] tIDENTIFIER[id] tLEFT_PAR tRIGHT_PAR func_type[id_type] block_body[block] tSEMICOLON
        {
            $root = newFuncDec(newIdentifier($id, @id.first_line), NULL, $id_type, $block, @func.first_line);
        }
    ;

/**
 * Function parameters
 **/
func_params[root]
    : func_params[params] tCOMMA identifiers[ids] identifier_type[id_type]
        {
            $root = newFuncParams($params, $ids, $id_type, @params.first_line);
        }
    | identifiers[ids] identifier_type[id_type]
        {
            $root = newFuncParams(NULL, $ids, $id_type, @ids.first_line);
        }
    ;

/**
 * One or more identifier
 **/
identifiers[root]
    : identifiers[ids] tCOMMA tIDENTIFIER[id]
        {
			$root = newIdentifiers($ids, newIdentifier($id, @id.first_line), @ids.first_line);
        }
    | tIDENTIFIER[id]
        {
            $root = newIdentifiers(NULL, newIdentifier($id, @id.first_line), @id.first_line);
        }
    ;

/**
 * Function type
 **/
func_type[root]
    : identifier_type[id_type]
        {
			$root = newFuncType($id_type, @id_type.first_line);
        }
    | %empty
        {
			$root = NULL;
        }
    ;
    

/****************************
 *      STATEMENTS
 ****************************/
statements[root]
    : statements[stmts] statement[stmt]
        {
			$root = newStatements($stmts, $stmt, @stmts.first_line);
        }
    | %empty
        {
			$root = NULL;
        }
    ;
  
/****************************
 *      IF STATEMENT
 ****************************/

/**
 * 'if' statement declaration
 **/
if_stmt[root]
    : tIF expression[condition] block_body[body] else_stmt[elseBody]
    {
    	$root=newIfStmt(NULL,$condition,$body,$elseBody,@1.first_line);
    }
    | tIF simple_statement_dec[decl] expression[condition] block_body[body] else_stmt[elseBody]
    {
    	$root=newIfStmt($decl,$condition,$body,$elseBody,@1.first_line);
    }
;

/**
 * Optional 'else' statement
 **/
else_stmt[root]
    : tELSE if_stmt[ifstmt]
        {
			$root=newElseStmt($ifstmt,NULL,@1.first_line);
        }
    | tELSE block_body[body]
    {
    	$root=newElseStmt(NULL,$body,@1.first_line);
    }
    | %empty
        {
			$root=NULL;
        }
    ;

/****************************
 *      FOR STATEMENT
 ****************************/

/**
 * For declaration
 **/
for_dec[root]
    : tFOR[for] for_condition[condition] block_body[body] tSEMICOLON
        {
            $root=newForDec($condition,$body,@for.first_line);
        }
    ;

/**
 * For condition forms
 **/
for_condition[root]
    : expression[expr]
        {
            $root=newForCondition(NULL,$expr,NULL,@expr.first_line);
        }
    | simple_statement[left] tSEMICOLON expression_opt[expr] tSEMICOLON simple_statement[right]
        {
            $root=newForCondition($left,$expr,$right,@left.first_line);
        }
    | %empty
        {
            $root=NULL;
        }
    ;

/****************************
 *     SWITCH STATEMENT
 ****************************/

/**
 * Swtich declaration
 **/
switch_dec[root]
    : switch_def[switch] tLEFT_CURL switch_cases[body] tRIGHT_CURL tSEMICOLON
        {
            $root=newSwitchDec($switch, $body, @switch.first_line);
        }
    ;

/**
 * Switch definition
 **/
switch_def[root]
    : tSWITCH[switch] simple_statement[simple] tSEMICOLON expression_opt[expr]
        {
            $root=newSwitchDef($simple,$expr,@switch.first_line);
        }
    | tSWITCH[switch] expression_opt[expr]
        {
            $root=newSwitchDef(NULL,$expr,@switch.first_line);
        }
    ;

/**
 * Switch statement body
 **/
switch_cases[root]
    : switch_cases[cases] tCASE expressions[exprs] tCOLON statements[stmts]
        {
            $root=newSwitchCases($cases,$exprs,$stmts,k_NodeKindSwitchCases,@2.first_line);
        }
    | switch_cases[cases] tDEFAULT tCOLON[colon] statements[stmts]
        {
            $root=newSwitchCases($cases,NULL,$stmts,k_NodeKindDefault,@2.first_line);
        }
    | %empty
        {
			$root=NULL;
        }
    ;

/****************************
 *     BLOCK STATEMENT
 ****************************/

/**
 * Block statement
 **/
block_dec[root]
    : block_body[block] tSEMICOLON
        {
			$root = newBlockDec($1, @1.first_line);
        }
    ;

/**
 * Block statement body
 **/
block_body[root]
    : tLEFT_CURL[left_curl] statements[stmts] tRIGHT_CURL
        {
            $root=newBlockBody($stmts,@left_curl.first_line);
        }
    ;

/****************************
 *     OTHER STATEMENTS
 ****************************/

/** 
 * Complex Statement declaration
 **/
statement[root]
    : var_dec
        {
            $root = newStatement($1, @1.first_line);
        }
    | type_dec
        {
            $root = newStatement($1, @1.first_line);
        }
    | simple_statement_dec
        {
            $root = newStatement($1, @1.first_line);
        }
    | return_dec
        {
            $root = newStatement($1, @1.first_line);
        }
    | break_dec
        {
            $root = newStatement($1, @1.first_line);
        }
    | continue_dec
        {
            $root = newStatement($1, @1.first_line);
        }
    | block_dec
        {
            $root = newStatement($1, @1.first_line);
        }
    | if_stmt
        {
            $root = newStatement($1, @1.first_line);
        }
    | for_dec
        {
            $root = newStatement($1, @1.first_line);
        }
    | print_dec
        {
            $root = newStatement($1, @1.first_line);
        }
    | println_dec
        {
            $root = newStatement($1, @1.first_line);
        }
    | switch_dec
        {
            $root = newStatement($1, @1.first_line);
        }
    ;

/**
 * Simple statement declaration
 **/
 simple_statement_dec[root]
    : simple_statement[simple] tSEMICOLON
        {
            $root = newSimpleStatementDec($simple, @simple.first_line);
        }
    ;

/**
 * Print statement
 **/
print_dec[root]
    : tPRINT tLEFT_PAR[left_par] expressions_opt[exprs] tRIGHT_PAR tSEMICOLON
        {
            $root = newPrintStatement($exprs, k_NodeKindPrintStatement, @left_par.first_line);
        }
    ;

/**
 * Println statement
 **/
println_dec[root]
    : tPRINTLN tLEFT_PAR[left_par] expressions_opt[exprs] tRIGHT_PAR tSEMICOLON
        {
            $root = newPrintStatement($exprs, k_NodeKindPrintlnStatement, @left_par.first_line);
        }
    ;

/**
 * Break statement
 **/
break_dec[root]
    : tBREAK[break] tSEMICOLON
        {
            $root=newBreak(@break.first_line);
        }
    ;

/**
 * Continue statement
 **/
continue_dec[root]
    : tCONTINUE[continue] tSEMICOLON
        {
            $root=newContinue(@continue.first_line);
        }
    ;

/**
 * Simple statement
 **/
simple_statement[root]
    : expression[expr]
        {
            $root = newSimpleStatement($expr, NULL, k_NodeKindSimpleStatementExp, @expr.first_line);
        }
    | expression[expr] tINC
        {
			$root = newSimpleStatement($expr, NULL, k_NodeKindSimpleStatementInc, @expr.first_line);
        }
    | expression[expr] tDEC
        {
			$root = newSimpleStatement($expr, NULL, k_NodeKindSimpleStatementDecrease, @expr.first_line);
        }
    | expressions[left] tEQUAL expressions[right]
        {
			$root = newSimpleStatement($left, $right, k_NodeKindSimpleStatementEqual, @left.first_line);
        }
    | expression[left] tPLUS_EQUAL expression[right]
        {
			$root = newSimpleStatement($left, $right, k_NodeKindSimpleStatementPlusEqual, @left.first_line);
        }
    | expression[left] tMINUS_EQUAL expression[right]
        {
$root = newSimpleStatement($left, $right, k_NodeKindSimpleStatementMinEqual, @left.first_line);
        }
    | expression[left] tMULTIPLY_EQUAL expression[right]
        {
$root = newSimpleStatement($left, $right, k_NodeKindSimpleStatementMulEqual, @left.first_line);
        }
    | expression[left] tDIVIDE_EQUAL expression[right]
        {
$root = newSimpleStatement($left, $right, k_NodeKindSimpleStatementDivEqual, @left.first_line);
        }
    | expression[left] tMODULO_EQUAL expression[right]
        {
$root = newSimpleStatement($left, $right, k_NodeKindSimpleStatementModEqual, @left.first_line);
        }
    | expression[left] tBIT_AND_EQUAL expression[right]
        {
$root = newSimpleStatement($left, $right, k_NodeKindSimpleStatementBitAndEqual, @left.first_line);
        }
    | expression[left] tBIT_OR_EQUAL expression[right]
        {
$root = newSimpleStatement($left, $right, k_NodeKindSimpleStatementBitOrEqual, @left.first_line);
        }
    | expression[left] tBIT_XOR_EQUAL expression[right]
        {
$root = newSimpleStatement($left, $right, k_NodeKindSimpleStatementBitXorEqual, @left.first_line);
        }
    | expression[left] tLEFT_SHIFT_EQUAL expression[right]
        {
$root = newSimpleStatement($left, $right, k_NodeKindSimpleStatementLeftShiftEqual, @left.first_line);
        }
    | expression[left] tRIGHT_SHIFT_EQUAL expression[right]
        {
$root = newSimpleStatement($left, $right, k_NodeKindSimpleStatementRightShiftEqual, @left.first_line);
        }
    | expression[left] tBIT_CLEAR_EQUAL expression[right]
        {
$root = newSimpleStatement($left, $right, k_NodeKindSimpleStatementBitClearEqual, @left.first_line);
        }
    | expressions[left] tDECLARATION expressions[right]
        {
       $root = newSimpleStatement($left, $right, k_NodeKindSimpleStatementDeclEqual, @left.first_line);     
        }
    | %empty
        {
			$root=NULL;
        }
    ;

/**
 * Return statement
 **/
return_dec[root]
    : tRETURN[re] expression_opt[expr] tSEMICOLON
        {
           $root=newReturnStatement($expr, @re.first_line);
        }
    ;

/****************************
 *       EXPRESSIONS
 ****************************/
/**
 * One or more expressions
 **/
expressions[root]
    : expressions tCOMMA expression[expr]
        {

        }
    | expression[expr]
        {
           
        }
    ;

/**
 * Option expression
 **/
expression_opt[root]
    : expression[expr]
        {

        }
    | %empty
        {

        }
    ;

/**
 * Zero or more expressions
 **/
expressions_opt[root]
    : expressions[exprs]
        {

        }
    | %empty
        {

        }
    ;


/**
 * All kinds of expressions
 **/
expression[root]
    : binary_expression[binary]
        {
			$root=newExpression($binary,@binary.first_line);
        }
    | unary_expression[unary]
        {
			$root=newExpression($unary,@unary.first_line);
        }
    | primary_expression[primary]
        {
			$root=newExpression($primary,@primary.first_line);
        }
	| literals[l]
        {
            $root=newExpression($l,@l.first_line);
        }
    | builtins[b]
    	{
            $root=newExpression($b,@b.first_line);
        }
	| func_call[fun]
        {
			$root=newExpression($fun,@fun.first_line);
        }
	| tLEFT_PAR expression[expr] tRIGHT_PAR
        {
            $root=newExpression($expr,@expr.first_line);
        }
    ;

/**
 * All the binary expressions
 **/
binary_expression[root]
    : expression[left] tPLUS expression[right]
        {
			$root = newExpressionBinary($left, $right, k_NodeKindExpressionBinaryPlus, @left.first_line);
        }
    | expression[left] tMINUS expression[right]
        {
			$root= newExpressionBinary($left, $right, k_NodeKindExpressionBinaryMinus, @left.first_line);
        }
    | expression[left] tMULTIPLY expression[right]
        {
			$root =newExpressionBinary($left, $right, k_NodeKindExpressionBinaryMultiply, @left.first_line);
        }
    | expression[left] tDIVIDE expression[right]
        {
			$root =newExpressionBinary($left, $right, k_NodeKindExpressionBinaryDivide, @left.first_line);
        }
    | expression[left] tMODULO expression[right]
        {
			$root =newExpressionBinary($left, $right, k_NodeKindExpressionBinaryModulo, @left.first_line);
        }
    | expression[left] tBIT_AND expression[right]
        {
			$root =newExpressionBinary($left, $right, k_NodeKindExpressionBinaryBitAnd, @left.first_line);
        }
    | expression[left] tBIT_OR expression[right]
        {
			$root =newExpressionBinary($left, $right, k_NodeKindExpressionBinaryBitOr, @left.first_line);
        }
    | expression[left] tBIT_XOR expression[right]
        {
			$root =newExpressionBinary($left, $right, k_NodeKindExpressionBinaryBitXor, @left.first_line);
        }
    | expression[left] tBIT_CLEAR expression[right]
        {
			$root=newExpressionBinary($left, $right, k_NodeKindExpressionBinarybitClear, @left.first_line);
        }
    | expression[left] tLEFT_SHIFT expression[right]
        {
			$root=newExpressionBinary($left, $right, k_NodeKindExpressionBinaryLeftShit, @left.first_line);
        }
    | expression[left] tRIGHT_SHIFT expression[right]
        {
			$root=newExpressionBinary($left, $right, k_NodeKindExpressionBinaryRightShift, @left.first_line);
        }
    | expression[left] tIS_EQUAL expression[right]
        {
			$root=newExpressionBinary($left, $right, k_NodeKindExpressionBinaryisEqual, @left.first_line);
        }
    | expression[left] tIS_NOT_EQUAL expression[right]
        {
			$root=newExpressionBinary($left, $right, k_NodeKindExpressionBinaryisNotEqual, @left.first_line);
        }
    | expression[left] tLESS_THAN expression[right]
        {
			$root=newExpressionBinary($left, $right, k_NodeKindExpressionBinaryLessThan, @left.first_line);
        }
    | expression[left] tGREATER_THAN expression[right]
        {
			$root=newExpressionBinary($left, $right, k_NodeKindExpressionBinaryGreaterThan, @left.first_line);
        }
    | expression[left] tLESS_THAN_EQUAL expression[right]
        {
			$root=newExpressionBinary($left, $right, k_NodeKindExpressionBinaryLessThanEqual, @left.first_line);
        }
    | expression[left] tGREATER_THAN_EQUAL expression[right]
        {
			$root=newExpressionBinary($left, $right, k_NodeKindExpressionBinaryGreaterThanEqual, @left.first_line);
        }
    | expression[left] tAND expression[right]
        {
			$root=newExpressionBinary($left, $right, k_NodeKindExpressionBinaryAnd, @left.first_line);
        }
    | expression[left] tOR expression[right]
        {
			$root=newExpressionBinary($left, $right, k_NodeKindExpressionBinaryOr, @left.first_line);
        }
    ;

/**
 * All the unary expressions
 **/
unary_expression[root]
    : tMINUS expression[operand] %prec pNEG
        {
			$root=newExpressionUnary($operand,k_NodeKindUMinus,@operand.first_line);
        }
    | tPLUS expression[operand] %prec pPOS
        {
			$root=newExpressionUnary($operand,k_NodeKindUPlus,@operand.first_line);
        }
    | tNOT expression[operand] %prec pNOT
        {
			$root=newExpressionUnary($operand,k_NodeKindUNot,@operand.first_line);
        }
    | tBIT_XOR expression[operand] %prec pXOR
        {
			$root=newExpressionUnary($operand,k_NodeKindUXor,@operand.first_line);
        }
    ;
    

/**
 * Literals
 **/
 literals[root]
 	:tINT[integer]
        {
            $root=newIntLiteral($integer,@integer.first_line);
        }
    | tFLOAT[float]
        {
            $root=newFloatLiteral($float,@float.first_line);
        }
    | tSTRING[string]
        {
            $root=newStringLiteral($string,@string.first_line);
        }
    | tRUNE[rune]
        {
            $root=newRuneLiteral($rune,@rune.first_line);
        }
    ;
    
/**
 * Builtins
 **/
builtins[root]
	: tAPPEND tLEFT_PAR expression[left] tCOMMA expression[right] tRIGHT_PAR
        {
            $root=newBuiltin($left,$right,k_NodeKindAppend,@1.first_line);
        }
	| tLEN tLEFT_PAR expression[left] tRIGHT_PAR
		{
			$root=newBuiltin($left,NULL,k_NodeKindLen,@1.first_line);
		}
	| tCAP tLEFT_PAR expression[left] tRIGHT_PAR
		{
			$root=newBuiltin($left,NULL,k_NodeKindCap,@1.first_line);
		}
	;

/**
 * Function call
 **/
func_call[root]
	:tIDENTIFIER[id] tLEFT_PAR[left_par] expressions_opt[exprs] tRIGHT_PAR	// identifier(...)
        {
			$root=newFuncCall(newIdentifier($id,@id.first_line),$exprs,@id.first_line);
        }
    ;

/**
 * Expression decorators
 * Append syntax to an expression
 **/
primary_expression[root]
    : primary_expression selector[id_selector]
        {
			$root=newExpressionPrimary($1,$id_selector,NULL,NULL,@id_selector.first_line);
        }
    | primary_expression index[i]
        {
			$root=newExpressionPrimary($1,NULL,$i,NULL,@i.first_line);
        }
    | tIDENTIFIER[id]
        {
     $root=newExpressionPrimary(NULL,NULL,NULL,newIdentifier($id,@id.first_line),@id.first_line);
        }
    ;

/**
 * Member access
 **/
selector[root]
    : tDOT tIDENTIFIER[id]
        {
			$root=newSelector(newIdentifier($id,@id.first_line),@id.first_line);
        }
    ;

/**
 * Array indices
 **/
index[root]
    : tLEFT_SQUARE expression[expr] tRIGHT_SQUARE
        {
			$root=newIndex($expr,@1.first_line);
        }
    ;






