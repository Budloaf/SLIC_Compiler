%{
/*
 *-------------------------------------------------------------------------
 * Project   - SLIC Compiler
 * Course    - CS4223
 * Developer - Hayden Berry
 * 
 * Description - This is the parser for the compiler.
 * Due - 02.04.2025
 *-------------------------------------------------------------------------
 */

#include <stdio.h>
#include "symbolTable.h"
#include "ast.h"

struct listnode * list;
struct SymbolTable * st;

char exp_has_float(struct expression *, struct expression *);
int yyerror();
int yylex();

char * name;
char dataType;
char varType;
int size;

%}

%union {
	int intval;
	char *strval;
	float floval;
	struct listnode * listpoint;
    struct expression * exppoint;
	struct statement * stmtpoint;
	struct printitem * itempoint;
	struct plist * plistpoint;
}
%token			MAIN
%token			EXIT
%token			END
%token			IF
%token			ELSE
%token			WHILE
%token			DATA
%token			ALGORITHM
%token			INTEGER_TYPE
%token			REAL_TYPE
%token			COUNTING
%token			UPWARD
%token			DOWNWARD
%token			TO
%token			READ
%token			PRINT

%token			PLUS
%token			MINUS
%token			MULTIPLY
%token			DIVIDE
%token			MODULO
%token			EQUAL
%token			LESS_THAN
%token			GREATER_THAN
%token			LESS_THAN_EQUAL
%token			GREATER_THAN_EQUAL
%token			NOT_EQUAL
%token			SEMICOLON
%token			EXCLAMATION_MARK
%token			COMMA
%token			COLON
%token			OPEN_BRACKET
%token			CLOSE_BRACKET
%token			BOOL_AND
%token			BOOL_OR
%token			BOOL_NOT
%token			ASSIGNMENT
%token			OPEN_PARENTHESIS
%token			CLOSE_PARENTHESIS

%token			WHITESPACE
%token <strval>	VARIABLE_NAME
%token <strval>	CHAR_STRING
%token <intval>	INTEGER_CONSTANT
%token <floval>	FLOAT_CONSTANT
%token			COMMENT
%token        	NEWLINE

%token			TRASH

%type  <listpoint> statementlist

%type  <stmtpoint>  statement

%type  <stmtpoint>  printstatement
%type  <plistpoint> printlist
%type  <itempoint>  printitem

%type  <stmtpoint> assignstatement
%type  <stmtpoint> readstatement
%type  <stmtpoint> exitstatement

%type  <stmtpoint> ifstatement
%type  <stmtpoint> ifelsestatement
%type  <listpoint> body

%type  <stmtpoint> whileloop

%type  <stmtpoint> countingloop


%type  <exppoint>  exp
%type  <exppoint>  comp
%type  <exppoint>  arithexp
%type  <exppoint>  term
%type  <exppoint>  fact
%type  <exppoint>  unit


%%
prog    		: MAIN SEMICOLON DATA COLON declist ALGORITHM COLON statementlist END MAIN SEMICOLON
				{
					list = $8;
				}
        		;

declist 		: declist dec 
				| 
				;


dec 			: type COLON varlist SEMICOLON
				;

varlist			: vardec COMMA varlist
				| vardec
				;

type 			: INTEGER_TYPE 
				{
					dataType = DT_INTEGER;
				}
				| REAL_TYPE 	
				{
					dataType = DT_REAL;
				}
				;

vardec 			:  VARIABLE_NAME 											 
				{
					insertST(st,$1,dataType,VT_SCALAR,1);
					
				}
				|  VARIABLE_NAME OPEN_BRACKET INTEGER_CONSTANT CLOSE_BRACKET 
				{
					insertST(st,$1,dataType,VT_ARRAY,$3);
				}
				;

statementlist 	: statement statementlist
				{
					$$ = malloc(sizeof(struct listnode));
					$$->stmt = $1;
					$$->link = $2;
				}
				| statement
				{
					$$ = malloc(sizeof(struct listnode));
					$$->stmt = $1;
					$$->link = NULL;
				}
				;

statement 		: printstatement
				{
					$$ = $1;
				}
				| assignstatement
				{
					$$ = $1;
				}
				| readstatement
				{
					$$ = $1;
				}
				| ifstatement
				{
					$$ = $1;
				}
				| ifelsestatement
				{
					$$ = $1;
				}
				| whileloop
				{
					$$ = $1;
				}
				| exitstatement
				{
					$$ = $1;
				}
				| countingloop
				{
					$$ = $1;
				}
				;

printstatement	: PRINT printlist SEMICOLON
				{
					$$ = malloc(sizeof(struct statement));
					$$->args = $2;
					$$->sk = SK_P;
				}
				;
			
printlist		: printitem COMMA printlist
				{
					$$ = malloc(sizeof(struct plist));
					$$->item = $1;
					$$->link = $3;
					
					
				}
				| printitem
				{
					$$ = malloc(sizeof(struct plist));
					$$->item = $1;
					$$->link = NULL;
				}
				;

printitem 		: exp
				{
					$$ = malloc(sizeof(struct printitem));
					$$->pk = PK_E;
					$$->exp = $1;
				}
				| EXCLAMATION_MARK
				{
					$$ = malloc(sizeof(struct printitem));
					$$->pk = PK_NL;
				}
				| CHAR_STRING
				{
					$$ = malloc(sizeof(struct printitem));
					$$->pk = PK_CS;
					$$->char_string = $1;
				}
				;

assignstatement : VARIABLE_NAME ASSIGNMENT exp SEMICOLON
				{
					$$ = malloc(sizeof(struct statement));
					$$->sk = SK_A;
					$$->arg_exp = $3;
					$$->dest_base_address = addressofST(st,$1);
				}
				| VARIABLE_NAME OPEN_BRACKET exp CLOSE_BRACKET ASSIGNMENT exp SEMICOLON
				{
					$$ = malloc(sizeof(struct statement));
					$$->sk = SK_A;
					$$->index_exp = $3;
					$$->arg_exp = $6;
					$$->dest_base_address = addressofST(st,$1);
				}
				;
readstatement   : READ VARIABLE_NAME SEMICOLON
				{
					$$ = malloc(sizeof(struct statement));
					$$->sk = SK_R;
					$$->dest_base_address = addressofST(st,$2);
					$$->index_exp = NULL;
				}
				| READ VARIABLE_NAME OPEN_BRACKET exp CLOSE_BRACKET SEMICOLON
				{
					$$ = malloc(sizeof(struct statement));
					$$->sk = SK_R;
					$$->dest_base_address = addressofST(st,$2);
					$$->index_exp = $4;
				}
				;
ifstatement		: IF exp SEMICOLON body END IF SEMICOLON
				{
					$$ = malloc(sizeof(struct statement));
					$$->arg_exp = $2;
					$$->sk = SK_IF;
					$$->body = $4;
				}
				;

body			: statementlist
				{
					$$ = $1;
				}
				;

ifelsestatement : IF exp SEMICOLON body ELSE SEMICOLON body END IF SEMICOLON
				{
					$$ = malloc(sizeof(struct statement));
					$$->arg_exp = $2;
					$$->sk = SK_IFE;
					$$->body = $4;
					$$->elsebody = $7;
				}

whileloop	  	: WHILE exp SEMICOLON body END WHILE SEMICOLON
				{
					$$ = malloc(sizeof(struct statement));
					$$->arg_exp = $2;
					$$->sk = SK_W;
					$$->body = $4;
				}
				;

exitstatement   : EXIT SEMICOLON
				{
					$$ = malloc(sizeof(struct statement));
					$$->sk = SK_EX;
				}
				;

countingloop 	: COUNTING VARIABLE_NAME UPWARD exp TO exp SEMICOLON body END COUNTING SEMICOLON
				{
					$$ = malloc(sizeof(struct statement));
					$$->iterator_address = addressofST(st,$2);
					$$->start_exp = $4;
					$$->finish_exp = $6;
					$$->body = $8;
					$$->sk = SK_CU;
				}
				| COUNTING VARIABLE_NAME DOWNWARD exp TO exp SEMICOLON body END COUNTING SEMICOLON
				{
					$$ = malloc(sizeof(struct statement));
					$$->iterator_address = addressofST(st,$2);
					$$->start_exp = $4;
					$$->finish_exp = $6;
					$$->body = $8;
					$$->sk = SK_CD;
				}
				;

//---------------------------------------------------------------------------------------------------------

exp				: exp BOOL_AND comp
				{
					$$ = malloc(sizeof(struct expression));
					$$->kind 	  = EK_OP;
					$$->l_operand = $1;
					$$->r_operand = $3;
					$$->return_type = EK_INT;
					if (exp_has_float($1,$3)) {
						$$->operator = OP_ANF;
					} 
					else {
						$$->operator = OP_ANI;
					}
				}
				| exp BOOL_OR comp
				{
					$$ = malloc(sizeof(struct expression));
					$$->kind 	  = EK_OP;
					$$->l_operand = $1;
					$$->r_operand = $3;
					$$->return_type = EK_INT;
					if (exp_has_float($1,$3)) {
						$$->operator = OP_ORF;
					} 
					else {
						$$->operator = OP_ORI;
					}
				}
				| comp
				{
					$$ = $1;
				}
				;

comp 			: comp LESS_THAN arithexp
				{
					$$ = malloc(sizeof(struct expression));
					$$->kind 	  = EK_OP;
					$$->l_operand = $1;
					$$->r_operand = $3;
					$$->return_type = EK_INT;
					if (exp_has_float($1,$3)) {
						$$->operator = OP_LTF;
					} 
					else {
						$$->operator = OP_LTI;
					}
				}
				| comp LESS_THAN_EQUAL arithexp
				{
					$$ = malloc(sizeof(struct expression));
					$$->kind 	  = EK_OP;
					$$->l_operand = $1;
					$$->r_operand = $3;
					$$->return_type = EK_INT;
					if (exp_has_float($1,$3)) {
						$$->operator = OP_LEF;
					} 
					else {
						$$->operator = OP_LEI;
					}
				}
				| comp GREATER_THAN arithexp
				{
					$$ = malloc(sizeof(struct expression));
					$$->kind 	  = EK_OP;
					$$->l_operand = $1;
					$$->r_operand = $3;
					$$->return_type = EK_INT;
					if (exp_has_float($1,$3)) {
						$$->operator = OP_GTF;
					} 
					else {
						$$->operator = OP_GTI;
					}
				}
				| comp GREATER_THAN_EQUAL arithexp
				{
					$$ = malloc(sizeof(struct expression));
					$$->kind 	  = EK_OP;
					$$->l_operand = $1;
					$$->r_operand = $3;
					$$->return_type = EK_INT;
					if (exp_has_float($1,$3)) {
						$$->operator = OP_GEF;
					} 
					else {
						$$->operator = OP_GEI;
					}
				}
				| comp EQUAL arithexp
				{
					$$ = malloc(sizeof(struct expression));
					$$->kind 	  = EK_OP;
					$$->l_operand = $1;
					$$->r_operand = $3;
					$$->return_type = EK_INT;
					if (exp_has_float($1,$3)) {
						$$->operator = OP_EQF;
					} 
					else {
						$$->operator = OP_EQI;
					}
				}
				| comp NOT_EQUAL arithexp
				{
					$$ = malloc(sizeof(struct expression));
					$$->kind 	  = EK_OP;
					$$->l_operand = $1;
					$$->r_operand = $3;
					$$->return_type = EK_INT;
					if (exp_has_float($1,$3)) {
						$$->operator = OP_NEF;
					} 
					else {
						$$->operator = OP_NEI;
					}
				}
				| arithexp
				{
					$$ = $1;
				}
				;

arithexp 		: arithexp PLUS term
				{
					$$ = malloc(sizeof(struct expression));
					$$->kind 	  = EK_OP;
					$$->l_operand = $1;
					$$->r_operand = $3;
					if (exp_has_float($1,$3)) {
						$$->operator = OP_ADF;
						$$->return_type = EK_FLO;
					} 
					else {
						$$->operator = OP_ADI;
						$$->return_type = EK_INT;
					}
				}
				| arithexp MINUS term
				{
					$$ = malloc(sizeof(struct expression));
					$$->kind 	  = EK_OP;
					$$->l_operand = $1;
					$$->r_operand = $3;
					if (exp_has_float($1,$3)) {
						$$->operator = OP_SBF;
						$$->return_type = EK_FLO;
					} 
					else {
						$$->operator = OP_SBI;
						$$->return_type = EK_INT;
					}
				}
				| term
				{
					$$ = $1;
				}
				;

term			: term MULTIPLY fact
				{
					$$ = malloc(sizeof(struct expression));
					$$->kind 	  = EK_OP;
					$$->l_operand = $1;
					$$->r_operand = $3;
					if (exp_has_float($1,$3)) {
						$$->operator = OP_MLF;
						$$->return_type = EK_FLO;
					} 
					else {
						$$->operator = OP_MLI;
						$$->return_type = EK_INT;
					}
				}
				| term DIVIDE fact
				{
					$$ = malloc(sizeof(struct expression));
					$$->kind 	  = EK_OP;
					$$->l_operand = $1;
					$$->r_operand = $3;
					if (exp_has_float($1,$3)) {
						$$->operator = OP_DVF;
						$$->return_type = EK_FLO;
					} 
					else {
						$$->operator = OP_DVI;
						$$->return_type = EK_INT;
					}
				}
				| term MODULO fact
				{
					// see note on mod operator in main.c exprgen function
					$$ = malloc(sizeof(struct expression));
					$$->kind 	= EK_OP;
					$$->l_operand = $1;
					$$->r_operand = $3;
					$$->operator = OP_RMI;
					$$->return_type = EK_INT;
				}
				| fact
				{
					$$ = $1;
				}
				;

fact			: MINUS unit
				{
					$$ = malloc(sizeof(struct expression));
					$$->kind 	  = EK_OP;
					$$->l_operand = NULL;
					$$->r_operand = $2;
					if ($2->kind == EK_FLO || $2->return_type == EK_FLO) {
						$$->operator = OP_NGF;
						$$->return_type = EK_FLO;
					} 
					else {
						$$->operator = OP_NGI;
						$$->return_type = EK_INT;
					}
				}
				| BOOL_NOT unit
				{
					$$ = malloc(sizeof(struct expression));
					$$->kind 	  = EK_OP;
					$$->l_operand = NULL;
					$$->r_operand = $2;
					$$->return_type = EK_INT;
					if ($2->kind == EK_FLO || $2->return_type == EK_FLO) {
						$$->operator = OP_NTF;
					} 
					else {
						$$->operator = OP_NTI;
					}
				}
				| unit
				{
					$$ = $1;
				}
				;

unit			: OPEN_PARENTHESIS exp CLOSE_PARENTHESIS
				{
					$$ = $2;
				}
				| FLOAT_CONSTANT
				{
					$$ = malloc(sizeof(struct expression));
					$$->kind 	  = EK_FLO;
					$$->var = 0;
					$$->float_value = $1;
				}
				| INTEGER_CONSTANT
				{
					$$ = malloc(sizeof(struct expression));
					$$->kind 	  = EK_INT;
					$$->var = 0;
					$$->int_value = $1;
				}
				| VARIABLE_NAME
				{
					$$ = malloc(sizeof(struct expression));
					$$->src_base_address = addressofST(st,$1);
					$$->var = 1;
					$$->kind = st->entry[lookupSTbyAddress(st,$$->src_base_address)].dataType;
					$$->index_exp = NULL;
				}
				| VARIABLE_NAME OPEN_BRACKET exp CLOSE_BRACKET
				{
					$$ = malloc(sizeof(struct expression));
					$$->src_base_address = addressofST(st,$1);
					$$->var = 1;
					$$->kind = st->entry[lookupSTbyAddress(st,$$->src_base_address)].dataType;
					$$->index_exp = $3;
				}
				;

%%
char exp_has_float(struct expression * l, struct expression * r) {
	return (l->return_type == EK_FLO || l->kind == EK_FLO ||  r->return_type == EK_FLO || r->kind == EK_FLO);
}
int yyerror() {
   printf("Called yyerror()\n");
   return  0;
}
