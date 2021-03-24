
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* All symbols defined below should begin with ff or FF, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define FFBISON 1

/* Bison version.  */
#define FFBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define FFSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define FFPURE 0

/* Push parsers.  */
#define FFPUSH 0

/* Pull parsers.  */
#define FFPULL 1

/* Using locations.  */
#define FFLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "eval.y"

/************************************************************************/
/*                                                                      */
/*                       CFITSIO Lexical Parser                         */
/*                                                                      */
/* This file is one of 3 files containing code which parses an          */
/* arithmetic expression and evaluates it in the context of an input    */
/* FITS file table extension.  The CFITSIO lexical parser is divided    */
/* into the following 3 parts/files: the CFITSIO "front-end",           */
/* eval_f.c, contains the interface between the user/CFITSIO and the    */
/* real core of the parser; the FLEX interpreter, eval_l.c, takes the   */
/* input string and parses it into tokens and identifies the FITS       */
/* information required to evaluate the expression (ie, keywords and    */
/* columns); and, the BISON grammar and evaluation routines, eval_y.c,  */
/* receives the FLEX output and determines and performs the actual      */
/* operations.  The files eval_l.c and eval_y.c are produced from       */
/* running flex and bison on the files eval.l and eval.y, respectively. */
/* (flex and bison are available from any GNU archive: see www.gnu.org) */
/*                                                                      */
/* The grammar rules, rather than evaluating the expression in situ,    */
/* builds a tree, or Nodal, structure mapping out the order of          */
/* operations and expression dependencies.  This "compilation" process  */
/* allows for much faster processing of multiple rows.  This technique  */
/* was developed by Uwe Lammers of the XMM Science Analysis System,     */
/* although the CFITSIO implementation is entirely code original.       */
/*                                                                      */
/*                                                                      */
/* Modification History:                                                */
/*                                                                      */
/*   Kent Blackburn      c1992  Original parser code developed for the  */
/*                              FTOOLS software package, in particular, */
/*                              the fselect task.                       */
/*   Kent Blackburn      c1995  BIT column support added                */
/*   Peter D Wilson   Feb 1998  Vector column support added             */
/*   Peter D Wilson   May 1998  Ported to CFITSIO library.  User        */
/*                              interface routines written, in essence  */
/*                              making fselect, fcalc, and maketime     */
/*                              capabilities available to all tools     */
/*                              via single function calls.              */
/*   Peter D Wilson   Jun 1998  Major rewrite of parser core, so as to  */
/*                              create a run-time evaluation tree,      */
/*                              inspired by the work of Uwe Lammers,    */
/*                              resulting in a speed increase of        */
/*                              10-100 times.                           */
/*   Peter D Wilson   Jul 1998  gtifilter(a,b,c,d) function added       */
/*   Peter D Wilson   Aug 1998  regfilter(a,b,c,d) function added       */
/*   Peter D Wilson   Jul 1999  Make parser fitsfile-independent,       */
/*                              allowing a purely vector-based usage    */
/*  Craig B Markwardt Jun 2004  Add MEDIAN() function                   */
/*  Craig B Markwardt Jun 2004  Add SUM(), and MIN/MAX() for bit arrays */
/*  Craig B Markwardt Jun 2004  Allow subscripting of nX bit arrays     */
/*  Craig B Markwardt Jun 2004  Implement statistical functions         */
/*                              NVALID(), AVERAGE(), and STDDEV()       */
/*                              for integer and floating point vectors  */
/*  Craig B Markwardt Jun 2004  Use NULL values for range errors instead*/
/*                              of throwing a parse error               */
/*  Craig B Markwardt Oct 2004  Add ACCUM() and SEQDIFF() functions     */
/*  Craig B Markwardt Feb 2005  Add ANGSEP() function                   */
/*  Craig B Markwardt Aug 2005  CIRCLE, BOX, ELLIPSE, NEAR and REGFILTER*/
/*                              functions now accept vector arguments   */
/*  Craig B Markwardt Sum 2006  Add RANDOMN() and RANDOMP() functions   */
/*  Craig B Markwardt Mar 2007  Allow arguments to RANDOM and RANDOMN to*/
/*                              determine the output dimensions         */
/*  Craig B Markwardt Aug 2009  Add substring STRMID() and string search*/
/*                              STRSTR() functions; more overflow checks*/
/*  Craig B Markwardt Dec 2019  Add bit/hex/oct literal strings and     */
/*                              bitwise operatiosn between integers     */
/*                                                                      */
/************************************************************************/

#define  APPROX 1.0e-7
#include "eval_defs.h"
#include "region.h"
#include <time.h>

#include <stdlib.h>

#ifndef alloca
#define alloca malloc
#endif

/* Random number generators for various distributions */
#include "simplerng.h"

   /*  Shrink the initial stack depth to keep local data <32K (mac limit)  */
   /*  yacc will allocate more space if needed, though.                    */
#define  FFINITDEPTH   100

/***************************************************************/
/*  Replace Bison's BACKUP macro with one that fixes a bug --  */
/*  must update state after popping the stack -- and allows    */
/*  popping multiple terms at one time.                        */
/***************************************************************/

#define FFNEWBACKUP(token, value)                               \
   do								\
     if (ffchar == FFEMPTY )   					\
       { ffchar = (token);                                      \
         memcpy( &fflval, &(value), sizeof(value) );            \
         ffchar1 = FFTRANSLATE (ffchar);			\
         while (fflen--) FFPOPSTACK;				\
         ffstate = *ffssp;					\
         goto ffbackup;						\
       }							\
     else							\
       { fferror ("syntax error: cannot back up"); FFERROR; }	\
   while (0)

/***************************************************************/
/*  Useful macros for accessing/testing Nodes                  */
/***************************************************************/

#define TEST(a)        if( (a)<0 ) FFERROR
#define SIZE(a)        gParse.Nodes[ a ].value.nelem
#define TYPE(a)        gParse.Nodes[ a ].type
#define OPER(a)        gParse.Nodes[ a ].operation
#define PROMOTE(a,b)   if( TYPE(a) > TYPE(b) )                  \
                          b = New_Unary( TYPE(a), 0, b );       \
                       else if( TYPE(a) < TYPE(b) )             \
	                  a = New_Unary( TYPE(b), 0, a );

/*****  Internal functions  *****/

#ifdef __cplusplus
extern "C" {
#endif

static int  Alloc_Node    ( void );
static void Free_Last_Node( void );
static void Evaluate_Node ( int thisNode );

static int  New_Const ( int returnType, void *value, long len );
static int  New_Column( int ColNum );
static int  New_Offset( int ColNum, int offset );
static int  New_Unary ( int returnType, int Op, int Node1 );
static int  New_BinOp ( int returnType, int Node1, int Op, int Node2 );
static int  New_Func  ( int returnType, funcOp Op, int nNodes,
			int Node1, int Node2, int Node3, int Node4, 
			int Node5, int Node6, int Node7 );
static int  New_FuncSize( int returnType, funcOp Op, int nNodes,
			int Node1, int Node2, int Node3, int Node4, 
			  int Node5, int Node6, int Node7, int Size);
static int  New_Deref ( int Var,  int nDim,
			int Dim1, int Dim2, int Dim3, int Dim4, int Dim5 );
static int  New_GTI   ( char *fname, int Node1, char *start, char *stop );
static int  New_REG   ( char *fname, int NodeX, int NodeY, char *colNames );
static int  New_Vector( int subNode );
static int  Close_Vec ( int vecNode );
static int  Locate_Col( Node *this );
static int  Test_Dims ( int Node1, int Node2 );
static void Copy_Dims ( int Node1, int Node2 );

static void Allocate_Ptrs( Node *this );
static void Do_Unary     ( Node *this );
static void Do_Offset    ( Node *this );
static void Do_BinOp_bit ( Node *this );
static void Do_BinOp_str ( Node *this );
static void Do_BinOp_log ( Node *this );
static void Do_BinOp_lng ( Node *this );
static void Do_BinOp_dbl ( Node *this );
static void Do_Func      ( Node *this );
static void Do_Deref     ( Node *this );
static void Do_GTI       ( Node *this );
static void Do_REG       ( Node *this );
static void Do_Vector    ( Node *this );

static long Search_GTI   ( double evtTime, long nGTI, double *start,
			   double *stop, int ordered );

static char  saobox (double xcen, double ycen, double xwid, double ywid,
		     double rot,  double xcol, double ycol);
static char  ellipse(double xcen, double ycen, double xrad, double yrad,
		     double rot, double xcol, double ycol);
static char  circle (double xcen, double ycen, double rad,
		     double xcol, double ycol);
static char  bnear  (double x, double y, double tolerance);
static char  bitcmp (char *bitstrm1, char *bitstrm2);
static char  bitlgte(char *bits1, int oper, char *bits2);

static void  bitand(char *result, char *bitstrm1, char *bitstrm2);
static void  bitor (char *result, char *bitstrm1, char *bitstrm2);
static void  bitnot(char *result, char *bits);
static int cstrmid(char *dest_str, int dest_len,
		   char *src_str,  int src_len, int pos);

static void  fferror(char *msg);

#ifdef __cplusplus
    }
#endif



/* Line 189 of yacc.c  */
#line 267 "y.tab.c"

/* Enabling traces.  */
#ifndef FFDEBUG
# define FFDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef FFERROR_VERBOSE
# undef FFERROR_VERBOSE
# define FFERROR_VERBOSE 1
#else
# define FFERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef FFTOKEN_TABLE
# define FFTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef FFTOKENTYPE
# define FFTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum fftokentype {
     BOOLEAN = 258,
     LONG = 259,
     DOUBLE = 260,
     STRING = 261,
     BITSTR = 262,
     FUNCTION = 263,
     BFUNCTION = 264,
     IFUNCTION = 265,
     GTIFILTER = 266,
     REGFILTER = 267,
     COLUMN = 268,
     BCOLUMN = 269,
     SCOLUMN = 270,
     BITCOL = 271,
     ROWREF = 272,
     NULLREF = 273,
     SNULLREF = 274,
     OR = 275,
     AND = 276,
     NE = 277,
     EQ = 278,
     GTE = 279,
     LTE = 280,
     LT = 281,
     GT = 282,
     XOR = 283,
     POWER = 284,
     NOT = 285,
     FLTCAST = 286,
     INTCAST = 287,
     UMINUS = 288,
     DIFF = 289,
     ACCUM = 290
   };
#endif
/* Tokens.  */
#define BOOLEAN 258
#define LONG 259
#define DOUBLE 260
#define STRING 261
#define BITSTR 262
#define FUNCTION 263
#define BFUNCTION 264
#define IFUNCTION 265
#define GTIFILTER 266
#define REGFILTER 267
#define COLUMN 268
#define BCOLUMN 269
#define SCOLUMN 270
#define BITCOL 271
#define ROWREF 272
#define NULLREF 273
#define SNULLREF 274
#define OR 275
#define AND 276
#define NE 277
#define EQ 278
#define GTE 279
#define LTE 280
#define LT 281
#define GT 282
#define XOR 283
#define POWER 284
#define NOT 285
#define FLTCAST 286
#define INTCAST 287
#define UMINUS 288
#define DIFF 289
#define ACCUM 290




#if ! defined FFSTYPE && ! defined FFSTYPE_IS_DECLARED
typedef union FFSTYPE
{

/* Line 214 of yacc.c  */
#line 194 "eval.y"

    int    Node;        /* Index of Node */
    double dbl;         /* real value    */
    long   lng;         /* integer value */
    char   log;         /* logical value */
    char   str[MAX_STRLEN];    /* string value  */



/* Line 214 of yacc.c  */
#line 383 "y.tab.c"
} FFSTYPE;
# define FFSTYPE_IS_TRIVIAL 1
# define ffstype FFSTYPE /* obsolescent; will be withdrawn */
# define FFSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 395 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef FFTYPE_UINT8
typedef FFTYPE_UINT8 fftype_uint8;
#else
typedef unsigned char fftype_uint8;
#endif

#ifdef FFTYPE_INT8
typedef FFTYPE_INT8 fftype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char fftype_int8;
#else
typedef short int fftype_int8;
#endif

#ifdef FFTYPE_UINT16
typedef FFTYPE_UINT16 fftype_uint16;
#else
typedef unsigned short int fftype_uint16;
#endif

#ifdef FFTYPE_INT16
typedef FFTYPE_INT16 fftype_int16;
#else
typedef short int fftype_int16;
#endif

#ifndef FFSIZE_T
# ifdef __SIZE_TYPE__
#  define FFSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define FFSIZE_T size_t
# elif ! defined FFSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define FFSIZE_T size_t
# else
#  define FFSIZE_T unsigned int
# endif
#endif

#define FFSIZE_MAXIMUM ((FFSIZE_T) -1)

#ifndef FF_
# if FFENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define FF_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef FF_
#  define FF_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define FFUSE(e) ((void) (e))
#else
# define FFUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define FFID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
FFID (int ffi)
#else
static int
FFID (ffi)
    int ffi;
#endif
{
  return ffi;
}
#endif

#if ! defined ffoverflow || FFERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef FFSTACK_USE_ALLOCA
#  if FFSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define FFSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define FFSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define FFSTACK_ALLOC alloca
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

# ifdef FFSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define FFSTACK_FREE(Ptr) do { /* empty */; } while (FFID (0))
#  ifndef FFSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define FFSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define FFSTACK_ALLOC FFMALLOC
#  define FFSTACK_FREE FFFREE
#  ifndef FFSTACK_ALLOC_MAXIMUM
#   define FFSTACK_ALLOC_MAXIMUM FFSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined FFMALLOC || defined malloc) \
	     && (defined FFFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef FFMALLOC
#   define FFMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (FFSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef FFFREE
#   define FFFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined ffoverflow || FFERROR_VERBOSE */


#if (! defined ffoverflow \
     && (! defined __cplusplus \
	 || (defined FFSTYPE_IS_TRIVIAL && FFSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union ffalloc
{
  fftype_int16 ffss_alloc;
  FFSTYPE ffvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define FFSTACK_GAP_MAXIMUM (sizeof (union ffalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define FFSTACK_BYTES(N) \
     ((N) * (sizeof (fftype_int16) + sizeof (FFSTYPE)) \
      + FFSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef FFCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define FFCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define FFCOPY(To, From, Count)		\
      do					\
	{					\
	  FFSIZE_T ffi;				\
	  for (ffi = 0; ffi < (Count); ffi++)	\
	    (To)[ffi] = (From)[ffi];		\
	}					\
      while (FFID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables FFSIZE and FFSTACKSIZE give the old and new number of
   elements in the stack, and FFPTR gives the new location of the
   stack.  Advance FFPTR to a properly aligned location for the next
   stack.  */
# define FFSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	FFSIZE_T ffnewbytes;						\
	FFCOPY (&ffptr->Stack_alloc, Stack, ffsize);			\
	Stack = &ffptr->Stack_alloc;					\
	ffnewbytes = ffstacksize * sizeof (*Stack) + FFSTACK_GAP_MAXIMUM; \
	ffptr += ffnewbytes / sizeof (*ffptr);				\
      }									\
    while (FFID (0))

#endif

/* FFFINAL -- State number of the termination state.  */
#define FFFINAL  2
/* FFLAST -- Last index in FFTABLE.  */
#define FFLAST   1655

/* FFNTOKENS -- Number of terminals.  */
#define FFNTOKENS  55
/* FFNNTS -- Number of nonterminals.  */
#define FFNNTS  9
/* FFNRULES -- Number of rules.  */
#define FFNRULES  128
/* FFNRULES -- Number of states.  */
#define FFNSTATES  296

/* FFTRANSLATE(FFLEX) -- Bison symbol number corresponding to FFLEX.  */
#define FFUNDEFTOK  2
#define FFMAXUTOK   290

#define FFTRANSLATE(FFX)						\
  ((unsigned int) (FFX) <= FFMAXUTOK ? fftranslate[FFX] : FFUNDEFTOK)

/* FFTRANSLATE[FFLEX] -- Bison symbol number corresponding to FFLEX.  */
static const fftype_uint8 fftranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      51,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    37,    41,     2,
      53,    54,    38,    35,    20,    36,     2,    39,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    22,     2,
       2,    21,     2,    25,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    48,     2,    52,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    23,    40,    24,    28,     2,     2,     2,
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
      15,    16,    17,    18,    19,    26,    27,    29,    30,    31,
      32,    33,    34,    42,    43,    44,    45,    46,    47,    49,
      50
};

#if FFDEBUG
/* FFPRHS[FFN] -- Index of the first RHS symbol of rule number FFN in
   FFRHS.  */
static const fftype_uint16 ffprhs[] =
{
       0,     0,     3,     4,     7,     9,    12,    15,    18,    21,
      24,    27,    31,    34,    38,    42,    46,    49,    52,    54,
      56,    61,    65,    69,    73,    78,    85,    94,   105,   118,
     121,   125,   127,   129,   131,   136,   138,   140,   144,   148,
     152,   156,   160,   164,   168,   172,   176,   179,   182,   186,
     190,   194,   200,   206,   212,   215,   219,   223,   227,   231,
     237,   243,   253,   258,   265,   274,   285,   298,   301,   304,
     307,   310,   312,   314,   319,   323,   327,   331,   335,   339,
     343,   347,   351,   355,   359,   363,   367,   371,   375,   379,
     383,   387,   391,   395,   399,   403,   407,   411,   417,   423,
     427,   431,   435,   441,   449,   461,   477,   480,   484,   490,
     500,   504,   512,   522,   527,   534,   543,   554,   567,   570,
     574,   576,   578,   583,   585,   589,   593,   599,   605
};

/* FFRHS -- A `-1'-separated list of the rules' RHS.  */
static const fftype_int8 ffrhs[] =
{
      56,     0,    -1,    -1,    56,    57,    -1,    51,    -1,    60,
      51,    -1,    61,    51,    -1,    63,    51,    -1,    62,    51,
      -1,     1,    51,    -1,    23,    61,    -1,    58,    20,    61,
      -1,    23,    60,    -1,    59,    20,    60,    -1,    59,    20,
      61,    -1,    58,    20,    60,    -1,    59,    24,    -1,    58,
      24,    -1,     7,    -1,    16,    -1,    16,    23,    60,    24,
      -1,    62,    41,    62,    -1,    62,    40,    62,    -1,    62,
      35,    62,    -1,    62,    48,    60,    52,    -1,    62,    48,
      60,    20,    60,    52,    -1,    62,    48,    60,    20,    60,
      20,    60,    52,    -1,    62,    48,    60,    20,    60,    20,
      60,    20,    60,    52,    -1,    62,    48,    60,    20,    60,
      20,    60,    20,    60,    20,    60,    52,    -1,    44,    62,
      -1,    53,    62,    54,    -1,     4,    -1,     5,    -1,    13,
      -1,    13,    23,    60,    24,    -1,    17,    -1,    18,    -1,
      60,    37,    60,    -1,    60,    35,    60,    -1,    60,    36,
      60,    -1,    60,    38,    60,    -1,    60,    39,    60,    -1,
      60,    41,    60,    -1,    60,    40,    60,    -1,    60,    42,
      60,    -1,    60,    43,    60,    -1,    35,    60,    -1,    36,
      60,    -1,    53,    60,    54,    -1,    60,    38,    61,    -1,
      61,    38,    60,    -1,    61,    25,    60,    22,    60,    -1,
      61,    25,    61,    22,    60,    -1,    61,    25,    60,    22,
      61,    -1,     8,    54,    -1,     8,    61,    54,    -1,     8,
      63,    54,    -1,     8,    62,    54,    -1,     8,    60,    54,
      -1,    10,    63,    20,    63,    54,    -1,     8,    60,    20,
      60,    54,    -1,     8,    60,    20,    60,    20,    60,    20,
      60,    54,    -1,    60,    48,    60,    52,    -1,    60,    48,
      60,    20,    60,    52,    -1,    60,    48,    60,    20,    60,
      20,    60,    52,    -1,    60,    48,    60,    20,    60,    20,
      60,    20,    60,    52,    -1,    60,    48,    60,    20,    60,
      20,    60,    20,    60,    20,    60,    52,    -1,    46,    60,
      -1,    46,    61,    -1,    45,    60,    -1,    45,    61,    -1,
       3,    -1,    14,    -1,    14,    23,    60,    24,    -1,    62,
      30,    62,    -1,    62,    29,    62,    -1,    62,    33,    62,
      -1,    62,    32,    62,    -1,    62,    34,    62,    -1,    62,
      31,    62,    -1,    60,    34,    60,    -1,    60,    33,    60,
      -1,    60,    31,    60,    -1,    60,    32,    60,    -1,    60,
      28,    60,    -1,    60,    30,    60,    -1,    60,    29,    60,
      -1,    63,    30,    63,    -1,    63,    29,    63,    -1,    63,
      34,    63,    -1,    63,    31,    63,    -1,    63,    33,    63,
      -1,    63,    32,    63,    -1,    61,    27,    61,    -1,    61,
      26,    61,    -1,    61,    30,    61,    -1,    61,    29,    61,
      -1,    60,    21,    60,    22,    60,    -1,    61,    25,    61,
      22,    61,    -1,     9,    60,    54,    -1,     9,    61,    54,
      -1,     9,    63,    54,    -1,     8,    61,    20,    61,    54,
      -1,     9,    60,    20,    60,    20,    60,    54,    -1,     9,
      60,    20,    60,    20,    60,    20,    60,    20,    60,    54,
      -1,     9,    60,    20,    60,    20,    60,    20,    60,    20,
      60,    20,    60,    20,    60,    54,    -1,    11,    54,    -1,
      11,     6,    54,    -1,    11,     6,    20,    60,    54,    -1,
      11,     6,    20,    60,    20,     6,    20,     6,    54,    -1,
      12,     6,    54,    -1,    12,     6,    20,    60,    20,    60,
      54,    -1,    12,     6,    20,    60,    20,    60,    20,     6,
      54,    -1,    61,    48,    60,    52,    -1,    61,    48,    60,
      20,    60,    52,    -1,    61,    48,    60,    20,    60,    20,
      60,    52,    -1,    61,    48,    60,    20,    60,    20,    60,
      20,    60,    52,    -1,    61,    48,    60,    20,    60,    20,
      60,    20,    60,    20,    60,    52,    -1,    44,    61,    -1,
      53,    61,    54,    -1,     6,    -1,    15,    -1,    15,    23,
      60,    24,    -1,    19,    -1,    53,    63,    54,    -1,    63,
      35,    63,    -1,    61,    25,    63,    22,    63,    -1,     8,
      63,    20,    63,    54,    -1,     8,    63,    20,    60,    20,
      60,    54,    -1
};

/* FFRLINE[FFN] -- source line where rule number FFN was defined.  */
static const fftype_uint16 ffrline[] =
{
       0,   246,   246,   247,   250,   251,   257,   263,   269,   275,
     278,   280,   293,   295,   308,   319,   333,   337,   341,   345,
     347,   356,   359,   362,   371,   373,   375,   377,   379,   381,
     384,   388,   390,   392,   394,   403,   405,   407,   410,   413,
     416,   419,   422,   431,   440,   449,   452,   454,   456,   458,
     462,   466,   485,   504,   523,   534,   548,   560,   591,   686,
     694,   755,   779,   781,   783,   785,   787,   789,   791,   793,
     795,   799,   801,   803,   812,   815,   818,   821,   824,   827,
     830,   833,   836,   839,   842,   845,   848,   851,   854,   857,
     860,   863,   866,   869,   871,   873,   875,   878,   885,   902,
     915,   928,   939,   955,   979,  1007,  1044,  1048,  1052,  1055,
    1059,  1063,  1066,  1070,  1072,  1074,  1076,  1078,  1080,  1082,
    1086,  1089,  1091,  1100,  1102,  1104,  1113,  1132,  1151
};
#endif

#if FFDEBUG || FFERROR_VERBOSE || FFTOKEN_TABLE
/* FFTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at FFNTOKENS, nonterminals.  */
static const char *const fftname[] =
{
  "$end", "error", "$undefined", "BOOLEAN", "LONG", "DOUBLE", "STRING",
  "BITSTR", "FUNCTION", "BFUNCTION", "IFUNCTION", "GTIFILTER", "REGFILTER",
  "COLUMN", "BCOLUMN", "SCOLUMN", "BITCOL", "ROWREF", "NULLREF",
  "SNULLREF", "','", "'='", "':'", "'{'", "'}'", "'?'", "OR", "AND", "'~'",
  "NE", "EQ", "GTE", "LTE", "LT", "GT", "'+'", "'-'", "'%'", "'*'", "'/'",
  "'|'", "'&'", "XOR", "POWER", "NOT", "FLTCAST", "INTCAST", "UMINUS",
  "'['", "DIFF", "ACCUM", "'\\n'", "']'", "'('", "')'", "$accept", "lines",
  "line", "bvector", "vector", "expr", "bexpr", "bits", "sexpr", 0
};
#endif

# ifdef FFPRINT
/* FFTOKNUM[FFLEX-NUM] -- Internal token number corresponding to
   token FFLEX-NUM.  */
static const fftype_uint16 fftoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
      44,    61,    58,   123,   125,    63,   275,   276,   126,   277,
     278,   279,   280,   281,   282,    43,    45,    37,    42,    47,
     124,    38,   283,   284,   285,   286,   287,   288,    91,   289,
     290,    10,    93,    40,    41
};
# endif

/* FFR1[FFN] -- Symbol number of symbol that rule FFN derives.  */
static const fftype_uint8 ffr1[] =
{
       0,    55,    56,    56,    57,    57,    57,    57,    57,    57,
      58,    58,    59,    59,    59,    59,    60,    61,    62,    62,
      62,    62,    62,    62,    62,    62,    62,    62,    62,    62,
      62,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      63,    63,    63,    63,    63,    63,    63,    63,    63
};

/* FFR2[FFN] -- Number of symbols composing right hand side of rule FFN.  */
static const fftype_uint8 ffr2[] =
{
       0,     2,     0,     2,     1,     2,     2,     2,     2,     2,
       2,     3,     2,     3,     3,     3,     2,     2,     1,     1,
       4,     3,     3,     3,     4,     6,     8,    10,    12,     2,
       3,     1,     1,     1,     4,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     2,     3,     3,
       3,     5,     5,     5,     2,     3,     3,     3,     3,     5,
       5,     9,     4,     6,     8,    10,    12,     2,     2,     2,
       2,     1,     1,     4,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     5,     5,     3,
       3,     3,     5,     7,    11,    15,     2,     3,     5,     9,
       3,     7,     9,     4,     6,     8,    10,    12,     2,     3,
       1,     1,     4,     1,     3,     3,     5,     5,     7
};

/* FFDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when FFTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const fftype_uint8 ffdefact[] =
{
       2,     0,     1,     0,    71,    31,    32,   120,    18,     0,
       0,     0,     0,     0,    33,    72,   121,    19,    35,    36,
     123,     0,     0,     0,     0,     0,     0,     4,     0,     3,
       0,     0,     0,     0,     0,     0,     9,    54,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     106,     0,     0,     0,     0,     0,    12,    10,     0,    46,
      47,   118,    29,    69,    70,    67,    68,     0,     0,     0,
       0,     0,    17,     0,    16,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     5,     0,     0,     0,     0,     0,     0,
       0,     6,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     8,     0,     0,     0,     0,     0,     0,     0,
       7,     0,    58,     0,    55,    57,     0,    56,     0,    99,
     100,   101,     0,     0,   107,     0,   110,     0,     0,     0,
       0,    48,   119,    30,   124,    15,    11,    13,    14,     0,
      84,    86,    85,    82,    83,    81,    80,    38,    39,    37,
      40,    49,    41,    43,    42,    44,    45,     0,     0,     0,
       0,    94,    93,    96,    95,    50,     0,     0,     0,    75,
      74,    79,    77,    76,    78,    23,    22,    21,     0,    88,
      87,    90,    92,    91,    89,   125,     0,     0,     0,     0,
       0,     0,     0,     0,    34,    73,   122,    20,     0,     0,
      62,     0,     0,     0,     0,   113,    29,     0,     0,    24,
       0,    60,   102,     0,   127,     0,    59,     0,   108,     0,
      97,     0,    51,    53,    52,    98,   126,     0,     0,     0,
       0,     0,     0,     0,     0,    63,     0,   114,     0,    25,
       0,   128,     0,   103,     0,     0,   111,     0,     0,     0,
       0,     0,     0,     0,     0,    64,     0,   115,     0,    26,
      61,     0,   109,   112,     0,     0,     0,     0,     0,    65,
       0,   116,     0,    27,     0,   104,     0,     0,     0,     0,
      66,   117,    28,     0,     0,   105
};

/* FFDEFGOTO[NTERM-NUM].  */
static const fftype_int8 ffdefgoto[] =
{
      -1,     1,    29,    30,    31,    46,    47,    44,    58
};

/* FFPACT[STATE-NUM] -- Index in FFTABLE of the portion describing
   STATE-NUM.  */
#define FFPACT_NINF -50
static const fftype_int16 ffpact[] =
{
     -50,   306,   -50,   -49,   -50,   -50,   -50,   -50,   -50,   357,
     409,   409,    -5,    -3,   -18,     5,     6,    46,   -50,   -50,
     -50,   409,   409,   409,   409,   409,   409,   -50,   409,   -50,
      39,    80,  1126,   305,  1503,   -17,   -50,   -50,   436,    10,
    1451,   138,   465,   155,  1547,   166,  1431,   117,  1597,   -16,
     -50,   -13,   409,   409,   409,   409,  1431,   117,   417,   -22,
     -22,    14,    23,   -22,    14,   -22,    14,   638,   246,  1477,
     207,   409,   -50,   409,   -50,   409,   409,   409,   409,   409,
     409,   409,   409,   409,   409,   409,   409,   409,   409,   409,
     409,   409,   409,   -50,   409,   409,   409,   409,   409,   409,
     409,   -50,     3,     3,     3,     3,     3,     3,     3,     3,
       3,   409,   -50,   409,   409,   409,   409,   409,   409,   409,
     -50,   409,   -50,   409,   -50,   -50,   409,   -50,   409,   -50,
     -50,   -50,   409,   409,   -50,   409,   -50,  1293,  1316,  1339,
    1362,   -50,   -50,   -50,   -50,  1431,   117,  1431,   117,  1385,
    1573,  1573,  1573,  1598,  1598,  1598,  1598,   356,   356,   356,
     296,    14,   296,   -37,   -37,   -37,   -37,   751,  1408,   260,
    1620,   185,    67,    19,    19,   296,   776,     3,     3,    20,
      20,    20,    20,    20,    20,    25,    23,    23,   801,    11,
      11,    57,    57,    57,    57,   -50,   494,   352,  1150,   354,
    1174,   404,   523,  1198,   -50,   -50,   -50,   -50,   409,   409,
     -50,   409,   409,   409,   409,   -50,    23,  1549,   409,   -50,
     409,   -50,   -50,   409,   -50,   409,   -50,    89,   -50,   409,
    1527,   826,  1527,   117,  1527,   117,   417,   851,   876,  1222,
     666,   552,    81,   581,   409,   -50,   409,   -50,   409,   -50,
     409,   -50,   409,   -50,   100,   101,   -50,   901,   926,   951,
     694,  1246,    54,    55,   409,   -50,   409,   -50,   409,   -50,
     -50,   409,   -50,   -50,   976,  1001,  1026,   610,   409,   -50,
     409,   -50,   409,   -50,   409,   -50,  1051,  1076,  1101,  1270,
     -50,   -50,   -50,   409,   722,   -50
};

/* FFPGOTO[NTERM-NUM].  */
static const fftype_int16 ffpgoto[] =
{
     -50,   -50,   -50,   -50,   -50,    -1,    93,   150,    22
};

/* FFTABLE[FFPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what FFDEFACT says.
   If FFTABLE_NINF, syntax error.  */
#define FFTABLE_NINF -1
static const fftype_uint16 fftable[] =
{
      32,    49,    36,    51,   133,    52,    91,   135,    38,    42,
       8,    92,   113,   114,   115,   116,   117,   118,   119,    17,
      56,    59,    60,    35,    63,    65,    92,    67,    53,    54,
     123,    41,    45,    48,   120,    94,    95,    96,   134,    97,
      98,   136,   115,   116,   117,   118,   119,   177,    99,    50,
      70,   137,   138,   139,   140,   108,   178,    99,   100,    71,
     109,   110,   100,    72,   124,   109,   110,   100,   111,    55,
     145,   111,   147,   111,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   162,   163,   164,   165,
     166,   167,   119,   168,    33,   242,    97,    98,   175,   176,
      73,   254,    39,    43,    74,    99,   262,   263,   272,   273,
     188,     0,     0,     0,    57,   100,   170,    61,    64,    66,
     196,    68,     0,     0,     0,   198,     0,   200,     0,     0,
       0,     0,   202,     0,   203,   189,   190,   191,   192,   193,
     194,   195,    94,    95,    96,     0,    97,    98,   199,     0,
       0,    34,     0,     0,   201,    99,     0,     0,   126,    40,
       0,     0,     0,     0,   146,   100,   148,   113,   114,   115,
     116,   117,   118,   119,    62,     0,     0,     0,    69,   161,
      94,    95,    96,     0,    97,    98,     0,   169,   171,   172,
     173,   174,   127,    99,     0,   113,   114,   115,   116,   117,
     118,   119,     0,   100,     0,     0,     0,   230,   231,   130,
     232,   234,    96,   237,    97,    98,   197,   238,     0,   239,
     131,     0,   240,    99,   241,     0,     0,     0,   243,     0,
       0,     0,     0,   100,     0,   236,   113,   114,   115,   116,
     117,   118,   119,   257,     0,   258,     0,   259,     0,   260,
       0,   261,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   144,     0,   274,     0,   275,     0,   276,     0,     0,
     277,    94,    95,    96,     0,    97,    98,   286,     0,   287,
       0,   288,   212,   289,    99,    94,    95,    96,     0,    97,
      98,     0,   294,     0,   100,     0,     0,     0,    99,     0,
     142,     0,     0,     0,   233,   235,     2,     3,   100,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,     0,   216,   217,    21,
      94,    95,    96,     0,    97,    98,    88,    89,    90,    91,
       0,    22,    23,    99,    92,     0,     0,     0,     0,     0,
      24,    25,    26,   100,     0,     0,   101,    27,     0,    28,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    94,    95,    96,
      21,    97,    98,   113,   114,   115,   116,   117,   118,   119,
      99,     0,    22,    23,    86,    87,    88,    89,    90,    91,
     100,    24,    25,    26,    92,     0,   222,     0,   224,     0,
      28,    37,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,     0,
       0,     0,    21,   113,   114,   115,   116,   117,   118,   119,
       0,     0,     0,     0,    22,    23,   113,   114,   115,   116,
     117,   118,   119,    24,    25,    26,   121,    75,   226,     0,
       0,     0,    28,     0,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
       0,     0,     0,     0,    92,   128,    75,     0,     0,     0,
     122,     0,     0,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,     0,
       0,     0,     0,    92,   220,    75,     0,     0,     0,   129,
       0,     0,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,     0,     0,
       0,     0,    92,   227,    75,     0,     0,     0,   221,     0,
       0,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,     0,     0,     0,
       0,    92,   252,    75,     0,     0,     0,   228,     0,     0,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,     0,     0,     0,     0,
      92,   255,    75,     0,     0,     0,   253,     0,     0,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,     0,     0,     0,     0,    92,
     284,    75,     0,     0,     0,   256,     0,     0,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,     0,     0,     0,     0,    92,    75,
       0,     0,     0,     0,   285,     0,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,     0,     0,     0,     0,    92,    75,     0,     0,
       0,     0,   141,     0,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
       0,     0,     0,     0,    92,    75,     0,     0,     0,     0,
     251,     0,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,     0,     0,
       0,     0,    92,    75,     0,     0,     0,     0,   270,     0,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,     0,     0,     0,     0,
      92,   209,    75,     0,     0,     0,   295,     0,     0,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,     0,   214,    75,     0,    92,
       0,     0,     0,   210,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
       0,   218,    75,     0,    92,     0,     0,     0,   215,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,     0,   244,    75,     0,    92,
       0,     0,     0,   219,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
       0,   246,    75,     0,    92,     0,     0,     0,   245,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,     0,   248,    75,     0,    92,
       0,     0,     0,   247,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
       0,   264,    75,     0,    92,     0,     0,     0,   249,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,     0,   266,    75,     0,    92,
       0,     0,     0,   265,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
       0,   268,    75,     0,    92,     0,     0,     0,   267,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,     0,   278,    75,     0,    92,
       0,     0,     0,   269,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
       0,   280,    75,     0,    92,     0,     0,     0,   279,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,     0,   282,    75,     0,    92,
       0,     0,     0,   281,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
       0,     0,    75,     0,    92,     0,     0,     0,   283,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,     0,     0,    75,     0,    92,
       0,     0,     0,   290,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
       0,     0,    75,     0,    92,     0,     0,     0,   291,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,     0,     0,    75,     0,    92,
       0,     0,     0,   292,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
     223,    75,     0,     0,    92,     0,     0,    93,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,   225,    75,     0,     0,    92,     0,
       0,     0,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,   229,    75,
       0,     0,    92,     0,     0,     0,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,   250,    75,     0,     0,    92,     0,     0,     0,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,   271,    75,     0,     0,
      92,     0,     0,     0,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
     293,    75,     0,     0,    92,     0,     0,     0,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    75,     0,     0,   204,    92,     0,
       0,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    75,     0,     0,
     205,    92,     0,     0,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      75,     0,     0,   206,    92,     0,     0,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    75,     0,     0,   207,    92,     0,     0,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    75,   208,     0,     0,
      92,     0,     0,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    75,
     211,     0,     0,    92,     0,     0,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    75,     0,     0,     0,    92,     0,     0,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,     0,     0,     0,     0,    92,
     102,   103,   104,   105,   106,   107,   108,     0,     0,     0,
       0,   109,   110,     0,     0,     0,     0,     0,     0,   111,
       0,     0,     0,     0,     0,   125,   102,   103,   104,   105,
     106,   107,   108,     0,     0,     0,     0,   109,   110,     0,
       0,     0,     0,     0,     0,   111,     0,     0,     0,     0,
       0,   143,   102,   103,   104,   105,   106,   107,   108,     0,
       0,     0,     0,   109,   110,     0,     0,     0,     0,     0,
       0,   111,     0,     0,   112,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,     0,     0,     0,     0,    92,   102,   103,   104,   105,
     106,   107,   108,     0,   108,     0,     0,   109,   110,   109,
     110,     0,     0,     0,     0,   111,     0,   111,     0,     0,
       0,     0,     0,   143,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,   132,     0,     0,
       0,    92,     0,     0,     0,     0,   113,   114,   115,   116,
     117,   118,   119,    83,    84,    85,    86,    87,    88,    89,
      90,    91,   213,     0,     0,     0,    92,     0,     0,   113,
     114,   115,   116,   117,   118,   119
};

static const fftype_int16 ffcheck[] =
{
       1,     6,    51,     6,    20,    23,    43,    20,     9,    10,
       7,    48,    29,    30,    31,    32,    33,    34,    35,    16,
      21,    22,    23,     1,    25,    26,    48,    28,    23,    23,
      20,     9,    10,    11,    51,    25,    26,    27,    54,    29,
      30,    54,    31,    32,    33,    34,    35,    44,    38,    54,
      28,    52,    53,    54,    55,    35,    53,    38,    48,    20,
      40,    41,    48,    24,    54,    40,    41,    48,    48,    23,
      71,    48,    73,    48,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    35,    94,     1,     6,    29,    30,    99,   100,
      20,    20,     9,    10,    24,    38,     6,     6,    54,    54,
     111,    -1,    -1,    -1,    21,    48,    94,    24,    25,    26,
     121,    28,    -1,    -1,    -1,   126,    -1,   128,    -1,    -1,
      -1,    -1,   133,    -1,   135,   113,   114,   115,   116,   117,
     118,   119,    25,    26,    27,    -1,    29,    30,   126,    -1,
      -1,     1,    -1,    -1,   132,    38,    -1,    -1,    20,     9,
      -1,    -1,    -1,    -1,    71,    48,    73,    29,    30,    31,
      32,    33,    34,    35,    24,    -1,    -1,    -1,    28,    86,
      25,    26,    27,    -1,    29,    30,    -1,    94,    95,    96,
      97,    98,    54,    38,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    48,    -1,    -1,    -1,   208,   209,    54,
     211,   212,    27,   214,    29,    30,   123,   218,    -1,   220,
      54,    -1,   223,    38,   225,    -1,    -1,    -1,   229,    -1,
      -1,    -1,    -1,    48,    -1,   213,    29,    30,    31,    32,
      33,    34,    35,   244,    -1,   246,    -1,   248,    -1,   250,
      -1,   252,   102,   103,   104,   105,   106,   107,   108,   109,
     110,    54,    -1,   264,    -1,   266,    -1,   268,    -1,    -1,
     271,    25,    26,    27,    -1,    29,    30,   278,    -1,   280,
      -1,   282,    22,   284,    38,    25,    26,    27,    -1,    29,
      30,    -1,   293,    -1,    48,    -1,    -1,    -1,    38,    -1,
      54,    -1,    -1,    -1,   211,   212,     0,     1,    48,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    -1,   177,   178,    23,
      25,    26,    27,    -1,    29,    30,    40,    41,    42,    43,
      -1,    35,    36,    38,    48,    -1,    -1,    -1,    -1,    -1,
      44,    45,    46,    48,    -1,    -1,    51,    51,    -1,    53,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    25,    26,    27,
      23,    29,    30,    29,    30,    31,    32,    33,    34,    35,
      38,    -1,    35,    36,    38,    39,    40,    41,    42,    43,
      48,    44,    45,    46,    48,    -1,    54,    -1,    54,    -1,
      53,    54,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    -1,
      -1,    -1,    23,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    35,    36,    29,    30,    31,    32,
      33,    34,    35,    44,    45,    46,    20,    21,    54,    -1,
      -1,    -1,    53,    -1,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      -1,    -1,    -1,    -1,    48,    20,    21,    -1,    -1,    -1,
      54,    -1,    -1,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    -1,
      -1,    -1,    -1,    48,    20,    21,    -1,    -1,    -1,    54,
      -1,    -1,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    -1,    -1,
      -1,    -1,    48,    20,    21,    -1,    -1,    -1,    54,    -1,
      -1,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    -1,    -1,    -1,
      -1,    48,    20,    21,    -1,    -1,    -1,    54,    -1,    -1,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    -1,    -1,    -1,    -1,
      48,    20,    21,    -1,    -1,    -1,    54,    -1,    -1,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    -1,    -1,    -1,    -1,    48,
      20,    21,    -1,    -1,    -1,    54,    -1,    -1,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    -1,    -1,    -1,    -1,    48,    21,
      -1,    -1,    -1,    -1,    54,    -1,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    -1,    -1,    -1,    -1,    48,    21,    -1,    -1,
      -1,    -1,    54,    -1,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      -1,    -1,    -1,    -1,    48,    21,    -1,    -1,    -1,    -1,
      54,    -1,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    -1,    -1,
      -1,    -1,    48,    21,    -1,    -1,    -1,    -1,    54,    -1,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    -1,    -1,    -1,    -1,
      48,    20,    21,    -1,    -1,    -1,    54,    -1,    -1,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    -1,    20,    21,    -1,    48,
      -1,    -1,    -1,    52,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      -1,    20,    21,    -1,    48,    -1,    -1,    -1,    52,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    -1,    20,    21,    -1,    48,
      -1,    -1,    -1,    52,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      -1,    20,    21,    -1,    48,    -1,    -1,    -1,    52,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    -1,    20,    21,    -1,    48,
      -1,    -1,    -1,    52,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      -1,    20,    21,    -1,    48,    -1,    -1,    -1,    52,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    -1,    20,    21,    -1,    48,
      -1,    -1,    -1,    52,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      -1,    20,    21,    -1,    48,    -1,    -1,    -1,    52,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    -1,    20,    21,    -1,    48,
      -1,    -1,    -1,    52,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      -1,    20,    21,    -1,    48,    -1,    -1,    -1,    52,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    -1,    20,    21,    -1,    48,
      -1,    -1,    -1,    52,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      -1,    -1,    21,    -1,    48,    -1,    -1,    -1,    52,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    -1,    -1,    21,    -1,    48,
      -1,    -1,    -1,    52,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      -1,    -1,    21,    -1,    48,    -1,    -1,    -1,    52,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    -1,    -1,    21,    -1,    48,
      -1,    -1,    -1,    52,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      20,    21,    -1,    -1,    48,    -1,    -1,    51,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    20,    21,    -1,    -1,    48,    -1,
      -1,    -1,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    20,    21,
      -1,    -1,    48,    -1,    -1,    -1,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    20,    21,    -1,    -1,    48,    -1,    -1,    -1,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    20,    21,    -1,    -1,
      48,    -1,    -1,    -1,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      20,    21,    -1,    -1,    48,    -1,    -1,    -1,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    21,    -1,    -1,    24,    48,    -1,
      -1,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    21,    -1,    -1,
      24,    48,    -1,    -1,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      21,    -1,    -1,    24,    48,    -1,    -1,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    21,    -1,    -1,    24,    48,    -1,    -1,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    21,    22,    -1,    -1,
      48,    -1,    -1,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    21,
      22,    -1,    -1,    48,    -1,    -1,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    21,    -1,    -1,    -1,    48,    -1,    -1,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    -1,    -1,    -1,    -1,    48,
      29,    30,    31,    32,    33,    34,    35,    -1,    -1,    -1,
      -1,    40,    41,    -1,    -1,    -1,    -1,    -1,    -1,    48,
      -1,    -1,    -1,    -1,    -1,    54,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    -1,    40,    41,    -1,
      -1,    -1,    -1,    -1,    -1,    48,    -1,    -1,    -1,    -1,
      -1,    54,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    40,    41,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    -1,    -1,    51,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    -1,    -1,    -1,    -1,    48,    29,    30,    31,    32,
      33,    34,    35,    -1,    35,    -1,    -1,    40,    41,    40,
      41,    -1,    -1,    -1,    -1,    48,    -1,    48,    -1,    -1,
      -1,    -1,    -1,    54,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    20,    -1,    -1,
      -1,    48,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    22,    -1,    -1,    -1,    48,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35
};

/* FFSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const fftype_uint8 ffstos[] =
{
       0,    56,     0,     1,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    23,    35,    36,    44,    45,    46,    51,    53,    57,
      58,    59,    60,    61,    62,    63,    51,    54,    60,    61,
      62,    63,    60,    61,    62,    63,    60,    61,    63,     6,
      54,     6,    23,    23,    23,    23,    60,    61,    63,    60,
      60,    61,    62,    60,    61,    60,    61,    60,    61,    62,
      63,    20,    24,    20,    24,    21,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    48,    51,    25,    26,    27,    29,    30,    38,
      48,    51,    29,    30,    31,    32,    33,    34,    35,    40,
      41,    48,    51,    29,    30,    31,    32,    33,    34,    35,
      51,    20,    54,    20,    54,    54,    20,    54,    20,    54,
      54,    54,    20,    20,    54,    20,    54,    60,    60,    60,
      60,    54,    54,    54,    54,    60,    61,    60,    61,    60,
      60,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    61,    60,    60,    60,    60,    60,    60,    60,    61,
      63,    61,    61,    61,    61,    60,    60,    44,    53,    62,
      62,    62,    62,    62,    62,    62,    62,    62,    60,    63,
      63,    63,    63,    63,    63,    63,    60,    61,    60,    63,
      60,    63,    60,    60,    24,    24,    24,    24,    22,    20,
      52,    22,    22,    22,    20,    52,    62,    62,    20,    52,
      20,    54,    54,    20,    54,    20,    54,    20,    54,    20,
      60,    60,    60,    61,    60,    61,    63,    60,    60,    60,
      60,    60,     6,    60,    20,    52,    20,    52,    20,    52,
      20,    54,    20,    54,    20,    20,    54,    60,    60,    60,
      60,    60,     6,     6,    20,    52,    20,    52,    20,    52,
      54,    20,    54,    54,    60,    60,    60,    60,    20,    52,
      20,    52,    20,    52,    20,    54,    60,    60,    60,    60,
      52,    52,    52,    20,    60,    54
};

#define fferrok		(fferrstatus = 0)
#define ffclearin	(ffchar = FFEMPTY)
#define FFEMPTY		(-2)
#define FFEOF		0

#define FFACCEPT	goto ffacceptlab
#define FFABORT		goto ffabortlab
#define FFERROR		goto fferrorlab


/* Like FFERROR except do call fferror.  This remains here temporarily
   to ease the transition to the new meaning of FFERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define FFFAIL		goto fferrlab

#define FFRECOVERING()  (!!fferrstatus)

#define FFBACKUP(Token, Value)					\
do								\
  if (ffchar == FFEMPTY && fflen == 1)				\
    {								\
      ffchar = (Token);						\
      fflval = (Value);						\
      fftoken = FFTRANSLATE (ffchar);				\
      FFPOPSTACK (1);						\
      goto ffbackup;						\
    }								\
  else								\
    {								\
      fferror (FF_("syntax error: cannot back up")); \
      FFERROR;							\
    }								\
while (FFID (0))


#define FFTERROR	1
#define FFERRCODE	256


/* FFLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define FFRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef FFLLOC_DEFAULT
# define FFLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (FFID (N))                                                    \
	{								\
	  (Current).first_line   = FFRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = FFRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = FFRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = FFRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    FFRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    FFRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (FFID (0))
#endif


/* FF_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef FF_LOCATION_PRINT
# if FFLTYPE_IS_TRIVIAL
#  define FF_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define FF_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* FFLEX -- calling `fflex' with the right arguments.  */

#ifdef FFLEX_PARAM
# define FFLEX fflex (FFLEX_PARAM)
#else
# define FFLEX fflex ()
#endif

/* Enable debugging if requested.  */
#if FFDEBUG

# ifndef FFFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define FFFPRINTF fprintf
# endif

# define FFDPRINTF(Args)			\
do {						\
  if (ffdebug)					\
    FFFPRINTF Args;				\
} while (FFID (0))

# define FF_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (ffdebug)								  \
    {									  \
      FFFPRINTF (stderr, "%s ", Title);					  \
      ff_symbol_print (stderr,						  \
		  Type, Value); \
      FFFPRINTF (stderr, "\n");						  \
    }									  \
} while (FFID (0))


/*--------------------------------.
| Print this symbol on FFOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
ff_symbol_value_print (FILE *ffoutput, int fftype, FFSTYPE const * const ffvaluep)
#else
static void
ff_symbol_value_print (ffoutput, fftype, ffvaluep)
    FILE *ffoutput;
    int fftype;
    FFSTYPE const * const ffvaluep;
#endif
{
  if (!ffvaluep)
    return;
# ifdef FFPRINT
  if (fftype < FFNTOKENS)
    FFPRINT (ffoutput, fftoknum[fftype], *ffvaluep);
# else
  FFUSE (ffoutput);
# endif
  switch (fftype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on FFOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
ff_symbol_print (FILE *ffoutput, int fftype, FFSTYPE const * const ffvaluep)
#else
static void
ff_symbol_print (ffoutput, fftype, ffvaluep)
    FILE *ffoutput;
    int fftype;
    FFSTYPE const * const ffvaluep;
#endif
{
  if (fftype < FFNTOKENS)
    FFFPRINTF (ffoutput, "token %s (", fftname[fftype]);
  else
    FFFPRINTF (ffoutput, "nterm %s (", fftname[fftype]);

  ff_symbol_value_print (ffoutput, fftype, ffvaluep);
  FFFPRINTF (ffoutput, ")");
}

/*------------------------------------------------------------------.
| ff_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
ff_stack_print (fftype_int16 *ffbottom, fftype_int16 *fftop)
#else
static void
ff_stack_print (ffbottom, fftop)
    fftype_int16 *ffbottom;
    fftype_int16 *fftop;
#endif
{
  FFFPRINTF (stderr, "Stack now");
  for (; ffbottom <= fftop; ffbottom++)
    {
      int ffbot = *ffbottom;
      FFFPRINTF (stderr, " %d", ffbot);
    }
  FFFPRINTF (stderr, "\n");
}

# define FF_STACK_PRINT(Bottom, Top)				\
do {								\
  if (ffdebug)							\
    ff_stack_print ((Bottom), (Top));				\
} while (FFID (0))


/*------------------------------------------------.
| Report that the FFRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
ff_reduce_print (FFSTYPE *ffvsp, int ffrule)
#else
static void
ff_reduce_print (ffvsp, ffrule)
    FFSTYPE *ffvsp;
    int ffrule;
#endif
{
  int ffnrhs = ffr2[ffrule];
  int ffi;
  unsigned long int fflno = ffrline[ffrule];
  FFFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     ffrule - 1, fflno);
  /* The symbols being reduced.  */
  for (ffi = 0; ffi < ffnrhs; ffi++)
    {
      FFFPRINTF (stderr, "   $%d = ", ffi + 1);
      ff_symbol_print (stderr, ffrhs[ffprhs[ffrule] + ffi],
		       &(ffvsp[(ffi + 1) - (ffnrhs)])
		       		       );
      FFFPRINTF (stderr, "\n");
    }
}

# define FF_REDUCE_PRINT(Rule)		\
do {					\
  if (ffdebug)				\
    ff_reduce_print (ffvsp, Rule); \
} while (FFID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int ffdebug;
#else /* !FFDEBUG */
# define FFDPRINTF(Args)
# define FF_SYMBOL_PRINT(Title, Type, Value, Location)
# define FF_STACK_PRINT(Bottom, Top)
# define FF_REDUCE_PRINT(Rule)
#endif /* !FFDEBUG */


/* FFINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	FFINITDEPTH
# define FFINITDEPTH 200
#endif

/* FFMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   FFSTACK_ALLOC_MAXIMUM < FFSTACK_BYTES (FFMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef FFMAXDEPTH
# define FFMAXDEPTH 10000
#endif



#if FFERROR_VERBOSE

# ifndef ffstrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define ffstrlen strlen
#  else
/* Return the length of FFSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static FFSIZE_T
ffstrlen (const char *ffstr)
#else
static FFSIZE_T
ffstrlen (ffstr)
    const char *ffstr;
#endif
{
  FFSIZE_T fflen;
  for (fflen = 0; ffstr[fflen]; fflen++)
    continue;
  return fflen;
}
#  endif
# endif

# ifndef ffstpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define ffstpcpy stpcpy
#  else
/* Copy FFSRC to FFDEST, returning the address of the terminating '\0' in
   FFDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
ffstpcpy (char *ffdest, const char *ffsrc)
#else
static char *
ffstpcpy (ffdest, ffsrc)
    char *ffdest;
    const char *ffsrc;
#endif
{
  char *ffd = ffdest;
  const char *ffs = ffsrc;

  while ((*ffd++ = *ffs++) != '\0')
    continue;

  return ffd - 1;
}
#  endif
# endif

# ifndef fftnamerr
/* Copy to FFRES the contents of FFSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for fferror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  FFSTR is taken from fftname.  If FFRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static FFSIZE_T
fftnamerr (char *ffres, const char *ffstr)
{
  if (*ffstr == '"')
    {
      FFSIZE_T ffn = 0;
      char const *ffp = ffstr;

      for (;;)
	switch (*++ffp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++ffp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (ffres)
	      ffres[ffn] = *ffp;
	    ffn++;
	    break;

	  case '"':
	    if (ffres)
	      ffres[ffn] = '\0';
	    return ffn;
	  }
    do_not_strip_quotes: ;
    }

  if (! ffres)
    return ffstrlen (ffstr);

  return ffstpcpy (ffres, ffstr) - ffres;
}
# endif

/* Copy into FFRESULT an error message about the unexpected token
   FFCHAR while in state FFSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If FFRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return FFSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static FFSIZE_T
ffsyntax_error (char *ffresult, int ffstate, int ffchar)
{
  int ffn = ffpact[ffstate];

  if (! (FFPACT_NINF < ffn && ffn <= FFLAST))
    return 0;
  else
    {
      int fftype = FFTRANSLATE (ffchar);
      FFSIZE_T ffsize0 = fftnamerr (0, fftname[fftype]);
      FFSIZE_T ffsize = ffsize0;
      FFSIZE_T ffsize1;
      int ffsize_overflow = 0;
      enum { FFERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *ffarg[FFERROR_VERBOSE_ARGS_MAXIMUM];
      int ffx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      FF_("syntax error, unexpected %s");
      FF_("syntax error, unexpected %s, expecting %s");
      FF_("syntax error, unexpected %s, expecting %s or %s");
      FF_("syntax error, unexpected %s, expecting %s or %s or %s");
      FF_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *fffmt;
      char const *fff;
      static char const ffunexpected[] = "syntax error, unexpected %s";
      static char const ffexpecting[] = ", expecting %s";
      static char const ffor[] = " or %s";
      char ffformat[sizeof ffunexpected
		    + sizeof ffexpecting - 1
		    + ((FFERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof ffor - 1))];
      char const *ffprefix = ffexpecting;

      /* Start FFX at -FFN if negative to avoid negative indexes in
	 FFCHECK.  */
      int ffxbegin = ffn < 0 ? -ffn : 0;

      /* Stay within bounds of both ffcheck and fftname.  */
      int ffchecklim = FFLAST - ffn + 1;
      int ffxend = ffchecklim < FFNTOKENS ? ffchecklim : FFNTOKENS;
      int ffcount = 1;

      ffarg[0] = fftname[fftype];
      fffmt = ffstpcpy (ffformat, ffunexpected);

      for (ffx = ffxbegin; ffx < ffxend; ++ffx)
	if (ffcheck[ffx + ffn] == ffx && ffx != FFTERROR)
	  {
	    if (ffcount == FFERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		ffcount = 1;
		ffsize = ffsize0;
		ffformat[sizeof ffunexpected - 1] = '\0';
		break;
	      }
	    ffarg[ffcount++] = fftname[ffx];
	    ffsize1 = ffsize + fftnamerr (0, fftname[ffx]);
	    ffsize_overflow |= (ffsize1 < ffsize);
	    ffsize = ffsize1;
	    fffmt = ffstpcpy (fffmt, ffprefix);
	    ffprefix = ffor;
	  }

      fff = FF_(ffformat);
      ffsize1 = ffsize + ffstrlen (fff);
      ffsize_overflow |= (ffsize1 < ffsize);
      ffsize = ffsize1;

      if (ffsize_overflow)
	return FFSIZE_MAXIMUM;

      if (ffresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *ffp = ffresult;
	  int ffi = 0;
	  while ((*ffp = *fff) != '\0')
	    {
	      if (*ffp == '%' && fff[1] == 's' && ffi < ffcount)
		{
		  ffp += fftnamerr (ffp, ffarg[ffi++]);
		  fff += 2;
		}
	      else
		{
		  ffp++;
		  fff++;
		}
	    }
	}
      return ffsize;
    }
}
#endif /* FFERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
ffdestruct (const char *ffmsg, int fftype, FFSTYPE *ffvaluep)
#else
static void
ffdestruct (ffmsg, fftype, ffvaluep)
    const char *ffmsg;
    int fftype;
    FFSTYPE *ffvaluep;
#endif
{
  FFUSE (ffvaluep);

  if (!ffmsg)
    ffmsg = "Deleting";
  FF_SYMBOL_PRINT (ffmsg, fftype, ffvaluep, fflocationp);

  switch (fftype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef FFPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int ffparse (void *FFPARSE_PARAM);
#else
int ffparse ();
#endif
#else /* ! FFPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int ffparse (void);
#else
int ffparse ();
#endif
#endif /* ! FFPARSE_PARAM */


/* The lookahead symbol.  */
int ffchar;

/* The semantic value of the lookahead symbol.  */
FFSTYPE fflval;

/* Number of syntax errors so far.  */
int ffnerrs;



/*-------------------------.
| ffparse or ffpush_parse.  |
`-------------------------*/

#ifdef FFPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
ffparse (void *FFPARSE_PARAM)
#else
int
ffparse (FFPARSE_PARAM)
    void *FFPARSE_PARAM;
#endif
#else /* ! FFPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
ffparse (void)
#else
int
ffparse ()

#endif
#endif
{


    int ffstate;
    /* Number of tokens to shift before error messages enabled.  */
    int fferrstatus;

    /* The stacks and their tools:
       `ffss': related to states.
       `ffvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow ffoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    fftype_int16 ffssa[FFINITDEPTH];
    fftype_int16 *ffss;
    fftype_int16 *ffssp;

    /* The semantic value stack.  */
    FFSTYPE ffvsa[FFINITDEPTH];
    FFSTYPE *ffvs;
    FFSTYPE *ffvsp;

    FFSIZE_T ffstacksize;

  int ffn;
  int ffresult;
  /* Lookahead token as an internal (translated) token number.  */
  int fftoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  FFSTYPE ffval;

#if FFERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char ffmsgbuf[128];
  char *ffmsg = ffmsgbuf;
  FFSIZE_T ffmsg_alloc = sizeof ffmsgbuf;
#endif

#define FFPOPSTACK(N)   (ffvsp -= (N), ffssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int fflen = 0;

  fftoken = 0;
  ffss = ffssa;
  ffvs = ffvsa;
  ffstacksize = FFINITDEPTH;

  FFDPRINTF ((stderr, "Starting parse\n"));

  ffstate = 0;
  fferrstatus = 0;
  ffnerrs = 0;
  ffchar = FFEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  ffssp = ffss;
  ffvsp = ffvs;

  goto ffsetstate;

/*------------------------------------------------------------.
| ffnewstate -- Push a new state, which is found in ffstate.  |
`------------------------------------------------------------*/
 ffnewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  ffssp++;

 ffsetstate:
  *ffssp = ffstate;

  if (ffss + ffstacksize - 1 <= ffssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      FFSIZE_T ffsize = ffssp - ffss + 1;

#ifdef ffoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	FFSTYPE *ffvs1 = ffvs;
	fftype_int16 *ffss1 = ffss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if ffoverflow is a macro.  */
	ffoverflow (FF_("memory exhausted"),
		    &ffss1, ffsize * sizeof (*ffssp),
		    &ffvs1, ffsize * sizeof (*ffvsp),
		    &ffstacksize);

	ffss = ffss1;
	ffvs = ffvs1;
      }
#else /* no ffoverflow */
# ifndef FFSTACK_RELOCATE
      goto ffexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (FFMAXDEPTH <= ffstacksize)
	goto ffexhaustedlab;
      ffstacksize *= 2;
      if (FFMAXDEPTH < ffstacksize)
	ffstacksize = FFMAXDEPTH;

      {
	fftype_int16 *ffss1 = ffss;
	union ffalloc *ffptr =
	  (union ffalloc *) FFSTACK_ALLOC (FFSTACK_BYTES (ffstacksize));
	if (! ffptr)
	  goto ffexhaustedlab;
	FFSTACK_RELOCATE (ffss_alloc, ffss);
	FFSTACK_RELOCATE (ffvs_alloc, ffvs);
#  undef FFSTACK_RELOCATE
	if (ffss1 != ffssa)
	  FFSTACK_FREE (ffss1);
      }
# endif
#endif /* no ffoverflow */

      ffssp = ffss + ffsize - 1;
      ffvsp = ffvs + ffsize - 1;

      FFDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) ffstacksize));

      if (ffss + ffstacksize - 1 <= ffssp)
	FFABORT;
    }

  FFDPRINTF ((stderr, "Entering state %d\n", ffstate));

  if (ffstate == FFFINAL)
    FFACCEPT;

  goto ffbackup;

/*-----------.
| ffbackup.  |
`-----------*/
ffbackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  ffn = ffpact[ffstate];
  if (ffn == FFPACT_NINF)
    goto ffdefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* FFCHAR is either FFEMPTY or FFEOF or a valid lookahead symbol.  */
  if (ffchar == FFEMPTY)
    {
      FFDPRINTF ((stderr, "Reading a token: "));
      ffchar = FFLEX;
    }

  if (ffchar <= FFEOF)
    {
      ffchar = fftoken = FFEOF;
      FFDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      fftoken = FFTRANSLATE (ffchar);
      FF_SYMBOL_PRINT ("Next token is", fftoken, &fflval, &fflloc);
    }

  /* If the proper action on seeing token FFTOKEN is to reduce or to
     detect an error, take that action.  */
  ffn += fftoken;
  if (ffn < 0 || FFLAST < ffn || ffcheck[ffn] != fftoken)
    goto ffdefault;
  ffn = fftable[ffn];
  if (ffn <= 0)
    {
      if (ffn == 0 || ffn == FFTABLE_NINF)
	goto fferrlab;
      ffn = -ffn;
      goto ffreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (fferrstatus)
    fferrstatus--;

  /* Shift the lookahead token.  */
  FF_SYMBOL_PRINT ("Shifting", fftoken, &fflval, &fflloc);

  /* Discard the shifted token.  */
  ffchar = FFEMPTY;

  ffstate = ffn;
  *++ffvsp = fflval;

  goto ffnewstate;


/*-----------------------------------------------------------.
| ffdefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
ffdefault:
  ffn = ffdefact[ffstate];
  if (ffn == 0)
    goto fferrlab;
  goto ffreduce;


/*-----------------------------.
| ffreduce -- Do a reduction.  |
`-----------------------------*/
ffreduce:
  /* ffn is the number of a rule to reduce with.  */
  fflen = ffr2[ffn];

  /* If FFLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets FFVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to FFVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that FFVAL may be used uninitialized.  */
  ffval = ffvsp[1-fflen];


  FF_REDUCE_PRINT (ffn);
  switch (ffn)
    {
        case 4:

/* Line 1455 of yacc.c  */
#line 250 "eval.y"
    {}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 252 "eval.y"
    { if( (ffvsp[(1) - (2)].Node)<0 ) {
		     fferror("Couldn't build node structure: out of memory?");
		     FFERROR;  }
                  gParse.resultNode = (ffvsp[(1) - (2)].Node);
		}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 258 "eval.y"
    { if( (ffvsp[(1) - (2)].Node)<0 ) {
		     fferror("Couldn't build node structure: out of memory?");
		     FFERROR;  }
                  gParse.resultNode = (ffvsp[(1) - (2)].Node);
		}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 264 "eval.y"
    { if( (ffvsp[(1) - (2)].Node)<0 ) {
		     fferror("Couldn't build node structure: out of memory?");
		     FFERROR;  } 
                  gParse.resultNode = (ffvsp[(1) - (2)].Node);
		}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 270 "eval.y"
    { if( (ffvsp[(1) - (2)].Node)<0 ) {
		     fferror("Couldn't build node structure: out of memory?");
		     FFERROR;  }
                  gParse.resultNode = (ffvsp[(1) - (2)].Node);
		}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 275 "eval.y"
    {  fferrok;  }
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 279 "eval.y"
    { (ffval.Node) = New_Vector( (ffvsp[(2) - (2)].Node) ); TEST((ffval.Node)); }
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 281 "eval.y"
    {
                  if( gParse.Nodes[(ffvsp[(1) - (3)].Node)].nSubNodes >= MAXSUBS ) {
		     (ffvsp[(1) - (3)].Node) = Close_Vec( (ffvsp[(1) - (3)].Node) ); TEST((ffvsp[(1) - (3)].Node));
		     (ffval.Node) = New_Vector( (ffvsp[(1) - (3)].Node) ); TEST((ffval.Node));
                  } else {
                     (ffval.Node) = (ffvsp[(1) - (3)].Node);
                  }
		  gParse.Nodes[(ffval.Node)].SubNodes[ gParse.Nodes[(ffval.Node)].nSubNodes++ ]
		     = (ffvsp[(3) - (3)].Node);
                }
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 294 "eval.y"
    { (ffval.Node) = New_Vector( (ffvsp[(2) - (2)].Node) ); TEST((ffval.Node)); }
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 296 "eval.y"
    {
                  if( TYPE((ffvsp[(1) - (3)].Node)) < TYPE((ffvsp[(3) - (3)].Node)) )
                     TYPE((ffvsp[(1) - (3)].Node)) = TYPE((ffvsp[(3) - (3)].Node));
                  if( gParse.Nodes[(ffvsp[(1) - (3)].Node)].nSubNodes >= MAXSUBS ) {
		     (ffvsp[(1) - (3)].Node) = Close_Vec( (ffvsp[(1) - (3)].Node) ); TEST((ffvsp[(1) - (3)].Node));
		     (ffval.Node) = New_Vector( (ffvsp[(1) - (3)].Node) ); TEST((ffval.Node));
                  } else {
                     (ffval.Node) = (ffvsp[(1) - (3)].Node);
                  }
		  gParse.Nodes[(ffval.Node)].SubNodes[ gParse.Nodes[(ffval.Node)].nSubNodes++ ]
		     = (ffvsp[(3) - (3)].Node);
                }
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 309 "eval.y"
    {
                  if( gParse.Nodes[(ffvsp[(1) - (3)].Node)].nSubNodes >= MAXSUBS ) {
		     (ffvsp[(1) - (3)].Node) = Close_Vec( (ffvsp[(1) - (3)].Node) ); TEST((ffvsp[(1) - (3)].Node));
		     (ffval.Node) = New_Vector( (ffvsp[(1) - (3)].Node) ); TEST((ffval.Node));
                  } else {
                     (ffval.Node) = (ffvsp[(1) - (3)].Node);
                  }
		  gParse.Nodes[(ffval.Node)].SubNodes[ gParse.Nodes[(ffval.Node)].nSubNodes++ ]
		     = (ffvsp[(3) - (3)].Node);
                }
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 320 "eval.y"
    {
                  TYPE((ffvsp[(1) - (3)].Node)) = TYPE((ffvsp[(3) - (3)].Node));
                  if( gParse.Nodes[(ffvsp[(1) - (3)].Node)].nSubNodes >= MAXSUBS ) {
		     (ffvsp[(1) - (3)].Node) = Close_Vec( (ffvsp[(1) - (3)].Node) ); TEST((ffvsp[(1) - (3)].Node));
		     (ffval.Node) = New_Vector( (ffvsp[(1) - (3)].Node) ); TEST((ffval.Node));
                  } else {
                     (ffval.Node) = (ffvsp[(1) - (3)].Node);
                  }
		  gParse.Nodes[(ffval.Node)].SubNodes[ gParse.Nodes[(ffval.Node)].nSubNodes++ ]
		     = (ffvsp[(3) - (3)].Node);
                }
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 334 "eval.y"
    { (ffval.Node) = Close_Vec( (ffvsp[(1) - (2)].Node) ); TEST((ffval.Node)); }
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 338 "eval.y"
    { (ffval.Node) = Close_Vec( (ffvsp[(1) - (2)].Node) ); TEST((ffval.Node)); }
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 342 "eval.y"
    {
                  (ffval.Node) = New_Const( BITSTR, (ffvsp[(1) - (1)].str), strlen((ffvsp[(1) - (1)].str))+1 ); TEST((ffval.Node));
		  SIZE((ffval.Node)) = strlen((ffvsp[(1) - (1)].str)); }
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 346 "eval.y"
    { (ffval.Node) = New_Column( (ffvsp[(1) - (1)].lng) ); TEST((ffval.Node)); }
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 348 "eval.y"
    {
                  if( TYPE((ffvsp[(3) - (4)].Node)) != LONG
		      || OPER((ffvsp[(3) - (4)].Node)) != CONST_OP ) {
		     fferror("Offset argument must be a constant integer");
		     FFERROR;
		  }
                  (ffval.Node) = New_Offset( (ffvsp[(1) - (4)].lng), (ffvsp[(3) - (4)].Node) ); TEST((ffval.Node));
                }
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 357 "eval.y"
    { (ffval.Node) = New_BinOp( BITSTR, (ffvsp[(1) - (3)].Node), '&', (ffvsp[(3) - (3)].Node) ); TEST((ffval.Node));
                  SIZE((ffval.Node)) = ( SIZE((ffvsp[(1) - (3)].Node))>SIZE((ffvsp[(3) - (3)].Node)) ? SIZE((ffvsp[(1) - (3)].Node)) : SIZE((ffvsp[(3) - (3)].Node)) );  }
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 360 "eval.y"
    { (ffval.Node) = New_BinOp( BITSTR, (ffvsp[(1) - (3)].Node), '|', (ffvsp[(3) - (3)].Node) ); TEST((ffval.Node));
                  SIZE((ffval.Node)) = ( SIZE((ffvsp[(1) - (3)].Node))>SIZE((ffvsp[(3) - (3)].Node)) ? SIZE((ffvsp[(1) - (3)].Node)) : SIZE((ffvsp[(3) - (3)].Node)) );  }
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 363 "eval.y"
    { 
		  if (SIZE((ffvsp[(1) - (3)].Node))+SIZE((ffvsp[(3) - (3)].Node)) >= MAX_STRLEN) {
		    fferror("Combined bit string size exceeds " MAX_STRLEN_S " bits");
		    FFERROR;
		  }
		  (ffval.Node) = New_BinOp( BITSTR, (ffvsp[(1) - (3)].Node), '+', (ffvsp[(3) - (3)].Node) ); TEST((ffval.Node));
                  SIZE((ffval.Node)) = SIZE((ffvsp[(1) - (3)].Node)) + SIZE((ffvsp[(3) - (3)].Node)); 
		}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 372 "eval.y"
    { (ffval.Node) = New_Deref( (ffvsp[(1) - (4)].Node), 1, (ffvsp[(3) - (4)].Node),  0,  0,  0,   0 ); TEST((ffval.Node)); }
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 374 "eval.y"
    { (ffval.Node) = New_Deref( (ffvsp[(1) - (6)].Node), 2, (ffvsp[(3) - (6)].Node), (ffvsp[(5) - (6)].Node),  0,  0,   0 ); TEST((ffval.Node)); }
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 376 "eval.y"
    { (ffval.Node) = New_Deref( (ffvsp[(1) - (8)].Node), 3, (ffvsp[(3) - (8)].Node), (ffvsp[(5) - (8)].Node), (ffvsp[(7) - (8)].Node),  0,   0 ); TEST((ffval.Node)); }
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 378 "eval.y"
    { (ffval.Node) = New_Deref( (ffvsp[(1) - (10)].Node), 4, (ffvsp[(3) - (10)].Node), (ffvsp[(5) - (10)].Node), (ffvsp[(7) - (10)].Node), (ffvsp[(9) - (10)].Node),   0 ); TEST((ffval.Node)); }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 380 "eval.y"
    { (ffval.Node) = New_Deref( (ffvsp[(1) - (12)].Node), 5, (ffvsp[(3) - (12)].Node), (ffvsp[(5) - (12)].Node), (ffvsp[(7) - (12)].Node), (ffvsp[(9) - (12)].Node), (ffvsp[(11) - (12)].Node) ); TEST((ffval.Node)); }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 382 "eval.y"
    { (ffval.Node) = New_Unary( BITSTR, NOT, (ffvsp[(2) - (2)].Node) ); TEST((ffval.Node));     }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 385 "eval.y"
    { (ffval.Node) = (ffvsp[(2) - (3)].Node); }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 389 "eval.y"
    { (ffval.Node) = New_Const( LONG,   &((ffvsp[(1) - (1)].lng)), sizeof(long)   ); TEST((ffval.Node)); }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 391 "eval.y"
    { (ffval.Node) = New_Const( DOUBLE, &((ffvsp[(1) - (1)].dbl)), sizeof(double) ); TEST((ffval.Node)); }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 393 "eval.y"
    { (ffval.Node) = New_Column( (ffvsp[(1) - (1)].lng) ); TEST((ffval.Node)); }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 395 "eval.y"
    {
                  if( TYPE((ffvsp[(3) - (4)].Node)) != LONG
		      || OPER((ffvsp[(3) - (4)].Node)) != CONST_OP ) {
		     fferror("Offset argument must be a constant integer");
		     FFERROR;
		  }
                  (ffval.Node) = New_Offset( (ffvsp[(1) - (4)].lng), (ffvsp[(3) - (4)].Node) ); TEST((ffval.Node));
                }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 404 "eval.y"
    { (ffval.Node) = New_Func( LONG, row_fct,  0, 0, 0, 0, 0, 0, 0, 0 ); }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 406 "eval.y"
    { (ffval.Node) = New_Func( LONG, null_fct, 0, 0, 0, 0, 0, 0, 0, 0 ); }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 408 "eval.y"
    { PROMOTE((ffvsp[(1) - (3)].Node),(ffvsp[(3) - (3)].Node)); (ffval.Node) = New_BinOp( TYPE((ffvsp[(1) - (3)].Node)), (ffvsp[(1) - (3)].Node), '%', (ffvsp[(3) - (3)].Node) );
		  TEST((ffval.Node));                                                }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 411 "eval.y"
    { PROMOTE((ffvsp[(1) - (3)].Node),(ffvsp[(3) - (3)].Node)); (ffval.Node) = New_BinOp( TYPE((ffvsp[(1) - (3)].Node)), (ffvsp[(1) - (3)].Node), '+', (ffvsp[(3) - (3)].Node) );
		  TEST((ffval.Node));                                                }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 414 "eval.y"
    { PROMOTE((ffvsp[(1) - (3)].Node),(ffvsp[(3) - (3)].Node)); (ffval.Node) = New_BinOp( TYPE((ffvsp[(1) - (3)].Node)), (ffvsp[(1) - (3)].Node), '-', (ffvsp[(3) - (3)].Node) ); 
		  TEST((ffval.Node));                                                }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 417 "eval.y"
    { PROMOTE((ffvsp[(1) - (3)].Node),(ffvsp[(3) - (3)].Node)); (ffval.Node) = New_BinOp( TYPE((ffvsp[(1) - (3)].Node)), (ffvsp[(1) - (3)].Node), '*', (ffvsp[(3) - (3)].Node) ); 
		  TEST((ffval.Node));                                                }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 420 "eval.y"
    { PROMOTE((ffvsp[(1) - (3)].Node),(ffvsp[(3) - (3)].Node)); (ffval.Node) = New_BinOp( TYPE((ffvsp[(1) - (3)].Node)), (ffvsp[(1) - (3)].Node), '/', (ffvsp[(3) - (3)].Node) ); 
		  TEST((ffval.Node));                                                }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 423 "eval.y"
    { 
                   if (TYPE((ffvsp[(1) - (3)].Node)) != LONG ||
		       TYPE((ffvsp[(3) - (3)].Node)) != LONG) {
                     fferror("Bitwise operations with incompatible types; only (bit OP bit) and (int OP int) are allowed");
                      FFERROR;
                   }
                   (ffval.Node) = New_BinOp( TYPE((ffvsp[(1) - (3)].Node)), (ffvsp[(1) - (3)].Node), '&', (ffvsp[(3) - (3)].Node) );
                }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 432 "eval.y"
    { 
                   if (TYPE((ffvsp[(1) - (3)].Node)) != LONG ||
		       TYPE((ffvsp[(3) - (3)].Node)) != LONG) {
                     fferror("Bitwise operations with incompatible types; only (bit OP bit) and (int OP int) are allowed");
                      FFERROR;
                   }
                   (ffval.Node) = New_BinOp( TYPE((ffvsp[(1) - (3)].Node)), (ffvsp[(1) - (3)].Node), '|', (ffvsp[(3) - (3)].Node) );
                }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 441 "eval.y"
    { 
                   if (TYPE((ffvsp[(1) - (3)].Node)) != LONG ||
		       TYPE((ffvsp[(3) - (3)].Node)) != LONG) {
                     fferror("Bitwise operations with incompatible types; only (bit OP bit) and (int OP int) are allowed");
                      FFERROR;
                   }
                   (ffval.Node) = New_BinOp( TYPE((ffvsp[(1) - (3)].Node)), (ffvsp[(1) - (3)].Node), '^', (ffvsp[(3) - (3)].Node) );
                }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 450 "eval.y"
    { PROMOTE((ffvsp[(1) - (3)].Node),(ffvsp[(3) - (3)].Node)); (ffval.Node) = New_BinOp( TYPE((ffvsp[(1) - (3)].Node)), (ffvsp[(1) - (3)].Node), POWER, (ffvsp[(3) - (3)].Node) );
		  TEST((ffval.Node));                                                }
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 453 "eval.y"
    { (ffval.Node) = (ffvsp[(2) - (2)].Node); }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 455 "eval.y"
    { (ffval.Node) = New_Unary( TYPE((ffvsp[(2) - (2)].Node)), UMINUS, (ffvsp[(2) - (2)].Node) ); TEST((ffval.Node)); }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 457 "eval.y"
    { (ffval.Node) = (ffvsp[(2) - (3)].Node); }
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 459 "eval.y"
    { (ffvsp[(3) - (3)].Node) = New_Unary( TYPE((ffvsp[(1) - (3)].Node)), 0, (ffvsp[(3) - (3)].Node) );
                  (ffval.Node) = New_BinOp( TYPE((ffvsp[(1) - (3)].Node)), (ffvsp[(1) - (3)].Node), '*', (ffvsp[(3) - (3)].Node) ); 
		  TEST((ffval.Node));                                }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 463 "eval.y"
    { (ffvsp[(1) - (3)].Node) = New_Unary( TYPE((ffvsp[(3) - (3)].Node)), 0, (ffvsp[(1) - (3)].Node) );
                  (ffval.Node) = New_BinOp( TYPE((ffvsp[(3) - (3)].Node)), (ffvsp[(1) - (3)].Node), '*', (ffvsp[(3) - (3)].Node) );
                  TEST((ffval.Node));                                }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 467 "eval.y"
    {
                  PROMOTE((ffvsp[(3) - (5)].Node),(ffvsp[(5) - (5)].Node));
                  if( ! Test_Dims((ffvsp[(3) - (5)].Node),(ffvsp[(5) - (5)].Node)) ) {
                     fferror("Incompatible dimensions in '?:' arguments");
		     FFERROR;
                  }
                  (ffval.Node) = New_Func( 0, ifthenelse_fct, 3, (ffvsp[(3) - (5)].Node), (ffvsp[(5) - (5)].Node), (ffvsp[(1) - (5)].Node),
                                 0, 0, 0, 0 );
                  TEST((ffval.Node));
                  if( SIZE((ffvsp[(3) - (5)].Node))<SIZE((ffvsp[(5) - (5)].Node)) )  Copy_Dims((ffval.Node), (ffvsp[(5) - (5)].Node));
                  TYPE((ffvsp[(1) - (5)].Node)) = TYPE((ffvsp[(3) - (5)].Node));
                  if( ! Test_Dims((ffvsp[(1) - (5)].Node),(ffval.Node)) ) {
                     fferror("Incompatible dimensions in '?:' condition");
		     FFERROR;
                  }
                  TYPE((ffvsp[(1) - (5)].Node)) = BOOLEAN;
                  if( SIZE((ffval.Node))<SIZE((ffvsp[(1) - (5)].Node)) )  Copy_Dims((ffval.Node), (ffvsp[(1) - (5)].Node));
                }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 486 "eval.y"
    {
                  PROMOTE((ffvsp[(3) - (5)].Node),(ffvsp[(5) - (5)].Node));
                  if( ! Test_Dims((ffvsp[(3) - (5)].Node),(ffvsp[(5) - (5)].Node)) ) {
                     fferror("Incompatible dimensions in '?:' arguments");
		     FFERROR;
                  }
                  (ffval.Node) = New_Func( 0, ifthenelse_fct, 3, (ffvsp[(3) - (5)].Node), (ffvsp[(5) - (5)].Node), (ffvsp[(1) - (5)].Node),
                                 0, 0, 0, 0 );
                  TEST((ffval.Node));
                  if( SIZE((ffvsp[(3) - (5)].Node))<SIZE((ffvsp[(5) - (5)].Node)) )  Copy_Dims((ffval.Node), (ffvsp[(5) - (5)].Node));
                  TYPE((ffvsp[(1) - (5)].Node)) = TYPE((ffvsp[(3) - (5)].Node));
                  if( ! Test_Dims((ffvsp[(1) - (5)].Node),(ffval.Node)) ) {
                     fferror("Incompatible dimensions in '?:' condition");
		     FFERROR;
                  }
                  TYPE((ffvsp[(1) - (5)].Node)) = BOOLEAN;
                  if( SIZE((ffval.Node))<SIZE((ffvsp[(1) - (5)].Node)) )  Copy_Dims((ffval.Node), (ffvsp[(1) - (5)].Node));
                }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 505 "eval.y"
    {
                  PROMOTE((ffvsp[(3) - (5)].Node),(ffvsp[(5) - (5)].Node));
                  if( ! Test_Dims((ffvsp[(3) - (5)].Node),(ffvsp[(5) - (5)].Node)) ) {
                     fferror("Incompatible dimensions in '?:' arguments");
		     FFERROR;
                  }
                  (ffval.Node) = New_Func( 0, ifthenelse_fct, 3, (ffvsp[(3) - (5)].Node), (ffvsp[(5) - (5)].Node), (ffvsp[(1) - (5)].Node),
                                 0, 0, 0, 0 );
                  TEST((ffval.Node));
                  if( SIZE((ffvsp[(3) - (5)].Node))<SIZE((ffvsp[(5) - (5)].Node)) )  Copy_Dims((ffval.Node), (ffvsp[(5) - (5)].Node));
                  TYPE((ffvsp[(1) - (5)].Node)) = TYPE((ffvsp[(3) - (5)].Node));
                  if( ! Test_Dims((ffvsp[(1) - (5)].Node),(ffval.Node)) ) {
                     fferror("Incompatible dimensions in '?:' condition");
		     FFERROR;
                  }
                  TYPE((ffvsp[(1) - (5)].Node)) = BOOLEAN;
                  if( SIZE((ffval.Node))<SIZE((ffvsp[(1) - (5)].Node)) )  Copy_Dims((ffval.Node), (ffvsp[(1) - (5)].Node));
                }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 524 "eval.y"
    { if (FSTRCMP((ffvsp[(1) - (2)].str),"RANDOM(") == 0) {  /* Scalar RANDOM() */
                     (ffval.Node) = New_Func( DOUBLE, rnd_fct, 0, 0, 0, 0, 0, 0, 0, 0 );
		  } else if (FSTRCMP((ffvsp[(1) - (2)].str),"RANDOMN(") == 0) {/*Scalar RANDOMN()*/
		     (ffval.Node) = New_Func( DOUBLE, gasrnd_fct, 0, 0, 0, 0, 0, 0, 0, 0 );
                  } else {
                     fferror("Function() not supported");
		     FFERROR;
		  }
                  TEST((ffval.Node)); 
                }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 535 "eval.y"
    { if (FSTRCMP((ffvsp[(1) - (3)].str),"SUM(") == 0) {
		     (ffval.Node) = New_Func( LONG, sum_fct, 1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
                  } else if (FSTRCMP((ffvsp[(1) - (3)].str),"NELEM(") == 0) {
                     (ffval.Node) = New_Const( LONG, &( SIZE((ffvsp[(2) - (3)].Node)) ), sizeof(long) );
                  } else if (FSTRCMP((ffvsp[(1) - (3)].str),"ACCUM(") == 0) {
		    long zero = 0;
		    (ffval.Node) = New_BinOp( LONG , (ffvsp[(2) - (3)].Node), ACCUM, New_Const( LONG, &zero, sizeof(zero) ));
		  } else {
                     fferror("Function(bool) not supported");
		     FFERROR;
		  }
                  TEST((ffval.Node)); 
		}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 549 "eval.y"
    { if (FSTRCMP((ffvsp[(1) - (3)].str),"NELEM(") == 0) {
                     (ffval.Node) = New_Const( LONG, &( SIZE((ffvsp[(2) - (3)].Node)) ), sizeof(long) );
		  } else if (FSTRCMP((ffvsp[(1) - (3)].str),"NVALID(") == 0) {
		     (ffval.Node) = New_Func( LONG, nonnull_fct, 1, (ffvsp[(2) - (3)].Node),
				    0, 0, 0, 0, 0, 0 );
		  } else {
                     fferror("Function(str) not supported");
		     FFERROR;
		  }
                  TEST((ffval.Node)); 
		}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 561 "eval.y"
    { if (FSTRCMP((ffvsp[(1) - (3)].str),"NELEM(") == 0) {
                     (ffval.Node) = New_Const( LONG, &( SIZE((ffvsp[(2) - (3)].Node)) ), sizeof(long) );
		} else if (FSTRCMP((ffvsp[(1) - (3)].str),"NVALID(") == 0) { /* Bit arrays do not have NULL */
                     (ffval.Node) = New_Const( LONG, &( SIZE((ffvsp[(2) - (3)].Node)) ), sizeof(long) );
		} else if (FSTRCMP((ffvsp[(1) - (3)].str),"SUM(") == 0) {
		     (ffval.Node) = New_Func( LONG, sum_fct, 1, (ffvsp[(2) - (3)].Node),
				    0, 0, 0, 0, 0, 0 );
		} else if (FSTRCMP((ffvsp[(1) - (3)].str),"MIN(") == 0) {
		     (ffval.Node) = New_Func( TYPE((ffvsp[(2) - (3)].Node)),  /* Force 1D result */
				    min1_fct, 1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
		     /* Note: $2 is a vector so the result can never
		        be a constant.  Therefore it will never be set
		        inside New_Func(), and it is safe to set SIZE() */
		     SIZE((ffval.Node)) = 1;
		} else if (FSTRCMP((ffvsp[(1) - (3)].str),"ACCUM(") == 0) {
		    long zero = 0;
		    (ffval.Node) = New_BinOp( LONG , (ffvsp[(2) - (3)].Node), ACCUM, New_Const( LONG, &zero, sizeof(zero) ));
		} else if (FSTRCMP((ffvsp[(1) - (3)].str),"MAX(") == 0) {
		     (ffval.Node) = New_Func( TYPE((ffvsp[(2) - (3)].Node)),  /* Force 1D result */
				    max1_fct, 1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
		     /* Note: $2 is a vector so the result can never
		        be a constant.  Therefore it will never be set
		        inside New_Func(), and it is safe to set SIZE() */
		     SIZE((ffval.Node)) = 1;
		} else {
                     fferror("Function(bits) not supported");
		     FFERROR;
		  }
                  TEST((ffval.Node)); 
		}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 592 "eval.y"
    { if (FSTRCMP((ffvsp[(1) - (3)].str),"SUM(") == 0)
		     (ffval.Node) = New_Func( TYPE((ffvsp[(2) - (3)].Node)), sum_fct, 1, (ffvsp[(2) - (3)].Node),
				    0, 0, 0, 0, 0, 0 );
		  else if (FSTRCMP((ffvsp[(1) - (3)].str),"AVERAGE(") == 0)
		     (ffval.Node) = New_Func( DOUBLE, average_fct, 1, (ffvsp[(2) - (3)].Node),
				    0, 0, 0, 0, 0, 0 );
		  else if (FSTRCMP((ffvsp[(1) - (3)].str),"STDDEV(") == 0)
		     (ffval.Node) = New_Func( DOUBLE, stddev_fct, 1, (ffvsp[(2) - (3)].Node),
				    0, 0, 0, 0, 0, 0 );
		  else if (FSTRCMP((ffvsp[(1) - (3)].str),"MEDIAN(") == 0)
		     (ffval.Node) = New_Func( TYPE((ffvsp[(2) - (3)].Node)), median_fct, 1, (ffvsp[(2) - (3)].Node),
				    0, 0, 0, 0, 0, 0 );
		  else if (FSTRCMP((ffvsp[(1) - (3)].str),"NELEM(") == 0)
                     (ffval.Node) = New_Const( LONG, &( SIZE((ffvsp[(2) - (3)].Node)) ), sizeof(long) );
		  else if (FSTRCMP((ffvsp[(1) - (3)].str),"NVALID(") == 0)
		     (ffval.Node) = New_Func( LONG, nonnull_fct, 1, (ffvsp[(2) - (3)].Node),
				    0, 0, 0, 0, 0, 0 );
		  else if   ((FSTRCMP((ffvsp[(1) - (3)].str),"ACCUM(") == 0) && (TYPE((ffvsp[(2) - (3)].Node)) == LONG)) {
		    long zero = 0;
		    (ffval.Node) = New_BinOp( LONG ,   (ffvsp[(2) - (3)].Node), ACCUM, New_Const( LONG,   &zero, sizeof(zero) ));
		  } else if ((FSTRCMP((ffvsp[(1) - (3)].str),"ACCUM(") == 0) && (TYPE((ffvsp[(2) - (3)].Node)) == DOUBLE)) {
		    double zero = 0;
		    (ffval.Node) = New_BinOp( DOUBLE , (ffvsp[(2) - (3)].Node), ACCUM, New_Const( DOUBLE, &zero, sizeof(zero) ));
		  } else if ((FSTRCMP((ffvsp[(1) - (3)].str),"SEQDIFF(") == 0) && (TYPE((ffvsp[(2) - (3)].Node)) == LONG)) {
		    long zero = 0;
		    (ffval.Node) = New_BinOp( LONG ,   (ffvsp[(2) - (3)].Node), DIFF, New_Const( LONG,   &zero, sizeof(zero) ));
		  } else if ((FSTRCMP((ffvsp[(1) - (3)].str),"SEQDIFF(") == 0) && (TYPE((ffvsp[(2) - (3)].Node)) == DOUBLE)) {
		    double zero = 0;
		    (ffval.Node) = New_BinOp( DOUBLE , (ffvsp[(2) - (3)].Node), DIFF, New_Const( DOUBLE, &zero, sizeof(zero) ));
		  } else if (FSTRCMP((ffvsp[(1) - (3)].str),"ABS(") == 0)
		     (ffval.Node) = New_Func( 0, abs_fct, 1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
 		  else if (FSTRCMP((ffvsp[(1) - (3)].str),"MIN(") == 0)
		     (ffval.Node) = New_Func( TYPE((ffvsp[(2) - (3)].Node)),  /* Force 1D result */
				    min1_fct, 1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
		  else if (FSTRCMP((ffvsp[(1) - (3)].str),"MAX(") == 0)
		     (ffval.Node) = New_Func( TYPE((ffvsp[(2) - (3)].Node)),  /* Force 1D result */
				    max1_fct, 1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
		  else if (FSTRCMP((ffvsp[(1) - (3)].str),"RANDOM(") == 0) { /* Vector RANDOM() */
                     (ffval.Node) = New_Func( 0, rnd_fct, 1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
		     TEST((ffval.Node));
		     TYPE((ffval.Node)) = DOUBLE;
		  } else if (FSTRCMP((ffvsp[(1) - (3)].str),"RANDOMN(") == 0) {
		     (ffval.Node) = New_Func( 0, gasrnd_fct, 1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
		     TEST((ffval.Node));
		     TYPE((ffval.Node)) = DOUBLE;
                  } 
  		  else {  /*  These all take DOUBLE arguments  */
		     if( TYPE((ffvsp[(2) - (3)].Node)) != DOUBLE ) (ffvsp[(2) - (3)].Node) = New_Unary( DOUBLE, 0, (ffvsp[(2) - (3)].Node) );
                     if (FSTRCMP((ffvsp[(1) - (3)].str),"SIN(") == 0)
			(ffval.Node) = New_Func( 0, sin_fct,  1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[(1) - (3)].str),"COS(") == 0)
			(ffval.Node) = New_Func( 0, cos_fct,  1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[(1) - (3)].str),"TAN(") == 0)
			(ffval.Node) = New_Func( 0, tan_fct,  1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[(1) - (3)].str),"ARCSIN(") == 0
			      || FSTRCMP((ffvsp[(1) - (3)].str),"ASIN(") == 0)
			(ffval.Node) = New_Func( 0, asin_fct, 1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[(1) - (3)].str),"ARCCOS(") == 0
			      || FSTRCMP((ffvsp[(1) - (3)].str),"ACOS(") == 0)
			(ffval.Node) = New_Func( 0, acos_fct, 1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[(1) - (3)].str),"ARCTAN(") == 0
			      || FSTRCMP((ffvsp[(1) - (3)].str),"ATAN(") == 0)
			(ffval.Node) = New_Func( 0, atan_fct, 1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[(1) - (3)].str),"SINH(") == 0)
			(ffval.Node) = New_Func( 0, sinh_fct,  1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[(1) - (3)].str),"COSH(") == 0)
			(ffval.Node) = New_Func( 0, cosh_fct,  1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[(1) - (3)].str),"TANH(") == 0)
			(ffval.Node) = New_Func( 0, tanh_fct,  1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[(1) - (3)].str),"EXP(") == 0)
			(ffval.Node) = New_Func( 0, exp_fct,  1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[(1) - (3)].str),"LOG(") == 0)
			(ffval.Node) = New_Func( 0, log_fct,  1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[(1) - (3)].str),"LOG10(") == 0)
			(ffval.Node) = New_Func( 0, log10_fct, 1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[(1) - (3)].str),"SQRT(") == 0)
			(ffval.Node) = New_Func( 0, sqrt_fct, 1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[(1) - (3)].str),"ROUND(") == 0)
			(ffval.Node) = New_Func( 0, round_fct, 1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[(1) - (3)].str),"FLOOR(") == 0)
			(ffval.Node) = New_Func( 0, floor_fct, 1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[(1) - (3)].str),"CEIL(") == 0)
			(ffval.Node) = New_Func( 0, ceil_fct, 1, (ffvsp[(2) - (3)].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[(1) - (3)].str),"RANDOMP(") == 0) {
		       (ffval.Node) = New_Func( 0, poirnd_fct, 1, (ffvsp[(2) - (3)].Node), 
				      0, 0, 0, 0, 0, 0 );
		       TYPE((ffval.Node)) = LONG;
		     } else {
			fferror("Function(expr) not supported");
			FFERROR;
		     }
		  }
                  TEST((ffval.Node)); 
                }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 687 "eval.y"
    { 
		  if (FSTRCMP((ffvsp[(1) - (5)].str),"STRSTR(") == 0) {
		    (ffval.Node) = New_Func( LONG, strpos_fct, 2, (ffvsp[(2) - (5)].Node), (ffvsp[(4) - (5)].Node), 0, 
				   0, 0, 0, 0 );
		    TEST((ffval.Node));
		  }
                }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 695 "eval.y"
    { 
		   if (FSTRCMP((ffvsp[(1) - (5)].str),"DEFNULL(") == 0) {
		      if( SIZE((ffvsp[(2) - (5)].Node))>=SIZE((ffvsp[(4) - (5)].Node)) && Test_Dims( (ffvsp[(2) - (5)].Node), (ffvsp[(4) - (5)].Node) ) ) {
			 PROMOTE((ffvsp[(2) - (5)].Node),(ffvsp[(4) - (5)].Node));
			 (ffval.Node) = New_Func( 0, defnull_fct, 2, (ffvsp[(2) - (5)].Node), (ffvsp[(4) - (5)].Node), 0,
					0, 0, 0, 0 );
			 TEST((ffval.Node)); 
		      } else {
			 fferror("Dimensions of DEFNULL arguments "
				 "are not compatible");
			 FFERROR;
		      }
		   } else if (FSTRCMP((ffvsp[(1) - (5)].str),"ARCTAN2(") == 0) {
		     if( TYPE((ffvsp[(2) - (5)].Node)) != DOUBLE ) (ffvsp[(2) - (5)].Node) = New_Unary( DOUBLE, 0, (ffvsp[(2) - (5)].Node) );
		     if( TYPE((ffvsp[(4) - (5)].Node)) != DOUBLE ) (ffvsp[(4) - (5)].Node) = New_Unary( DOUBLE, 0, (ffvsp[(4) - (5)].Node) );
		     if( Test_Dims( (ffvsp[(2) - (5)].Node), (ffvsp[(4) - (5)].Node) ) ) {
			(ffval.Node) = New_Func( 0, atan2_fct, 2, (ffvsp[(2) - (5)].Node), (ffvsp[(4) - (5)].Node), 0, 0, 0, 0, 0 );
			TEST((ffval.Node)); 
			if( SIZE((ffvsp[(2) - (5)].Node))<SIZE((ffvsp[(4) - (5)].Node)) ) Copy_Dims((ffval.Node), (ffvsp[(4) - (5)].Node));
		     } else {
			fferror("Dimensions of arctan2 arguments "
				"are not compatible");
			FFERROR;
		     }
		   } else if (FSTRCMP((ffvsp[(1) - (5)].str),"MIN(") == 0) {
		      PROMOTE( (ffvsp[(2) - (5)].Node), (ffvsp[(4) - (5)].Node) );
		      if( Test_Dims( (ffvsp[(2) - (5)].Node), (ffvsp[(4) - (5)].Node) ) ) {
			(ffval.Node) = New_Func( 0, min2_fct, 2, (ffvsp[(2) - (5)].Node), (ffvsp[(4) - (5)].Node), 0, 0, 0, 0, 0 );
			TEST((ffval.Node));
			if( SIZE((ffvsp[(2) - (5)].Node))<SIZE((ffvsp[(4) - (5)].Node)) ) Copy_Dims((ffval.Node), (ffvsp[(4) - (5)].Node));
		      } else {
			fferror("Dimensions of min(a,b) arguments "
				"are not compatible");
			FFERROR;
		      }
		   } else if (FSTRCMP((ffvsp[(1) - (5)].str),"MAX(") == 0) {
		      PROMOTE( (ffvsp[(2) - (5)].Node), (ffvsp[(4) - (5)].Node) );
		      if( Test_Dims( (ffvsp[(2) - (5)].Node), (ffvsp[(4) - (5)].Node) ) ) {
			(ffval.Node) = New_Func( 0, max2_fct, 2, (ffvsp[(2) - (5)].Node), (ffvsp[(4) - (5)].Node), 0, 0, 0, 0, 0 );
			TEST((ffval.Node));
			if( SIZE((ffvsp[(2) - (5)].Node))<SIZE((ffvsp[(4) - (5)].Node)) ) Copy_Dims((ffval.Node), (ffvsp[(4) - (5)].Node));
		      } else {
			fferror("Dimensions of max(a,b) arguments "
				"are not compatible");
			FFERROR;
		      }
		   } else if (FSTRCMP((ffvsp[(1) - (5)].str),"SETNULL(") == 0) {
		     if (OPER((ffvsp[(2) - (5)].Node)) != CONST_OP
			 || SIZE((ffvsp[(2) - (5)].Node)) != 1) {
		       fferror("SETNULL first argument must be a scalar constant");
		       FFERROR;
		     }
		     /* Make sure first arg is same type as second arg */
		     if ( TYPE((ffvsp[(2) - (5)].Node)) != TYPE((ffvsp[(4) - (5)].Node)) ) (ffvsp[(2) - (5)].Node) = New_Unary( TYPE((ffvsp[(4) - (5)].Node)), 0, (ffvsp[(2) - (5)].Node) );
		     (ffval.Node) = New_Func( 0, setnull_fct, 2, (ffvsp[(4) - (5)].Node), (ffvsp[(2) - (5)].Node), 0, 0, 0, 0, 0 );
		   } else {
		      fferror("Function(expr,expr) not supported");
		      FFERROR;
		   }
                }
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 756 "eval.y"
    { 
		  if (FSTRCMP((ffvsp[(1) - (9)].str),"ANGSEP(") == 0) {
		    if( TYPE((ffvsp[(2) - (9)].Node)) != DOUBLE ) (ffvsp[(2) - (9)].Node) = New_Unary( DOUBLE, 0, (ffvsp[(2) - (9)].Node) );
		    if( TYPE((ffvsp[(4) - (9)].Node)) != DOUBLE ) (ffvsp[(4) - (9)].Node) = New_Unary( DOUBLE, 0, (ffvsp[(4) - (9)].Node) );
		    if( TYPE((ffvsp[(6) - (9)].Node)) != DOUBLE ) (ffvsp[(6) - (9)].Node) = New_Unary( DOUBLE, 0, (ffvsp[(6) - (9)].Node) );
		    if( TYPE((ffvsp[(8) - (9)].Node)) != DOUBLE ) (ffvsp[(8) - (9)].Node) = New_Unary( DOUBLE, 0, (ffvsp[(8) - (9)].Node) );
		    if( Test_Dims( (ffvsp[(2) - (9)].Node), (ffvsp[(4) - (9)].Node) ) && Test_Dims( (ffvsp[(4) - (9)].Node), (ffvsp[(6) - (9)].Node) ) && 
			Test_Dims( (ffvsp[(6) - (9)].Node), (ffvsp[(8) - (9)].Node) ) ) {
		      (ffval.Node) = New_Func( 0, angsep_fct, 4, (ffvsp[(2) - (9)].Node), (ffvsp[(4) - (9)].Node), (ffvsp[(6) - (9)].Node), (ffvsp[(8) - (9)].Node),0,0,0 );
		      TEST((ffval.Node)); 
		      if( SIZE((ffvsp[(2) - (9)].Node))<SIZE((ffvsp[(4) - (9)].Node)) ) Copy_Dims((ffval.Node), (ffvsp[(4) - (9)].Node));
		      if( SIZE((ffvsp[(4) - (9)].Node))<SIZE((ffvsp[(6) - (9)].Node)) ) Copy_Dims((ffval.Node), (ffvsp[(6) - (9)].Node));
		      if( SIZE((ffvsp[(6) - (9)].Node))<SIZE((ffvsp[(8) - (9)].Node)) ) Copy_Dims((ffval.Node), (ffvsp[(8) - (9)].Node));
		    } else {
		      fferror("Dimensions of ANGSEP arguments "
			      "are not compatible");
		      FFERROR;
		    }
		   } else {
		      fferror("Function(expr,expr,expr,expr) not supported");
		      FFERROR;
		   }
                }
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 780 "eval.y"
    { (ffval.Node) = New_Deref( (ffvsp[(1) - (4)].Node), 1, (ffvsp[(3) - (4)].Node),  0,  0,  0,   0 ); TEST((ffval.Node)); }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 782 "eval.y"
    { (ffval.Node) = New_Deref( (ffvsp[(1) - (6)].Node), 2, (ffvsp[(3) - (6)].Node), (ffvsp[(5) - (6)].Node),  0,  0,   0 ); TEST((ffval.Node)); }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 784 "eval.y"
    { (ffval.Node) = New_Deref( (ffvsp[(1) - (8)].Node), 3, (ffvsp[(3) - (8)].Node), (ffvsp[(5) - (8)].Node), (ffvsp[(7) - (8)].Node),  0,   0 ); TEST((ffval.Node)); }
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 786 "eval.y"
    { (ffval.Node) = New_Deref( (ffvsp[(1) - (10)].Node), 4, (ffvsp[(3) - (10)].Node), (ffvsp[(5) - (10)].Node), (ffvsp[(7) - (10)].Node), (ffvsp[(9) - (10)].Node),   0 ); TEST((ffval.Node)); }
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 788 "eval.y"
    { (ffval.Node) = New_Deref( (ffvsp[(1) - (12)].Node), 5, (ffvsp[(3) - (12)].Node), (ffvsp[(5) - (12)].Node), (ffvsp[(7) - (12)].Node), (ffvsp[(9) - (12)].Node), (ffvsp[(11) - (12)].Node) ); TEST((ffval.Node)); }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 790 "eval.y"
    { (ffval.Node) = New_Unary( LONG,   INTCAST, (ffvsp[(2) - (2)].Node) );  TEST((ffval.Node));  }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 792 "eval.y"
    { (ffval.Node) = New_Unary( LONG,   INTCAST, (ffvsp[(2) - (2)].Node) );  TEST((ffval.Node));  }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 794 "eval.y"
    { (ffval.Node) = New_Unary( DOUBLE, FLTCAST, (ffvsp[(2) - (2)].Node) );  TEST((ffval.Node));  }
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 796 "eval.y"
    { (ffval.Node) = New_Unary( DOUBLE, FLTCAST, (ffvsp[(2) - (2)].Node) );  TEST((ffval.Node));  }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 800 "eval.y"
    { (ffval.Node) = New_Const( BOOLEAN, &((ffvsp[(1) - (1)].log)), sizeof(char) ); TEST((ffval.Node)); }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 802 "eval.y"
    { (ffval.Node) = New_Column( (ffvsp[(1) - (1)].lng) ); TEST((ffval.Node)); }
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 804 "eval.y"
    {
                  if( TYPE((ffvsp[(3) - (4)].Node)) != LONG
		      || OPER((ffvsp[(3) - (4)].Node)) != CONST_OP ) {
		     fferror("Offset argument must be a constant integer");
		     FFERROR;
		  }
                  (ffval.Node) = New_Offset( (ffvsp[(1) - (4)].lng), (ffvsp[(3) - (4)].Node) ); TEST((ffval.Node));
                }
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 813 "eval.y"
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (3)].Node), EQ,  (ffvsp[(3) - (3)].Node) ); TEST((ffval.Node));
		  SIZE((ffval.Node)) = 1;                                     }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 816 "eval.y"
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (3)].Node), NE,  (ffvsp[(3) - (3)].Node) ); TEST((ffval.Node)); 
		  SIZE((ffval.Node)) = 1;                                     }
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 819 "eval.y"
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (3)].Node), LT,  (ffvsp[(3) - (3)].Node) ); TEST((ffval.Node)); 
		  SIZE((ffval.Node)) = 1;                                     }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 822 "eval.y"
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (3)].Node), LTE, (ffvsp[(3) - (3)].Node) ); TEST((ffval.Node)); 
		  SIZE((ffval.Node)) = 1;                                     }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 825 "eval.y"
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (3)].Node), GT,  (ffvsp[(3) - (3)].Node) ); TEST((ffval.Node)); 
		  SIZE((ffval.Node)) = 1;                                     }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 828 "eval.y"
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (3)].Node), GTE, (ffvsp[(3) - (3)].Node) ); TEST((ffval.Node)); 
		  SIZE((ffval.Node)) = 1;                                     }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 831 "eval.y"
    { PROMOTE((ffvsp[(1) - (3)].Node),(ffvsp[(3) - (3)].Node)); (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (3)].Node), GT,  (ffvsp[(3) - (3)].Node) );
                  TEST((ffval.Node));                                               }
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 834 "eval.y"
    { PROMOTE((ffvsp[(1) - (3)].Node),(ffvsp[(3) - (3)].Node)); (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (3)].Node), LT,  (ffvsp[(3) - (3)].Node) );
                  TEST((ffval.Node));                                               }
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 837 "eval.y"
    { PROMOTE((ffvsp[(1) - (3)].Node),(ffvsp[(3) - (3)].Node)); (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (3)].Node), GTE, (ffvsp[(3) - (3)].Node) );
                  TEST((ffval.Node));                                               }
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 840 "eval.y"
    { PROMOTE((ffvsp[(1) - (3)].Node),(ffvsp[(3) - (3)].Node)); (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (3)].Node), LTE, (ffvsp[(3) - (3)].Node) );
                  TEST((ffval.Node));                                               }
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 843 "eval.y"
    { PROMOTE((ffvsp[(1) - (3)].Node),(ffvsp[(3) - (3)].Node)); (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (3)].Node), '~', (ffvsp[(3) - (3)].Node) );
                  TEST((ffval.Node));                                               }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 846 "eval.y"
    { PROMOTE((ffvsp[(1) - (3)].Node),(ffvsp[(3) - (3)].Node)); (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (3)].Node), EQ,  (ffvsp[(3) - (3)].Node) );
                  TEST((ffval.Node));                                               }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 849 "eval.y"
    { PROMOTE((ffvsp[(1) - (3)].Node),(ffvsp[(3) - (3)].Node)); (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (3)].Node), NE,  (ffvsp[(3) - (3)].Node) );
                  TEST((ffval.Node));                                               }
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 852 "eval.y"
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (3)].Node), EQ,  (ffvsp[(3) - (3)].Node) ); TEST((ffval.Node));
                  SIZE((ffval.Node)) = 1; }
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 855 "eval.y"
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (3)].Node), NE,  (ffvsp[(3) - (3)].Node) ); TEST((ffval.Node));
                  SIZE((ffval.Node)) = 1; }
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 858 "eval.y"
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (3)].Node), GT,  (ffvsp[(3) - (3)].Node) ); TEST((ffval.Node));
                  SIZE((ffval.Node)) = 1; }
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 861 "eval.y"
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (3)].Node), GTE, (ffvsp[(3) - (3)].Node) ); TEST((ffval.Node));
                  SIZE((ffval.Node)) = 1; }
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 864 "eval.y"
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (3)].Node), LT,  (ffvsp[(3) - (3)].Node) ); TEST((ffval.Node));
                  SIZE((ffval.Node)) = 1; }
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 867 "eval.y"
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (3)].Node), LTE, (ffvsp[(3) - (3)].Node) ); TEST((ffval.Node));
                  SIZE((ffval.Node)) = 1; }
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 870 "eval.y"
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (3)].Node), AND, (ffvsp[(3) - (3)].Node) ); TEST((ffval.Node)); }
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 872 "eval.y"
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (3)].Node), OR,  (ffvsp[(3) - (3)].Node) ); TEST((ffval.Node)); }
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 874 "eval.y"
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (3)].Node), EQ,  (ffvsp[(3) - (3)].Node) ); TEST((ffval.Node)); }
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 876 "eval.y"
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (3)].Node), NE,  (ffvsp[(3) - (3)].Node) ); TEST((ffval.Node)); }
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 879 "eval.y"
    { PROMOTE((ffvsp[(1) - (5)].Node),(ffvsp[(3) - (5)].Node)); PROMOTE((ffvsp[(1) - (5)].Node),(ffvsp[(5) - (5)].Node)); PROMOTE((ffvsp[(3) - (5)].Node),(ffvsp[(5) - (5)].Node));
		  (ffvsp[(3) - (5)].Node) = New_BinOp( BOOLEAN, (ffvsp[(3) - (5)].Node), LTE, (ffvsp[(1) - (5)].Node) );
                  (ffvsp[(5) - (5)].Node) = New_BinOp( BOOLEAN, (ffvsp[(1) - (5)].Node), LTE, (ffvsp[(5) - (5)].Node) );
                  (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[(3) - (5)].Node), AND, (ffvsp[(5) - (5)].Node) );
                  TEST((ffval.Node));                                         }
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 886 "eval.y"
    {
                  if( ! Test_Dims((ffvsp[(3) - (5)].Node),(ffvsp[(5) - (5)].Node)) ) {
                     fferror("Incompatible dimensions in '?:' arguments");
		     FFERROR;
                  }
                  (ffval.Node) = New_Func( 0, ifthenelse_fct, 3, (ffvsp[(3) - (5)].Node), (ffvsp[(5) - (5)].Node), (ffvsp[(1) - (5)].Node),
                                 0, 0, 0, 0 );
                  TEST((ffval.Node));
                  if( SIZE((ffvsp[(3) - (5)].Node))<SIZE((ffvsp[(5) - (5)].Node)) )  Copy_Dims((ffval.Node), (ffvsp[(5) - (5)].Node));
                  if( ! Test_Dims((ffvsp[(1) - (5)].Node),(ffval.Node)) ) {
                     fferror("Incompatible dimensions in '?:' condition");
		     FFERROR;
                  }
                  if( SIZE((ffval.Node))<SIZE((ffvsp[(1) - (5)].Node)) )  Copy_Dims((ffval.Node), (ffvsp[(1) - (5)].Node));
                }
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 903 "eval.y"
    {
		   if (FSTRCMP((ffvsp[(1) - (3)].str),"ISNULL(") == 0) {
		      (ffval.Node) = New_Func( 0, isnull_fct, 1, (ffvsp[(2) - (3)].Node), 0, 0,
				     0, 0, 0, 0 );
		      TEST((ffval.Node)); 
                      /* Use expression's size, but return BOOLEAN */
		      TYPE((ffval.Node)) = BOOLEAN;
		   } else {
		      fferror("Boolean Function(expr) not supported");
		      FFERROR;
		   }
		}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 916 "eval.y"
    {
		   if (FSTRCMP((ffvsp[(1) - (3)].str),"ISNULL(") == 0) {
		      (ffval.Node) = New_Func( 0, isnull_fct, 1, (ffvsp[(2) - (3)].Node), 0, 0,
				     0, 0, 0, 0 );
		      TEST((ffval.Node)); 
                      /* Use expression's size, but return BOOLEAN */
		      TYPE((ffval.Node)) = BOOLEAN;
		   } else {
		      fferror("Boolean Function(expr) not supported");
		      FFERROR;
		   }
		}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 929 "eval.y"
    {
		   if (FSTRCMP((ffvsp[(1) - (3)].str),"ISNULL(") == 0) {
		      (ffval.Node) = New_Func( BOOLEAN, isnull_fct, 1, (ffvsp[(2) - (3)].Node), 0, 0,
				     0, 0, 0, 0 );
		      TEST((ffval.Node)); 
		   } else {
		      fferror("Boolean Function(expr) not supported");
		      FFERROR;
		   }
		}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 940 "eval.y"
    {
		   if (FSTRCMP((ffvsp[(1) - (5)].str),"DEFNULL(") == 0) {
		      if( SIZE((ffvsp[(2) - (5)].Node))>=SIZE((ffvsp[(4) - (5)].Node)) && Test_Dims( (ffvsp[(2) - (5)].Node), (ffvsp[(4) - (5)].Node) ) ) {
			 (ffval.Node) = New_Func( 0, defnull_fct, 2, (ffvsp[(2) - (5)].Node), (ffvsp[(4) - (5)].Node), 0,
					0, 0, 0, 0 );
			 TEST((ffval.Node)); 
		      } else {
			 fferror("Dimensions of DEFNULL arguments are not compatible");
			 FFERROR;
		      }
		   } else {
		      fferror("Boolean Function(expr,expr) not supported");
		      FFERROR;
		   }
		}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 956 "eval.y"
    {
		   if( TYPE((ffvsp[(2) - (7)].Node)) != DOUBLE ) (ffvsp[(2) - (7)].Node) = New_Unary( DOUBLE, 0, (ffvsp[(2) - (7)].Node) );
		   if( TYPE((ffvsp[(4) - (7)].Node)) != DOUBLE ) (ffvsp[(4) - (7)].Node) = New_Unary( DOUBLE, 0, (ffvsp[(4) - (7)].Node) );
		   if( TYPE((ffvsp[(6) - (7)].Node)) != DOUBLE ) (ffvsp[(6) - (7)].Node) = New_Unary( DOUBLE, 0, (ffvsp[(6) - (7)].Node) );
		   if( ! (Test_Dims( (ffvsp[(2) - (7)].Node), (ffvsp[(4) - (7)].Node) ) && Test_Dims( (ffvsp[(4) - (7)].Node), (ffvsp[(6) - (7)].Node) ) ) ) {
		       fferror("Dimensions of NEAR arguments "
			       "are not compatible");
		       FFERROR;
		   } else {
		     if (FSTRCMP((ffvsp[(1) - (7)].str),"NEAR(") == 0) {
		       (ffval.Node) = New_Func( BOOLEAN, near_fct, 3, (ffvsp[(2) - (7)].Node), (ffvsp[(4) - (7)].Node), (ffvsp[(6) - (7)].Node),
				      0, 0, 0, 0 );
		     } else {
		       fferror("Boolean Function not supported");
		       FFERROR;
		     }
		     TEST((ffval.Node)); 

		     if( SIZE((ffval.Node))<SIZE((ffvsp[(2) - (7)].Node)) )  Copy_Dims((ffval.Node), (ffvsp[(2) - (7)].Node));
		     if( SIZE((ffvsp[(2) - (7)].Node))<SIZE((ffvsp[(4) - (7)].Node)) )  Copy_Dims((ffval.Node), (ffvsp[(4) - (7)].Node));
		     if( SIZE((ffvsp[(4) - (7)].Node))<SIZE((ffvsp[(6) - (7)].Node)) )  Copy_Dims((ffval.Node), (ffvsp[(6) - (7)].Node));
		   }
		}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 980 "eval.y"
    {
		   if( TYPE((ffvsp[(2) - (11)].Node)) != DOUBLE ) (ffvsp[(2) - (11)].Node) = New_Unary( DOUBLE, 0, (ffvsp[(2) - (11)].Node) );
		   if( TYPE((ffvsp[(4) - (11)].Node)) != DOUBLE ) (ffvsp[(4) - (11)].Node) = New_Unary( DOUBLE, 0, (ffvsp[(4) - (11)].Node) );
		   if( TYPE((ffvsp[(6) - (11)].Node)) != DOUBLE ) (ffvsp[(6) - (11)].Node) = New_Unary( DOUBLE, 0, (ffvsp[(6) - (11)].Node) );
		   if( TYPE((ffvsp[(8) - (11)].Node)) != DOUBLE ) (ffvsp[(8) - (11)].Node) = New_Unary( DOUBLE, 0, (ffvsp[(8) - (11)].Node) );
		   if( TYPE((ffvsp[(10) - (11)].Node))!= DOUBLE ) (ffvsp[(10) - (11)].Node)= New_Unary( DOUBLE, 0, (ffvsp[(10) - (11)].Node));
		   if( ! (Test_Dims( (ffvsp[(2) - (11)].Node), (ffvsp[(4) - (11)].Node) ) && Test_Dims( (ffvsp[(4) - (11)].Node), (ffvsp[(6) - (11)].Node) ) && 
			  Test_Dims( (ffvsp[(6) - (11)].Node), (ffvsp[(8) - (11)].Node) ) && Test_Dims( (ffvsp[(8) - (11)].Node), (ffvsp[(10) - (11)].Node) )) ) {
		     fferror("Dimensions of CIRCLE arguments "
			     "are not compatible");
		     FFERROR;
		   } else {
		     if (FSTRCMP((ffvsp[(1) - (11)].str),"CIRCLE(") == 0) {
		       (ffval.Node) = New_Func( BOOLEAN, circle_fct, 5, (ffvsp[(2) - (11)].Node), (ffvsp[(4) - (11)].Node), (ffvsp[(6) - (11)].Node), (ffvsp[(8) - (11)].Node),
				      (ffvsp[(10) - (11)].Node), 0, 0 );
		     } else {
		       fferror("Boolean Function not supported");
		       FFERROR;
		     }
		     TEST((ffval.Node)); 
		     if( SIZE((ffval.Node))<SIZE((ffvsp[(2) - (11)].Node)) )  Copy_Dims((ffval.Node), (ffvsp[(2) - (11)].Node));
		     if( SIZE((ffvsp[(2) - (11)].Node))<SIZE((ffvsp[(4) - (11)].Node)) )  Copy_Dims((ffval.Node), (ffvsp[(4) - (11)].Node));
		     if( SIZE((ffvsp[(4) - (11)].Node))<SIZE((ffvsp[(6) - (11)].Node)) )  Copy_Dims((ffval.Node), (ffvsp[(6) - (11)].Node));
		     if( SIZE((ffvsp[(6) - (11)].Node))<SIZE((ffvsp[(8) - (11)].Node)) )  Copy_Dims((ffval.Node), (ffvsp[(8) - (11)].Node));
		     if( SIZE((ffvsp[(8) - (11)].Node))<SIZE((ffvsp[(10) - (11)].Node)) ) Copy_Dims((ffval.Node), (ffvsp[(10) - (11)].Node));
		   }
		}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 1008 "eval.y"
    {
		   if( TYPE((ffvsp[(2) - (15)].Node)) != DOUBLE ) (ffvsp[(2) - (15)].Node) = New_Unary( DOUBLE, 0, (ffvsp[(2) - (15)].Node) );
		   if( TYPE((ffvsp[(4) - (15)].Node)) != DOUBLE ) (ffvsp[(4) - (15)].Node) = New_Unary( DOUBLE, 0, (ffvsp[(4) - (15)].Node) );
		   if( TYPE((ffvsp[(6) - (15)].Node)) != DOUBLE ) (ffvsp[(6) - (15)].Node) = New_Unary( DOUBLE, 0, (ffvsp[(6) - (15)].Node) );
		   if( TYPE((ffvsp[(8) - (15)].Node)) != DOUBLE ) (ffvsp[(8) - (15)].Node) = New_Unary( DOUBLE, 0, (ffvsp[(8) - (15)].Node) );
		   if( TYPE((ffvsp[(10) - (15)].Node))!= DOUBLE ) (ffvsp[(10) - (15)].Node)= New_Unary( DOUBLE, 0, (ffvsp[(10) - (15)].Node));
		   if( TYPE((ffvsp[(12) - (15)].Node))!= DOUBLE ) (ffvsp[(12) - (15)].Node)= New_Unary( DOUBLE, 0, (ffvsp[(12) - (15)].Node));
		   if( TYPE((ffvsp[(14) - (15)].Node))!= DOUBLE ) (ffvsp[(14) - (15)].Node)= New_Unary( DOUBLE, 0, (ffvsp[(14) - (15)].Node));
		   if( ! (Test_Dims( (ffvsp[(2) - (15)].Node), (ffvsp[(4) - (15)].Node) ) && Test_Dims( (ffvsp[(4) - (15)].Node), (ffvsp[(6) - (15)].Node) ) && 
			  Test_Dims( (ffvsp[(6) - (15)].Node), (ffvsp[(8) - (15)].Node) ) && Test_Dims( (ffvsp[(8) - (15)].Node), (ffvsp[(10) - (15)].Node) ) &&
			  Test_Dims((ffvsp[(10) - (15)].Node),(ffvsp[(12) - (15)].Node) ) && Test_Dims((ffvsp[(12) - (15)].Node), (ffvsp[(14) - (15)].Node) ) ) ) {
		     fferror("Dimensions of BOX or ELLIPSE arguments "
			     "are not compatible");
		     FFERROR;
		   } else {
		     if (FSTRCMP((ffvsp[(1) - (15)].str),"BOX(") == 0) {
		       (ffval.Node) = New_Func( BOOLEAN, box_fct, 7, (ffvsp[(2) - (15)].Node), (ffvsp[(4) - (15)].Node), (ffvsp[(6) - (15)].Node), (ffvsp[(8) - (15)].Node),
				      (ffvsp[(10) - (15)].Node), (ffvsp[(12) - (15)].Node), (ffvsp[(14) - (15)].Node) );
		     } else if (FSTRCMP((ffvsp[(1) - (15)].str),"ELLIPSE(") == 0) {
		       (ffval.Node) = New_Func( BOOLEAN, elps_fct, 7, (ffvsp[(2) - (15)].Node), (ffvsp[(4) - (15)].Node), (ffvsp[(6) - (15)].Node), (ffvsp[(8) - (15)].Node),
				      (ffvsp[(10) - (15)].Node), (ffvsp[(12) - (15)].Node), (ffvsp[(14) - (15)].Node) );
		     } else {
		       fferror("SAO Image Function not supported");
		       FFERROR;
		     }
		     TEST((ffval.Node)); 
		     if( SIZE((ffval.Node))<SIZE((ffvsp[(2) - (15)].Node)) )  Copy_Dims((ffval.Node), (ffvsp[(2) - (15)].Node));
		     if( SIZE((ffvsp[(2) - (15)].Node))<SIZE((ffvsp[(4) - (15)].Node)) )  Copy_Dims((ffval.Node), (ffvsp[(4) - (15)].Node));
		     if( SIZE((ffvsp[(4) - (15)].Node))<SIZE((ffvsp[(6) - (15)].Node)) )  Copy_Dims((ffval.Node), (ffvsp[(6) - (15)].Node));
		     if( SIZE((ffvsp[(6) - (15)].Node))<SIZE((ffvsp[(8) - (15)].Node)) )  Copy_Dims((ffval.Node), (ffvsp[(8) - (15)].Node));
		     if( SIZE((ffvsp[(8) - (15)].Node))<SIZE((ffvsp[(10) - (15)].Node)) ) Copy_Dims((ffval.Node), (ffvsp[(10) - (15)].Node));
		     if( SIZE((ffvsp[(10) - (15)].Node))<SIZE((ffvsp[(12) - (15)].Node)) ) Copy_Dims((ffval.Node), (ffvsp[(12) - (15)].Node));
		     if( SIZE((ffvsp[(12) - (15)].Node))<SIZE((ffvsp[(14) - (15)].Node)) ) Copy_Dims((ffval.Node), (ffvsp[(14) - (15)].Node));
		   }
		}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 1045 "eval.y"
    { /* Use defaults for all elements */
                   (ffval.Node) = New_GTI( "", -99, "*START*", "*STOP*" );
                   TEST((ffval.Node));                                        }
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 1049 "eval.y"
    { /* Use defaults for all except filename */
                   (ffval.Node) = New_GTI( (ffvsp[(2) - (3)].str), -99, "*START*", "*STOP*" );
                   TEST((ffval.Node));                                        }
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 1053 "eval.y"
    {  (ffval.Node) = New_GTI( (ffvsp[(2) - (5)].str), (ffvsp[(4) - (5)].Node), "*START*", "*STOP*" );
                   TEST((ffval.Node));                                        }
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 1056 "eval.y"
    {  (ffval.Node) = New_GTI( (ffvsp[(2) - (9)].str), (ffvsp[(4) - (9)].Node), (ffvsp[(6) - (9)].str), (ffvsp[(8) - (9)].str) );
                   TEST((ffval.Node));                                        }
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 1060 "eval.y"
    { /* Use defaults for all except filename */
                   (ffval.Node) = New_REG( (ffvsp[(2) - (3)].str), -99, -99, "" );
                   TEST((ffval.Node));                                        }
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 1064 "eval.y"
    {  (ffval.Node) = New_REG( (ffvsp[(2) - (7)].str), (ffvsp[(4) - (7)].Node), (ffvsp[(6) - (7)].Node), "" );
                   TEST((ffval.Node));                                        }
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 1067 "eval.y"
    {  (ffval.Node) = New_REG( (ffvsp[(2) - (9)].str), (ffvsp[(4) - (9)].Node), (ffvsp[(6) - (9)].Node), (ffvsp[(8) - (9)].str) );
                   TEST((ffval.Node));                                        }
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 1071 "eval.y"
    { (ffval.Node) = New_Deref( (ffvsp[(1) - (4)].Node), 1, (ffvsp[(3) - (4)].Node),  0,  0,  0,   0 ); TEST((ffval.Node)); }
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 1073 "eval.y"
    { (ffval.Node) = New_Deref( (ffvsp[(1) - (6)].Node), 2, (ffvsp[(3) - (6)].Node), (ffvsp[(5) - (6)].Node),  0,  0,   0 ); TEST((ffval.Node)); }
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 1075 "eval.y"
    { (ffval.Node) = New_Deref( (ffvsp[(1) - (8)].Node), 3, (ffvsp[(3) - (8)].Node), (ffvsp[(5) - (8)].Node), (ffvsp[(7) - (8)].Node),  0,   0 ); TEST((ffval.Node)); }
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 1077 "eval.y"
    { (ffval.Node) = New_Deref( (ffvsp[(1) - (10)].Node), 4, (ffvsp[(3) - (10)].Node), (ffvsp[(5) - (10)].Node), (ffvsp[(7) - (10)].Node), (ffvsp[(9) - (10)].Node),   0 ); TEST((ffval.Node)); }
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 1079 "eval.y"
    { (ffval.Node) = New_Deref( (ffvsp[(1) - (12)].Node), 5, (ffvsp[(3) - (12)].Node), (ffvsp[(5) - (12)].Node), (ffvsp[(7) - (12)].Node), (ffvsp[(9) - (12)].Node), (ffvsp[(11) - (12)].Node) ); TEST((ffval.Node)); }
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 1081 "eval.y"
    { (ffval.Node) = New_Unary( BOOLEAN, NOT, (ffvsp[(2) - (2)].Node) ); TEST((ffval.Node)); }
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 1083 "eval.y"
    { (ffval.Node) = (ffvsp[(2) - (3)].Node); }
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 1087 "eval.y"
    { (ffval.Node) = New_Const( STRING, (ffvsp[(1) - (1)].str), strlen((ffvsp[(1) - (1)].str))+1 ); TEST((ffval.Node));
                  SIZE((ffval.Node)) = strlen((ffvsp[(1) - (1)].str)); }
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 1090 "eval.y"
    { (ffval.Node) = New_Column( (ffvsp[(1) - (1)].lng) ); TEST((ffval.Node)); }
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 1092 "eval.y"
    {
                  if( TYPE((ffvsp[(3) - (4)].Node)) != LONG
		      || OPER((ffvsp[(3) - (4)].Node)) != CONST_OP ) {
		     fferror("Offset argument must be a constant integer");
		     FFERROR;
		  }
                  (ffval.Node) = New_Offset( (ffvsp[(1) - (4)].lng), (ffvsp[(3) - (4)].Node) ); TEST((ffval.Node));
                }
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 1101 "eval.y"
    { (ffval.Node) = New_Func( STRING, null_fct, 0, 0, 0, 0, 0, 0, 0, 0 ); }
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 1103 "eval.y"
    { (ffval.Node) = (ffvsp[(2) - (3)].Node); }
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 1105 "eval.y"
    { 
		  if (SIZE((ffvsp[(1) - (3)].Node))+SIZE((ffvsp[(3) - (3)].Node)) >= MAX_STRLEN) {
		    fferror("Combined string size exceeds " MAX_STRLEN_S " characters");
		    FFERROR;
		  }
		  (ffval.Node) = New_BinOp( STRING, (ffvsp[(1) - (3)].Node), '+', (ffvsp[(3) - (3)].Node) );  TEST((ffval.Node));
		  SIZE((ffval.Node)) = SIZE((ffvsp[(1) - (3)].Node)) + SIZE((ffvsp[(3) - (3)].Node));
		}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 1114 "eval.y"
    {
		  int outSize;
                  if( SIZE((ffvsp[(1) - (5)].Node))!=1 ) {
                     fferror("Cannot have a vector string column");
		     FFERROR;
                  }
		  /* Since the output can be calculated now, as a constant
		     scalar, we must precalculate the output size, in
		     order to avoid an overflow. */
		  outSize = SIZE((ffvsp[(3) - (5)].Node));
		  if (SIZE((ffvsp[(5) - (5)].Node)) > outSize) outSize = SIZE((ffvsp[(5) - (5)].Node));
                  (ffval.Node) = New_FuncSize( 0, ifthenelse_fct, 3, (ffvsp[(3) - (5)].Node), (ffvsp[(5) - (5)].Node), (ffvsp[(1) - (5)].Node),
				     0, 0, 0, 0, outSize);
		  
                  TEST((ffval.Node));
                  if( SIZE((ffvsp[(3) - (5)].Node))<SIZE((ffvsp[(5) - (5)].Node)) )  Copy_Dims((ffval.Node), (ffvsp[(5) - (5)].Node));
                }
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 1133 "eval.y"
    { 
		  if (FSTRCMP((ffvsp[(1) - (5)].str),"DEFNULL(") == 0) {
		     int outSize;
		     /* Since the output can be calculated now, as a constant
			scalar, we must precalculate the output size, in
			order to avoid an overflow. */
		     outSize = SIZE((ffvsp[(2) - (5)].Node));
		     if (SIZE((ffvsp[(4) - (5)].Node)) > outSize) outSize = SIZE((ffvsp[(4) - (5)].Node));
		     
		     (ffval.Node) = New_FuncSize( 0, defnull_fct, 2, (ffvsp[(2) - (5)].Node), (ffvsp[(4) - (5)].Node), 0,
					0, 0, 0, 0, outSize );
		     TEST((ffval.Node)); 
		     if( SIZE((ffvsp[(4) - (5)].Node))>SIZE((ffvsp[(2) - (5)].Node)) ) SIZE((ffval.Node)) = SIZE((ffvsp[(4) - (5)].Node));
		  } else {
		     fferror("Function(string,string) not supported");
		     FFERROR;
		  }
		}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 1152 "eval.y"
    { 
		  if (FSTRCMP((ffvsp[(1) - (7)].str),"STRMID(") == 0) {
		    int len;
		    if( TYPE((ffvsp[(4) - (7)].Node)) != LONG || SIZE((ffvsp[(4) - (7)].Node)) != 1 ||
			TYPE((ffvsp[(6) - (7)].Node)) != LONG || SIZE((ffvsp[(6) - (7)].Node)) != 1) {
		      fferror("When using STRMID(S,P,N), P and N must be integers (and not vector columns)");
		      FFERROR;
		    }
		    if (OPER((ffvsp[(6) - (7)].Node)) == CONST_OP) {
		      /* Constant value: use that directly */
		      len = (gParse.Nodes[(ffvsp[(6) - (7)].Node)].value.data.lng);
		    } else {
		      /* Variable value: use the maximum possible (from $2) */
		      len = SIZE((ffvsp[(2) - (7)].Node));
		    }
		    if (len <= 0 || len >= MAX_STRLEN) {
		      fferror("STRMID(S,P,N), N must be 1-" MAX_STRLEN_S);
		      FFERROR;
		    }
		    (ffval.Node) = New_FuncSize( 0, strmid_fct, 3, (ffvsp[(2) - (7)].Node), (ffvsp[(4) - (7)].Node),(ffvsp[(6) - (7)].Node),0,0,0,0,len);
		    TEST((ffval.Node));
		  } else {
		     fferror("Function(string,expr,expr) not supported");
		     FFERROR;
		  }
		}
    break;



/* Line 1455 of yacc.c  */
#line 3644 "y.tab.c"
      default: break;
    }
  FF_SYMBOL_PRINT ("-> $$ =", ffr1[ffn], &ffval, &ffloc);

  FFPOPSTACK (fflen);
  fflen = 0;
  FF_STACK_PRINT (ffss, ffssp);

  *++ffvsp = ffval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  ffn = ffr1[ffn];

  ffstate = ffpgoto[ffn - FFNTOKENS] + *ffssp;
  if (0 <= ffstate && ffstate <= FFLAST && ffcheck[ffstate] == *ffssp)
    ffstate = fftable[ffstate];
  else
    ffstate = ffdefgoto[ffn - FFNTOKENS];

  goto ffnewstate;


/*------------------------------------.
| fferrlab -- here on detecting error |
`------------------------------------*/
fferrlab:
  /* If not already recovering from an error, report this error.  */
  if (!fferrstatus)
    {
      ++ffnerrs;
#if ! FFERROR_VERBOSE
      fferror (FF_("syntax error"));
#else
      {
	FFSIZE_T ffsize = ffsyntax_error (0, ffstate, ffchar);
	if (ffmsg_alloc < ffsize && ffmsg_alloc < FFSTACK_ALLOC_MAXIMUM)
	  {
	    FFSIZE_T ffalloc = 2 * ffsize;
	    if (! (ffsize <= ffalloc && ffalloc <= FFSTACK_ALLOC_MAXIMUM))
	      ffalloc = FFSTACK_ALLOC_MAXIMUM;
	    if (ffmsg != ffmsgbuf)
	      FFSTACK_FREE (ffmsg);
	    ffmsg = (char *) FFSTACK_ALLOC (ffalloc);
	    if (ffmsg)
	      ffmsg_alloc = ffalloc;
	    else
	      {
		ffmsg = ffmsgbuf;
		ffmsg_alloc = sizeof ffmsgbuf;
	      }
	  }

	if (0 < ffsize && ffsize <= ffmsg_alloc)
	  {
	    (void) ffsyntax_error (ffmsg, ffstate, ffchar);
	    fferror (ffmsg);
	  }
	else
	  {
	    fferror (FF_("syntax error"));
	    if (ffsize != 0)
	      goto ffexhaustedlab;
	  }
      }
#endif
    }



  if (fferrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (ffchar <= FFEOF)
	{
	  /* Return failure if at end of input.  */
	  if (ffchar == FFEOF)
	    FFABORT;
	}
      else
	{
	  ffdestruct ("Error: discarding",
		      fftoken, &fflval);
	  ffchar = FFEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto fferrlab1;


/*---------------------------------------------------.
| fferrorlab -- error raised explicitly by FFERROR.  |
`---------------------------------------------------*/
fferrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     FFERROR and the label fferrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto fferrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this FFERROR.  */
  FFPOPSTACK (fflen);
  fflen = 0;
  FF_STACK_PRINT (ffss, ffssp);
  ffstate = *ffssp;
  goto fferrlab1;


/*-------------------------------------------------------------.
| fferrlab1 -- common code for both syntax error and FFERROR.  |
`-------------------------------------------------------------*/
fferrlab1:
  fferrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      ffn = ffpact[ffstate];
      if (ffn != FFPACT_NINF)
	{
	  ffn += FFTERROR;
	  if (0 <= ffn && ffn <= FFLAST && ffcheck[ffn] == FFTERROR)
	    {
	      ffn = fftable[ffn];
	      if (0 < ffn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (ffssp == ffss)
	FFABORT;


      ffdestruct ("Error: popping",
		  ffstos[ffstate], ffvsp);
      FFPOPSTACK (1);
      ffstate = *ffssp;
      FF_STACK_PRINT (ffss, ffssp);
    }

  *++ffvsp = fflval;


  /* Shift the error token.  */
  FF_SYMBOL_PRINT ("Shifting", ffstos[ffn], ffvsp, fflsp);

  ffstate = ffn;
  goto ffnewstate;


/*-------------------------------------.
| ffacceptlab -- FFACCEPT comes here.  |
`-------------------------------------*/
ffacceptlab:
  ffresult = 0;
  goto ffreturn;

/*-----------------------------------.
| ffabortlab -- FFABORT comes here.  |
`-----------------------------------*/
ffabortlab:
  ffresult = 1;
  goto ffreturn;

#if !defined(ffoverflow) || FFERROR_VERBOSE
/*-------------------------------------------------.
| ffexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
ffexhaustedlab:
  fferror (FF_("memory exhausted"));
  ffresult = 2;
  /* Fall through.  */
#endif

ffreturn:
  if (ffchar != FFEMPTY)
     ffdestruct ("Cleanup: discarding lookahead",
		 fftoken, &fflval);
  /* Do not reclaim the symbols of the rule which action triggered
     this FFABORT or FFACCEPT.  */
  FFPOPSTACK (fflen);
  FF_STACK_PRINT (ffss, ffssp);
  while (ffssp != ffss)
    {
      ffdestruct ("Cleanup: popping",
		  ffstos[*ffssp], ffvsp);
      FFPOPSTACK (1);
    }
#ifndef ffoverflow
  if (ffss != ffssa)
    FFSTACK_FREE (ffss);
#endif
#if FFERROR_VERBOSE
  if (ffmsg != ffmsgbuf)
    FFSTACK_FREE (ffmsg);
#endif
  /* Make sure FFID is used.  */
  return FFID (ffresult);
}



/* Line 1675 of yacc.c  */
#line 1181 "eval.y"


/*************************************************************************/
/*  Start of "New" routines which build the expression Nodal structure   */
/*************************************************************************/

static int Alloc_Node( void )
{
                      /* Use this for allocation to guarantee *Nodes */
   Node *newNodePtr;  /* survives on failure, making it still valid  */
                      /* while working our way out of this error     */

   if( gParse.nNodes == gParse.nNodesAlloc ) {
      if( gParse.Nodes ) {
	 gParse.nNodesAlloc += gParse.nNodesAlloc;
	 newNodePtr = (Node *)realloc( gParse.Nodes,
				       sizeof(Node)*gParse.nNodesAlloc );
      } else {
	 gParse.nNodesAlloc = 100;
	 newNodePtr = (Node *)malloc ( sizeof(Node)*gParse.nNodesAlloc );
      }	 

      if( newNodePtr ) {
	 gParse.Nodes = newNodePtr;
      } else {
	 gParse.status = MEMORY_ALLOCATION;
	 return( -1 );
      }
   }

   return ( gParse.nNodes++ );
}

static void Free_Last_Node( void )
{
   if( gParse.nNodes ) gParse.nNodes--;
}

static int New_Const( int returnType, void *value, long len )
{
   Node *this;
   int n;

   n = Alloc_Node();
   if( n>=0 ) {
      this             = gParse.Nodes + n;
      this->operation  = CONST_OP;             /* Flag a constant */
      this->DoOp       = NULL;
      this->nSubNodes  = 0;
      this->type       = returnType;
      memcpy( &(this->value.data), value, len );
      this->value.undef = NULL;
      this->value.nelem = 1;
      this->value.naxis = 1;
      this->value.naxes[0] = 1;
   }
   return(n);
}

static int New_Column( int ColNum )
{
   Node *this;
   int  n, i;

   n = Alloc_Node();
   if( n>=0 ) {
      this              = gParse.Nodes + n;
      this->operation   = -ColNum;
      this->DoOp        = NULL;
      this->nSubNodes   = 0;
      this->type        = gParse.varData[ColNum].type;
      this->value.nelem = gParse.varData[ColNum].nelem;
      this->value.naxis = gParse.varData[ColNum].naxis;
      for( i=0; i<gParse.varData[ColNum].naxis; i++ )
	 this->value.naxes[i] = gParse.varData[ColNum].naxes[i];
   }
   return(n);
}

static int New_Offset( int ColNum, int offsetNode )
{
   Node *this;
   int  n, i, colNode;

   colNode = New_Column( ColNum );
   if( colNode<0 ) return(-1);

   n = Alloc_Node();
   if( n>=0 ) {
      this              = gParse.Nodes + n;
      this->operation   = '{';
      this->DoOp        = Do_Offset;
      this->nSubNodes   = 2;
      this->SubNodes[0] = colNode;
      this->SubNodes[1] = offsetNode;
      this->type        = gParse.varData[ColNum].type;
      this->value.nelem = gParse.varData[ColNum].nelem;
      this->value.naxis = gParse.varData[ColNum].naxis;
      for( i=0; i<gParse.varData[ColNum].naxis; i++ )
	 this->value.naxes[i] = gParse.varData[ColNum].naxes[i];
   }
   return(n);
}

static int New_Unary( int returnType, int Op, int Node1 )
{
   Node *this, *that;
   int  i,n;

   if( Node1<0 ) return(-1);
   that = gParse.Nodes + Node1;

   if( !Op ) Op = returnType;

   if( (Op==DOUBLE || Op==FLTCAST) && that->type==DOUBLE  ) return( Node1 );
   if( (Op==LONG   || Op==INTCAST) && that->type==LONG    ) return( Node1 );
   if( (Op==BOOLEAN              ) && that->type==BOOLEAN ) return( Node1 );
   
   n = Alloc_Node();
   if( n>=0 ) {
      this              = gParse.Nodes + n;
      this->operation   = Op;
      this->DoOp        = Do_Unary;
      this->nSubNodes   = 1;
      this->SubNodes[0] = Node1;
      this->type        = returnType;

      that              = gParse.Nodes + Node1; /* Reset in case .Nodes mv'd */
      this->value.nelem = that->value.nelem;
      this->value.naxis = that->value.naxis;
      for( i=0; i<that->value.naxis; i++ )
	 this->value.naxes[i] = that->value.naxes[i];

      if( that->operation==CONST_OP ) this->DoOp( this );
   }
   return( n );
}

static int New_BinOp( int returnType, int Node1, int Op, int Node2 )
{
   Node *this,*that1,*that2;
   int  n,i,constant;

   if( Node1<0 || Node2<0 ) return(-1);

   n = Alloc_Node();
   if( n>=0 ) {
      this             = gParse.Nodes + n;
      this->operation  = Op;
      this->nSubNodes  = 2;
      this->SubNodes[0]= Node1;
      this->SubNodes[1]= Node2;
      this->type       = returnType;

      that1            = gParse.Nodes + Node1;
      that2            = gParse.Nodes + Node2;
      constant         = (that1->operation==CONST_OP
                          && that2->operation==CONST_OP);
      if( that1->type!=STRING && that1->type!=BITSTR )
	 if( !Test_Dims( Node1, Node2 ) ) {
	    Free_Last_Node();
	    fferror("Array sizes/dims do not match for binary operator");
	    return(-1);
	 }
      if( that1->value.nelem == 1 ) that1 = that2;

      this->value.nelem = that1->value.nelem;
      this->value.naxis = that1->value.naxis;
      for( i=0; i<that1->value.naxis; i++ )
	 this->value.naxes[i] = that1->value.naxes[i];

      if ( Op == ACCUM && that1->type == BITSTR ) {
	/* ACCUM is rank-reducing on bit strings */
	this->value.nelem = 1;
	this->value.naxis = 1;
	this->value.naxes[0] = 1;
      }

      /*  Both subnodes should be of same time  */
      switch( that1->type ) {
      case BITSTR:  this->DoOp = Do_BinOp_bit;  break;
      case STRING:  this->DoOp = Do_BinOp_str;  break;
      case BOOLEAN: this->DoOp = Do_BinOp_log;  break;
      case LONG:    this->DoOp = Do_BinOp_lng;  break;
      case DOUBLE:  this->DoOp = Do_BinOp_dbl;  break;
      }
      if( constant ) this->DoOp( this );
   }
   return( n );
}

static int New_Func( int returnType, funcOp Op, int nNodes,
		     int Node1, int Node2, int Node3, int Node4, 
		     int Node5, int Node6, int Node7 )
{
  return New_FuncSize(returnType, Op, nNodes,
		      Node1, Node2, Node3, Node4, 
		      Node5, Node6, Node7, 0);
}

static int New_FuncSize( int returnType, funcOp Op, int nNodes,
		     int Node1, int Node2, int Node3, int Node4, 
			 int Node5, int Node6, int Node7, int Size )
/* If returnType==0 , use Node1's type and vector sizes as returnType, */
/* else return a single value of type returnType                       */
{
   Node *this, *that;
   int  i,n,constant;

   if( Node1<0 || Node2<0 || Node3<0 || Node4<0 || 
       Node5<0 || Node6<0 || Node7<0 ) return(-1);

   n = Alloc_Node();
   if( n>=0 ) {
      this              = gParse.Nodes + n;
      this->operation   = (int)Op;
      this->DoOp        = Do_Func;
      this->nSubNodes   = nNodes;
      this->SubNodes[0] = Node1;
      this->SubNodes[1] = Node2;
      this->SubNodes[2] = Node3;
      this->SubNodes[3] = Node4;
      this->SubNodes[4] = Node5;
      this->SubNodes[5] = Node6;
      this->SubNodes[6] = Node7;
      i = constant = nNodes;    /* Functions with zero params are not const */
      if (Op == poirnd_fct) constant = 0; /* Nor is Poisson deviate */

      while( i-- )
	constant = ( constant && OPER(this->SubNodes[i]) == CONST_OP );
      
      if( returnType ) {
	 this->type           = returnType;
	 this->value.nelem    = 1;
	 this->value.naxis    = 1;
	 this->value.naxes[0] = 1;
      } else {
	 that              = gParse.Nodes + Node1;
	 this->type        = that->type;
	 this->value.nelem = that->value.nelem;
	 this->value.naxis = that->value.naxis;
	 for( i=0; i<that->value.naxis; i++ )
	    this->value.naxes[i] = that->value.naxes[i];
      }
      /* Force explicit size before evaluating */
      if (Size > 0) this->value.nelem = Size;

      if( constant ) this->DoOp( this );
   }
   return( n );
}

static int New_Deref( int Var,  int nDim,
		      int Dim1, int Dim2, int Dim3, int Dim4, int Dim5 )
{
   int n, idx, constant;
   long elem=0;
   Node *this, *theVar, *theDim[MAXDIMS];

   if( Var<0 || Dim1<0 || Dim2<0 || Dim3<0 || Dim4<0 || Dim5<0 ) return(-1);

   theVar = gParse.Nodes + Var;
   if( theVar->operation==CONST_OP || theVar->value.nelem==1 ) {
      fferror("Cannot index a scalar value");
      return(-1);
   }

   n = Alloc_Node();
   if( n>=0 ) {
      this              = gParse.Nodes + n;
      this->nSubNodes   = nDim+1;
      theVar            = gParse.Nodes + (this->SubNodes[0]=Var);
      theDim[0]         = gParse.Nodes + (this->SubNodes[1]=Dim1);
      theDim[1]         = gParse.Nodes + (this->SubNodes[2]=Dim2);
      theDim[2]         = gParse.Nodes + (this->SubNodes[3]=Dim3);
      theDim[3]         = gParse.Nodes + (this->SubNodes[4]=Dim4);
      theDim[4]         = gParse.Nodes + (this->SubNodes[5]=Dim5);
      constant          = theVar->operation==CONST_OP;
      for( idx=0; idx<nDim; idx++ )
	 constant = (constant && theDim[idx]->operation==CONST_OP);

      for( idx=0; idx<nDim; idx++ )
	 if( theDim[idx]->value.nelem>1 ) {
	    Free_Last_Node();
	    fferror("Cannot use an array as an index value");
	    return(-1);
	 } else if( theDim[idx]->type!=LONG ) {
	    Free_Last_Node();
	    fferror("Index value must be an integer type");
	    return(-1);
	 }

      this->operation   = '[';
      this->DoOp        = Do_Deref;
      this->type        = theVar->type;

      if( theVar->value.naxis == nDim ) { /* All dimensions specified */
	 this->value.nelem    = 1;
	 this->value.naxis    = 1;
	 this->value.naxes[0] = 1;
      } else if( nDim==1 ) { /* Dereference only one dimension */
	 elem=1;
	 this->value.naxis = theVar->value.naxis-1;
	 for( idx=0; idx<this->value.naxis; idx++ ) {
	    elem *= ( this->value.naxes[idx] = theVar->value.naxes[idx] );
	 }
	 this->value.nelem = elem;
      } else {
	 Free_Last_Node();
	 fferror("Must specify just one or all indices for vector");
	 return(-1);
      }
      if( constant ) this->DoOp( this );
   }
   return(n);
}

extern int ffGetVariable( char *varName, FFSTYPE *varVal );

static int New_GTI( char *fname, int Node1, char *start, char *stop )
{
   fitsfile *fptr;
   Node *this, *that0, *that1;
   int  type,i,n, startCol, stopCol, Node0;
   int  hdutype, hdunum, evthdu, samefile, extvers, movetotype, tstat;
   char extname[100];
   long nrows;
   double timeZeroI[2], timeZeroF[2], dt, timeSpan;
   char xcol[20], xexpr[20];
   FFSTYPE colVal;

   if( Node1==-99 ) {
      type = ffGetVariable( "TIME", &colVal );
      if( type==COLUMN ) {
	 Node1 = New_Column( (int)colVal.lng );
      } else {
	 fferror("Could not build TIME column for GTIFILTER");
	 return(-1);
      }
   }
   Node1 = New_Unary( DOUBLE, 0, Node1 );
   Node0 = Alloc_Node(); /* This will hold the START/STOP times */
   if( Node1<0 || Node0<0 ) return(-1);

   /*  Record current HDU number in case we need to move within this file  */

   fptr = gParse.def_fptr;
   ffghdn( fptr, &evthdu );

   /*  Look for TIMEZERO keywords in current extension  */

   tstat = 0;
   if( ffgkyd( fptr, "TIMEZERO", timeZeroI, NULL, &tstat ) ) {
      tstat = 0;
      if( ffgkyd( fptr, "TIMEZERI", timeZeroI, NULL, &tstat ) ) {
	 timeZeroI[0] = timeZeroF[0] = 0.0;
      } else if( ffgkyd( fptr, "TIMEZERF", timeZeroF, NULL, &tstat ) ) {
	 timeZeroF[0] = 0.0;
      }
   } else {
      timeZeroF[0] = 0.0;
   }

   /*  Resolve filename parameter  */

   switch( fname[0] ) {
   case '\0':
      samefile = 1;
      hdunum = 1;
      break;
   case '[':
      samefile = 1;
      i = 1;
      while( fname[i] != '\0' && fname[i] != ']' ) i++;
      if( fname[i] ) {
	 fname[i] = '\0';
	 fname++;
	 ffexts( fname, &hdunum, extname, &extvers, &movetotype,
		 xcol, xexpr, &gParse.status );
         if( *extname ) {
	    ffmnhd( fptr, movetotype, extname, extvers, &gParse.status );
	    ffghdn( fptr, &hdunum );
	 } else if( hdunum ) {
	    ffmahd( fptr, ++hdunum, &hdutype, &gParse.status );
	 } else if( !gParse.status ) {
	    fferror("Cannot use primary array for GTI filter");
	    return( -1 );
	 }
      } else {
	 fferror("File extension specifier lacks closing ']'");
	 return( -1 );
      }
      break;
   case '+':
      samefile = 1;
      hdunum = atoi( fname ) + 1;
      if( hdunum>1 )
	 ffmahd( fptr, hdunum, &hdutype, &gParse.status );
      else {
	 fferror("Cannot use primary array for GTI filter");
	 return( -1 );
      }
      break;
   default:
      samefile = 0;
      if( ! ffopen( &fptr, fname, READONLY, &gParse.status ) )
	 ffghdn( fptr, &hdunum );
      break;
   }
   if( gParse.status ) return(-1);

   /*  If at primary, search for GTI extension  */

   if( hdunum==1 ) {
      while( 1 ) {
	 hdunum++;
	 if( ffmahd( fptr, hdunum, &hdutype, &gParse.status ) ) break;
	 if( hdutype==IMAGE_HDU ) continue;
	 tstat = 0;
	 if( ffgkys( fptr, "EXTNAME", extname, NULL, &tstat ) ) continue;
	 ffupch( extname );
	 if( strstr( extname, "GTI" ) ) break;
      }
      if( gParse.status ) {
	 if( gParse.status==END_OF_FILE )
	    fferror("GTI extension not found in this file");
	 return(-1);
      }
   }

   /*  Locate START/STOP Columns  */

   ffgcno( fptr, CASEINSEN, start, &startCol, &gParse.status );
   ffgcno( fptr, CASEINSEN, stop,  &stopCol,  &gParse.status );
   if( gParse.status ) return(-1);

   /*  Look for TIMEZERO keywords in GTI extension  */

   tstat = 0;
   if( ffgkyd( fptr, "TIMEZERO", timeZeroI+1, NULL, &tstat ) ) {
      tstat = 0;
      if( ffgkyd( fptr, "TIMEZERI", timeZeroI+1, NULL, &tstat ) ) {
	 timeZeroI[1] = timeZeroF[1] = 0.0;
      } else if( ffgkyd( fptr, "TIMEZERF", timeZeroF+1, NULL, &tstat ) ) {
	 timeZeroF[1] = 0.0;
      }
   } else {
      timeZeroF[1] = 0.0;
   }

   n = Alloc_Node();
   if( n >= 0 ) {
      this                 = gParse.Nodes + n;
      this->nSubNodes      = 2;
      this->SubNodes[1]    = Node1;
      this->operation      = (int)gtifilt_fct;
      this->DoOp           = Do_GTI;
      this->type           = BOOLEAN;
      that1                = gParse.Nodes + Node1;
      this->value.nelem    = that1->value.nelem;
      this->value.naxis    = that1->value.naxis;
      for( i=0; i < that1->value.naxis; i++ )
	 this->value.naxes[i] = that1->value.naxes[i];

      /* Init START/STOP node to be treated as a "constant" */

      this->SubNodes[0]    = Node0;
      that0                = gParse.Nodes + Node0;
      that0->operation     = CONST_OP;
      that0->DoOp          = NULL;
      that0->value.data.ptr= NULL;

      /*  Read in START/STOP times  */

      if( ffgkyj( fptr, "NAXIS2", &nrows, NULL, &gParse.status ) )
	 return(-1);
      that0->value.nelem = nrows;
      if( nrows ) {

	 that0->value.data.dblptr = (double*)malloc( 2*nrows*sizeof(double) );
	 if( !that0->value.data.dblptr ) {
	    gParse.status = MEMORY_ALLOCATION;
	    return(-1);
	 }
	 
	 ffgcvd( fptr, startCol, 1L, 1L, nrows, 0.0,
		 that0->value.data.dblptr, &i, &gParse.status );
	 ffgcvd( fptr, stopCol, 1L, 1L, nrows, 0.0,
		 that0->value.data.dblptr+nrows, &i, &gParse.status );
	 if( gParse.status ) {
	    free( that0->value.data.dblptr );
	    return(-1);
	 }

	 /*  Test for fully time-ordered GTI... both START && STOP  */

	 that0->type = 1; /*  Assume yes  */
	 i = nrows;
	 while( --i )
	    if(    that0->value.data.dblptr[i-1]
                   >= that0->value.data.dblptr[i]
		|| that0->value.data.dblptr[i-1+nrows]
		   >= that0->value.data.dblptr[i+nrows] ) {
	       that0->type = 0;
	       break;
	    }
	 
	 /*  Handle TIMEZERO offset, if any  */
	 
	 dt = (timeZeroI[1] - timeZeroI[0]) + (timeZeroF[1] - timeZeroF[0]);
	 timeSpan = that0->value.data.dblptr[nrows+nrows-1]
	    - that0->value.data.dblptr[0];
	 
	 if( fabs( dt / timeSpan ) > 1e-12 ) {
	    for( i=0; i<(nrows+nrows); i++ )
	       that0->value.data.dblptr[i] += dt;
	 }
      }
      if( OPER(Node1)==CONST_OP )
	 this->DoOp( this );
   }

   if( samefile )
      ffmahd( fptr, evthdu, &hdutype, &gParse.status );
   else
      ffclos( fptr, &gParse.status );

   return( n );
}

static int New_REG( char *fname, int NodeX, int NodeY, char *colNames )
{
   Node *this, *that0;
   int  type, n, Node0;
   int  Xcol, Ycol, tstat;
   WCSdata wcs;
   SAORegion *Rgn;
   char *cX, *cY;
   FFSTYPE colVal;

   if( NodeX==-99 ) {
      type = ffGetVariable( "X", &colVal );
      if( type==COLUMN ) {
	 NodeX = New_Column( (int)colVal.lng );
      } else {
	 fferror("Could not build X column for REGFILTER");
	 return(-1);
      }
   }
   if( NodeY==-99 ) {
      type = ffGetVariable( "Y", &colVal );
      if( type==COLUMN ) {
	 NodeY = New_Column( (int)colVal.lng );
      } else {
	 fferror("Could not build Y column for REGFILTER");
	 return(-1);
      }
   }
   NodeX = New_Unary( DOUBLE, 0, NodeX );
   NodeY = New_Unary( DOUBLE, 0, NodeY );
   Node0 = Alloc_Node(); /* This will hold the Region Data */
   if( NodeX<0 || NodeY<0 || Node0<0 ) return(-1);

   if( ! (Test_Dims( NodeX, NodeY ) ) ) {
     fferror("Dimensions of REGFILTER arguments are not compatible");
     return (-1);
   }

   n = Alloc_Node();
   if( n >= 0 ) {
      this                 = gParse.Nodes + n;
      this->nSubNodes      = 3;
      this->SubNodes[0]    = Node0;
      this->SubNodes[1]    = NodeX;
      this->SubNodes[2]    = NodeY;
      this->operation      = (int)regfilt_fct;
      this->DoOp           = Do_REG;
      this->type           = BOOLEAN;
      this->value.nelem    = 1;
      this->value.naxis    = 1;
      this->value.naxes[0] = 1;
      
      Copy_Dims(n, NodeX);
      if( SIZE(NodeX)<SIZE(NodeY) )  Copy_Dims(n, NodeY);

      /* Init Region node to be treated as a "constant" */

      that0                = gParse.Nodes + Node0;
      that0->operation     = CONST_OP;
      that0->DoOp          = NULL;

      /*  Identify what columns to use for WCS information  */

      Xcol = Ycol = 0;
      if( *colNames ) {
	 /*  Use the column names in this string for WCS info  */
	 while( *colNames==' ' ) colNames++;
	 cX = cY = colNames;
	 while( *cY && *cY!=' ' && *cY!=',' ) cY++;
	 if( *cY )
	    *(cY++) = '\0';
	 while( *cY==' ' ) cY++;
	 if( !*cY ) {
	    fferror("Could not extract valid pair of column names from REGFILTER");
	    Free_Last_Node();
	    return( -1 );
	 }
	 fits_get_colnum( gParse.def_fptr, CASEINSEN, cX, &Xcol,
			  &gParse.status );
	 fits_get_colnum( gParse.def_fptr, CASEINSEN, cY, &Ycol,
			  &gParse.status );
	 if( gParse.status ) {
	    fferror("Could not locate columns indicated for WCS info");
	    Free_Last_Node();
	    return( -1 );
	 }

      } else {
	 /*  Try to find columns used in X/Y expressions  */
	 Xcol = Locate_Col( gParse.Nodes + NodeX );
	 Ycol = Locate_Col( gParse.Nodes + NodeY );
	 if( Xcol<0 || Ycol<0 ) {
	    fferror("Found multiple X/Y column references in REGFILTER");
	    Free_Last_Node();
	    return( -1 );
	 }
      }

      /*  Now, get the WCS info, if it exists, from the indicated columns  */
      wcs.exists = 0;
      if( Xcol>0 && Ycol>0 ) {
	 tstat = 0;
	 ffgtcs( gParse.def_fptr, Xcol, Ycol,
		 &wcs.xrefval, &wcs.yrefval,
		 &wcs.xrefpix, &wcs.yrefpix,
		 &wcs.xinc,    &wcs.yinc,
		 &wcs.rot,      wcs.type,
		 &tstat );
	 if( tstat==NO_WCS_KEY ) {
	    wcs.exists = 0;
	 } else if( tstat ) {
	    gParse.status = tstat;
	    Free_Last_Node();
	    return( -1 );
	 } else {
	    wcs.exists = 1;
	 }
      }

      /*  Read in Region file  */

      fits_read_rgnfile( fname, &wcs, &Rgn, &gParse.status );
      if( gParse.status ) {
	 Free_Last_Node();
	 return( -1 );
      }

      that0->value.data.ptr = Rgn;

      if( OPER(NodeX)==CONST_OP && OPER(NodeY)==CONST_OP )
	 this->DoOp( this );
   }

   return( n );
}

static int New_Vector( int subNode )
{
   Node *this, *that;
   int n;

   n = Alloc_Node();
   if( n >= 0 ) {
      this              = gParse.Nodes + n;
      that              = gParse.Nodes + subNode;
      this->type        = that->type;
      this->nSubNodes   = 1;
      this->SubNodes[0] = subNode;
      this->operation   = '{';
      this->DoOp        = Do_Vector;
   }

   return( n );
}

static int Close_Vec( int vecNode )
{
   Node *this;
   int n, nelem=0;

   this = gParse.Nodes + vecNode;
   for( n=0; n < this->nSubNodes; n++ ) {
      if( TYPE( this->SubNodes[n] ) != this->type ) {
	 this->SubNodes[n] = New_Unary( this->type, 0, this->SubNodes[n] );
	 if( this->SubNodes[n]<0 ) return(-1);
      }
      nelem += SIZE(this->SubNodes[n]);
   }
   this->value.naxis    = 1;
   this->value.nelem    = nelem;
   this->value.naxes[0] = nelem;

   return( vecNode );
}

static int Locate_Col( Node *this )
/*  Locate the TABLE column number of any columns in "this" calculation.  */
/*  Return ZERO if none found, or negative if more than 1 found.          */
{
   Node *that;
   int  i, col=0, newCol, nfound=0;
   
   if( this->nSubNodes==0
       && this->operation<=0 && this->operation!=CONST_OP )
      return gParse.colData[ - this->operation].colnum;

   for( i=0; i<this->nSubNodes; i++ ) {
      that = gParse.Nodes + this->SubNodes[i];
      if( that->operation>0 ) {
	 newCol = Locate_Col( that );
	 if( newCol<=0 ) {
	    nfound += -newCol;
	 } else {
	    if( !nfound ) {
	       col = newCol;
	       nfound++;
	    } else if( col != newCol ) {
	       nfound++;
	    }
	 }
      } else if( that->operation!=CONST_OP ) {
	 /*  Found a Column  */
	 newCol = gParse.colData[- that->operation].colnum;
	 if( !nfound ) {
	    col = newCol;
	    nfound++;
	 } else if( col != newCol ) {
	    nfound++;
	 }
      }
   }
   if( nfound!=1 )
      return( - nfound );
   else
      return( col );
}

static int Test_Dims( int Node1, int Node2 )
{
   Node *that1, *that2;
   int valid, i;

   if( Node1<0 || Node2<0 ) return(0);

   that1 = gParse.Nodes + Node1;
   that2 = gParse.Nodes + Node2;

   if( that1->value.nelem==1 || that2->value.nelem==1 )
      valid = 1;
   else if( that1->type==that2->type
	    && that1->value.nelem==that2->value.nelem
	    && that1->value.naxis==that2->value.naxis ) {
      valid = 1;
      for( i=0; i<that1->value.naxis; i++ ) {
	 if( that1->value.naxes[i]!=that2->value.naxes[i] )
	    valid = 0;
      }
   } else
      valid = 0;
   return( valid );
}   

static void Copy_Dims( int Node1, int Node2 )
{
   Node *that1, *that2;
   int i;

   if( Node1<0 || Node2<0 ) return;

   that1 = gParse.Nodes + Node1;
   that2 = gParse.Nodes + Node2;

   that1->value.nelem = that2->value.nelem;
   that1->value.naxis = that2->value.naxis;
   for( i=0; i<that2->value.naxis; i++ )
      that1->value.naxes[i] = that2->value.naxes[i];
}

/********************************************************************/
/*    Routines for actually evaluating the expression start here    */
/********************************************************************/

void Evaluate_Parser( long firstRow, long nRows )
    /***********************************************************************/
    /*  Reset the parser for processing another batch of data...           */
    /*    firstRow:  Row number of the first element to evaluate           */
    /*    nRows:     Number of rows to be processed                        */
    /*  Initialize each COLUMN node so that its UNDEF and DATA pointers    */
    /*  point to the appropriate column arrays.                            */
    /*  Finally, call Evaluate_Node for final node.                        */
    /***********************************************************************/
{
   int     i, column;
   long    offset, rowOffset;
   static int rand_initialized = 0;

   /* Initialize the random number generator once and only once */
   if (rand_initialized == 0) {
     simplerng_srand( (unsigned int) time(NULL) );
     rand_initialized = 1;
   }

   gParse.firstRow = firstRow;
   gParse.nRows    = nRows;

   /*  Reset Column Nodes' pointers to point to right data and UNDEF arrays  */

   rowOffset = firstRow - gParse.firstDataRow;
   for( i=0; i<gParse.nNodes; i++ ) {
     if(    OPER(i) >  0 || OPER(i) == CONST_OP ) continue;

      column = -OPER(i);
      offset = gParse.varData[column].nelem * rowOffset;

      gParse.Nodes[i].value.undef = gParse.varData[column].undef + offset;

      switch( gParse.Nodes[i].type ) {
      case BITSTR:
	 gParse.Nodes[i].value.data.strptr =
	    (char**)gParse.varData[column].data + rowOffset;
	 gParse.Nodes[i].value.undef       = NULL;
	 break;
      case STRING:
	 gParse.Nodes[i].value.data.strptr = 
	    (char**)gParse.varData[column].data + rowOffset;
	 gParse.Nodes[i].value.undef = gParse.varData[column].undef + rowOffset;
	 break;
      case BOOLEAN:
	 gParse.Nodes[i].value.data.logptr = 
	    (char*)gParse.varData[column].data + offset;
	 break;
      case LONG:
	 gParse.Nodes[i].value.data.lngptr = 
	    (long*)gParse.varData[column].data + offset;
	 break;
      case DOUBLE:
	 gParse.Nodes[i].value.data.dblptr = 
	    (double*)gParse.varData[column].data + offset;
	 break;
      }
   }

   Evaluate_Node( gParse.resultNode );
}

static void Evaluate_Node( int thisNode )
    /**********************************************************************/
    /*  Recursively evaluate thisNode's subNodes, then call one of the    */
    /*  Do_<Action> functions pointed to by thisNode's DoOp element.      */
    /**********************************************************************/
{
   Node *this;
   int i;
   
   if( gParse.status ) return;

   this = gParse.Nodes + thisNode;
   if( this->operation>0 ) {  /* <=0 indicate constants and columns */
      i = this->nSubNodes;
      while( i-- ) {
	 Evaluate_Node( this->SubNodes[i] );
	 if( gParse.status ) return;
      }
      this->DoOp( this );
   }
}

static void Allocate_Ptrs( Node *this )
{
   long elem, row, size;

   if( this->type==BITSTR || this->type==STRING ) {

      this->value.data.strptr = (char**)malloc( gParse.nRows
						* sizeof(char*) );
      if( this->value.data.strptr ) {
	 this->value.data.strptr[0] = (char*)malloc( gParse.nRows
						     * (this->value.nelem+2)
						     * sizeof(char) );
	 if( this->value.data.strptr[0] ) {
	    row = 0;
	    while( (++row)<gParse.nRows ) {
	       this->value.data.strptr[row] =
		  this->value.data.strptr[row-1] + this->value.nelem+1;
	    }
	    if( this->type==STRING ) {
	       this->value.undef = this->value.data.strptr[row-1]
                                   + this->value.nelem+1;
	    } else {
	       this->value.undef = NULL;  /* BITSTRs don't use undef array */
	    }
	 } else {
	    gParse.status = MEMORY_ALLOCATION;
	    free( this->value.data.strptr );
	 }
      } else {
	 gParse.status = MEMORY_ALLOCATION;
      }

   } else {

      elem = this->value.nelem * gParse.nRows;
      switch( this->type ) {
      case DOUBLE:  size = sizeof( double ); break;
      case LONG:    size = sizeof( long   ); break;
      case BOOLEAN: size = sizeof( char   ); break;
      default:      size = 1;                break;
      }

      this->value.data.ptr = calloc(size+1, elem);

      if( this->value.data.ptr==NULL ) {
	 gParse.status = MEMORY_ALLOCATION;
      } else {
	 this->value.undef = (char *)this->value.data.ptr + elem*size;
      }
   }
}

static void Do_Unary( Node *this )
{
   Node *that;
   long elem;

   that = gParse.Nodes + this->SubNodes[0];

   if( that->operation==CONST_OP ) {  /* Operating on a constant! */
      switch( this->operation ) {
      case DOUBLE:
      case FLTCAST:
	 if( that->type==LONG )
	    this->value.data.dbl = (double)that->value.data.lng;
	 else if( that->type==BOOLEAN )
	    this->value.data.dbl = ( that->value.data.log ? 1.0 : 0.0 );
	 break;
      case LONG:
      case INTCAST:
	 if( that->type==DOUBLE )
	    this->value.data.lng = (long)that->value.data.dbl;
	 else if( that->type==BOOLEAN )
	    this->value.data.lng = ( that->value.data.log ? 1L : 0L );
	 break;
      case BOOLEAN:
	 if( that->type==DOUBLE )
	    this->value.data.log = ( that->value.data.dbl != 0.0 );
	 else if( that->type==LONG )
	    this->value.data.log = ( that->value.data.lng != 0L );
	 break;
      case UMINUS:
	 if( that->type==DOUBLE )
	    this->value.data.dbl = - that->value.data.dbl;
	 else if( that->type==LONG )
	    this->value.data.lng = - that->value.data.lng;
	 break;
      case NOT:
	 if( that->type==BOOLEAN )
	    this->value.data.log = ( ! that->value.data.log );
	 else if( that->type==BITSTR )
	    bitnot( this->value.data.str, that->value.data.str );
	 break;
      }
      this->operation = CONST_OP;

   } else {

      Allocate_Ptrs( this );

      if( !gParse.status ) {

	 if( this->type!=BITSTR ) {
	    elem = gParse.nRows;
	    if( this->type!=STRING )
	       elem *= this->value.nelem;
	    while( elem-- )
	       this->value.undef[elem] = that->value.undef[elem];
	 }

	 elem = gParse.nRows * this->value.nelem;

	 switch( this->operation ) {

	 case BOOLEAN:
	    if( that->type==DOUBLE )
	       while( elem-- )
		  this->value.data.logptr[elem] =
		     ( that->value.data.dblptr[elem] != 0.0 );
	    else if( that->type==LONG )
	       while( elem-- )
		  this->value.data.logptr[elem] =
		     ( that->value.data.lngptr[elem] != 0L );
	    break;

	 case DOUBLE:
	 case FLTCAST:
	    if( that->type==LONG )
	       while( elem-- )
		  this->value.data.dblptr[elem] =
		     (double)that->value.data.lngptr[elem];
	    else if( that->type==BOOLEAN )
	       while( elem-- )
		  this->value.data.dblptr[elem] =
		     ( that->value.data.logptr[elem] ? 1.0 : 0.0 );
	    break;

	 case LONG:
	 case INTCAST:
	    if( that->type==DOUBLE )
	       while( elem-- )
		  this->value.data.lngptr[elem] =
		     (long)that->value.data.dblptr[elem];
	    else if( that->type==BOOLEAN )
	       while( elem-- )
		  this->value.data.lngptr[elem] =
		     ( that->value.data.logptr[elem] ? 1L : 0L );
	    break;

	 case UMINUS:
	    if( that->type==DOUBLE ) {
	       while( elem-- )
		  this->value.data.dblptr[elem] =
		     - that->value.data.dblptr[elem];
	    } else if( that->type==LONG ) {
	       while( elem-- )
		  this->value.data.lngptr[elem] =
		     - that->value.data.lngptr[elem];
	    }
	    break;

	 case NOT:
	    if( that->type==BOOLEAN ) {
	       while( elem-- )
		  this->value.data.logptr[elem] =
		     ( ! that->value.data.logptr[elem] );
	    } else if( that->type==BITSTR ) {
	       elem = gParse.nRows;
	       while( elem-- )
		  bitnot( this->value.data.strptr[elem],
			  that->value.data.strptr[elem] );
	    }
	    break;
	 }
      }
   }

   if( that->operation>0 ) {
      free( that->value.data.ptr );
   }
}

static void Do_Offset( Node *this )
{
   Node *col;
   long fRow, nRowOverlap, nRowReload, rowOffset;
   long nelem, elem, offset, nRealElem;
   int status;

   col       = gParse.Nodes + this->SubNodes[0];
   rowOffset = gParse.Nodes[  this->SubNodes[1] ].value.data.lng;

   Allocate_Ptrs( this );

   fRow   = gParse.firstRow + rowOffset;
   if( this->type==STRING || this->type==BITSTR )
      nRealElem = 1;
   else
      nRealElem = this->value.nelem;

   nelem = nRealElem;

   if( fRow < gParse.firstDataRow ) {

      /* Must fill in data at start of array */

      nRowReload = gParse.firstDataRow - fRow;
      if( nRowReload > gParse.nRows ) nRowReload = gParse.nRows;
      nRowOverlap = gParse.nRows - nRowReload;

      offset = 0;

      /*  NULLify any values falling out of bounds  */

      while( fRow<1 && nRowReload>0 ) {
	 if( this->type == BITSTR ) {
	    nelem = this->value.nelem;
	    this->value.data.strptr[offset][ nelem ] = '\0';
	    while( nelem-- ) this->value.data.strptr[offset][nelem] = '0';
	    offset++;
	 } else {
	    while( nelem-- )
	       this->value.undef[offset++] = 1;
	 }
	 nelem = nRealElem;
	 fRow++;
	 nRowReload--;
      }

   } else if( fRow + gParse.nRows > gParse.firstDataRow + gParse.nDataRows ) {

      /* Must fill in data at end of array */

      nRowReload = (fRow+gParse.nRows) - (gParse.firstDataRow+gParse.nDataRows);
      if( nRowReload>gParse.nRows ) {
	 nRowReload = gParse.nRows;
      } else {
	 fRow = gParse.firstDataRow + gParse.nDataRows;
      }
      nRowOverlap = gParse.nRows - nRowReload;

      offset = nRowOverlap * nelem;

      /*  NULLify any values falling out of bounds  */

      elem = gParse.nRows * nelem;
      while( fRow+nRowReload>gParse.totalRows && nRowReload>0 ) {
	 if( this->type == BITSTR ) {
	    nelem = this->value.nelem;
	    elem--;
	    this->value.data.strptr[elem][ nelem ] = '\0';
	    while( nelem-- ) this->value.data.strptr[elem][nelem] = '0';
	 } else {
	    while( nelem-- )
	       this->value.undef[--elem] = 1;
	 }
	 nelem = nRealElem;
	 nRowReload--;
      }

   } else {

      nRowReload  = 0;
      nRowOverlap = gParse.nRows;
      offset      = 0;

   }

   if( nRowReload>0 ) {
      switch( this->type ) {
      case BITSTR:
      case STRING:
	 status = (*gParse.loadData)( -col->operation, fRow, nRowReload,
				      this->value.data.strptr+offset,
				      this->value.undef+offset );
	 break;
      case BOOLEAN:
	 status = (*gParse.loadData)( -col->operation, fRow, nRowReload,
				      this->value.data.logptr+offset,
				      this->value.undef+offset );
	 break;
      case LONG:
	 status = (*gParse.loadData)( -col->operation, fRow, nRowReload,
				      this->value.data.lngptr+offset,
				      this->value.undef+offset );
	 break;
      case DOUBLE:
	 status = (*gParse.loadData)( -col->operation, fRow, nRowReload,
				      this->value.data.dblptr+offset,
				      this->value.undef+offset );
	 break;
      }
   }

   /*  Now copy over the overlapping region, if any  */

   if( nRowOverlap <= 0 ) return;

   if( rowOffset>0 )
      elem = nRowOverlap * nelem;
   else
      elem = gParse.nRows * nelem;

   offset = nelem * rowOffset;
   while( nRowOverlap-- && !gParse.status ) {
      while( nelem-- && !gParse.status ) {
	 elem--;
	 if( this->type != BITSTR )
	    this->value.undef[elem] = col->value.undef[elem+offset];
	 switch( this->type ) {
	 case BITSTR:
	    strcpy( this->value.data.strptr[elem       ],
                     col->value.data.strptr[elem+offset] );
	    break;
	 case STRING:
	    strcpy( this->value.data.strptr[elem       ],
                     col->value.data.strptr[elem+offset] );
	    break;
	 case BOOLEAN:
	    this->value.data.logptr[elem] = col->value.data.logptr[elem+offset];
	    break;
	 case LONG:
	    this->value.data.lngptr[elem] = col->value.data.lngptr[elem+offset];
	    break;
	 case DOUBLE:
	    this->value.data.dblptr[elem] = col->value.data.dblptr[elem+offset];
	    break;
	 }
      }
      nelem = nRealElem;
   }
}

static void Do_BinOp_bit( Node *this )
{
   Node *that1, *that2;
   char *sptr1=NULL, *sptr2=NULL;
   int  const1, const2;
   long rows;

   that1 = gParse.Nodes + this->SubNodes[0];
   that2 = gParse.Nodes + this->SubNodes[1];

   const1 = ( that1->operation==CONST_OP );
   const2 = ( that2->operation==CONST_OP );
   sptr1  = ( const1 ? that1->value.data.str : NULL );
   sptr2  = ( const2 ? that2->value.data.str : NULL );

   if( const1 && const2 ) {
      switch( this->operation ) {
      case NE:
	 this->value.data.log = !bitcmp( sptr1, sptr2 );
	 break;
      case EQ:
	 this->value.data.log =  bitcmp( sptr1, sptr2 );
	 break;
      case GT:
      case LT:
      case LTE:
      case GTE:
	 this->value.data.log = bitlgte( sptr1, this->operation, sptr2 );
	 break;
      case '|': 
	 bitor( this->value.data.str, sptr1, sptr2 );
	 break;
      case '&': 
	 bitand( this->value.data.str, sptr1, sptr2 );
	 break;
      case '+':
	 strcpy( this->value.data.str, sptr1 );
	 strcat( this->value.data.str, sptr2 );
	 break;
      case ACCUM:
	this->value.data.lng = 0;
	while( *sptr1 ) {
	  if ( *sptr1 == '1' ) this->value.data.lng ++;
	  sptr1 ++;
	}
	break;
	
      }
      this->operation = CONST_OP;

   } else {

      Allocate_Ptrs( this );

      if( !gParse.status ) {
	 rows  = gParse.nRows;
	 switch( this->operation ) {

	    /*  BITSTR comparisons  */

	 case NE:
	 case EQ:
	 case GT:
	 case LT:
	 case LTE:
	 case GTE:
	    while( rows-- ) {
	       if( !const1 )
		  sptr1 = that1->value.data.strptr[rows];
	       if( !const2 )
		  sptr2 = that2->value.data.strptr[rows];
	       switch( this->operation ) {
	       case NE:  this->value.data.logptr[rows] = 
                                                      !bitcmp( sptr1, sptr2 );
                         break;
	       case EQ:  this->value.data.logptr[rows] = 
                                                       bitcmp( sptr1, sptr2 );
                         break;
	       case GT:
	       case LT:
	       case LTE:
	       case GTE: this->value.data.logptr[rows] = 
                                     bitlgte( sptr1, this->operation, sptr2 );
	                 break;
	       }
	       this->value.undef[rows] = 0;
	    }
	    break;
	 
	    /*  BITSTR AND/ORs ...  no UNDEFS in or out */
      
	 case '|': 
	 case '&': 
	 case '+':
	    while( rows-- ) {
	       if( !const1 )
		  sptr1 = that1->value.data.strptr[rows];
	       if( !const2 )
		  sptr2 = that2->value.data.strptr[rows];
	       if( this->operation=='|' )
		  bitor(  this->value.data.strptr[rows], sptr1, sptr2 );
	       else if( this->operation=='&' )
		  bitand( this->value.data.strptr[rows], sptr1, sptr2 );
	       else {
		  strcpy( this->value.data.strptr[rows], sptr1 );
		  strcat( this->value.data.strptr[rows], sptr2 );
	       }
	    }
	    break;

	    /* Accumulate 1 bits */
	 case ACCUM:
	   { 
	     long i, previous, curr;

	     previous = that2->value.data.lng;
	     
	      /* Cumulative sum of this chunk */
	     for (i=0; i<rows; i++) {
	       sptr1 = that1->value.data.strptr[i];
	       for (curr = 0; *sptr1; sptr1 ++) {
		 if ( *sptr1 == '1' ) curr ++;
	       }
	       previous += curr;
	       this->value.data.lngptr[i] = previous;
	       this->value.undef[i] = 0;
	     }
	     
	      /* Store final cumulant for next pass */
	     that2->value.data.lng = previous;
	   }
	 }
      }
   }

   if( that1->operation>0 ) {
      free( that1->value.data.strptr[0] );
      free( that1->value.data.strptr    );
   }
   if( that2->operation>0 ) {
      free( that2->value.data.strptr[0] );
      free( that2->value.data.strptr    );
   }
}

static void Do_BinOp_str( Node *this )
{
   Node *that1, *that2;
   char *sptr1, *sptr2, null1=0, null2=0;
   int const1, const2, val;
   long rows;

   that1 = gParse.Nodes + this->SubNodes[0];
   that2 = gParse.Nodes + this->SubNodes[1];

   const1 = ( that1->operation==CONST_OP );
   const2 = ( that2->operation==CONST_OP );
   sptr1  = ( const1 ? that1->value.data.str : NULL );
   sptr2  = ( const2 ? that2->value.data.str : NULL );

   if( const1 && const2 ) {  /*  Result is a constant  */
      switch( this->operation ) {

	 /*  Compare Strings  */

      case NE:
      case EQ:
	 val = ( FSTRCMP( sptr1, sptr2 ) == 0 );
	 this->value.data.log = ( this->operation==EQ ? val : !val );
	 break;
      case GT:
	 this->value.data.log = ( FSTRCMP( sptr1, sptr2 ) > 0 );
	 break;
      case LT:
	 this->value.data.log = ( FSTRCMP( sptr1, sptr2 ) < 0 );
	 break;
      case GTE:
	 this->value.data.log = ( FSTRCMP( sptr1, sptr2 ) >= 0 );
	 break;
      case LTE:
	 this->value.data.log = ( FSTRCMP( sptr1, sptr2 ) <= 0 );
	 break;

	 /*  Concat Strings  */

      case '+':
	 strcpy( this->value.data.str, sptr1 );
	 strcat( this->value.data.str, sptr2 );
	 break;
      }
      this->operation = CONST_OP;

   } else {  /*  Not a constant  */

      Allocate_Ptrs( this );

      if( !gParse.status ) {

	 rows = gParse.nRows;
	 switch( this->operation ) {

	    /*  Compare Strings  */

	 case NE:
	 case EQ:
	    while( rows-- ) {
	       if( !const1 ) null1 = that1->value.undef[rows];
	       if( !const2 ) null2 = that2->value.undef[rows];
	       this->value.undef[rows] = (null1 || null2);
	       if( ! this->value.undef[rows] ) {
		  if( !const1 ) sptr1  = that1->value.data.strptr[rows];
		  if( !const2 ) sptr2  = that2->value.data.strptr[rows];
		  val = ( FSTRCMP( sptr1, sptr2 ) == 0 );
		  this->value.data.logptr[rows] =
		     ( this->operation==EQ ? val : !val );
	       }
	    }
	    break;
	    
	 case GT:
	 case LT:
	    while( rows-- ) {
	       if( !const1 ) null1 = that1->value.undef[rows];
	       if( !const2 ) null2 = that2->value.undef[rows];
	       this->value.undef[rows] = (null1 || null2);
	       if( ! this->value.undef[rows] ) {
		  if( !const1 ) sptr1  = that1->value.data.strptr[rows];
		  if( !const2 ) sptr2  = that2->value.data.strptr[rows];
		  val = ( FSTRCMP( sptr1, sptr2 ) );
		  this->value.data.logptr[rows] =
		     ( this->operation==GT ? val>0 : val<0 );
	       }
	    }
	    break;

	 case GTE:
	 case LTE:
	    while( rows-- ) {
	       if( !const1 ) null1 = that1->value.undef[rows];
	       if( !const2 ) null2 = that2->value.undef[rows];
	       this->value.undef[rows] = (null1 || null2);
	       if( ! this->value.undef[rows] ) {
		  if( !const1 ) sptr1  = that1->value.data.strptr[rows];
		  if( !const2 ) sptr2  = that2->value.data.strptr[rows];
		  val = ( FSTRCMP( sptr1, sptr2 ) );
		  this->value.data.logptr[rows] =
		     ( this->operation==GTE ? val>=0 : val<=0 );
	       }
	    }
	    break;

	    /*  Concat Strings  */
	    
	 case '+':
	    while( rows-- ) {
	       if( !const1 ) null1 = that1->value.undef[rows];
	       if( !const2 ) null2 = that2->value.undef[rows];
	       this->value.undef[rows] = (null1 || null2);
	       if( ! this->value.undef[rows] ) {
		  if( !const1 ) sptr1  = that1->value.data.strptr[rows];
		  if( !const2 ) sptr2  = that2->value.data.strptr[rows];
		  strcpy( this->value.data.strptr[rows], sptr1 );
		  strcat( this->value.data.strptr[rows], sptr2 );
	       }
	    }
	    break;
	 }
      }
   }

   if( that1->operation>0 ) {
      free( that1->value.data.strptr[0] );
      free( that1->value.data.strptr );
   }
   if( that2->operation>0 ) {
      free( that2->value.data.strptr[0] );
      free( that2->value.data.strptr );
   }
}

static void Do_BinOp_log( Node *this )
{
   Node *that1, *that2;
   int vector1, vector2;
   char val1=0, val2=0, null1=0, null2=0;
   long rows, nelem, elem;

   that1 = gParse.Nodes + this->SubNodes[0];
   that2 = gParse.Nodes + this->SubNodes[1];

   vector1 = ( that1->operation!=CONST_OP );
   if( vector1 )
      vector1 = that1->value.nelem;
   else {
      val1  = that1->value.data.log;
   }

   vector2 = ( that2->operation!=CONST_OP );
   if( vector2 )
      vector2 = that2->value.nelem;
   else {
      val2  = that2->value.data.log;
   }

   if( !vector1 && !vector2 ) {  /*  Result is a constant  */
      switch( this->operation ) {
      case OR:
	 this->value.data.log = (val1 || val2);
	 break;
      case AND:
	 this->value.data.log = (val1 && val2);
	 break;
      case EQ:
	 this->value.data.log = ( (val1 && val2) || (!val1 && !val2) );
	 break;
      case NE:
	 this->value.data.log = ( (val1 && !val2) || (!val1 && val2) );
	 break;
      case ACCUM:
	 this->value.data.lng = val1;
	 break;
      }
      this->operation=CONST_OP;
   } else if (this->operation == ACCUM) {
      long i, previous, curr;
      rows  = gParse.nRows;
      nelem = this->value.nelem;
      elem  = this->value.nelem * rows;
      
      Allocate_Ptrs( this );
      
      if( !gParse.status ) {
	previous = that2->value.data.lng;
	
	/* Cumulative sum of this chunk */
	for (i=0; i<elem; i++) {
	  if (!that1->value.undef[i]) {
	    curr = that1->value.data.logptr[i];
	    previous += curr;
	  }
	  this->value.data.lngptr[i] = previous;
	  this->value.undef[i] = 0;
	}
	
	/* Store final cumulant for next pass */
	that2->value.data.lng = previous;
      }
      
   } else {
      rows  = gParse.nRows;
      nelem = this->value.nelem;
      elem  = this->value.nelem * rows;

      Allocate_Ptrs( this );

      if( !gParse.status ) {
	
	 if (this->operation == ACCUM) {
	   long i, previous, curr;
	   
	   previous = that2->value.data.lng;
	   
	   /* Cumulative sum of this chunk */
	   for (i=0; i<elem; i++) {
	     if (!that1->value.undef[i]) {
	       curr = that1->value.data.logptr[i];
	       previous += curr;
	     }
	     this->value.data.lngptr[i] = previous;
	     this->value.undef[i] = 0;
	   }
	   
	   /* Store final cumulant for next pass */
	   that2->value.data.lng = previous;
	 }
	
	 while( rows-- ) {
	    while( nelem-- ) {
	       elem--;

	       if( vector1>1 ) {
		  val1  = that1->value.data.logptr[elem];
		  null1 = that1->value.undef[elem];
	       } else if( vector1 ) {
		  val1  = that1->value.data.logptr[rows];
		  null1 = that1->value.undef[rows];
	       }

	       if( vector2>1 ) {
		  val2  = that2->value.data.logptr[elem];
		  null2 = that2->value.undef[elem];
	       } else if( vector2 ) {
		  val2  = that2->value.data.logptr[rows];
		  null2 = that2->value.undef[rows];
	       }

	       this->value.undef[elem] = (null1 || null2);
	       switch( this->operation ) {

	       case OR:
		  /*  This is more complicated than others to suppress UNDEFs */
		  /*  in those cases where the other argument is DEF && TRUE  */

		  if( !null1 && !null2 ) {
		     this->value.data.logptr[elem] = (val1 || val2);
		  } else if( (null1 && !null2 && val2)
			     || ( !null1 && null2 && val1 ) ) {
		     this->value.data.logptr[elem] = 1;
		     this->value.undef[elem] = 0;
		  }
		  break;

	       case AND:
		  /*  This is more complicated than others to suppress UNDEFs */
		  /*  in those cases where the other argument is DEF && FALSE */

		  if( !null1 && !null2 ) {
		     this->value.data.logptr[elem] = (val1 && val2);
		  } else if( (null1 && !null2 && !val2)
			     || ( !null1 && null2 && !val1 ) ) {
		     this->value.data.logptr[elem] = 0;
		     this->value.undef[elem] = 0;
		  }
		  break;

	       case EQ:
		  this->value.data.logptr[elem] = 
		     ( (val1 && val2) || (!val1 && !val2) );
		  break;

	       case NE:
		  this->value.data.logptr[elem] =
		     ( (val1 && !val2) || (!val1 && val2) );
		  break;
	       }
	    }
	    nelem = this->value.nelem;
	 }
      }
   }

   if( that1->operation>0 ) {
      free( that1->value.data.ptr );
   }
   if( that2->operation>0 ) {
      free( that2->value.data.ptr );
   }
}

static void Do_BinOp_lng( Node *this )
{
   Node *that1, *that2;
   int  vector1, vector2;
   long val1=0, val2=0;
   char null1=0, null2=0;
   long rows, nelem, elem;

   that1 = gParse.Nodes + this->SubNodes[0];
   that2 = gParse.Nodes + this->SubNodes[1];

   vector1 = ( that1->operation!=CONST_OP );
   if( vector1 )
      vector1 = that1->value.nelem;
   else {
      val1  = that1->value.data.lng;
   }

   vector2 = ( that2->operation!=CONST_OP );
   if( vector2 )
      vector2 = that2->value.nelem;
   else {
      val2  = that2->value.data.lng;
   }

   if( !vector1 && !vector2 ) {  /*  Result is a constant  */

      switch( this->operation ) {
      case '~':   /* Treat as == for LONGS */
      case EQ:    this->value.data.log = (val1 == val2);   break;
      case NE:    this->value.data.log = (val1 != val2);   break;
      case GT:    this->value.data.log = (val1 >  val2);   break;
      case LT:    this->value.data.log = (val1 <  val2);   break;
      case LTE:   this->value.data.log = (val1 <= val2);   break;
      case GTE:   this->value.data.log = (val1 >= val2);   break;

      case '+':   this->value.data.lng = (val1  + val2);   break;
      case '-':   this->value.data.lng = (val1  - val2);   break;
      case '*':   this->value.data.lng = (val1  * val2);   break;

      case '&':   this->value.data.lng = (val1  & val2);   break;
      case '|':   this->value.data.lng = (val1  | val2);   break;
      case '^':   this->value.data.lng = (val1  ^ val2);   break;

      case '%':
	 if( val2 ) this->value.data.lng = (val1 % val2);
	 else       fferror("Divide by Zero");
	 break;
      case '/': 
	 if( val2 ) this->value.data.lng = (val1 / val2); 
	 else       fferror("Divide by Zero");
	 break;
      case POWER:
	 this->value.data.lng = (long)pow((double)val1,(double)val2);
	 break;
      case ACCUM:
	 this->value.data.lng = val1;
	 break;
      case DIFF:
	 this->value.data.lng = 0;
	 break;
      }
      this->operation=CONST_OP;

   } else if ((this->operation == ACCUM) || (this->operation == DIFF)) {
      long i, previous, curr;
      long undef;
      rows  = gParse.nRows;
      nelem = this->value.nelem;
      elem  = this->value.nelem * rows;
      
      Allocate_Ptrs( this );
      
      if( !gParse.status ) {
	previous = that2->value.data.lng;
	undef    = (long) that2->value.undef;
	
	if (this->operation == ACCUM) {
	  /* Cumulative sum of this chunk */
	  for (i=0; i<elem; i++) {
	    if (!that1->value.undef[i]) {
	      curr = that1->value.data.lngptr[i];
	      previous += curr;
	    }
	    this->value.data.lngptr[i] = previous;
	    this->value.undef[i] = 0;
	  }
	} else {
	  /* Sequential difference for this chunk */
	  for (i=0; i<elem; i++) {
	    curr = that1->value.data.lngptr[i];
	    if (that1->value.undef[i] || undef) {
	      /* Either this, or previous, value was undefined */
	      this->value.data.lngptr[i] = 0;
	      this->value.undef[i] = 1;
	    } else {
	      /* Both defined, we are okay! */
	      this->value.data.lngptr[i] = curr - previous;
	      this->value.undef[i] = 0;
	    }

	    previous = curr;
	    undef = that1->value.undef[i];
	  }
	}	  
	
	/* Store final cumulant for next pass */
	that2->value.data.lng = previous;
	that2->value.undef    = (char *) undef; /* XXX evil, but no harm here */
      }
      
   } else {

      rows  = gParse.nRows;
      nelem = this->value.nelem;
      elem  = this->value.nelem * rows;

      Allocate_Ptrs( this );

      while( rows-- && !gParse.status ) {
	 while( nelem-- && !gParse.status ) {
	    elem--;

	    if( vector1>1 ) {
	       val1  = that1->value.data.lngptr[elem];
	       null1 = that1->value.undef[elem];
	    } else if( vector1 ) {
	       val1  = that1->value.data.lngptr[rows];
	       null1 = that1->value.undef[rows];
	    }

	    if( vector2>1 ) {
	       val2  = that2->value.data.lngptr[elem];
	       null2 = that2->value.undef[elem];
	    } else if( vector2 ) {
	       val2  = that2->value.data.lngptr[rows];
	       null2 = that2->value.undef[rows];
	    }

	    this->value.undef[elem] = (null1 || null2);
	    switch( this->operation ) {
	    case '~':   /* Treat as == for LONGS */
	    case EQ:   this->value.data.logptr[elem] = (val1 == val2);   break;
	    case NE:   this->value.data.logptr[elem] = (val1 != val2);   break;
	    case GT:   this->value.data.logptr[elem] = (val1 >  val2);   break;
	    case LT:   this->value.data.logptr[elem] = (val1 <  val2);   break;
	    case LTE:  this->value.data.logptr[elem] = (val1 <= val2);   break;
	    case GTE:  this->value.data.logptr[elem] = (val1 >= val2);   break;
	       
	    case '+':  this->value.data.lngptr[elem] = (val1  + val2);   break;
	    case '-':  this->value.data.lngptr[elem] = (val1  - val2);   break;
	    case '*':  this->value.data.lngptr[elem] = (val1  * val2);   break;

	    case '&':  this->value.data.lngptr[elem] = (val1  & val2);   break;
	    case '|':  this->value.data.lngptr[elem] = (val1  | val2);   break;
	    case '^':  this->value.data.lngptr[elem] = (val1  ^ val2);   break;

	    case '%':   
	       if( val2 ) this->value.data.lngptr[elem] = (val1 % val2);
	       else {
		 this->value.data.lngptr[elem] = 0;
		 this->value.undef[elem] = 1;
	       }
	       break;
	    case '/': 
	       if( val2 ) this->value.data.lngptr[elem] = (val1 / val2); 
	       else {
		 this->value.data.lngptr[elem] = 0;
		 this->value.undef[elem] = 1;
	       }
	       break;
	    case POWER:
	       this->value.data.lngptr[elem] = (long)pow((double)val1,(double)val2);
	       break;
	    }
	 }
	 nelem = this->value.nelem;
      }
   }

   if( that1->operation>0 ) {
      free( that1->value.data.ptr );
   }
   if( that2->operation>0 ) {
      free( that2->value.data.ptr );
   }
}

static void Do_BinOp_dbl( Node *this )
{
   Node   *that1, *that2;
   int    vector1, vector2;
   double val1=0.0, val2=0.0;
   char   null1=0, null2=0;
   long   rows, nelem, elem;

   that1 = gParse.Nodes + this->SubNodes[0];
   that2 = gParse.Nodes + this->SubNodes[1];

   vector1 = ( that1->operation!=CONST_OP );
   if( vector1 )
      vector1 = that1->value.nelem;
   else {
      val1  = that1->value.data.dbl;
   }

   vector2 = ( that2->operation!=CONST_OP );
   if( vector2 )
      vector2 = that2->value.nelem;
   else {
      val2  = that2->value.data.dbl;
   } 

   if( !vector1 && !vector2 ) {  /*  Result is a constant  */

      switch( this->operation ) {
      case '~':   this->value.data.log = ( fabs(val1-val2) < APPROX );   break;
      case EQ:    this->value.data.log = (val1 == val2);   break;
      case NE:    this->value.data.log = (val1 != val2);   break;
      case GT:    this->value.data.log = (val1 >  val2);   break;
      case LT:    this->value.data.log = (val1 <  val2);   break;
      case LTE:   this->value.data.log = (val1 <= val2);   break;
      case GTE:   this->value.data.log = (val1 >= val2);   break;

      case '+':   this->value.data.dbl = (val1  + val2);   break;
      case '-':   this->value.data.dbl = (val1  - val2);   break;
      case '*':   this->value.data.dbl = (val1  * val2);   break;

      case '%':
	 if( val2 ) this->value.data.dbl = val1 - val2*((int)(val1/val2));
	 else       fferror("Divide by Zero");
	 break;
      case '/': 
	 if( val2 ) this->value.data.dbl = (val1 / val2); 
	 else       fferror("Divide by Zero");
	 break;
      case POWER:
	 this->value.data.dbl = (double)pow(val1,val2);
	 break;
      case ACCUM:
	 this->value.data.dbl = val1;
	 break;
      case DIFF:
	this->value.data.dbl = 0;
	 break;
      }
      this->operation=CONST_OP;

   } else if ((this->operation == ACCUM) || (this->operation == DIFF)) {
      long i;
      long undef;
      double previous, curr;
      rows  = gParse.nRows;
      nelem = this->value.nelem;
      elem  = this->value.nelem * rows;
      
      Allocate_Ptrs( this );
      
      if( !gParse.status ) {
	previous = that2->value.data.dbl;
	undef    = (long) that2->value.undef;
	
	if (this->operation == ACCUM) {
	  /* Cumulative sum of this chunk */
	  for (i=0; i<elem; i++) {
	    if (!that1->value.undef[i]) {
	      curr = that1->value.data.dblptr[i];
	      previous += curr;
	    }
	    this->value.data.dblptr[i] = previous;
	    this->value.undef[i] = 0;
	  }
	} else {
	  /* Sequential difference for this chunk */
	  for (i=0; i<elem; i++) {
	    curr = that1->value.data.dblptr[i];
	    if (that1->value.undef[i] || undef) {
	      /* Either this, or previous, value was undefined */
	      this->value.data.dblptr[i] = 0;
	      this->value.undef[i] = 1;
	    } else {
	      /* Both defined, we are okay! */
	      this->value.data.dblptr[i] = curr - previous;
	      this->value.undef[i] = 0;
	    }

	    previous = curr;
	    undef = that1->value.undef[i];
	  }
	}	  
	
	/* Store final cumulant for next pass */
	that2->value.data.dbl = previous;
	that2->value.undef    = (char *) undef; /* XXX evil, but no harm here */
      }
      
   } else {

      rows  = gParse.nRows;
      nelem = this->value.nelem;
      elem  = this->value.nelem * rows;

      Allocate_Ptrs( this );

      while( rows-- && !gParse.status ) {
	 while( nelem-- && !gParse.status ) {
	    elem--;

	    if( vector1>1 ) {
	       val1  = that1->value.data.dblptr[elem];
	       null1 = that1->value.undef[elem];
	    } else if( vector1 ) {
	       val1  = that1->value.data.dblptr[rows];
	       null1 = that1->value.undef[rows];
	    }

	    if( vector2>1 ) {
	       val2  = that2->value.data.dblptr[elem];
	       null2 = that2->value.undef[elem];
	    } else if( vector2 ) {
	       val2  = that2->value.data.dblptr[rows];
	       null2 = that2->value.undef[rows];
	    }

	    this->value.undef[elem] = (null1 || null2);
	    switch( this->operation ) {
	    case '~':   this->value.data.logptr[elem] =
                                          ( fabs(val1-val2) < APPROX );   break;
	    case EQ:    this->value.data.logptr[elem] = (val1 == val2);   break;
	    case NE:    this->value.data.logptr[elem] = (val1 != val2);   break;
	    case GT:    this->value.data.logptr[elem] = (val1 >  val2);   break;
	    case LT:    this->value.data.logptr[elem] = (val1 <  val2);   break;
	    case LTE:   this->value.data.logptr[elem] = (val1 <= val2);   break;
	    case GTE:   this->value.data.logptr[elem] = (val1 >= val2);   break;
	       
	    case '+':   this->value.data.dblptr[elem] = (val1  + val2);   break;
	    case '-':   this->value.data.dblptr[elem] = (val1  - val2);   break;
	    case '*':   this->value.data.dblptr[elem] = (val1  * val2);   break;

	    case '%':
	       if( val2 ) this->value.data.dblptr[elem] =
                                val1 - val2*((int)(val1/val2));
	       else {
		 this->value.data.dblptr[elem] = 0.0;
		 this->value.undef[elem] = 1;
	       }
	       break;
	    case '/': 
	       if( val2 ) this->value.data.dblptr[elem] = (val1 / val2); 
	       else {
		 this->value.data.dblptr[elem] = 0.0;
		 this->value.undef[elem] = 1;
	       }
	       break;
	    case POWER:
	       this->value.data.dblptr[elem] = (double)pow(val1,val2);
	       break;
	    }
	 }
	 nelem = this->value.nelem;
      }
   }

   if( that1->operation>0 ) {
      free( that1->value.data.ptr );
   }
   if( that2->operation>0 ) {
      free( that2->value.data.ptr );
   }
}

/*
 *  This Quickselect routine is based on the algorithm described in
 *  "Numerical recipes in C", Second Edition,
 *  Cambridge University Press, 1992, Section 8.5, ISBN 0-521-43108-5
 *  This code by Nicolas Devillard - 1998. Public domain.
 * http://ndevilla.free.fr/median/median/src/quickselect.c
 */

#define ELEM_SWAP(a,b) { register long t=(a);(a)=(b);(b)=t; }

/* 
 * qselect_median_lng - select the median value of a long array
 *
 * This routine selects the median value of the long integer array
 * arr[].  If there are an even number of elements, the "lower median"
 * is selected.
 *
 * The array arr[] is scrambled, so users must operate on a scratch
 * array if they wish the values to be preserved.
 *
 * long arr[] - array of values
 * int n - number of elements in arr
 *
 * RETURNS: the lower median value of arr[]
 *
 */
long qselect_median_lng(long arr[], int n)
{
    int low, high ;
    int median;
    int middle, ll, hh;

    low = 0 ; high = n-1 ; median = (low + high) / 2;
    for (;;) {

        if (high <= low) { /* One element only */
	  return arr[median];	  
	}

        if (high == low + 1) {  /* Two elements only */
            if (arr[low] > arr[high])
                ELEM_SWAP(arr[low], arr[high]) ;
	    return arr[median];
        }

    /* Find median of low, middle and high items; swap into position low */
    middle = (low + high) / 2;
    if (arr[middle] > arr[high])    ELEM_SWAP(arr[middle], arr[high]) ;
    if (arr[low] > arr[high])       ELEM_SWAP(arr[low], arr[high]) ;
    if (arr[middle] > arr[low])     ELEM_SWAP(arr[middle], arr[low]) ;

    /* Swap low item (now in position middle) into position (low+1) */
    ELEM_SWAP(arr[middle], arr[low+1]) ;

    /* Nibble from each end towards middle, swapping items when stuck */
    ll = low + 1;
    hh = high;
    for (;;) {
        do ll++; while (arr[low] > arr[ll]) ;
        do hh--; while (arr[hh]  > arr[low]) ;

        if (hh < ll)
        break;

        ELEM_SWAP(arr[ll], arr[hh]) ;
    }

    /* Swap middle item (in position low) back into correct position */
    ELEM_SWAP(arr[low], arr[hh]) ;

    /* Re-set active partition */
    if (hh <= median)
        low = ll;
        if (hh >= median)
        high = hh - 1;
    }
}

#undef ELEM_SWAP

#define ELEM_SWAP(a,b) { register double t=(a);(a)=(b);(b)=t; }

/* 
 * qselect_median_dbl - select the median value of a double array
 *
 * This routine selects the median value of the double array
 * arr[].  If there are an even number of elements, the "lower median"
 * is selected.
 *
 * The array arr[] is scrambled, so users must operate on a scratch
 * array if they wish the values to be preserved.
 *
 * double arr[] - array of values
 * int n - number of elements in arr
 *
 * RETURNS: the lower median value of arr[]
 *
 */
double qselect_median_dbl(double arr[], int n)
{
    int low, high ;
    int median;
    int middle, ll, hh;

    low = 0 ; high = n-1 ; median = (low + high) / 2;
    for (;;) {
        if (high <= low) { /* One element only */
            return arr[median] ;
	}

        if (high == low + 1) {  /* Two elements only */
            if (arr[low] > arr[high])
                ELEM_SWAP(arr[low], arr[high]) ;
            return arr[median] ;
        }

    /* Find median of low, middle and high items; swap into position low */
    middle = (low + high) / 2;
    if (arr[middle] > arr[high])    ELEM_SWAP(arr[middle], arr[high]) ;
    if (arr[low] > arr[high])       ELEM_SWAP(arr[low], arr[high]) ;
    if (arr[middle] > arr[low])     ELEM_SWAP(arr[middle], arr[low]) ;

    /* Swap low item (now in position middle) into position (low+1) */
    ELEM_SWAP(arr[middle], arr[low+1]) ;

    /* Nibble from each end towards middle, swapping items when stuck */
    ll = low + 1;
    hh = high;
    for (;;) {
        do ll++; while (arr[low] > arr[ll]) ;
        do hh--; while (arr[hh]  > arr[low]) ;

        if (hh < ll)
        break;

        ELEM_SWAP(arr[ll], arr[hh]) ;
    }

    /* Swap middle item (in position low) back into correct position */
    ELEM_SWAP(arr[low], arr[hh]) ;

    /* Re-set active partition */
    if (hh <= median)
        low = ll;
        if (hh >= median)
        high = hh - 1;
    }
}

#undef ELEM_SWAP

/*
 * angsep_calc - compute angular separation between celestial coordinates
 *   
 * This routine computes the angular separation between to coordinates
 * on the celestial sphere (i.e. RA and Dec).  Note that all units are
 * in DEGREES, unlike the other trig functions in the calculator.
 *
 * double ra1, dec1 - RA and Dec of the first position in degrees
 * double ra2, dec2 - RA and Dec of the second position in degrees
 * 
 * RETURNS: (double) angular separation in degrees
 *
 */
double angsep_calc(double ra1, double dec1, double ra2, double dec2)
{
/*  double cd;  */
  static double deg = 0;
  double a, sdec, sra;
  
  if (deg == 0) deg = ((double)4)*atan((double)1)/((double)180);
  /* deg = 1.0; **** UNCOMMENT IF YOU WANT RADIANS */

  /* The algorithm is the law of Haversines.  This algorithm is
     stable even when the points are close together.  The normal
     Law of Cosines fails for angles around 0.1 arcsec. */

  sra  = sin( (ra2 - ra1)*deg / 2 );
  sdec = sin( (dec2 - dec1)*deg / 2);
  a = sdec*sdec + cos(dec1*deg)*cos(dec2*deg)*sra*sra;

  /* Sanity checking to avoid a range error in the sqrt()'s below */
  if (a < 0) { a = 0; }
  if (a > 1) { a = 1; }

  return 2.0*atan2(sqrt(a), sqrt(1.0 - a)) / deg;
}

static void Do_Func( Node *this )
{
   Node *theParams[MAXSUBS];
   int  vector[MAXSUBS], allConst;
   lval pVals[MAXSUBS];
   char pNull[MAXSUBS];
   long   ival;
   double dval;
   int  i, valInit;
   long row, elem, nelem;

   i = this->nSubNodes;
   allConst = 1;
   while( i-- ) {
      theParams[i] = gParse.Nodes + this->SubNodes[i];
      vector[i]   = ( theParams[i]->operation!=CONST_OP );
      if( vector[i] ) {
	 allConst = 0;
	 vector[i] = theParams[i]->value.nelem;
      } else {
	 if( theParams[i]->type==DOUBLE ) {
	    pVals[i].data.dbl = theParams[i]->value.data.dbl;
	 } else if( theParams[i]->type==LONG ) {
	    pVals[i].data.lng = theParams[i]->value.data.lng;
	 } else if( theParams[i]->type==BOOLEAN ) {
	    pVals[i].data.log = theParams[i]->value.data.log;
	 } else
	    strcpy(pVals[i].data.str, theParams[i]->value.data.str);
	 pNull[i] = 0;
      }
   }

   if( this->nSubNodes==0 ) allConst = 0; /* These do produce scalars */
   /* Random numbers are *never* constant !! */
   if( this->operation == poirnd_fct ) allConst = 0;
   if( this->operation == gasrnd_fct ) allConst = 0;
   if( this->operation == rnd_fct ) allConst = 0;

   if( allConst ) {

      switch( this->operation ) {

	    /* Non-Trig single-argument functions */

	 case sum_fct:
	    if( theParams[0]->type==BOOLEAN )
	       this->value.data.lng = ( pVals[0].data.log ? 1 : 0 );
	    else if( theParams[0]->type==LONG )
	       this->value.data.lng = pVals[0].data.lng;
	    else if( theParams[0]->type==DOUBLE )
	       this->value.data.dbl = pVals[0].data.dbl;
	    else if( theParams[0]->type==BITSTR )
	      strcpy(this->value.data.str, pVals[0].data.str);
	    break;
         case average_fct:
	    if( theParams[0]->type==LONG )
	       this->value.data.dbl = pVals[0].data.lng;
	    else if( theParams[0]->type==DOUBLE )
	       this->value.data.dbl = pVals[0].data.dbl;
	    break;
         case stddev_fct:
	    this->value.data.dbl = 0;  /* Standard deviation of a constant = 0 */
	    break;
	 case median_fct:
	    if( theParams[0]->type==BOOLEAN )
	       this->value.data.lng = ( pVals[0].data.log ? 1 : 0 );
	    else if( theParams[0]->type==LONG )
	       this->value.data.lng = pVals[0].data.lng;
	    else
	       this->value.data.dbl = pVals[0].data.dbl;
	    break;

	 case poirnd_fct:
	    if( theParams[0]->type==DOUBLE )
	      this->value.data.lng = simplerng_getpoisson(pVals[0].data.dbl);
	    else
	      this->value.data.lng = simplerng_getpoisson(pVals[0].data.lng);
	    break;

	 case abs_fct:
	    if( theParams[0]->type==DOUBLE ) {
	       dval = pVals[0].data.dbl;
	       this->value.data.dbl = (dval>0.0 ? dval : -dval);
	    } else {
	       ival = pVals[0].data.lng;
	       this->value.data.lng = (ival> 0  ? ival : -ival);
	    }
	    break;

            /* Special Null-Handling Functions */

         case nonnull_fct:
	    this->value.data.lng = 1; /* Constants are always 1-element and defined */
	    break;
         case isnull_fct:  /* Constants are always defined */
	    this->value.data.log = 0;
	    break;
         case defnull_fct:
	    if( this->type==BOOLEAN )
	       this->value.data.log = pVals[0].data.log;
            else if( this->type==LONG )
	       this->value.data.lng = pVals[0].data.lng;
            else if( this->type==DOUBLE )
	       this->value.data.dbl = pVals[0].data.dbl;
            else if( this->type==STRING )
	       strcpy(this->value.data.str,pVals[0].data.str);
	    break;
        case setnull_fct: /* Only defined for numeric expressions */
            if( this->type==LONG )
 	      this->value.data.lng = pVals[0].data.lng;
            else if( this->type==DOUBLE )
	       this->value.data.dbl = pVals[0].data.dbl;
	    break;

	    /* Math functions with 1 double argument */

	 case sin_fct:
	    this->value.data.dbl = sin( pVals[0].data.dbl );
	    break;
	 case cos_fct:
	    this->value.data.dbl = cos( pVals[0].data.dbl );
	    break;
	 case tan_fct:
	    this->value.data.dbl = tan( pVals[0].data.dbl );
	    break;
	 case asin_fct:
	    dval = pVals[0].data.dbl;
	    if( dval<-1.0 || dval>1.0 )
	       fferror("Out of range argument to arcsin");
	    else
	       this->value.data.dbl = asin( dval );
	    break;
	 case acos_fct:
	    dval = pVals[0].data.dbl;
	    if( dval<-1.0 || dval>1.0 )
	       fferror("Out of range argument to arccos");
	    else
	       this->value.data.dbl = acos( dval );
	    break;
	 case atan_fct:
	    this->value.data.dbl = atan( pVals[0].data.dbl );
	    break;
	 case sinh_fct:
	    this->value.data.dbl = sinh( pVals[0].data.dbl );
	    break;
	 case cosh_fct:
	    this->value.data.dbl = cosh( pVals[0].data.dbl );
	    break;
	 case tanh_fct:
	    this->value.data.dbl = tanh( pVals[0].data.dbl );
	    break;
	 case exp_fct:
	    this->value.data.dbl = exp( pVals[0].data.dbl );
	    break;
	 case log_fct:
	    dval = pVals[0].data.dbl;
	    if( dval<=0.0 )
	       fferror("Out of range argument to log");
	    else
	       this->value.data.dbl = log( dval );
	    break;
	 case log10_fct:
	    dval = pVals[0].data.dbl;
	    if( dval<=0.0 )
	       fferror("Out of range argument to log10");
	    else
	       this->value.data.dbl = log10( dval );
	    break;
	 case sqrt_fct:
	    dval = pVals[0].data.dbl;
	    if( dval<0.0 )
	       fferror("Out of range argument to sqrt");
	    else
	       this->value.data.dbl = sqrt( dval );
	    break;
	 case ceil_fct:
	    this->value.data.dbl = ceil( pVals[0].data.dbl );
	    break;
	 case floor_fct:
	    this->value.data.dbl = floor( pVals[0].data.dbl );
	    break;
	 case round_fct:
	    this->value.data.dbl = floor( pVals[0].data.dbl + 0.5 );
	    break;

	    /* Two-argument Trig Functions */

	 case atan2_fct:
	    this->value.data.dbl =
	       atan2( pVals[0].data.dbl, pVals[1].data.dbl );
	    break;

	    /* Four-argument ANGSEP function */
         case angsep_fct:
	    this->value.data.dbl = 
	      angsep_calc(pVals[0].data.dbl, pVals[1].data.dbl,
			  pVals[2].data.dbl, pVals[3].data.dbl);

	    /*  Min/Max functions taking 1 or 2 arguments  */

         case min1_fct:
	    /* No constant vectors! */
	    if( this->type == DOUBLE )
	       this->value.data.dbl = pVals[0].data.dbl;
	    else if( this->type == LONG )
	       this->value.data.lng = pVals[0].data.lng;
	    else if( this->type == BITSTR )
	      strcpy(this->value.data.str, pVals[0].data.str);
	    break;
         case min2_fct:
	    if( this->type == DOUBLE )
	       this->value.data.dbl =
		  minvalue( pVals[0].data.dbl, pVals[1].data.dbl );
	    else if( this->type == LONG )
	       this->value.data.lng =
		  minvalue( pVals[0].data.lng, pVals[1].data.lng );
	    break;
         case max1_fct:
	    /* No constant vectors! */
	    if( this->type == DOUBLE )
	       this->value.data.dbl = pVals[0].data.dbl;
	    else if( this->type == LONG )
	       this->value.data.lng = pVals[0].data.lng;
	    else if( this->type == BITSTR )
	      strcpy(this->value.data.str, pVals[0].data.str);
	    break;
         case max2_fct:
	    if( this->type == DOUBLE )
	       this->value.data.dbl =
		  maxvalue( pVals[0].data.dbl, pVals[1].data.dbl );
	    else if( this->type == LONG )
	       this->value.data.lng =
		  maxvalue( pVals[0].data.lng, pVals[1].data.lng );
	    break;

	    /* Boolean SAO region Functions... scalar or vector dbls */

	 case near_fct:
	    this->value.data.log = bnear( pVals[0].data.dbl, pVals[1].data.dbl,
					  pVals[2].data.dbl );
	    break;
	 case circle_fct:
	    this->value.data.log = circle( pVals[0].data.dbl, pVals[1].data.dbl,
					   pVals[2].data.dbl, pVals[3].data.dbl,
					   pVals[4].data.dbl );
	    break;
	 case box_fct:
	    this->value.data.log = saobox( pVals[0].data.dbl, pVals[1].data.dbl,
					   pVals[2].data.dbl, pVals[3].data.dbl,
					   pVals[4].data.dbl, pVals[5].data.dbl,
					   pVals[6].data.dbl );
	    break;
	 case elps_fct:
	    this->value.data.log =
                               ellipse( pVals[0].data.dbl, pVals[1].data.dbl,
					pVals[2].data.dbl, pVals[3].data.dbl,
					pVals[4].data.dbl, pVals[5].data.dbl,
					pVals[6].data.dbl );
	    break;

            /* C Conditional expression:  bool ? expr : expr */

         case ifthenelse_fct:
            switch( this->type ) {
            case BOOLEAN:
               this->value.data.log = ( pVals[2].data.log ?
                                        pVals[0].data.log : pVals[1].data.log );
               break;
            case LONG:
               this->value.data.lng = ( pVals[2].data.log ?
                                        pVals[0].data.lng : pVals[1].data.lng );
               break;
            case DOUBLE:
               this->value.data.dbl = ( pVals[2].data.log ?
                                        pVals[0].data.dbl : pVals[1].data.dbl );
               break;
            case STRING:
	       strcpy(this->value.data.str, ( pVals[2].data.log ?
                                              pVals[0].data.str :
                                              pVals[1].data.str ) );
               break;
            }
            break;

	    /* String functions */
         case strmid_fct:
	   cstrmid(this->value.data.str, this->value.nelem, 
		   pVals[0].data.str,    pVals[0].nelem,
		   pVals[1].data.lng);
	   break;
         case strpos_fct:
	   {
	     char *res = strstr(pVals[0].data.str, pVals[1].data.str);
	     if (res == NULL) {
	       this->value.data.lng = 0; 
	     } else {
	       this->value.data.lng = (res - pVals[0].data.str) + 1;
	     }
	     break;
	   }

      }
      this->operation = CONST_OP;

   } else {

      Allocate_Ptrs( this );

      row  = gParse.nRows;
      elem = row * this->value.nelem;

      if( !gParse.status ) {
	 switch( this->operation ) {

	    /* Special functions with no arguments */

	 case row_fct:
	    while( row-- ) {
	       this->value.data.lngptr[row] = gParse.firstRow + row;
	       this->value.undef[row] = 0;
	    }
	    break;
	 case null_fct:
            if( this->type==LONG ) {
               while( row-- ) {
                  this->value.data.lngptr[row] = 0;
                  this->value.undef[row] = 1;
               }
            } else if( this->type==STRING ) {
               while( row-- ) {
                  this->value.data.strptr[row][0] = '\0';
                  this->value.undef[row] = 1;
               }
            }
	    break;
	 case rnd_fct:
	   while( elem-- ) {
	     this->value.data.dblptr[elem] = simplerng_getuniform();
	     this->value.undef[elem] = 0;
	    }
	    break;

	 case gasrnd_fct:
	    while( elem-- ) {
	       this->value.data.dblptr[elem] = simplerng_getnorm();
	       this->value.undef[elem] = 0;
	    }
	    break;

	 case poirnd_fct:
	   if( theParams[0]->type==DOUBLE ) {
	      if (theParams[0]->operation == CONST_OP) {
		while( elem-- ) {
		  this->value.undef[elem] = (pVals[0].data.dbl < 0);
		  if (! this->value.undef[elem]) {
		    this->value.data.lngptr[elem] = simplerng_getpoisson(pVals[0].data.dbl);
		  }
		} 
	      } else {
		while( elem-- ) {
		  this->value.undef[elem] = theParams[0]->value.undef[elem];
		  if (theParams[0]->value.data.dblptr[elem] < 0) 
		    this->value.undef[elem] = 1;
		  if (! this->value.undef[elem]) {
		    this->value.data.lngptr[elem] = 
		      simplerng_getpoisson(theParams[0]->value.data.dblptr[elem]);
		  }
		} /* while */
	      } /* ! CONST_OP */
	   } else {
	     /* LONG */
	      if (theParams[0]->operation == CONST_OP) {
		while( elem-- ) {
		  this->value.undef[elem] = (pVals[0].data.lng < 0);
		  if (! this->value.undef[elem]) {
		    this->value.data.lngptr[elem] = simplerng_getpoisson(pVals[0].data.lng);
		  }
		} 
	      } else {
		while( elem-- ) {
		  this->value.undef[elem] = theParams[0]->value.undef[elem];
		  if (theParams[0]->value.data.lngptr[elem] < 0) 
		    this->value.undef[elem] = 1;
		  if (! this->value.undef[elem]) {
		    this->value.data.lngptr[elem] = 
		      simplerng_getpoisson(theParams[0]->value.data.lngptr[elem]);
		  }
		} /* while */
	      } /* ! CONST_OP */
	   } /* END LONG */
	   break;


	    /* Non-Trig single-argument functions */
	    
	 case sum_fct:
	    elem = row * theParams[0]->value.nelem;
	    if( theParams[0]->type==BOOLEAN ) {
	       while( row-- ) {
		  this->value.data.lngptr[row] = 0;
		  /* Default is UNDEF until a defined value is found */
		  this->value.undef[row] = 1;
		  nelem = theParams[0]->value.nelem;
		  while( nelem-- ) {
		     elem--;
		     if ( ! theParams[0]->value.undef[elem] ) {
		       this->value.data.lngptr[row] +=
			 ( theParams[0]->value.data.logptr[elem] ? 1 : 0 );
		       this->value.undef[row] = 0;
		     }
		  }
	       }
	    } else if( theParams[0]->type==LONG ) {
	       while( row-- ) {
		  this->value.data.lngptr[row] = 0;
		  /* Default is UNDEF until a defined value is found */
		  this->value.undef[row] = 1;
		  nelem = theParams[0]->value.nelem;
		  while( nelem-- ) {
		     elem--;
		     if ( ! theParams[0]->value.undef[elem] ) {
		       this->value.data.lngptr[row] +=
			 theParams[0]->value.data.lngptr[elem];
		       this->value.undef[row] = 0;
		     }
		  }
	       }		  
	    } else if( theParams[0]->type==DOUBLE ){
	       while( row-- ) {
		  this->value.data.dblptr[row] = 0.0;
		  /* Default is UNDEF until a defined value is found */
		  this->value.undef[row] = 1;
		  nelem = theParams[0]->value.nelem;
		  while( nelem-- ) {
		     elem--;
		     if ( ! theParams[0]->value.undef[elem] ) {
		       this->value.data.dblptr[row] +=
			 theParams[0]->value.data.dblptr[elem];
		       this->value.undef[row] = 0;
		     }
		  }
	       }		  
	    } else { /* BITSTR */
	       nelem = theParams[0]->value.nelem;
	       while( row-- ) {
		  char *sptr1 = theParams[0]->value.data.strptr[row];
		  this->value.data.lngptr[row] = 0;
		  this->value.undef[row] = 0;
		  while (*sptr1) {
		    if (*sptr1 == '1') this->value.data.lngptr[row] ++;
		    sptr1++;
		  }
	       }		  
	    }
	    break;

	 case average_fct:
	    elem = row * theParams[0]->value.nelem;
	    if( theParams[0]->type==LONG ) {
	       while( row-- ) {
		  int count = 0;
		  this->value.data.dblptr[row] = 0;
		  nelem = theParams[0]->value.nelem;
		  while( nelem-- ) {
		     elem--;
		     if (theParams[0]->value.undef[elem] == 0) {
		       this->value.data.dblptr[row] +=
			 theParams[0]->value.data.lngptr[elem];
		       count ++;
		     }
		  }
		  if (count == 0) {
		    this->value.undef[row] = 1;
		  } else {
		    this->value.undef[row] = 0;
		    this->value.data.dblptr[row] /= count;
		  }
	       }		  
	    } else if( theParams[0]->type==DOUBLE ){
	       while( row-- ) {
		  int count = 0;
		  this->value.data.dblptr[row] = 0;
		  nelem = theParams[0]->value.nelem;
		  while( nelem-- ) {
		     elem--;
		     if (theParams[0]->value.undef[elem] == 0) {
		       this->value.data.dblptr[row] +=
			 theParams[0]->value.data.dblptr[elem];
		       count ++;
		     }
		  }
		  if (count == 0) {
		    this->value.undef[row] = 1;
		  } else {
		    this->value.undef[row] = 0;
		    this->value.data.dblptr[row] /= count;
		  }
	       }		  
	    }
	    break;
	 case stddev_fct:
	    elem = row * theParams[0]->value.nelem;
	    if( theParams[0]->type==LONG ) {

	       /* Compute the mean value */
	       while( row-- ) {
		  int count = 0;
		  double sum = 0, sum2 = 0;

		  nelem = theParams[0]->value.nelem;
		  while( nelem-- ) {
		     elem--;
		     if (theParams[0]->value.undef[elem] == 0) {
		       sum += theParams[0]->value.data.lngptr[elem];
		       count ++;
		     }
		  }
		  if (count > 1) {
		    sum /= count;

		    /* Compute the sum of squared deviations */
		    nelem = theParams[0]->value.nelem;
		    elem += nelem;  /* Reset elem for second pass */
		    while( nelem-- ) {
		      elem--;
		      if (theParams[0]->value.undef[elem] == 0) {
			double dx = (theParams[0]->value.data.lngptr[elem] - sum);
			sum2 += (dx*dx);
		      }
		    }

		    sum2 /= (double)count-1;

		    this->value.undef[row] = 0;
		    this->value.data.dblptr[row] = sqrt(sum2);
		  } else {
		    this->value.undef[row] = 0;       /* STDDEV => 0 */
		    this->value.data.dblptr[row] = 0;
		  }
	       }
	    } else if( theParams[0]->type==DOUBLE ){

	       /* Compute the mean value */
	       while( row-- ) {
		  int count = 0;
		  double sum = 0, sum2 = 0;

		  nelem = theParams[0]->value.nelem;
		  while( nelem-- ) {
		     elem--;
		     if (theParams[0]->value.undef[elem] == 0) {
		       sum += theParams[0]->value.data.dblptr[elem];
		       count ++;
		     }
		  }
		  if (count > 1) {
		    sum /= count;

		    /* Compute the sum of squared deviations */
		    nelem = theParams[0]->value.nelem;
		    elem += nelem;  /* Reset elem for second pass */
		    while( nelem-- ) {
		      elem--;
		      if (theParams[0]->value.undef[elem] == 0) {
			double dx = (theParams[0]->value.data.dblptr[elem] - sum);
			sum2 += (dx*dx);
		      }
		    }

		    sum2 /= (double)count-1;

		    this->value.undef[row] = 0;
		    this->value.data.dblptr[row] = sqrt(sum2);
		  } else {
		    this->value.undef[row] = 0;       /* STDDEV => 0 */
		    this->value.data.dblptr[row] = 0;
		  }
	       }
	    }
	    break;

	 case median_fct:
	   elem = row * theParams[0]->value.nelem;
	   nelem = theParams[0]->value.nelem;
	   if( theParams[0]->type==LONG ) {
	       long *dptr = theParams[0]->value.data.lngptr;
	       char *uptr = theParams[0]->value.undef;
	       long *mptr = (long *) malloc(sizeof(long)*nelem);
	       int irow;

	       /* Allocate temporary storage for this row, since the
                  quickselect function will scramble the contents */
	       if (mptr == 0) {
		 fferror("Could not allocate temporary memory in median function");
		 free( this->value.data.ptr );
		 break;
	       }

	       for (irow=0; irow<row; irow++) {
		  long *p = mptr;
		  int nelem1 = nelem;


		  while ( nelem1-- ) { 
		    if (*uptr == 0) {
		      *p++ = *dptr;   /* Only advance the dest pointer if we copied */
		    }
		    dptr ++;  /* Advance the source pointer ... */
		    uptr ++;  /* ... and source "undef" pointer */
		  }
		  
		  nelem1 = (p - mptr);  /* Number of accepted data points */
		  if (nelem1 > 0) {
		    this->value.undef[irow] = 0;
		    this->value.data.lngptr[irow] = qselect_median_lng(mptr, nelem1);
		  } else {
		    this->value.undef[irow] = 1;
		    this->value.data.lngptr[irow] = 0;
		  }
		    
	       }		  

	       free(mptr);
	    } else {
	       double *dptr = theParams[0]->value.data.dblptr;
	       char   *uptr = theParams[0]->value.undef;
	       double *mptr = (double *) malloc(sizeof(double)*nelem);
	       int irow;

	       /* Allocate temporary storage for this row, since the
                  quickselect function will scramble the contents */
	       if (mptr == 0) {
		 fferror("Could not allocate temporary memory in median function");
		 free( this->value.data.ptr );
		 break;
	       }

	       for (irow=0; irow<row; irow++) {
		  double *p = mptr;
		  int nelem1 = nelem;

		  while ( nelem1-- ) { 
		    if (*uptr == 0) {
		      *p++ = *dptr;   /* Only advance the dest pointer if we copied */
		    }
		    dptr ++;  /* Advance the source pointer ... */
		    uptr ++;  /* ... and source "undef" pointer */
		  }

		  nelem1 = (p - mptr);  /* Number of accepted data points */
		  if (nelem1 > 0) {
		    this->value.undef[irow] = 0;
		    this->value.data.dblptr[irow] = qselect_median_dbl(mptr, nelem1);
		  } else {
		    this->value.undef[irow] = 1;
		    this->value.data.dblptr[irow] = 0;
		  }

	       }
	       free(mptr);
	    }
	    break;
	 case abs_fct:
	    if( theParams[0]->type==DOUBLE )
	       while( elem-- ) {
		  dval = theParams[0]->value.data.dblptr[elem];
		  this->value.data.dblptr[elem] = (dval>0.0 ? dval : -dval);
		  this->value.undef[elem] = theParams[0]->value.undef[elem];
	       }
	    else
	       while( elem-- ) {
		  ival = theParams[0]->value.data.lngptr[elem];
		  this->value.data.lngptr[elem] = (ival> 0  ? ival : -ival);
		  this->value.undef[elem] = theParams[0]->value.undef[elem];
	       }
	    break;

            /* Special Null-Handling Functions */

	 case nonnull_fct:
	   nelem = theParams[0]->value.nelem;
	   if ( theParams[0]->type==STRING ) nelem = 1;
	   elem = row * nelem;
	   while( row-- ) {
	     int nelem1 = nelem;

	     this->value.undef[row] = 0;        /* Initialize to 0 (defined) */
	     this->value.data.lngptr[row] = 0;
	     while( nelem1-- ) {	
	       elem --;
	       if ( theParams[0]->value.undef[elem] == 0 ) this->value.data.lngptr[row] ++;
	     }
	   }
	   break;
	 case isnull_fct:
	    if( theParams[0]->type==STRING ) elem = row;
	    while( elem-- ) {
	       this->value.data.logptr[elem] = theParams[0]->value.undef[elem];
	       this->value.undef[elem] = 0;
	    }
	    break;
         case defnull_fct:
	    switch( this->type ) {
	    case BOOLEAN:
	       while( row-- ) {
		  nelem = this->value.nelem;
		  while( nelem-- ) {
		     elem--;
		     i=2; while( i-- )
			if( vector[i]>1 ) {
			   pNull[i] = theParams[i]->value.undef[elem];
			   pVals[i].data.log =
			      theParams[i]->value.data.logptr[elem];
			} else if( vector[i] ) {
			   pNull[i] = theParams[i]->value.undef[row];
			   pVals[i].data.log =
			      theParams[i]->value.data.logptr[row];
			}
		     if( pNull[0] ) {
			this->value.undef[elem] = pNull[1];
			this->value.data.logptr[elem] = pVals[1].data.log;
		     } else {
			this->value.undef[elem] = 0;
			this->value.data.logptr[elem] = pVals[0].data.log;
		     }
		  }
	       }
	       break;
	    case LONG:
	       while( row-- ) {
		  nelem = this->value.nelem;
		  while( nelem-- ) {
		     elem--;
		     i=2; while( i-- )
			if( vector[i]>1 ) {
			   pNull[i] = theParams[i]->value.undef[elem];
			   pVals[i].data.lng =
			      theParams[i]->value.data.lngptr[elem];
			} else if( vector[i] ) {
			   pNull[i] = theParams[i]->value.undef[row];
			   pVals[i].data.lng =
			      theParams[i]->value.data.lngptr[row];
			}
		     if( pNull[0] ) {
			this->value.undef[elem] = pNull[1];
			this->value.data.lngptr[elem] = pVals[1].data.lng;
		     } else {
			this->value.undef[elem] = 0;
			this->value.data.lngptr[elem] = pVals[0].data.lng;
		     }
		  }
	       }
	       break;
	    case DOUBLE:
	       while( row-- ) {
		  nelem = this->value.nelem;
		  while( nelem-- ) {
		     elem--;
		     i=2; while( i-- )
			if( vector[i]>1 ) {
			   pNull[i] = theParams[i]->value.undef[elem];
			   pVals[i].data.dbl =
			      theParams[i]->value.data.dblptr[elem];
			} else if( vector[i] ) {
			   pNull[i] = theParams[i]->value.undef[row];
			   pVals[i].data.dbl =
			      theParams[i]->value.data.dblptr[row];
			}
		     if( pNull[0] ) {
			this->value.undef[elem] = pNull[1];
			this->value.data.dblptr[elem] = pVals[1].data.dbl;
		     } else {
			this->value.undef[elem] = 0;
			this->value.data.dblptr[elem] = pVals[0].data.dbl;
		     }
		  }
	       }
	       break;
	    case STRING:
	       while( row-- ) {
		  i=2; while( i-- )
		     if( vector[i] ) {
			pNull[i] = theParams[i]->value.undef[row];
			strcpy(pVals[i].data.str,
			       theParams[i]->value.data.strptr[row]);
		     }
		  if( pNull[0] ) {
		     this->value.undef[row] = pNull[1];
		     strcpy(this->value.data.strptr[row],pVals[1].data.str);
		  } else {
		     this->value.undef[elem] = 0;
		     strcpy(this->value.data.strptr[row],pVals[0].data.str);
		  }
	       }
	    }
	    break;
         case setnull_fct:
	    switch( this->type ) {
	    case LONG:
	      while( elem-- ) {
		if ( theParams[1]->value.data.lng == 
		     theParams[0]->value.data.lngptr[elem] ) {
		  this->value.data.lngptr[elem] = 0;
		  this->value.undef[elem] = 1;
		} else {
		  this->value.data.lngptr[elem] = theParams[0]->value.data.lngptr[elem];
		  this->value.undef[elem] = theParams[0]->value.undef[elem];
		}
	      }
	      break;
	    case DOUBLE:
	      while( elem-- ) {
		if ( theParams[1]->value.data.dbl == 
		     theParams[0]->value.data.dblptr[elem] ) {
		  this->value.data.dblptr[elem] = 0;
		  this->value.undef[elem] = 1;
		} else {
		  this->value.data.dblptr[elem] = theParams[0]->value.data.dblptr[elem];
		  this->value.undef[elem] = theParams[0]->value.undef[elem];
		}
	      }
	      break;
	    }
	    break;

	    /* Math functions with 1 double argument */

	 case sin_fct:
	    while( elem-- )
	       if( !(this->value.undef[elem] = theParams[0]->value.undef[elem]) ) {
		  this->value.data.dblptr[elem] = 
		     sin( theParams[0]->value.data.dblptr[elem] );
	       }
	    break;
	 case cos_fct:
	    while( elem-- )
	       if( !(this->value.undef[elem] = theParams[0]->value.undef[elem]) ) {
		  this->value.data.dblptr[elem] = 
		     cos( theParams[0]->value.data.dblptr[elem] );
	       }
	    break;
	 case tan_fct:
	    while( elem-- )
	       if( !(this->value.undef[elem] = theParams[0]->value.undef[elem]) ) {
		  this->value.data.dblptr[elem] = 
		     tan( theParams[0]->value.data.dblptr[elem] );
	       }
	    break;
	 case asin_fct:
	    while( elem-- )
	       if( !(this->value.undef[elem] = theParams[0]->value.undef[elem]) ) {
		  dval = theParams[0]->value.data.dblptr[elem];
		  if( dval<-1.0 || dval>1.0 ) {
		     this->value.data.dblptr[elem] = 0.0;
		     this->value.undef[elem] = 1;
		  } else
		     this->value.data.dblptr[elem] = asin( dval );
	       }
	    break;
	 case acos_fct:
	    while( elem-- )
	       if( !(this->value.undef[elem] = theParams[0]->value.undef[elem]) ) {
		  dval = theParams[0]->value.data.dblptr[elem];
		  if( dval<-1.0 || dval>1.0 ) {
		     this->value.data.dblptr[elem] = 0.0;
		     this->value.undef[elem] = 1;
		  } else
		     this->value.data.dblptr[elem] = acos( dval );
	       }
	    break;
	 case atan_fct:
	    while( elem-- )
	       if( !(this->value.undef[elem] = theParams[0]->value.undef[elem]) ) {
		  dval = theParams[0]->value.data.dblptr[elem];
		  this->value.data.dblptr[elem] = atan( dval );
	       }
	    break;
	 case sinh_fct:
	    while( elem-- )
	       if( !(this->value.undef[elem] = theParams[0]->value.undef[elem]) ) {
		  this->value.data.dblptr[elem] = 
		     sinh( theParams[0]->value.data.dblptr[elem] );
	       }
	    break;
	 case cosh_fct:
	    while( elem-- )
	       if( !(this->value.undef[elem] = theParams[0]->value.undef[elem]) ) {
		  this->value.data.dblptr[elem] = 
		     cosh( theParams[0]->value.data.dblptr[elem] );
	       }
	    break;
	 case tanh_fct:
	    while( elem-- )
	       if( !(this->value.undef[elem] = theParams[0]->value.undef[elem]) ) {
		  this->value.data.dblptr[elem] = 
		     tanh( theParams[0]->value.data.dblptr[elem] );
	       }
	    break;
	 case exp_fct:
	    while( elem-- )
	       if( !(this->value.undef[elem] = theParams[0]->value.undef[elem]) ) {
		  dval = theParams[0]->value.data.dblptr[elem];
		  this->value.data.dblptr[elem] = exp( dval );
	       }
	    break;
	 case log_fct:
	    while( elem-- )
	       if( !(this->value.undef[elem] = theParams[0]->value.undef[elem]) ) {
		  dval = theParams[0]->value.data.dblptr[elem];
		  if( dval<=0.0 ) {
		     this->value.data.dblptr[elem] = 0.0;
		     this->value.undef[elem] = 1;
		  } else
		     this->value.data.dblptr[elem] = log( dval );
	       }
	    break;
	 case log10_fct:
	    while( elem-- )
	       if( !(this->value.undef[elem] = theParams[0]->value.undef[elem]) ) {
		  dval = theParams[0]->value.data.dblptr[elem];
		  if( dval<=0.0 ) {
		     this->value.data.dblptr[elem] = 0.0;
		     this->value.undef[elem] = 1;
		  } else
		     this->value.data.dblptr[elem] = log10( dval );
	       }
	    break;
	 case sqrt_fct:
	    while( elem-- )
	       if( !(this->value.undef[elem] = theParams[0]->value.undef[elem]) ) {
		  dval = theParams[0]->value.data.dblptr[elem];
		  if( dval<0.0 ) {
		     this->value.data.dblptr[elem] = 0.0;
		     this->value.undef[elem] = 1;
		  } else
		     this->value.data.dblptr[elem] = sqrt( dval );
	       }
	    break;
	 case ceil_fct:
	    while( elem-- )
	       if( !(this->value.undef[elem] = theParams[0]->value.undef[elem]) ) {
		  this->value.data.dblptr[elem] = 
		     ceil( theParams[0]->value.data.dblptr[elem] );
	       }
	    break;
	 case floor_fct:
	    while( elem-- )
	       if( !(this->value.undef[elem] = theParams[0]->value.undef[elem]) ) {
		  this->value.data.dblptr[elem] = 
		     floor( theParams[0]->value.data.dblptr[elem] );
	       }
	    break;
	 case round_fct:
	    while( elem-- )
	       if( !(this->value.undef[elem] = theParams[0]->value.undef[elem]) ) {
		  this->value.data.dblptr[elem] = 
		     floor( theParams[0]->value.data.dblptr[elem] + 0.5);
	       }
	    break;

	    /* Two-argument Trig Functions */
	    
	 case atan2_fct:
	    while( row-- ) {
	       nelem = this->value.nelem;
	       while( nelem-- ) {
		  elem--;
		  i=2; while( i-- )
		     if( vector[i]>1 ) {
			pVals[i].data.dbl =
			   theParams[i]->value.data.dblptr[elem];
			pNull[i] = theParams[i]->value.undef[elem];
		     } else if( vector[i] ) {
			pVals[i].data.dbl =
			   theParams[i]->value.data.dblptr[row];
			pNull[i] = theParams[i]->value.undef[row];
		     }
		  if( !(this->value.undef[elem] = (pNull[0] || pNull[1]) ) )
		     this->value.data.dblptr[elem] =
			atan2( pVals[0].data.dbl, pVals[1].data.dbl );
	       }
	    }
	    break;

	    /* Four-argument ANGSEP Function */
	    
	 case angsep_fct:
	    while( row-- ) {
	       nelem = this->value.nelem;
	       while( nelem-- ) {
		  elem--;
		  i=4; while( i-- )
		     if( vector[i]>1 ) {
			pVals[i].data.dbl =
			   theParams[i]->value.data.dblptr[elem];
			pNull[i] = theParams[i]->value.undef[elem];
		     } else if( vector[i] ) {
			pVals[i].data.dbl =
			   theParams[i]->value.data.dblptr[row];
			pNull[i] = theParams[i]->value.undef[row];
		     }
		  if( !(this->value.undef[elem] = (pNull[0] || pNull[1] ||
						   pNull[2] || pNull[3]) ) )
		     this->value.data.dblptr[elem] =
		       angsep_calc(pVals[0].data.dbl, pVals[1].data.dbl,
				   pVals[2].data.dbl, pVals[3].data.dbl);
	       }
	    }
	    break;



	    /*  Min/Max functions taking 1 or 2 arguments  */

         case min1_fct:
	    elem = row * theParams[0]->value.nelem;
	    if( this->type==LONG ) {
	       long minVal=0;
	       while( row-- ) {
		  valInit = 1;
		  this->value.undef[row] = 1;
		  nelem = theParams[0]->value.nelem;
		  while( nelem-- ) {
		     elem--;
		     if ( !theParams[0]->value.undef[elem] ) {
		       if ( valInit ) {
			 valInit = 0;
			 minVal  = theParams[0]->value.data.lngptr[elem];
		       } else {
			 minVal  = minvalue( minVal,
					     theParams[0]->value.data.lngptr[elem] );
		       }
		       this->value.undef[row] = 0;
		     }
		  }  
		  this->value.data.lngptr[row] = minVal;
	       }		  
	    } else if( this->type==DOUBLE ) {
	       double minVal=0.0;
	       while( row-- ) {
		  valInit = 1;
		  this->value.undef[row] = 1;
		  nelem = theParams[0]->value.nelem;
		  while( nelem-- ) {
		     elem--;
		     if ( !theParams[0]->value.undef[elem] ) {
		       if ( valInit ) {
			 valInit = 0;
			 minVal  = theParams[0]->value.data.dblptr[elem];
		       } else {
			 minVal  = minvalue( minVal,
					     theParams[0]->value.data.dblptr[elem] );
		       }
		       this->value.undef[row] = 0;
		     }
		  }  
		  this->value.data.dblptr[row] = minVal;
	       }		  
	    } else if( this->type==BITSTR ) {
	       char minVal;
	       while( row-- ) {
		  char *sptr1 = theParams[0]->value.data.strptr[row];
		  minVal = '1';
		  while (*sptr1) {
		    if (*sptr1 == '0') minVal = '0';
		    sptr1++;
		  }
		  this->value.data.strptr[row][0] = minVal;
		  this->value.data.strptr[row][1] = 0;     /* Null terminate */
	       }		  
	    }
	    break;
         case min2_fct:
	    if( this->type==LONG ) {
	       while( row-- ) {
		  nelem = this->value.nelem;
		  while( nelem-- ) {
		     elem--;
		     i=2; while( i-- )
			if( vector[i]>1 ) {
			   pVals[i].data.lng =
			      theParams[i]->value.data.lngptr[elem];
			   pNull[i] = theParams[i]->value.undef[elem];
			} else if( vector[i] ) {
			   pVals[i].data.lng =
			      theParams[i]->value.data.lngptr[row];
			   pNull[i] = theParams[i]->value.undef[row];
			}
		     if( pNull[0] && pNull[1] ) {
		       this->value.undef[elem] = 1;
		       this->value.data.lngptr[elem] = 0;
		     } else if (pNull[0]) {
		       this->value.undef[elem] = 0;
		       this->value.data.lngptr[elem] = pVals[1].data.lng;
		     } else if (pNull[1]) {
		       this->value.undef[elem] = 0;
		       this->value.data.lngptr[elem] = pVals[0].data.lng;
		     } else {
		       this->value.undef[elem] = 0;
		       this->value.data.lngptr[elem] =
			 minvalue( pVals[0].data.lng, pVals[1].data.lng );
		     }
		  }
	       }
	    } else if( this->type==DOUBLE ) {
	       while( row-- ) {
		  nelem = this->value.nelem;
		  while( nelem-- ) {
		     elem--;
		     i=2; while( i-- )
			if( vector[i]>1 ) {
			   pVals[i].data.dbl =
			      theParams[i]->value.data.dblptr[elem];
			   pNull[i] = theParams[i]->value.undef[elem];
			} else if( vector[i] ) {
			   pVals[i].data.dbl =
			      theParams[i]->value.data.dblptr[row];
			   pNull[i] = theParams[i]->value.undef[row];
			}
		     if( pNull[0] && pNull[1] ) {
		       this->value.undef[elem] = 1;
		       this->value.data.dblptr[elem] = 0;
		     } else if (pNull[0]) {
		       this->value.undef[elem] = 0;
		       this->value.data.dblptr[elem] = pVals[1].data.dbl;
		     } else if (pNull[1]) {
		       this->value.undef[elem] = 0;
		       this->value.data.dblptr[elem] = pVals[0].data.dbl;
		     } else {
		       this->value.undef[elem] = 0;
		       this->value.data.dblptr[elem] =
			 minvalue( pVals[0].data.dbl, pVals[1].data.dbl );
		     }
		  }
 	       }
	    }
	    break;

         case max1_fct:
	    elem = row * theParams[0]->value.nelem;
	    if( this->type==LONG ) {
	       long maxVal=0;
	       while( row-- ) {
		  valInit = 1;
		  this->value.undef[row] = 1;
		  nelem = theParams[0]->value.nelem;
		  while( nelem-- ) {
		     elem--;
		     if ( !theParams[0]->value.undef[elem] ) {
		       if ( valInit ) {
			 valInit = 0;
			 maxVal  = theParams[0]->value.data.lngptr[elem];
		       } else {
			 maxVal  = maxvalue( maxVal,
					     theParams[0]->value.data.lngptr[elem] );
		       }
		       this->value.undef[row] = 0;
		     }
		  }
		  this->value.data.lngptr[row] = maxVal;
	       }		  
	    } else if( this->type==DOUBLE ) {
	       double maxVal=0.0;
	       while( row-- ) {
		  valInit = 1;
		  this->value.undef[row] = 1;
		  nelem = theParams[0]->value.nelem;
		  while( nelem-- ) {
		     elem--;
		     if ( !theParams[0]->value.undef[elem] ) {
		       if ( valInit ) {
			 valInit = 0;
			 maxVal  = theParams[0]->value.data.dblptr[elem];
		       } else {
			 maxVal  = maxvalue( maxVal,
					     theParams[0]->value.data.dblptr[elem] );
		       }
		       this->value.undef[row] = 0;
		     }
		  }
		  this->value.data.dblptr[row] = maxVal;
	       }		  
	    } else if( this->type==BITSTR ) {
	       char maxVal;
	       while( row-- ) {
		  char *sptr1 = theParams[0]->value.data.strptr[row];
		  maxVal = '0';
		  while (*sptr1) {
		    if (*sptr1 == '1') maxVal = '1';
		    sptr1++;
		  }
		  this->value.data.strptr[row][0] = maxVal;
		  this->value.data.strptr[row][1] = 0;     /* Null terminate */
	       }		  
	    }
	    break;
         case max2_fct:
	    if( this->type==LONG ) {
	       while( row-- ) {
		  nelem = this->value.nelem;
		  while( nelem-- ) {
		     elem--;
		     i=2; while( i-- )
			if( vector[i]>1 ) {
			   pVals[i].data.lng =
			      theParams[i]->value.data.lngptr[elem];
			   pNull[i] = theParams[i]->value.undef[elem];
			} else if( vector[i] ) {
			   pVals[i].data.lng =
			      theParams[i]->value.data.lngptr[row];
			   pNull[i] = theParams[i]->value.undef[row];
			}
		     if( pNull[0] && pNull[1] ) {
		       this->value.undef[elem] = 1;
		       this->value.data.lngptr[elem] = 0;
		     } else if (pNull[0]) {
		       this->value.undef[elem] = 0;
		       this->value.data.lngptr[elem] = pVals[1].data.lng;
		     } else if (pNull[1]) {
		       this->value.undef[elem] = 0;
		       this->value.data.lngptr[elem] = pVals[0].data.lng;
		     } else {
		       this->value.undef[elem] = 0;
		       this->value.data.lngptr[elem] =
			 maxvalue( pVals[0].data.lng, pVals[1].data.lng );
		     }
		  }
	       }
	    } else if( this->type==DOUBLE ) {
	       while( row-- ) {
		  nelem = this->value.nelem;
		  while( nelem-- ) {
		     elem--;
		     i=2; while( i-- )
			if( vector[i]>1 ) {
			   pVals[i].data.dbl =
			      theParams[i]->value.data.dblptr[elem];
			   pNull[i] = theParams[i]->value.undef[elem];
			} else if( vector[i] ) {
			   pVals[i].data.dbl =
			      theParams[i]->value.data.dblptr[row];
			   pNull[i] = theParams[i]->value.undef[row];
			}
		     if( pNull[0] && pNull[1] ) {
		       this->value.undef[elem] = 1;
		       this->value.data.dblptr[elem] = 0;
		     } else if (pNull[0]) {
		       this->value.undef[elem] = 0;
		       this->value.data.dblptr[elem] = pVals[1].data.dbl;
		     } else if (pNull[1]) {
		       this->value.undef[elem] = 0;
		       this->value.data.dblptr[elem] = pVals[0].data.dbl;
		     } else {
		       this->value.undef[elem] = 0;
		       this->value.data.dblptr[elem] =
			 maxvalue( pVals[0].data.dbl, pVals[1].data.dbl );
		     }
		  }
	       }
	    }
	    break;

	    /* Boolean SAO region Functions... scalar or vector dbls */

	 case near_fct:
	    while( row-- ) {
	       nelem = this->value.nelem;
	       while( nelem-- ) {
		  elem--;
		  i=3; while( i-- )
		     if( vector[i]>1 ) {
			pVals[i].data.dbl =
			   theParams[i]->value.data.dblptr[elem];
			pNull[i] = theParams[i]->value.undef[elem];
		     } else if( vector[i] ) {
			pVals[i].data.dbl =
			   theParams[i]->value.data.dblptr[row];
			pNull[i] = theParams[i]->value.undef[row];
		     }
		  if( !(this->value.undef[elem] = (pNull[0] || pNull[1] ||
						   pNull[2]) ) )
		    this->value.data.logptr[elem] =
		      bnear( pVals[0].data.dbl, pVals[1].data.dbl,
			     pVals[2].data.dbl );
	       }
	    }
	    break;

	 case circle_fct:
	    while( row-- ) {
	       nelem = this->value.nelem;
	       while( nelem-- ) {
		  elem--;
		  i=5; while( i-- )
		     if( vector[i]>1 ) {
			pVals[i].data.dbl =
			   theParams[i]->value.data.dblptr[elem];
			pNull[i] = theParams[i]->value.undef[elem];
		     } else if( vector[i] ) {
			pVals[i].data.dbl =
			   theParams[i]->value.data.dblptr[row];
			pNull[i] = theParams[i]->value.undef[row];
		     }
		  if( !(this->value.undef[elem] = (pNull[0] || pNull[1] ||
						   pNull[2] || pNull[3] ||
						   pNull[4]) ) )
		    this->value.data.logptr[elem] =
		     circle( pVals[0].data.dbl, pVals[1].data.dbl,
			     pVals[2].data.dbl, pVals[3].data.dbl,
			     pVals[4].data.dbl );
	       }
	    }
	    break;

	 case box_fct:
	    while( row-- ) {
	       nelem = this->value.nelem;
	       while( nelem-- ) {
		  elem--;
		  i=7; while( i-- )
		     if( vector[i]>1 ) {
			pVals[i].data.dbl =
			   theParams[i]->value.data.dblptr[elem];
			pNull[i] = theParams[i]->value.undef[elem];
		     } else if( vector[i] ) {
			pVals[i].data.dbl =
			   theParams[i]->value.data.dblptr[row];
			pNull[i] = theParams[i]->value.undef[row];
		     }
		  if( !(this->value.undef[elem] = (pNull[0] || pNull[1] ||
						   pNull[2] || pNull[3] ||
						   pNull[4] || pNull[5] ||
						   pNull[6] ) ) )
		    this->value.data.logptr[elem] =
		     saobox( pVals[0].data.dbl, pVals[1].data.dbl,
			     pVals[2].data.dbl, pVals[3].data.dbl,
			     pVals[4].data.dbl, pVals[5].data.dbl,
			     pVals[6].data.dbl );	
	       }
	    }
	    break;

	 case elps_fct:
	    while( row-- ) {
	       nelem = this->value.nelem;
	       while( nelem-- ) {
		  elem--;
		  i=7; while( i-- )
		     if( vector[i]>1 ) {
			pVals[i].data.dbl =
			   theParams[i]->value.data.dblptr[elem];
			pNull[i] = theParams[i]->value.undef[elem];
		     } else if( vector[i] ) {
			pVals[i].data.dbl =
			   theParams[i]->value.data.dblptr[row];
			pNull[i] = theParams[i]->value.undef[row];
		     }
		  if( !(this->value.undef[elem] = (pNull[0] || pNull[1] ||
						   pNull[2] || pNull[3] ||
						   pNull[4] || pNull[5] ||
						   pNull[6] ) ) )
		    this->value.data.logptr[elem] =
		     ellipse( pVals[0].data.dbl, pVals[1].data.dbl,
			      pVals[2].data.dbl, pVals[3].data.dbl,
			      pVals[4].data.dbl, pVals[5].data.dbl,
			      pVals[6].data.dbl );
	       }
	    }
	    break;

            /* C Conditional expression:  bool ? expr : expr */

         case ifthenelse_fct:
            switch( this->type ) {
            case BOOLEAN:
	       while( row-- ) {
		  nelem = this->value.nelem;
		  while( nelem-- ) {
		     elem--;
                     if( vector[2]>1 ) {
                        pVals[2].data.log =
                           theParams[2]->value.data.logptr[elem];
                        pNull[2] = theParams[2]->value.undef[elem];
                     } else if( vector[2] ) {
                        pVals[2].data.log =
                           theParams[2]->value.data.logptr[row];
                        pNull[2] = theParams[2]->value.undef[row];
                     }
		     i=2; while( i-- )
			if( vector[i]>1 ) {
			   pVals[i].data.log =
			      theParams[i]->value.data.logptr[elem];
			   pNull[i] = theParams[i]->value.undef[elem];
			} else if( vector[i] ) {
			   pVals[i].data.log =
			      theParams[i]->value.data.logptr[row];
			   pNull[i] = theParams[i]->value.undef[row];
			}
		     if( !(this->value.undef[elem] = pNull[2]) ) {
                        if( pVals[2].data.log ) {
                           this->value.data.logptr[elem] = pVals[0].data.log;
                           this->value.undef[elem]       = pNull[0];
                        } else {
                           this->value.data.logptr[elem] = pVals[1].data.log;
                           this->value.undef[elem]       = pNull[1];
                        }
                     }
		  }
	       }
               break;
            case LONG:
	       while( row-- ) {
		  nelem = this->value.nelem;
		  while( nelem-- ) {
		     elem--;
                     if( vector[2]>1 ) {
                        pVals[2].data.log =
                           theParams[2]->value.data.logptr[elem];
                        pNull[2] = theParams[2]->value.undef[elem];
                     } else if( vector[2] ) {
                        pVals[2].data.log =
                           theParams[2]->value.data.logptr[row];
                        pNull[2] = theParams[2]->value.undef[row];
                     }
		     i=2; while( i-- )
			if( vector[i]>1 ) {
			   pVals[i].data.lng =
			      theParams[i]->value.data.lngptr[elem];
			   pNull[i] = theParams[i]->value.undef[elem];
			} else if( vector[i] ) {
			   pVals[i].data.lng =
			      theParams[i]->value.data.lngptr[row];
			   pNull[i] = theParams[i]->value.undef[row];
			}
		     if( !(this->value.undef[elem] = pNull[2]) ) {
                        if( pVals[2].data.log ) {
                           this->value.data.lngptr[elem] = pVals[0].data.lng;
                           this->value.undef[elem]       = pNull[0];
                        } else {
                           this->value.data.lngptr[elem] = pVals[1].data.lng;
                           this->value.undef[elem]       = pNull[1];
                        }
                     }
		  }
	       }
               break;
            case DOUBLE:
	       while( row-- ) {
		  nelem = this->value.nelem;
		  while( nelem-- ) {
		     elem--;
                     if( vector[2]>1 ) {
                        pVals[2].data.log =
                           theParams[2]->value.data.logptr[elem];
                        pNull[2] = theParams[2]->value.undef[elem];
                     } else if( vector[2] ) {
                        pVals[2].data.log =
                           theParams[2]->value.data.logptr[row];
                        pNull[2] = theParams[2]->value.undef[row];
                     }
		     i=2; while( i-- )
			if( vector[i]>1 ) {
			   pVals[i].data.dbl =
			      theParams[i]->value.data.dblptr[elem];
			   pNull[i] = theParams[i]->value.undef[elem];
			} else if( vector[i] ) {
			   pVals[i].data.dbl =
			      theParams[i]->value.data.dblptr[row];
			   pNull[i] = theParams[i]->value.undef[row];
			}
		     if( !(this->value.undef[elem] = pNull[2]) ) {
                        if( pVals[2].data.log ) {
                           this->value.data.dblptr[elem] = pVals[0].data.dbl;
                           this->value.undef[elem]       = pNull[0];
                        } else {
                           this->value.data.dblptr[elem] = pVals[1].data.dbl;
                           this->value.undef[elem]       = pNull[1];
                        }
                     }
		  }
	       }
               break;
            case STRING:
	       while( row-- ) {
                  if( vector[2] ) {
                     pVals[2].data.log = theParams[2]->value.data.logptr[row];
                     pNull[2] = theParams[2]->value.undef[row];
                  }
                  i=2; while( i-- )
                     if( vector[i] ) {
                        strcpy( pVals[i].data.str,
                                theParams[i]->value.data.strptr[row] );
                        pNull[i] = theParams[i]->value.undef[row];
                     }
                  if( !(this->value.undef[row] = pNull[2]) ) {
                     if( pVals[2].data.log ) {
                        strcpy( this->value.data.strptr[row],
                                pVals[0].data.str );
                        this->value.undef[row]       = pNull[0];
                     } else {
                        strcpy( this->value.data.strptr[row],
                                pVals[1].data.str );
                        this->value.undef[row]       = pNull[1];
                     }
                  } else {
                     this->value.data.strptr[row][0] = '\0';
                  }
	       }
               break;

            }
            break;

	    /* String functions */
            case strmid_fct:
	      {
		int strconst = theParams[0]->operation == CONST_OP;
		int posconst = theParams[1]->operation == CONST_OP;
		int lenconst = theParams[2]->operation == CONST_OP;
		int dest_len = this->value.nelem;
		int src_len  = theParams[0]->value.nelem;

		while (row--) {
		  int pos;
		  int len;
		  char *str;
		  int undef = 0;

		  if (posconst) {
		    pos = theParams[1]->value.data.lng;
		  } else {
		    pos = theParams[1]->value.data.lngptr[row];
		    if (theParams[1]->value.undef[row]) undef = 1;
		  }
		  if (strconst) {
		    str = theParams[0]->value.data.str;
		    if (src_len == 0) src_len = strlen(str);
		  } else {
		    str = theParams[0]->value.data.strptr[row];
		    if (theParams[0]->value.undef[row]) undef = 1;
		  }
		  if (lenconst) {
		    len = dest_len;
		  } else {
		    len = theParams[2]->value.data.lngptr[row];
		    if (theParams[2]->value.undef[row]) undef = 1;
		  }
		  this->value.data.strptr[row][0] = '\0';
		  if (pos == 0) undef = 1;
		  if (! undef ) {
		    if (cstrmid(this->value.data.strptr[row], len,
				str, src_len, pos) < 0) break;
		  }
		  this->value.undef[row] = undef;
		}
	      }		      
	      break;

	    /* String functions */
            case strpos_fct:
	      {
		int const1 = theParams[0]->operation == CONST_OP;
		int const2 = theParams[1]->operation == CONST_OP;

		while (row--) {
		  char *str1, *str2;
		  int undef = 0;

		  if (const1) {
		    str1 = theParams[0]->value.data.str;
		  } else {
		    str1 = theParams[0]->value.data.strptr[row];
		    if (theParams[0]->value.undef[row]) undef = 1;
		  }
		  if (const2) {
		    str2 = theParams[1]->value.data.str;
		  } else {
		    str2 = theParams[1]->value.data.strptr[row];
		    if (theParams[1]->value.undef[row]) undef = 1;
		  }
		  this->value.data.lngptr[row] = 0;
		  if (! undef ) {
		    char *res = strstr(str1, str2);
		    if (res == NULL) {
		      undef = 1;
		      this->value.data.lngptr[row] = 0; 
		    } else {
		      this->value.data.lngptr[row] = (res - str1) + 1;
		    }
		  }
		  this->value.undef[row] = undef;
		}
	      }
	      break;

		    
	 } /* End switch(this->operation) */
      } /* End if (!gParse.status) */
   } /* End non-constant operations */

   i = this->nSubNodes;
   while( i-- ) {
      if( theParams[i]->operation>0 ) {
	 /*  Currently only numeric params allowed  */
	 free( theParams[i]->value.data.ptr );
      }
   }
}

static void Do_Deref( Node *this )
{
   Node *theVar, *theDims[MAXDIMS];
   int  isConst[MAXDIMS], allConst;
   long dimVals[MAXDIMS];
   int  i, nDims;
   long row, elem, dsize;

   theVar = gParse.Nodes + this->SubNodes[0];

   i = nDims = this->nSubNodes-1;
   allConst = 1;
   while( i-- ) {
      theDims[i] = gParse.Nodes + this->SubNodes[i+1];
      isConst[i] = ( theDims[i]->operation==CONST_OP );
      if( isConst[i] )
	 dimVals[i] = theDims[i]->value.data.lng;
      else
	 allConst = 0;
   }

   if( this->type==DOUBLE ) {
      dsize = sizeof( double );
   } else if( this->type==LONG ) {
      dsize = sizeof( long );
   } else if( this->type==BOOLEAN ) {
      dsize = sizeof( char );
   } else
      dsize = 0;

   Allocate_Ptrs( this );

   if( !gParse.status ) {

      if( allConst && theVar->value.naxis==nDims ) {

	 /* Dereference completely using constant indices */

	 elem = 0;
	 i    = nDims;
	 while( i-- ) {
	    if( dimVals[i]<1 || dimVals[i]>theVar->value.naxes[i] ) break;
	    elem = theVar->value.naxes[i]*elem + dimVals[i]-1;
	 }
	 if( i<0 ) {
	    for( row=0; row<gParse.nRows; row++ ) {
	       if( this->type==STRING )
		 this->value.undef[row] = theVar->value.undef[row];
	       else if( this->type==BITSTR ) 
		 this->value.undef;  /* Dummy - BITSTRs do not have undefs */
	       else 
		 this->value.undef[row] = theVar->value.undef[elem];

	       if( this->type==DOUBLE )
		  this->value.data.dblptr[row] = 
		     theVar->value.data.dblptr[elem];
	       else if( this->type==LONG )
		  this->value.data.lngptr[row] = 
		     theVar->value.data.lngptr[elem];
	       else if( this->type==BOOLEAN )
		  this->value.data.logptr[row] = 
		     theVar->value.data.logptr[elem];
	       else {
		 /* XXX Note, the below expression uses knowledge of
                    the layout of the string format, namely (nelem+1)
                    characters per string, followed by (nelem+1)
                    "undef" values. */
		  this->value.data.strptr[row][0] = 
		     theVar->value.data.strptr[0][elem+row];
		  this->value.data.strptr[row][1] = 0;  /* Null terminate */
	       }
	       elem += theVar->value.nelem;
	    }
	 } else {
	    fferror("Index out of range");
	    free( this->value.data.ptr );
	 }
	 
      } else if( allConst && nDims==1 ) {
	 
	 /* Reduce dimensions by 1, using a constant index */
	 
	 if( dimVals[0] < 1 ||
	     dimVals[0] > theVar->value.naxes[ theVar->value.naxis-1 ] ) {
	    fferror("Index out of range");
	    free( this->value.data.ptr );
	 } else if ( this->type == BITSTR || this->type == STRING ) {
	    elem = this->value.nelem * (dimVals[0]-1);
	    for( row=0; row<gParse.nRows; row++ ) {
	      if (this->value.undef) 
		this->value.undef[row] = theVar->value.undef[row];
	      memcpy( (char*)this->value.data.strptr[0]
		      + row*sizeof(char)*(this->value.nelem+1),
		      (char*)theVar->value.data.strptr[0] + elem*sizeof(char),
		      this->value.nelem * sizeof(char) );
	      /* Null terminate */
	      this->value.data.strptr[row][this->value.nelem] = 0;
	      elem += theVar->value.nelem+1;
	    }	       
	 } else {
	    elem = this->value.nelem * (dimVals[0]-1);
	    for( row=0; row<gParse.nRows; row++ ) {
	       memcpy( this->value.undef + row*this->value.nelem,
		       theVar->value.undef + elem,
		       this->value.nelem * sizeof(char) );
	       memcpy( (char*)this->value.data.ptr
		       + row*dsize*this->value.nelem,
		       (char*)theVar->value.data.ptr + elem*dsize,
		       this->value.nelem * dsize );
	       elem += theVar->value.nelem;
	    }	       
	 }
      
      } else if( theVar->value.naxis==nDims ) {

	 /* Dereference completely using an expression for the indices */

	 for( row=0; row<gParse.nRows; row++ ) {

	    for( i=0; i<nDims; i++ ) {
	       if( !isConst[i] ) {
		  if( theDims[i]->value.undef[row] ) {
		     fferror("Null encountered as vector index");
		     free( this->value.data.ptr );
		     break;
		  } else
		     dimVals[i] = theDims[i]->value.data.lngptr[row];
	       }
	    }
	    if( gParse.status ) break;

	    elem = 0;
	    i    = nDims;
	    while( i-- ) {
	       if( dimVals[i]<1 || dimVals[i]>theVar->value.naxes[i] ) break;
	       elem = theVar->value.naxes[i]*elem + dimVals[i]-1;
	    }
	    if( i<0 ) {
	       elem += row*theVar->value.nelem;

	       if( this->type==STRING )
		 this->value.undef[row] = theVar->value.undef[row];
	       else if( this->type==BITSTR ) 
		 this->value.undef;  /* Dummy - BITSTRs do not have undefs */
	       else 
		 this->value.undef[row] = theVar->value.undef[elem];

	       if( this->type==DOUBLE )
		  this->value.data.dblptr[row] = 
		     theVar->value.data.dblptr[elem];
	       else if( this->type==LONG )
		  this->value.data.lngptr[row] = 
		     theVar->value.data.lngptr[elem];
	       else if( this->type==BOOLEAN )
		  this->value.data.logptr[row] = 
		     theVar->value.data.logptr[elem];
	       else {
		 /* XXX Note, the below expression uses knowledge of
                    the layout of the string format, namely (nelem+1)
                    characters per string, followed by (nelem+1)
                    "undef" values. */
		  this->value.data.strptr[row][0] = 
		     theVar->value.data.strptr[0][elem+row];
		  this->value.data.strptr[row][1] = 0;  /* Null terminate */
	       }
	    } else {
	       fferror("Index out of range");
	       free( this->value.data.ptr );
	    }
	 }

      } else {

	 /* Reduce dimensions by 1, using a nonconstant expression */

	 for( row=0; row<gParse.nRows; row++ ) {

	    /* Index cannot be a constant */

	    if( theDims[0]->value.undef[row] ) {
	       fferror("Null encountered as vector index");
	       free( this->value.data.ptr );
	       break;
	    } else
	       dimVals[0] = theDims[0]->value.data.lngptr[row];

	    if( dimVals[0] < 1 ||
		dimVals[0] > theVar->value.naxes[ theVar->value.naxis-1 ] ) {
	       fferror("Index out of range");
	       free( this->value.data.ptr );
	    } else if ( this->type == BITSTR || this->type == STRING ) {
	      elem = this->value.nelem * (dimVals[0]-1);
	      elem += row*(theVar->value.nelem+1);
	      if (this->value.undef) 
		this->value.undef[row] = theVar->value.undef[row];
	      memcpy( (char*)this->value.data.strptr[0]
		      + row*sizeof(char)*(this->value.nelem+1),
		      (char*)theVar->value.data.strptr[0] + elem*sizeof(char),
		      this->value.nelem * sizeof(char) );
	      /* Null terminate */
	      this->value.data.strptr[row][this->value.nelem] = 0;
	    } else {
	       elem  = this->value.nelem * (dimVals[0]-1);
	       elem += row*theVar->value.nelem;
	       memcpy( this->value.undef + row*this->value.nelem,
		       theVar->value.undef + elem,
		       this->value.nelem * sizeof(char) );
	       memcpy( (char*)this->value.data.ptr
		       + row*dsize*this->value.nelem,
		       (char*)theVar->value.data.ptr + elem*dsize,
		       this->value.nelem * dsize );
	    }
	 }
      }
   }

   if( theVar->operation>0 ) {
     if (theVar->type == STRING || theVar->type == BITSTR) 
       free(theVar->value.data.strptr[0] );
     else 
       free( theVar->value.data.ptr );
   }
   for( i=0; i<nDims; i++ )
      if( theDims[i]->operation>0 ) {
	 free( theDims[i]->value.data.ptr );
      }
}

static void Do_GTI( Node *this )
{
   Node *theExpr, *theTimes;
   double *start, *stop, *times;
   long elem, nGTI, gti;
   int ordered;

   theTimes = gParse.Nodes + this->SubNodes[0];
   theExpr  = gParse.Nodes + this->SubNodes[1];

   nGTI    = theTimes->value.nelem;
   start   = theTimes->value.data.dblptr;
   stop    = theTimes->value.data.dblptr + nGTI;
   ordered = theTimes->type;

   if( theExpr->operation==CONST_OP ) {

      this->value.data.log = 
	 (Search_GTI( theExpr->value.data.dbl, nGTI, start, stop, ordered )>=0);
      this->operation      = CONST_OP;

   } else {

      Allocate_Ptrs( this );

      times = theExpr->value.data.dblptr;
      if( !gParse.status ) {

	 elem = gParse.nRows * this->value.nelem;
	 if( nGTI ) {
	    gti = -1;
	    while( elem-- ) {
	       if( (this->value.undef[elem] = theExpr->value.undef[elem]) )
		  continue;

            /*  Before searching entire GTI, check the GTI found last time  */
	       if( gti<0 || times[elem]<start[gti] || times[elem]>stop[gti] ) {
		  gti = Search_GTI( times[elem], nGTI, start, stop, ordered );
	       }
	       this->value.data.logptr[elem] = ( gti>=0 );
	    }
	 } else
	    while( elem-- ) {
	       this->value.data.logptr[elem] = 0;
	       this->value.undef[elem]       = 0;
	    }
      }
   }

   if( theExpr->operation>0 )
      free( theExpr->value.data.ptr );
}

static long Search_GTI( double evtTime, long nGTI, double *start,
			double *stop, int ordered )
{
   long gti, step;
                             
   if( ordered && nGTI>15 ) { /*  If time-ordered and lots of GTIs,   */
                              /*  use "FAST" Binary search algorithm  */
      if( evtTime>=start[0] && evtTime<=stop[nGTI-1] ) {
	 gti = step = (nGTI >> 1);
	 while(1) {
	    if( step>1L ) step >>= 1;
	    
	    if( evtTime>stop[gti] ) {
	       if( evtTime>=start[gti+1] )
		  gti += step;
	       else {
		  gti = -1L;
		  break;
	       }
	    } else if( evtTime<start[gti] ) {
	       if( evtTime<=stop[gti-1] )
		  gti -= step;
	       else {
		  gti = -1L;
		  break;
	       }
	    } else {
	       break;
	    }
	 }
      } else
	 gti = -1L;
      
   } else { /*  Use "SLOW" linear search  */
      gti = nGTI;
      while( gti-- )
	 if( evtTime>=start[gti] && evtTime<=stop[gti] )
	    break;
   }
   return( gti );
}

static void Do_REG( Node *this )
{
   Node *theRegion, *theX, *theY;
   double Xval=0.0, Yval=0.0;
   char   Xnull=0, Ynull=0;
   int    Xvector, Yvector;
   long   nelem, elem, rows;

   theRegion = gParse.Nodes + this->SubNodes[0];
   theX      = gParse.Nodes + this->SubNodes[1];
   theY      = gParse.Nodes + this->SubNodes[2];

   Xvector = ( theX->operation!=CONST_OP );
   if( Xvector )
      Xvector = theX->value.nelem;
   else {
      Xval  = theX->value.data.dbl;
   }

   Yvector = ( theY->operation!=CONST_OP );
   if( Yvector )
      Yvector = theY->value.nelem;
   else {
      Yval  = theY->value.data.dbl;
   } 

   if( !Xvector && !Yvector ) {

      this->value.data.log =
	 ( fits_in_region( Xval, Yval, (SAORegion *)theRegion->value.data.ptr )
	   != 0 );
      this->operation      = CONST_OP;

   } else {

      Allocate_Ptrs( this );

      if( !gParse.status ) {

	 rows  = gParse.nRows;
	 nelem = this->value.nelem;
	 elem  = rows*nelem;

	 while( rows-- ) {
	    while( nelem-- ) {
	       elem--;

	       if( Xvector>1 ) {
		  Xval  = theX->value.data.dblptr[elem];
		  Xnull = theX->value.undef[elem];
	       } else if( Xvector ) {
		  Xval  = theX->value.data.dblptr[rows];
		  Xnull = theX->value.undef[rows];
	       }

	       if( Yvector>1 ) {
		  Yval  = theY->value.data.dblptr[elem];
		  Ynull = theY->value.undef[elem];
	       } else if( Yvector ) {
		  Yval  = theY->value.data.dblptr[rows];
		  Ynull = theY->value.undef[rows];
	       }

	       this->value.undef[elem] = ( Xnull || Ynull );
	       if( this->value.undef[elem] )
		  continue;

	       this->value.data.logptr[elem] = 
		  ( fits_in_region( Xval, Yval,
				    (SAORegion *)theRegion->value.data.ptr )
		    != 0 );
	    }
	    nelem = this->value.nelem;
	 }
      }
   }

   if( theX->operation>0 )
      free( theX->value.data.ptr );
   if( theY->operation>0 )
      free( theY->value.data.ptr );
}

static void Do_Vector( Node *this )
{
   Node *that;
   long row, elem, idx, jdx, offset=0;
   int node;

   Allocate_Ptrs( this );

   if( !gParse.status ) {

      for( node=0; node<this->nSubNodes; node++ ) {

	 that = gParse.Nodes + this->SubNodes[node];

	 if( that->operation == CONST_OP ) {

	    idx = gParse.nRows*this->value.nelem + offset;
	    while( (idx-=this->value.nelem)>=0 ) {
	       
	       this->value.undef[idx] = 0;

	       switch( this->type ) {
	       case BOOLEAN:
		  this->value.data.logptr[idx] = that->value.data.log;
		  break;
	       case LONG:
		  this->value.data.lngptr[idx] = that->value.data.lng;
		  break;
	       case DOUBLE:
		  this->value.data.dblptr[idx] = that->value.data.dbl;
		  break;
	       }
	    }
	    
	 } else {
	       
	    row  = gParse.nRows;
	    idx  = row * that->value.nelem;
	    while( row-- ) {
	       elem = that->value.nelem;
	       jdx = row*this->value.nelem + offset;
	       while( elem-- ) {
		  this->value.undef[jdx+elem] =
		     that->value.undef[--idx];

		  switch( this->type ) {
		  case BOOLEAN:
		     this->value.data.logptr[jdx+elem] =
			that->value.data.logptr[idx];
		     break;
		  case LONG:
		     this->value.data.lngptr[jdx+elem] =
			that->value.data.lngptr[idx];
		     break;
		  case DOUBLE:
		     this->value.data.dblptr[jdx+elem] =
			that->value.data.dblptr[idx];
		     break;
		  }
	       }
	    }
	 }
	 offset += that->value.nelem;
      }

   }

   for( node=0; node < this->nSubNodes; node++ )
     if( OPER(this->SubNodes[node])>0 )
       free( gParse.Nodes[this->SubNodes[node]].value.data.ptr );
}

/*****************************************************************************/
/*  Utility routines which perform the calculations on bits and SAO regions  */
/*****************************************************************************/

static char bitlgte(char *bits1, int oper, char *bits2)
{
 int val1, val2, nextbit;
 char result;
 int i, l1, l2, length, ldiff;
 char *stream=0;
 char chr1, chr2;

 l1 = strlen(bits1);
 l2 = strlen(bits2);
 length = (l1 > l2) ? l1 : l2;
 stream = (char *)malloc(sizeof(char)*(length+1));
 if (l1 < l2)
   {
    ldiff = l2 - l1;
    i=0;
    while( ldiff-- ) stream[i++] = '0';
    while( l1--    ) stream[i++] = *(bits1++);
    stream[i] = '\0';
    bits1 = stream;
   }
 else if (l2 < l1)
   {
    ldiff = l1 - l2;
    i=0;
    while( ldiff-- ) stream[i++] = '0';
    while( l2--    ) stream[i++] = *(bits2++);
    stream[i] = '\0';
    bits2 = stream;
   }

 val1 = val2 = 0;
 nextbit = 1;

 while( length-- )
    {
     chr1 = bits1[length];
     chr2 = bits2[length];
     if ((chr1 != 'x')&&(chr1 != 'X')&&(chr2 != 'x')&&(chr2 != 'X'))
       {
        if (chr1 == '1') val1 += nextbit;
        if (chr2 == '1') val2 += nextbit;
        nextbit *= 2;
       }
    }
 result = 0;
 switch (oper)
       {
        case LT:
             if (val1 < val2) result = 1;
             break;
        case LTE:
             if (val1 <= val2) result = 1;
             break;
        case GT:
             if (val1 > val2) result = 1;
             break;
        case GTE:
             if (val1 >= val2) result = 1;
             break;
       }
 free(stream);
 return (result);
}

static void bitand(char *result,char *bitstrm1,char *bitstrm2)
{
 int i, l1, l2, ldiff, largestStream;
 char *stream=0;
 char chr1, chr2;

 l1 = strlen(bitstrm1);
 l2 = strlen(bitstrm2);
 largestStream = (l1 > l2) ? l1 : l2;
 stream = (char *)malloc(sizeof(char)*(largestStream+1));
 if (l1 < l2)
   {
    ldiff = l2 - l1;
    i=0;
    while( ldiff-- ) stream[i++] = '0';
    while( l1--    ) stream[i++] = *(bitstrm1++);
    stream[i] = '\0';
    bitstrm1 = stream;
   }
 else if (l2 < l1)
   {
    ldiff = l1 - l2;
    i=0;
    while( ldiff-- ) stream[i++] = '0';
    while( l2--    ) stream[i++] = *(bitstrm2++);
    stream[i] = '\0';
    bitstrm2 = stream;
   }
 while ( (chr1 = *(bitstrm1++)) ) 
    {
       chr2 = *(bitstrm2++);
       if ((chr1 == 'x') || (chr2 == 'x'))
          *result = 'x';
       else if ((chr1 == '1') && (chr2 == '1'))
          *result = '1';
       else
          *result = '0';
       result++;
    }
 free(stream);
 *result = '\0';
}

static void bitor(char *result,char *bitstrm1,char *bitstrm2)
{
 int i, l1, l2, ldiff, largestStream;
 char *stream=0;
 char chr1, chr2;

 l1 = strlen(bitstrm1);
 l2 = strlen(bitstrm2);
 largestStream = (l1 > l2) ? l1 : l2;
 stream = (char *)malloc(sizeof(char)*(largestStream+1));
 if (l1 < l2)
   {
    ldiff = l2 - l1;
    i=0;
    while( ldiff-- ) stream[i++] = '0';
    while( l1--    ) stream[i++] = *(bitstrm1++);
    stream[i] = '\0';
    bitstrm1 = stream;
   }
 else if (l2 < l1)
   {
    ldiff = l1 - l2;
    i=0;
    while( ldiff-- ) stream[i++] = '0';
    while( l2--    ) stream[i++] = *(bitstrm2++);
    stream[i] = '\0';
    bitstrm2 = stream;
   }
 while ( (chr1 = *(bitstrm1++)) ) 
    {
       chr2 = *(bitstrm2++);
       if ((chr1 == '1') || (chr2 == '1'))
          *result = '1';
       else if ((chr1 == '0') || (chr2 == '0'))
          *result = '0';
       else
          *result = 'x';
       result++;
    }
 free(stream);
 *result = '\0';
}

static void bitnot(char *result,char *bits)
{
   int length;
   char chr;

   length = strlen(bits);
   while( length-- ) {
      chr = *(bits++);
      *(result++) = ( chr=='1' ? '0' : ( chr=='0' ? '1' : chr ) );
   }
   *result = '\0';
}

static char bitcmp(char *bitstrm1, char *bitstrm2)
{
 int i, l1, l2, ldiff, largestStream;
 char *stream=0;
 char chr1, chr2;

 l1 = strlen(bitstrm1);
 l2 = strlen(bitstrm2);
 largestStream = (l1 > l2) ? l1 : l2;
 stream = (char *)malloc(sizeof(char)*(largestStream+1));
 if (l1 < l2)
   {
    ldiff = l2 - l1;
    i=0;
    while( ldiff-- ) stream[i++] = '0';
    while( l1--    ) stream[i++] = *(bitstrm1++);
    stream[i] = '\0';
    bitstrm1 = stream;
   }
 else if (l2 < l1)
   {
    ldiff = l1 - l2;
    i=0;
    while( ldiff-- ) stream[i++] = '0';
    while( l2--    ) stream[i++] = *(bitstrm2++);
    stream[i] = '\0';
    bitstrm2 = stream;
   }
 while( (chr1 = *(bitstrm1++)) )
    {
       chr2 = *(bitstrm2++);
       if ( ((chr1 == '0') && (chr2 == '1'))
	    || ((chr1 == '1') && (chr2 == '0')) )
       {
          free(stream);
	  return( 0 );
       }
    }
 free(stream);
 return( 1 );
}

static char bnear(double x, double y, double tolerance)
{
 if (fabs(x - y) < tolerance)
   return ( 1 );
 else
   return ( 0 );
}

static char saobox(double xcen, double ycen, double xwid, double ywid,
		   double rot,  double xcol, double ycol)
{
 double x,y,xprime,yprime,xmin,xmax,ymin,ymax,theta;

 theta = (rot / 180.0) * myPI;
 xprime = xcol - xcen;
 yprime = ycol - ycen;
 x =  xprime * cos(theta) + yprime * sin(theta);
 y = -xprime * sin(theta) + yprime * cos(theta);
 xmin = - 0.5 * xwid; xmax = 0.5 * xwid;
 ymin = - 0.5 * ywid; ymax = 0.5 * ywid;
 if ((x >= xmin) && (x <= xmax) && (y >= ymin) && (y <= ymax))
   return ( 1 );
 else
   return ( 0 );
}

static char circle(double xcen, double ycen, double rad,
		   double xcol, double ycol)
{
 double r2,dx,dy,dlen;

 dx = xcol - xcen;
 dy = ycol - ycen;
 dx *= dx; dy *= dy;
 dlen = dx + dy;
 r2 = rad * rad;
 if (dlen <= r2)
   return ( 1 );
 else
   return ( 0 );
}

static char ellipse(double xcen, double ycen, double xrad, double yrad,
		    double rot, double xcol, double ycol)
{
 double x,y,xprime,yprime,dx,dy,dlen,theta;

 theta = (rot / 180.0) * myPI;
 xprime = xcol - xcen;
 yprime = ycol - ycen;
 x =  xprime * cos(theta) + yprime * sin(theta);
 y = -xprime * sin(theta) + yprime * cos(theta);
 dx = x / xrad; dy = y / yrad;
 dx *= dx; dy *= dy;
 dlen = dx + dy;
 if (dlen <= 1.0)
   return ( 1 );
 else
   return ( 0 );
}

/*
 * Extract substring
 */
int cstrmid(char *dest_str, int dest_len,
	    char *src_str,  int src_len,
	    int pos)
{
  /* char fill_char = ' '; */
  char fill_char = '\0';
  if (src_len == 0) { src_len = strlen(src_str); } /* .. if constant */

  /* Fill destination with blanks */
  if (pos < 0) { 
    fferror("STRMID(S,P,N) P must be 0 or greater");
    return -1;
  }
  if (pos > src_len || pos == 0) {
    /* pos==0: blank string requested */
    memset(dest_str, fill_char, dest_len);
  } else if (pos+dest_len > src_len) {
    /* Copy a subset */
    int nsub = src_len-pos+1;
    int npad = dest_len - nsub;
    memcpy(dest_str, src_str+pos-1, nsub);
    /* Fill remaining string with blanks */
    memset(dest_str+nsub, fill_char, npad);
  } else {
    /* Full string copy */
    memcpy(dest_str, src_str+pos-1, dest_len);
  }
  dest_str[dest_len] = '\0'; /* Null-terminate */

  return 0;
}


static void fferror(char *s)
{
    char msg[80];

    if( !gParse.status ) gParse.status = PARSE_SYNTAX_ERR;

    strncpy(msg, s, 80);
    msg[79] = '\0';
    ffpmsg(msg);
}

