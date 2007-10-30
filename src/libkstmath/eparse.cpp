/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

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
/* Tokens.  */
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
#line 2 "../../../src/libkstmath/eparse.y"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <objectstore.h>

#include "enodefactory.h"

#include "eparse-eh.h"
extern int yylex(Kst::ObjectStore *store);
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

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 20 "../../../src/libkstmath/eparse.y"
{
		char *data;
		double number;
		void *n; /* tree node */
		char character;
	   }
/* Line 193 of yacc.c.  */
#line 172 "eparse.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 185 "eparse.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
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
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   360

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  28
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  17
/* YYNRULES -- Number of rules.  */
#define YYNRULES  68
/* YYNRULES -- Number of states.  */
#define YYNSTATES  106

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   282

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
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
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    10,    12,    16,    19,
      21,    25,    28,    30,    34,    38,    42,    46,    50,    54,
      57,    60,    63,    66,    69,    72,    74,    78,    82,    86,
      90,    92,    95,    98,   102,   106,   110,   113,   116,   119,
     121,   124,   127,   130,   132,   136,   140,   143,   146,   148,
     152,   155,   157,   159,   164,   168,   174,   179,   183,   185,
     188,   190,   193,   195,   199,   201,   205,   206,   210
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
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
static const yytype_uint8 yyrline[] =
{
       0,    46,    46,    46,    57,    60,    63,    67,    69,    71,
      75,    77,    79,    83,    85,    87,    89,    91,    93,    95,
      97,    99,   101,   103,   105,   107,   111,   113,   115,   117,
     119,   121,   123,   127,   129,   131,   133,   135,   137,   139,
     143,   145,   147,   149,   153,   155,   157,   159,   161,   165,
     167,   169,   171,   173,   175,   180,   182,   184,   186,   188,
     190,   192,   196,   200,   202,   204,   206,   206,   210
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
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
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
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
static const yytype_uint8 yyr2[] =
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
static const yytype_uint8 yydefact[] =
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

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     2,    25,    75,    27,    28,    29,    30,    31,
      32,    33,    34,    76,    77,    78,    79
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -58
static const yytype_int16 yypact[] =
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
static const yytype_int16 yypgoto[] =
{
     -58,   -58,   -58,   -58,   212,   208,   160,   161,   147,   -35,
     -19,   148,   -58,   -58,   -58,   -58,   -57
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -67
static const yytype_int8 yytable[] =
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

static const yytype_int8 yycheck[] =
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
static const yytype_uint8 yystos[] =
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

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


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
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (store, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex (store)
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
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, store); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, Kst::ObjectStore *store)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, store)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    Kst::ObjectStore *store;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (store);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, Kst::ObjectStore *store)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, store)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    Kst::ObjectStore *store;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, store);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule, Kst::ObjectStore *store)
#else
static void
yy_reduce_print (yyvsp, yyrule, store)
    YYSTYPE *yyvsp;
    int yyrule;
    Kst::ObjectStore *store;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       , store);
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule, store); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
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
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, Kst::ObjectStore *store)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, store)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    Kst::ObjectStore *store;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (store);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (Kst::ObjectStore *store);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (Kst::ObjectStore *store)
#else
int
yyparse (store)
    Kst::ObjectStore *store;
#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

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
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
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

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
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
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
#line 46 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = 0L; yyClearErrors(); ParsedEquation = 0L; }
    break;

  case 3:
#line 47 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = ParsedEquation = (yyvsp[(2) - (2)].n);
				if (yyErrorCount() > 0) {
					DeleteNode((yyval.n));
					(yyval.n) = 0L;
					ParsedEquation = 0L;
					YYERROR;
				}
			}
    break;

  case 4:
#line 58 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = (yyvsp[(1) - (1)].n); }
    break;

  case 5:
#line 60 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = 0L; yyerror(store, EParseErrorEmpty); }
    break;

  case 6:
#line 64 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = (yyvsp[(1) - (1)].n); }
    break;

  case 7:
#line 68 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = NewLogicalOr((yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].n)); }
    break;

  case 8:
#line 70 "../../../src/libkstmath/eparse.y"
    { yyerror(store, EParseErrorTwoOperands); (yyval.n) = 0L; }
    break;

  case 9:
#line 72 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = (yyvsp[(1) - (1)].n); }
    break;

  case 10:
#line 76 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = NewLogicalAnd((yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].n)); }
    break;

  case 11:
#line 78 "../../../src/libkstmath/eparse.y"
    { yyerror(store, EParseErrorTwoOperands); (yyval.n) = 0L; }
    break;

  case 12:
#line 80 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = (yyvsp[(1) - (1)].n); }
    break;

  case 13:
#line 84 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = NewLessThan((yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].n)); }
    break;

  case 14:
#line 86 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = NewLessThanEqual((yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].n)); }
    break;

  case 15:
#line 88 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = NewGreaterThan((yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].n)); }
    break;

  case 16:
#line 90 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = NewGreaterThanEqual((yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].n)); }
    break;

  case 17:
#line 92 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = NewEqualTo((yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].n)); }
    break;

  case 18:
#line 94 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = NewNotEqualTo((yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].n)); }
    break;

  case 19:
#line 96 "../../../src/libkstmath/eparse.y"
    { yyerror(store, EParseErrorTwoOperands); (yyval.n) = 0L; }
    break;

  case 20:
#line 98 "../../../src/libkstmath/eparse.y"
    { yyerror(store, EParseErrorTwoOperands); (yyval.n) = 0L; }
    break;

  case 21:
#line 100 "../../../src/libkstmath/eparse.y"
    { yyerror(store, EParseErrorTwoOperands); (yyval.n) = 0L; }
    break;

  case 22:
#line 102 "../../../src/libkstmath/eparse.y"
    { yyerror(store, EParseErrorTwoOperands); (yyval.n) = 0L; }
    break;

  case 23:
#line 104 "../../../src/libkstmath/eparse.y"
    { yyerror(store, EParseErrorTwoOperands); (yyval.n) = 0L; }
    break;

  case 24:
#line 106 "../../../src/libkstmath/eparse.y"
    { yyerror(store, EParseErrorTwoOperands); (yyval.n) = 0L; }
    break;

  case 25:
#line 108 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = (yyvsp[(1) - (1)].n); }
    break;

  case 26:
#line 112 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = NewAddition((yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].n)); }
    break;

  case 27:
#line 114 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = NewSubtraction((yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].n)); }
    break;

  case 28:
#line 116 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = NewBitwiseOr((yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].n)); }
    break;

  case 29:
#line 118 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = NewBitwiseAnd((yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].n)); }
    break;

  case 30:
#line 120 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = (yyvsp[(1) - (1)].n); }
    break;

  case 31:
#line 122 "../../../src/libkstmath/eparse.y"
    { yyerror(store, EParseErrorTwoOperands); (yyval.n) = 0L; }
    break;

  case 32:
#line 124 "../../../src/libkstmath/eparse.y"
    { yyerror(store, EParseErrorTwoOperands); (yyval.n) = 0L; }
    break;

  case 33:
#line 128 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = NewMultiplication((yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].n)); }
    break;

  case 34:
#line 130 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = NewDivision((yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].n)); }
    break;

  case 35:
#line 132 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = NewModulo((yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].n)); }
    break;

  case 36:
#line 134 "../../../src/libkstmath/eparse.y"
    { yyerror(store, EParseErrorTwoOperands); (yyval.n) = 0L; }
    break;

  case 37:
#line 136 "../../../src/libkstmath/eparse.y"
    { yyerror(store, EParseErrorTwoOperands); (yyval.n) = 0L; }
    break;

  case 38:
#line 138 "../../../src/libkstmath/eparse.y"
    { yyerror(store, EParseErrorTwoOperands); (yyval.n) = 0L; }
    break;

  case 39:
#line 140 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = (yyvsp[(1) - (1)].n); }
    break;

  case 40:
#line 144 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = NewNegation((yyvsp[(2) - (2)].n)); }
    break;

  case 41:
#line 146 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = NewNot((yyvsp[(2) - (2)].n)); }
    break;

  case 42:
#line 148 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = 0L; yyerror(store, EParseErrorRequiresOperand); }
    break;

  case 43:
#line 150 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = (yyvsp[(1) - (1)].n); }
    break;

  case 44:
#line 154 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = NewPower((yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].n)); }
    break;

  case 45:
#line 156 "../../../src/libkstmath/eparse.y"
    { DeleteNode((yyvsp[(1) - (3)].n)); (yyval.n) = 0L; yyerror(store, EParseErrorTwoOperands); }
    break;

  case 46:
#line 158 "../../../src/libkstmath/eparse.y"
    { DeleteNode((yyvsp[(1) - (2)].n)); (yyval.n) = 0L; yyerror(store, EParseErrorTwoOperands); }
    break;

  case 47:
#line 160 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = 0L; yyerror(store, EParseErrorTwoOperands); }
    break;

  case 48:
#line 162 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = (yyvsp[(1) - (1)].n); }
    break;

  case 49:
#line 166 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = (yyvsp[(2) - (3)].n); ParenthesizeNode((yyval.n)); }
    break;

  case 50:
#line 168 "../../../src/libkstmath/eparse.y"
    { yyerror(store, EParseErrorMissingClosingParenthesis); (yyval.n) = 0L; }
    break;

  case 51:
#line 170 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = NewIdentifier((yyvsp[(1) - (1)].data)); }
    break;

  case 52:
#line 172 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = NewData(store, (yyvsp[(1) - (1)].data)); }
    break;

  case 53:
#line 174 "../../../src/libkstmath/eparse.y"
    { yyerror(store, EParseErrorNoImplicitMultiply); free((yyvsp[(1) - (4)].data)); (yyval.n) = 0L; }
    break;

  case 54:
#line 176 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = NewFunction((yyvsp[(1) - (3)].data), NewArgumentList()); }
    break;

  case 55:
#line 181 "../../../src/libkstmath/eparse.y"
    { yyerror(store, EParseErrorNoImplicitMultiply); DeleteNode((yyvsp[(3) - (5)].n)); free((yyvsp[(1) - (5)].data)); (yyval.n) = 0L; }
    break;

  case 56:
#line 183 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = NewFunction((yyvsp[(1) - (4)].data), (yyvsp[(3) - (4)].n)); }
    break;

  case 57:
#line 185 "../../../src/libkstmath/eparse.y"
    { yyerror(store, EParseErrorMissingClosingParenthesis); free((yyvsp[(1) - (3)].data)); (yyval.n) = 0L; }
    break;

  case 58:
#line 187 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = NewNumber((yyvsp[(1) - (1)].number)); }
    break;

  case 59:
#line 189 "../../../src/libkstmath/eparse.y"
    { yyerror(store, EParseErrorNoImplicitMultiply); (yyval.n) = 0L; }
    break;

  case 60:
#line 191 "../../../src/libkstmath/eparse.y"
    { yyerrortoken((yyvsp[(1) - (1)].character)); (yyval.n) = 0L; }
    break;

  case 61:
#line 193 "../../../src/libkstmath/eparse.y"
    { yyerror(store, EParseErrorEmptyParentheses); (yyval.n) = 0L; }
    break;

  case 62:
#line 197 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = (yyvsp[(1) - (1)].n); }
    break;

  case 63:
#line 201 "../../../src/libkstmath/eparse.y"
    { if ((yyvsp[(1) - (3)].n) && (yyvsp[(3) - (3)].n)) { AppendArgument((yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].n)); } else { DeleteNode((yyvsp[(1) - (3)].n)); DeleteNode((yyvsp[(3) - (3)].n)); (yyvsp[(1) - (3)].n) = 0L; } (yyval.n) = (yyvsp[(1) - (3)].n); }
    break;

  case 64:
#line 203 "../../../src/libkstmath/eparse.y"
    { if ((yyvsp[(1) - (1)].n)) { (yyval.n) = NewArgumentList(); AppendArgument((yyval.n), (yyvsp[(1) - (1)].n)); } else { (yyval.n) = 0L; } }
    break;

  case 65:
#line 205 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = 0L; DeleteNode((yyvsp[(1) - (3)].n)); yyerror(store, EParseErrorEmptyArg); }
    break;

  case 66:
#line 206 "../../../src/libkstmath/eparse.y"
    {}
    break;

  case 67:
#line 207 "../../../src/libkstmath/eparse.y"
    { yyerror(store, EParseErrorEmptyArg); DeleteNode((yyvsp[(3) - (3)].n)); (yyval.n) = 0L; }
    break;

  case 68:
#line 211 "../../../src/libkstmath/eparse.y"
    { (yyval.n) = (yyvsp[(1) - (1)].n); }
    break;


/* Line 1267 of yacc.c.  */
#line 1871 "eparse.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
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
#if ! YYERROR_VERBOSE
      yyerror (store, YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (store, yymsg);
	  }
	else
	  {
	    yyerror (store, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, store);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
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


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, store);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

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
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (store, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, store);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, store);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 214 "../../../src/libkstmath/eparse.y"



