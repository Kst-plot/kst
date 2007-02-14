/* A Bison parser, made by GNU Bison 1.875.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_NUMBER = 258,
     T_IDENTIFIER = 259,
     T_DATA = 260,
     T_OPENPAR = 261,
     T_CLOSEPAR = 262,
     T_COMMA = 263,
     T_INVALID = 264,
     T_LOR = 265,
     T_LAND = 266,
     T_OR = 267,
     T_AND = 268,
     T_NE = 269,
     T_EQ = 270,
     T_GE = 271,
     T_GT = 272,
     T_LE = 273,
     T_LT = 274,
     T_SUBTRACT = 275,
     T_ADD = 276,
     T_MOD = 277,
     T_DIVIDE = 278,
     T_MULTIPLY = 279,
     T_NOT = 280,
     U_SUBTRACT = 281,
     T_EXP = 282
   };
#endif
#define T_NUMBER 258
#define T_IDENTIFIER 259
#define T_DATA 260
#define T_OPENPAR 261
#define T_CLOSEPAR 262
#define T_COMMA 263
#define T_INVALID 264
#define T_LOR 265
#define T_LAND 266
#define T_OR 267
#define T_AND 268
#define T_NE 269
#define T_EQ 270
#define T_GE 271
#define T_GT 272
#define T_LE 273
#define T_LT 274
#define T_SUBTRACT 275
#define T_ADD 276
#define T_MOD 277
#define T_DIVIDE 278
#define T_MULTIPLY 279
#define T_NOT 280
#define U_SUBTRACT 281
#define T_EXP 282




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 15 "eparse.y"
typedef union YYSTYPE {
        char *data;
        double number;
	void *n; /* tree node */
        char character;
       } YYSTYPE;
/* Line 1249 of yacc.c.  */
#line 97 "eparse.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



