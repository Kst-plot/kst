%option noyywrap
%x DATA
%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "eparse.h"
#include "eparse-eh.h"

int bracketCount = 0;
char dataSpec[1024];
char *dataSpecPtr;
//void yyerror(Kst::ObjectStore *store, const char *s);
#define YY_DECL int yylex(Kst::ObjectStore *store)

%}

Number	(0|[1-9][0-9]*)([\.][0-9]+)?([eE][\+\-]?[0-9]+)?
Id	[A-Za-z]+

%%

{Number}	{
			yylval.number = atof(yytext);
			/*printf("Found a number %.15f\n", yylval.number);*/
			return T_NUMBER;
		}


{Id}		{
			yylval.data = strdup(yytext);
			/*printf("Found an ID [%s]\n", yylval.data);*/
			return T_IDENTIFIER;
		}

<DATA>.		{
			switch (*yytext) {
				case '[':
					++bracketCount;
					*dataSpecPtr = *yytext;
					++dataSpecPtr;
					break;
				case ']':
					if (--bracketCount == 0) {
						*dataSpecPtr = 0;
						yylval.data = strdup(dataSpec);
						if (dataSpecPtr == dataSpec) {
							yyerror(store, "Empty data reference.");
							yy_flush_buffer(YY_CURRENT_BUFFER);
							BEGIN(INITIAL);
							return T_INVALID;
						}
						BEGIN(INITIAL);
						return T_DATA;
					}
					*dataSpecPtr = *yytext;
					++dataSpecPtr;
					break;
				default:
					*dataSpecPtr = *yytext;
					++dataSpecPtr;
					break;
			}
		}

<DATA><<EOF>>	{
			yyerror(store, "Invalid data reference.");
			yy_flush_buffer(YY_CURRENT_BUFFER);
			BEGIN(INITIAL);
			return T_INVALID;
		}

"["		{
			bracketCount = 1;
			dataSpecPtr = dataSpec;
			*dataSpecPtr = 0;
			BEGIN(DATA);
		}

"]"		{
			yyerror(store, "Unmatched ']'.");
			yy_flush_buffer(YY_CURRENT_BUFFER);
			return T_INVALID;
		}

"!"		return T_NOT;

"&"		return T_AND;

"|"		return T_OR;

"&&"		return T_LAND;

"||"		return T_LOR;

"<"		return T_LT;

"<="		return T_LE;

">"		return T_GT;

">="		return T_GE;

"="		return T_EQ;

"=="		return T_EQ;

"!="		return T_NE;

"+"		return T_ADD;

"-"		return T_SUBTRACT;

"*"		return T_MULTIPLY;

"/"		return T_DIVIDE;
 
"%"		return T_MOD;
 
"^"		return T_EXP;
 
"("		return T_OPENPAR;
 
")"		return T_CLOSEPAR;
 
","		return T_COMMA;
 
[ \t]		{}

.		{
			/*printf("Found char '%c'\n", yytext[0]);*/
			yylval.character = yytext[0];
			return T_INVALID;
		}


<<EOF>>		{
			/*printf("yyterminate\n");*/
			yyterminate();
		}

%%

