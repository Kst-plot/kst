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

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */
#line 2 "eparse.y"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "enodefactory.h"

#include "eparse-eh.h"
extern int yylex();
void *ParsedEquation = 0L;



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 15 "eparse.y"
typedef union YYSTYPE {
        char *data;
        double number;
	void *n; /* tree node */
        char character;
       } YYSTYPE;
/* Line 191 of yacc.c.  */
#line 149 "eparse.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 161 "eparse.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   360

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  28
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  17
/* YYNRULES -- Number of rules. */
#define YYNRULES  68
/* YYNRULES -- Number of states. */
#define YYNSTATES  106

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   282

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    10,    12,    16,    19,
      21,    25,    28,    30,    34,    38,    42,    46,    50,    54,
      57,    60,    63,    66,    69,    72,    74,    78,    82,    86,
      90,    92,    95,    98,   102,   106,   110,   113,   116,   119,
     121,   124,   127,   130,   132,   136,   140,   143,   146,   148,
     152,   155,   157,   159,   164,   168,   174,   179,   183,   185,
     188,   190,   193,   195,   199,   201,   205,   206,   210
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      29,     0,    -1,    -1,    30,    31,    -1,    32,    -1,    -1,
      33,    -1,    33,    10,    34,    -1,    10,     1,    -1,    34,
      -1,    34,    11,    35,    -1,    11,     1,    -1,    35,    -1,
      35,    19,    36,    -1,    35,    18,    36,    -1,    35,    17,
      36,    -1,    35,    16,    36,    -1,    35,    15,    36,    -1,
      35,    14,    36,    -1,    19,     1,    -1,    17,     1,    -1,
      18,     1,    -1,    16,     1,    -1,    15,     1,    -1,    14,
       1,    -1,    36,    -1,    36,    21,    37,    -1,    36,    20,
      37,    -1,    36,    12,    37,    -1,    36,    13,    37,    -1,
      37,    -1,    12,     1,    -1,    13,     1,    -1,    37,    24,
      38,    -1,    37,    23,    38,    -1,    37,    22,    38,    -1,
      24,     1,    -1,    23,     1,    -1,    22,     1,    -1,    38,
      -1,    20,    38,    -1,    25,    38,    -1,    25,     1,    -1,
      39,    -1,    39,    27,    39,    -1,    39,    27,     1,    -1,
      39,    27,    -1,    27,     1,    -1,    40,    -1,     6,    33,
       7,    -1,     6,     1,    -1,     4,    -1,     5,    -1,     4,
       6,     7,     1,    -1,     4,     6,     7,    -1,     4,     6,
      41,     7,     1,    -1,     4,     6,    41,     7,    -1,     4,
       6,     1,    -1,     3,    -1,     3,     1,    -1,     9,    -1,
       6,     7,    -1,    42,    -1,    42,     8,    44,    -1,    44,
      -1,    42,     8,     1,    -1,    -1,    43,     8,    44,    -1,
      32,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned char yyrline[] =
{
       0,    41,    41,    41,    52,    55,    58,    62,    64,    66,
      70,    72,    74,    78,    80,    82,    84,    86,    88,    90,
      92,    94,    96,    98,   100,   102,   106,   108,   110,   112,
     114,   116,   118,   122,   124,   126,   128,   130,   132,   134,
     138,   140,   142,   144,   148,   150,   152,   154,   156,   160,
     162,   164,   166,   168,   170,   175,   177,   179,   181,   183,
     185,   187,   191,   195,   197,   199,   201,   201,   205
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_NUMBER", "T_IDENTIFIER", "T_DATA", 
  "T_OPENPAR", "T_CLOSEPAR", "T_COMMA", "T_INVALID", "T_LOR", "T_LAND", 
  "T_OR", "T_AND", "T_NE", "T_EQ", "T_GE", "T_GT", "T_LE", "T_LT", 
  "T_SUBTRACT", "T_ADD", "T_MOD", "T_DIVIDE", "T_MULTIPLY", "T_NOT", 
  "U_SUBTRACT", "T_EXP", "$accept", "WRAPPER", "@1", "PRESTART", "START", 
  "BOOLEAN_OR", "BOOLEAN_AND", "COMPARISON", "EQUATION", "TERM", "NEG", 
  "EXP", "ATOMIC", "ARGUMENTS", "ARGLIST", "@2", "ARGUMENT", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    28,    30,    29,    31,    31,    32,    33,    33,    33,
      34,    34,    34,    35,    35,    35,    35,    35,    35,    35,
      35,    35,    35,    35,    35,    35,    36,    36,    36,    36,
      36,    36,    36,    37,    37,    37,    37,    37,    37,    37,
      38,    38,    38,    38,    39,    39,    39,    39,    39,    40,
      40,    40,    40,    40,    40,    40,    40,    40,    40,    40,
      40,    40,    41,    42,    42,    42,    43,    42,    44
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     1,     0,     1,     3,     2,     1,
       3,     2,     1,     3,     3,     3,     3,     3,     3,     2,
       2,     2,     2,     2,     2,     1,     3,     3,     3,     3,
       1,     2,     2,     3,     3,     3,     2,     2,     2,     1,
       2,     2,     2,     1,     3,     3,     2,     2,     1,     3,
       2,     1,     1,     4,     3,     5,     4,     3,     1,     2,
       1,     2,     1,     3,     1,     3,     0,     3,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       2,     0,     5,     1,     0,    51,    52,     0,    60,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     3,     4,     6,     9,    12,
      25,    30,    39,    43,    48,    59,     0,    50,    61,     0,
       8,    11,    31,    32,    24,    23,    22,    20,    21,    19,
      40,    38,    37,    36,    42,    41,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    57,     0,    68,     0,    62,     0,    64,
      49,     7,    10,    18,    17,    16,    15,    14,    13,    28,
      29,    27,    26,    35,    34,    33,    45,    44,    53,     0,
       0,     0,    55,    65,    63,    67
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,     1,     2,    25,    75,    27,    28,    29,    30,    31,
      32,    33,    34,    76,    77,    78,    79
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -58
static const short yypact[] =
{
     -58,     1,   221,   -58,    46,    28,   -58,   146,   -58,    48,
      54,    76,    79,    98,    99,   100,   101,   104,   120,   333,
     141,   153,   166,   196,   177,   -58,   -58,   169,   181,    23,
      15,    52,   -58,   176,   -58,   -58,   121,   -58,   -58,    38,
     -58,   -58,   -58,   -58,   -58,   -58,   -58,   -58,   -58,   -58,
     -58,   -58,   -58,   -58,   -58,   -58,   -58,   246,   271,   296,
     296,   296,   296,   296,   296,   308,   308,   308,   308,   333,
     333,   333,     2,   -58,    71,   -58,   197,   204,   205,   -58,
     -58,   181,    23,    15,    15,    15,    15,    15,    15,    52,
      52,    52,    52,   -58,   -58,   -58,   -58,   176,   -58,    96,
     171,   221,   -58,   -58,   -58,   -58
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
     -58,   -58,   -58,   -58,   212,   208,   160,   161,   147,   -35,
     -19,   148,   -58,   -58,   -58,   -58,   -57
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -67
static const yysigned_char yytable[] =
{
      50,     3,   -46,    96,    55,     4,     5,     6,     7,   -46,
     -46,     8,   -46,   -46,   -46,   -46,   -46,   -46,   -46,   -46,
     -46,   -46,   -46,   -46,   -46,   -46,   -46,    65,    66,    24,
      89,    90,    91,    92,    36,    67,    68,    59,    60,    61,
      62,    63,    64,   104,   105,    80,   -58,    35,    57,    40,
      93,    94,    95,   -58,   -58,    41,   -58,   -58,   -58,   -58,
     -58,   -58,   -58,   -58,   -58,   -58,   -58,   -58,   -58,   -58,
     -58,   -54,    98,   -58,    69,    70,    71,    42,   -54,   -54,
      43,   -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,
     -54,   -54,   -54,   -54,   -54,   -54,   -56,   102,   -54,    44,
      45,    46,    47,   -56,   -56,    48,   -56,   -56,   -56,   -56,
     -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,
     -56,    49,    73,   -56,     4,     5,     6,     7,    74,   -66,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    51,    20,    21,    22,    23,    37,    24,     4,
       5,     6,     7,    38,    52,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    53,    20,    21,
      22,    23,   103,    24,     4,     5,     6,     7,    56,    57,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    58,    20,    21,    22,    23,    54,    24,     4,
       5,     6,     7,    72,    99,     8,    83,    84,    85,    86,
      87,    88,   100,   101,    26,    39,    19,    81,     0,    82,
      97,    23,     0,    24,     4,     5,     6,     7,     0,     0,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,     0,    20,    21,    22,    23,     0,    24,     4,
       5,     6,     7,     0,     0,     8,     0,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,     0,    20,    21,
      22,    23,     0,    24,     4,     5,     6,     7,     0,     0,
       8,     0,     0,    11,    12,    13,    14,    15,    16,    17,
      18,    19,     0,    20,    21,    22,    23,     0,    24,     4,
       5,     6,     7,     0,     0,     8,     0,     0,    11,    12,
       0,     4,     5,     6,     7,     0,    19,     8,    20,    21,
      22,    23,     0,    24,     0,     0,     0,     0,    19,     0,
      20,    21,    22,    23,     0,    24,     4,     5,     6,     7,
       0,     0,     8,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    19,     0,     0,     0,     0,    23,     0,
      24
};

static const yysigned_char yycheck[] =
{
      19,     0,     0,     1,    23,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    12,    13,    27,
      65,    66,    67,    68,     6,    20,    21,    14,    15,    16,
      17,    18,    19,   100,   101,     7,     0,     1,    10,     1,
      69,    70,    71,     7,     8,     1,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,     0,     1,    27,    22,    23,    24,     1,     7,     8,
       1,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,     0,     1,    27,     1,
       1,     1,     1,     7,     8,     1,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,     1,     1,    27,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,     1,    22,    23,    24,    25,     1,    27,     3,
       4,     5,     6,     7,     1,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,     1,    22,    23,
      24,    25,     1,    27,     3,     4,     5,     6,     1,    10,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    11,    22,    23,    24,    25,     1,    27,     3,
       4,     5,     6,    27,     7,     9,    59,    60,    61,    62,
      63,    64,     8,     8,     2,     7,    20,    57,    -1,    58,
      72,    25,    -1,    27,     3,     4,     5,     6,    -1,    -1,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    -1,    22,    23,    24,    25,    -1,    27,     3,
       4,     5,     6,    -1,    -1,     9,    -1,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    -1,    22,    23,
      24,    25,    -1,    27,     3,     4,     5,     6,    -1,    -1,
       9,    -1,    -1,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    -1,    22,    23,    24,    25,    -1,    27,     3,
       4,     5,     6,    -1,    -1,     9,    -1,    -1,    12,    13,
      -1,     3,     4,     5,     6,    -1,    20,     9,    22,    23,
      24,    25,    -1,    27,    -1,    -1,    -1,    -1,    20,    -1,
      22,    23,    24,    25,    -1,    27,     3,     4,     5,     6,
      -1,    -1,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    20,    -1,    -1,    -1,    -1,    25,    -1,
      27
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    29,    30,     0,     3,     4,     5,     6,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      22,    23,    24,    25,    27,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,     1,     6,     1,     7,    33,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
      38,     1,     1,     1,     1,    38,     1,    10,    11,    14,
      15,    16,    17,    18,    19,    12,    13,    20,    21,    22,
      23,    24,    27,     1,     7,    32,    41,    42,    43,    44,
       7,    34,    35,    36,    36,    36,    36,    36,    36,    37,
      37,    37,    37,    38,    38,    38,     1,    39,     1,     7,
       8,     8,     1,     1,    44,    44
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1

/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (cinluded).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylineno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylineno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 41 "eparse.y"
    { yyval.n = 0L; yyClearErrors(); ParsedEquation = 0L; }
    break;

  case 3:
#line 42 "eparse.y"
    { yyval.n = ParsedEquation = yyvsp[0].n;
				if (yyErrorCount() > 0) {
					DeleteNode(yyval.n);
					yyval.n = 0L;
					ParsedEquation = 0L;
					YYERROR;
				}
			}
    break;

  case 4:
#line 53 "eparse.y"
    { yyval.n = yyvsp[0].n; }
    break;

  case 5:
#line 55 "eparse.y"
    { yyval.n = 0L; yyerror(EParseErrorEmpty); }
    break;

  case 6:
#line 59 "eparse.y"
    { yyval.n = yyvsp[0].n; }
    break;

  case 7:
#line 63 "eparse.y"
    { yyval.n = NewLogicalOr(yyvsp[-2].n, yyvsp[0].n); }
    break;

  case 8:
#line 65 "eparse.y"
    { yyerror(EParseErrorTwoOperands); yyval.n = 0L; }
    break;

  case 9:
#line 67 "eparse.y"
    { yyval.n = yyvsp[0].n; }
    break;

  case 10:
#line 71 "eparse.y"
    { yyval.n = NewLogicalAnd(yyvsp[-2].n, yyvsp[0].n); }
    break;

  case 11:
#line 73 "eparse.y"
    { yyerror(EParseErrorTwoOperands); yyval.n = 0L; }
    break;

  case 12:
#line 75 "eparse.y"
    { yyval.n = yyvsp[0].n; }
    break;

  case 13:
#line 79 "eparse.y"
    { yyval.n = NewLessThan(yyvsp[-2].n, yyvsp[0].n); }
    break;

  case 14:
#line 81 "eparse.y"
    { yyval.n = NewLessThanEqual(yyvsp[-2].n, yyvsp[0].n); }
    break;

  case 15:
#line 83 "eparse.y"
    { yyval.n = NewGreaterThan(yyvsp[-2].n, yyvsp[0].n); }
    break;

  case 16:
#line 85 "eparse.y"
    { yyval.n = NewGreaterThanEqual(yyvsp[-2].n, yyvsp[0].n); }
    break;

  case 17:
#line 87 "eparse.y"
    { yyval.n = NewEqualTo(yyvsp[-2].n, yyvsp[0].n); }
    break;

  case 18:
#line 89 "eparse.y"
    { yyval.n = NewNotEqualTo(yyvsp[-2].n, yyvsp[0].n); }
    break;

  case 19:
#line 91 "eparse.y"
    { yyerror(EParseErrorTwoOperands); yyval.n = 0L; }
    break;

  case 20:
#line 93 "eparse.y"
    { yyerror(EParseErrorTwoOperands); yyval.n = 0L; }
    break;

  case 21:
#line 95 "eparse.y"
    { yyerror(EParseErrorTwoOperands); yyval.n = 0L; }
    break;

  case 22:
#line 97 "eparse.y"
    { yyerror(EParseErrorTwoOperands); yyval.n = 0L; }
    break;

  case 23:
#line 99 "eparse.y"
    { yyerror(EParseErrorTwoOperands); yyval.n = 0L; }
    break;

  case 24:
#line 101 "eparse.y"
    { yyerror(EParseErrorTwoOperands); yyval.n = 0L; }
    break;

  case 25:
#line 103 "eparse.y"
    { yyval.n = yyvsp[0].n; }
    break;

  case 26:
#line 107 "eparse.y"
    { yyval.n = NewAddition(yyvsp[-2].n, yyvsp[0].n); }
    break;

  case 27:
#line 109 "eparse.y"
    { yyval.n = NewSubtraction(yyvsp[-2].n, yyvsp[0].n); }
    break;

  case 28:
#line 111 "eparse.y"
    { yyval.n = NewBitwiseOr(yyvsp[-2].n, yyvsp[0].n); }
    break;

  case 29:
#line 113 "eparse.y"
    { yyval.n = NewBitwiseAnd(yyvsp[-2].n, yyvsp[0].n); }
    break;

  case 30:
#line 115 "eparse.y"
    { yyval.n = yyvsp[0].n; }
    break;

  case 31:
#line 117 "eparse.y"
    { yyerror(EParseErrorTwoOperands); yyval.n = 0L; }
    break;

  case 32:
#line 119 "eparse.y"
    { yyerror(EParseErrorTwoOperands); yyval.n = 0L; }
    break;

  case 33:
#line 123 "eparse.y"
    { yyval.n = NewMultiplication(yyvsp[-2].n, yyvsp[0].n); }
    break;

  case 34:
#line 125 "eparse.y"
    { yyval.n = NewDivision(yyvsp[-2].n, yyvsp[0].n); }
    break;

  case 35:
#line 127 "eparse.y"
    { yyval.n = NewModulo(yyvsp[-2].n, yyvsp[0].n); }
    break;

  case 36:
#line 129 "eparse.y"
    { yyerror(EParseErrorTwoOperands); yyval.n = 0L; }
    break;

  case 37:
#line 131 "eparse.y"
    { yyerror(EParseErrorTwoOperands); yyval.n = 0L; }
    break;

  case 38:
#line 133 "eparse.y"
    { yyerror(EParseErrorTwoOperands); yyval.n = 0L; }
    break;

  case 39:
#line 135 "eparse.y"
    { yyval.n = yyvsp[0].n; }
    break;

  case 40:
#line 139 "eparse.y"
    { yyval.n = NewNegation(yyvsp[0].n); }
    break;

  case 41:
#line 141 "eparse.y"
    { yyval.n = NewNot(yyvsp[0].n); }
    break;

  case 42:
#line 143 "eparse.y"
    { yyval.n = 0L; yyerror(EParseErrorRequiresOperand); }
    break;

  case 43:
#line 145 "eparse.y"
    { yyval.n = yyvsp[0].n; }
    break;

  case 44:
#line 149 "eparse.y"
    { yyval.n = NewPower(yyvsp[-2].n, yyvsp[0].n); }
    break;

  case 45:
#line 151 "eparse.y"
    { DeleteNode(yyvsp[-2].n); yyval.n = 0L; yyerror(EParseErrorTwoOperands); }
    break;

  case 46:
#line 153 "eparse.y"
    { DeleteNode(yyvsp[-1].n); yyval.n = 0L; yyerror(EParseErrorTwoOperands); }
    break;

  case 47:
#line 155 "eparse.y"
    { yyval.n = 0L; yyerror(EParseErrorTwoOperands); }
    break;

  case 48:
#line 157 "eparse.y"
    { yyval.n = yyvsp[0].n; }
    break;

  case 49:
#line 161 "eparse.y"
    { yyval.n = yyvsp[-1].n; ParenthesizeNode(yyval.n); }
    break;

  case 50:
#line 163 "eparse.y"
    { yyerror(EParseErrorMissingClosingParenthesis); yyval.n = 0L; }
    break;

  case 51:
#line 165 "eparse.y"
    { yyval.n = NewIdentifier(yyvsp[0].data); }
    break;

  case 52:
#line 167 "eparse.y"
    { yyval.n = NewData(yyvsp[0].data); }
    break;

  case 53:
#line 169 "eparse.y"
    { yyerror(EParseErrorNoImplicitMultiply); free(yyvsp[-3].data); yyval.n = 0L; }
    break;

  case 54:
#line 171 "eparse.y"
    { yyval.n = NewFunction(yyvsp[-2].data, NewArgumentList()); }
    break;

  case 55:
#line 176 "eparse.y"
    { yyerror(EParseErrorNoImplicitMultiply); DeleteNode(yyvsp[-2].n); free(yyvsp[-4].data); yyval.n = 0L; }
    break;

  case 56:
#line 178 "eparse.y"
    { yyval.n = NewFunction(yyvsp[-3].data, yyvsp[-1].n); }
    break;

  case 57:
#line 180 "eparse.y"
    { yyerror(EParseErrorMissingClosingParenthesis); free(yyvsp[-2].data); yyval.n = 0L; }
    break;

  case 58:
#line 182 "eparse.y"
    { yyval.n = NewNumber(yyvsp[0].number); }
    break;

  case 59:
#line 184 "eparse.y"
    { yyerror(EParseErrorNoImplicitMultiply); yyval.n = 0L; }
    break;

  case 60:
#line 186 "eparse.y"
    { yyerrortoken(yyvsp[0].character); yyval.n = 0L; }
    break;

  case 61:
#line 188 "eparse.y"
    { yyerror(EParseErrorEmptyParentheses); yyval.n = 0L; }
    break;

  case 62:
#line 192 "eparse.y"
    { yyval.n = yyvsp[0].n; }
    break;

  case 63:
#line 196 "eparse.y"
    { if (yyvsp[-2].n && yyvsp[0].n) { AppendArgument(yyvsp[-2].n, yyvsp[0].n); } else { DeleteNode(yyvsp[-2].n); DeleteNode(yyvsp[0].n); yyvsp[-2].n = 0L; } yyval.n = yyvsp[-2].n; }
    break;

  case 64:
#line 198 "eparse.y"
    { if (yyvsp[0].n) { yyval.n = NewArgumentList(); AppendArgument(yyval.n, yyvsp[0].n); } else { yyval.n = 0L; } }
    break;

  case 65:
#line 200 "eparse.y"
    { yyval.n = 0L; DeleteNode(yyvsp[-2].n); yyerror(EParseErrorEmptyArg); }
    break;

  case 66:
#line 201 "eparse.y"
    {}
    break;

  case 67:
#line 202 "eparse.y"
    { yyerror(EParseErrorEmptyArg); DeleteNode(yyvsp[0].n); yyval.n = 0L; }
    break;

  case 68:
#line 206 "eparse.y"
    { yyval.n = yyvsp[0].n; }
    break;


    }

/* Line 991 of yacc.c.  */
#line 1522 "eparse.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("syntax error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval);
      yychar = YYEMPTY;

    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab2;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:

  /* Suppress GCC warning that yyerrlab1 is unused when no action
     invokes YYERROR.  */
#if defined (__GNUC_MINOR__) && 2093 <= (__GNUC__ * 1000 + __GNUC_MINOR__) \
    && !defined __cplusplus
  __attribute__ ((__unused__))
#endif


  goto yyerrlab2;


/*---------------------------------------------------------------.
| yyerrlab2 -- pop states until the error token can be shifted.  |
`---------------------------------------------------------------*/
yyerrlab2:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      yyvsp--;
      yystate = *--yyssp;

      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 209 "eparse.y"



