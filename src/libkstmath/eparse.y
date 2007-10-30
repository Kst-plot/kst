
%{
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <objectstore.h>

#include "enodefactory.h"

#include "eparse-eh.h"
extern int yylex(Kst::ObjectStore *store);
void *ParsedEquation = 0L;

%}

%parse-param { Kst::ObjectStore *store }
%lex-param { Kst::ObjectStore *store }

%union {
		char *data;
		double number;
		void *n; /* tree node */
		char character;
	   }


%token T_NUMBER T_IDENTIFIER T_DATA T_OPENPAR T_CLOSEPAR T_COMMA T_INVALID

%left <operator> T_LOR
%left <operator> T_LAND
%left <operator> T_OR
%left <operator> T_AND
%left <operator> T_EQ T_NE
%left <operator> T_LT T_LE T_GT T_GE
%left <operator> T_ADD T_SUBTRACT
%left <operator> T_MULTIPLY T_DIVIDE T_MOD
%left <operator> T_NOT
%nonassoc U_SUBTRACT
%right <operator> T_EXP

%start WRAPPER

%%

WRAPPER		:	{ $<n>$ = 0L; yyClearErrors(); ParsedEquation = 0L; } PRESTART
			{ $<n>$ = ParsedEquation = $<n>2;
				if (yyErrorCount() > 0) {
					DeleteNode($<n>$);
					$<n>$ = 0L;
					ParsedEquation = 0L;
					YYERROR;
				}
			}
		;

PRESTART	:	START
			{ $<n>$ = $<n>1; }
		|	/**/
			{ $<n>$ = 0L; yyerror(store, EParseErrorEmpty); }
		;

START		:	BOOLEAN_OR
			{ $<n>$ = $<n>1; }
		;

BOOLEAN_OR	:	BOOLEAN_OR T_LOR BOOLEAN_AND
			{ $<n>$ = NewLogicalOr($<n>1, $<n>3); }
		|	T_LOR error
			{ yyerror(store, EParseErrorTwoOperands); $<n>$ = 0L; }
		|	BOOLEAN_AND
			{ $<n>$ = $<n>1; }
		;

BOOLEAN_AND	:	BOOLEAN_AND T_LAND COMPARISON
			{ $<n>$ = NewLogicalAnd($<n>1, $<n>3); }
		|	T_LAND error
			{ yyerror(store, EParseErrorTwoOperands); $<n>$ = 0L; }
		|	COMPARISON
			{ $<n>$ = $<n>1; }
		;

COMPARISON	:	COMPARISON T_LT EQUATION
			{ $<n>$ = NewLessThan($<n>1, $<n>3); }
		|	COMPARISON T_LE EQUATION
			{ $<n>$ = NewLessThanEqual($<n>1, $<n>3); }
		|	COMPARISON T_GT EQUATION
			{ $<n>$ = NewGreaterThan($<n>1, $<n>3); }
		|	COMPARISON T_GE EQUATION
			{ $<n>$ = NewGreaterThanEqual($<n>1, $<n>3); }
		|	COMPARISON T_EQ EQUATION
			{ $<n>$ = NewEqualTo($<n>1, $<n>3); }
		|	COMPARISON T_NE EQUATION
			{ $<n>$ = NewNotEqualTo($<n>1, $<n>3); }
		|	T_LT error
			{ yyerror(store, EParseErrorTwoOperands); $<n>$ = 0L; }
		|	T_GT error
			{ yyerror(store, EParseErrorTwoOperands); $<n>$ = 0L; }
		|	T_LE error
			{ yyerror(store, EParseErrorTwoOperands); $<n>$ = 0L; }
		|	T_GE error
			{ yyerror(store, EParseErrorTwoOperands); $<n>$ = 0L; }
		|	T_EQ error
			{ yyerror(store, EParseErrorTwoOperands); $<n>$ = 0L; }
		|	T_NE error
			{ yyerror(store, EParseErrorTwoOperands); $<n>$ = 0L; }
		|	EQUATION
			{ $<n>$ = $<n>1; }
		;

EQUATION	:	EQUATION T_ADD TERM
			{ $<n>$ = NewAddition($<n>1, $<n>3); }
		|	EQUATION T_SUBTRACT TERM 
			{ $<n>$ = NewSubtraction($<n>1, $<n>3); }
		|	EQUATION T_OR TERM 
			{ $<n>$ = NewBitwiseOr($<n>1, $<n>3); }
		|	EQUATION T_AND TERM 
			{ $<n>$ = NewBitwiseAnd($<n>1, $<n>3); }
		|	TERM
			{ $<n>$ = $<n>1; }
		|	T_OR error
			{ yyerror(store, EParseErrorTwoOperands); $<n>$ = 0L; }
		|	T_AND error
			{ yyerror(store, EParseErrorTwoOperands); $<n>$ = 0L; }
		;

TERM		:	TERM T_MULTIPLY NEG
			{ $<n>$ = NewMultiplication($<n>1, $<n>3); }
		|	TERM T_DIVIDE NEG
			{ $<n>$ = NewDivision($<n>1, $<n>3); }
		|	TERM T_MOD NEG
			{ $<n>$ = NewModulo($<n>1, $<n>3); }
		|	T_MULTIPLY error
			{ yyerror(store, EParseErrorTwoOperands); $<n>$ = 0L; }
		|	T_DIVIDE error
			{ yyerror(store, EParseErrorTwoOperands); $<n>$ = 0L; }
		|	T_MOD error
			{ yyerror(store, EParseErrorTwoOperands); $<n>$ = 0L; }
		|	NEG
			{ $<n>$ = $<n>1; }
		;

NEG		:	T_SUBTRACT NEG %prec U_SUBTRACT
			{ $<n>$ = NewNegation($<n>2); }
		|       T_NOT NEG
			{ $<n>$ = NewNot($<n>2); }
		|       T_NOT error
			{ $<n>$ = 0L; yyerror(store, EParseErrorRequiresOperand); }
		|	EXP
			{ $<n>$ = $<n>1; }
		;

EXP		:	EXP T_EXP EXP
			{ $<n>$ = NewPower($<n>1, $<n>3); }
		|	EXP T_EXP error
			{ DeleteNode($<n>1); $<n>$ = 0L; yyerror(store, EParseErrorTwoOperands); }
		|	EXP T_EXP /**/
			{ DeleteNode($<n>1); $<n>$ = 0L; yyerror(store, EParseErrorTwoOperands); }
		|	T_EXP error
			{ $<n>$ = 0L; yyerror(store, EParseErrorTwoOperands); }
		|	ATOMIC
			{ $<n>$ = $<n>1; }
		;

ATOMIC		:	T_OPENPAR BOOLEAN_OR T_CLOSEPAR
			{ $<n>$ = $<n>2; ParenthesizeNode($<n>$); }
		|	T_OPENPAR error
			{ yyerror(store, EParseErrorMissingClosingParenthesis); $<n>$ = 0L; }
		|	T_IDENTIFIER
			{ $<n>$ = NewIdentifier($<data>1); }
		|	T_DATA
			{ $<n>$ = NewData(store, $<data>1); }
		|	T_IDENTIFIER T_OPENPAR T_CLOSEPAR error
			{ yyerror(store, EParseErrorNoImplicitMultiply); free($<data>1); $<n>$ = 0L; }
		|	T_IDENTIFIER T_OPENPAR T_CLOSEPAR
			{ $<n>$ = NewFunction($<data>1, NewArgumentList()); }
/*		|	T_IDENTIFIER T_OPENPAR ARGUMENTS error
			{ yyerror(store, EParseErrorMissingClosingParenthesis); DeleteNode($<n>3); free($<data>1); $<n>$ = 0L; }
*/
		|	T_IDENTIFIER T_OPENPAR ARGUMENTS T_CLOSEPAR error
			{ yyerror(store, EParseErrorNoImplicitMultiply); DeleteNode($<n>3); free($<data>1); $<n>$ = 0L; }
		|	T_IDENTIFIER T_OPENPAR ARGUMENTS T_CLOSEPAR
			{ $<n>$ = NewFunction($<data>1, $<n>3); }
		|	T_IDENTIFIER T_OPENPAR error
			{ yyerror(store, EParseErrorMissingClosingParenthesis); free($<data>1); $<n>$ = 0L; }
		|	T_NUMBER
			{ $<n>$ = NewNumber($<number>1); }
		|	T_NUMBER error
			{ yyerror(store, EParseErrorNoImplicitMultiply); $<n>$ = 0L; }
		|	T_INVALID
			{ yyerrortoken($<character>1); $<n>$ = 0L; }
		|	T_OPENPAR T_CLOSEPAR
			{ yyerror(store, EParseErrorEmptyParentheses); $<n>$ = 0L; }
		;

ARGUMENTS	:	ARGLIST
			{ $<n>$ = $<n>1; }
		;

ARGLIST		:	ARGLIST T_COMMA ARGUMENT
			{ if ($<n>1 && $<n>3) { AppendArgument($<n>1, $<n>3); } else { DeleteNode($<n>1); DeleteNode($<n>3); $<n>1 = 0L; } $<n>$ = $<n>1; }
		|	ARGUMENT
			{ if ($<n>1) { $<n>$ = NewArgumentList(); AppendArgument($<n>$, $<n>1); } else { $<n>$ = 0L; } }
		|	ARGLIST T_COMMA error
			{ $<n>$ = 0L; DeleteNode($<n>1); yyerror(store, EParseErrorEmptyArg); }
		|	{} /**/ T_COMMA ARGUMENT
			{ yyerror(store, EParseErrorEmptyArg); DeleteNode($<n>3); $<n>$ = 0L; }
		;

ARGUMENT	:	START
			{ $<n>$ = $<n>1; }
		;

%%

