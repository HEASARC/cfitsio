/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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
#define FFBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define FFSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define FFPURE 0

/* Push parsers.  */
#define FFPUSH 0

/* Pull parsers.  */
#define FFPULL 1




/* Copy the first part of user declarations.  */
#line 1 "eval.y" /* yacc.c:339  */

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
/*  Craig B Markwardt Mar 2021  Add SETNULL() function                  */
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
static int  New_GTI   ( funcOp Op, char *fname, int Node1, int Node2, char *start, char *stop );
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
static void Do_GTI_Over  ( Node *this );
static void Do_REG       ( Node *this );
static void Do_Vector    ( Node *this );

static long Search_GTI   ( double evtTime, long nGTI, double *start,
			   double *stop, int ordered, long *nextGTI );
static double GTI_Over(double evtStart, double evtStop,
		       long nGTI, double *start, double *stop,
		       long *gtiout);

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


#line 264 "y.tab.c" /* yacc.c:339  */

# ifndef FF_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define FF_NULLPTR nullptr
#  else
#   define FF_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef FFERROR_VERBOSE
# undef FFERROR_VERBOSE
# define FFERROR_VERBOSE 1
#else
# define FFERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef FF_FF_Y_TAB_H_INCLUDED
# define FF_FF_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef FFDEBUG
# define FFDEBUG 0
#endif
#if FFDEBUG
extern int ffdebug;
#endif

/* Token type.  */
#ifndef FFTOKENTYPE
# define FFTOKENTYPE
  enum fftokentype
  {
    BOOLEAN = 258,
    LONG = 259,
    DOUBLE = 260,
    STRING = 261,
    BITSTR = 262,
    FUNCTION = 263,
    BFUNCTION = 264,
    IFUNCTION = 265,
    GTIFILTER = 266,
    GTIOVERLAP = 267,
    GTIFIND = 268,
    REGFILTER = 269,
    COLUMN = 270,
    BCOLUMN = 271,
    SCOLUMN = 272,
    BITCOL = 273,
    ROWREF = 274,
    NULLREF = 275,
    SNULLREF = 276,
    OR = 277,
    AND = 278,
    EQ = 279,
    NE = 280,
    GT = 281,
    LT = 282,
    LTE = 283,
    GTE = 284,
    XOR = 285,
    POWER = 286,
    NOT = 287,
    INTCAST = 288,
    FLTCAST = 289,
    UMINUS = 290,
    ACCUM = 291,
    DIFF = 292
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
#define GTIOVERLAP 267
#define GTIFIND 268
#define REGFILTER 269
#define COLUMN 270
#define BCOLUMN 271
#define SCOLUMN 272
#define BITCOL 273
#define ROWREF 274
#define NULLREF 275
#define SNULLREF 276
#define OR 277
#define AND 278
#define EQ 279
#define NE 280
#define GT 281
#define LT 282
#define LTE 283
#define GTE 284
#define XOR 285
#define POWER 286
#define NOT 287
#define INTCAST 288
#define FLTCAST 289
#define UMINUS 290
#define ACCUM 291
#define DIFF 292

/* Value type.  */
#if ! defined FFSTYPE && ! defined FFSTYPE_IS_DECLARED

union FFSTYPE
{
#line 199 "eval.y" /* yacc.c:355  */

    int    Node;        /* Index of Node */
    double dbl;         /* real value    */
    long   lng;         /* integer value */
    char   log;         /* logical value */
    char   str[MAX_STRLEN];    /* string value  */

#line 386 "y.tab.c" /* yacc.c:355  */
};

typedef union FFSTYPE FFSTYPE;
# define FFSTYPE_IS_TRIVIAL 1
# define FFSTYPE_IS_DECLARED 1
#endif


extern FFSTYPE fflval;

int ffparse (void);

#endif /* !FF_FF_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 403 "y.tab.c" /* yacc.c:358  */

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
#else
typedef signed char fftype_int8;
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
# elif ! defined FFSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define FFSIZE_T size_t
# else
#  define FFSIZE_T unsigned int
# endif
#endif

#define FFSIZE_MAXIMUM ((FFSIZE_T) -1)

#ifndef FF_
# if defined FFENABLE_NLS && FFENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define FF_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef FF_
#  define FF_(Msgid) Msgid
# endif
#endif

#ifndef FF_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define FF_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define FF_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef FF_ATTRIBUTE_PURE
# define FF_ATTRIBUTE_PURE   FF_ATTRIBUTE ((__pure__))
#endif

#ifndef FF_ATTRIBUTE_UNUSED
# define FF_ATTRIBUTE_UNUSED FF_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn FF_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define FFUSE(E) ((void) (E))
#else
# define FFUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about fflval being uninitialized.  */
# define FF_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define FF_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define FF_INITIAL_VALUE(Value) Value
#endif
#ifndef FF_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define FF_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define FF_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef FF_INITIAL_VALUE
# define FF_INITIAL_VALUE(Value) /* Nothing. */
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef FFSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define FFSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined FFMALLOC || defined malloc) \
             && (defined FFFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef FFMALLOC
#   define FFMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (FFSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef FFFREE
#   define FFFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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

# define FFCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables FFSIZE and FFSTACKSIZE give the old and new number of
   elements in the stack, and FFPTR gives the new location of the
   stack.  Advance FFPTR to a properly aligned location for the next
   stack.  */
# define FFSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        FFSIZE_T ffnewbytes;                                            \
        FFCOPY (&ffptr->Stack_alloc, Stack, ffsize);                    \
        Stack = &ffptr->Stack_alloc;                                    \
        ffnewbytes = ffstacksize * sizeof (*Stack) + FFSTACK_GAP_MAXIMUM; \
        ffptr += ffnewbytes / sizeof (*ffptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined FFCOPY_NEEDED && FFCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef FFCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define FFCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define FFCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          FFSIZE_T ffi;                         \
          for (ffi = 0; ffi < (Count); ffi++)   \
            (Dst)[ffi] = (Src)[ffi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !FFCOPY_NEEDED */

/* FFFINAL -- State number of the termination state.  */
#define FFFINAL  2
/* FFLAST -- Last index in FFTABLE.  */
#define FFLAST   1746

/* FFNTOKENS -- Number of terminals.  */
#define FFNTOKENS  57
/* FFNNTS -- Number of nonterminals.  */
#define FFNNTS  9
/* FFNRULES -- Number of rules.  */
#define FFNRULES  134
/* FFNSTATES -- Number of states.  */
#define FFNSTATES  320

/* FFTRANSLATE[FFX] -- Symbol number corresponding to FFX as returned
   by fflex, with out-of-bounds checking.  */
#define FFUNDEFTOK  2
#define FFMAXUTOK   292

#define FFTRANSLATE(FFX)                                                \
  ((unsigned int) (FFX) <= FFMAXUTOK ? fftranslate[FFX] : FFUNDEFTOK)

/* FFTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by fflex, without out-of-bounds checking.  */
static const fftype_uint8 fftranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      53,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    39,    43,     2,
      55,    56,    40,    37,    22,    38,     2,    41,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    24,     2,
       2,    23,     2,    27,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    50,     2,    54,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    25,    42,    26,    32,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    28,    29,    30,
      31,    33,    34,    35,    36,    44,    45,    46,    47,    48,
      49,    51,    52
};

#if FFDEBUG
  /* FFRLINE[FFN] -- Source line where rule number FFN was defined.  */
static const fftype_uint16 ffrline[] =
{
       0,   253,   253,   254,   257,   258,   264,   270,   276,   282,
     285,   287,   300,   302,   315,   326,   340,   344,   348,   352,
     354,   363,   366,   369,   378,   380,   382,   384,   386,   388,
     391,   395,   397,   399,   401,   410,   412,   414,   417,   420,
     423,   426,   429,   438,   447,   456,   459,   461,   463,   465,
     469,   473,   492,   511,   530,   541,   555,   567,   598,   693,
     701,   762,   786,   788,   790,   792,   794,   796,   798,   800,
     802,   806,   808,   810,   819,   822,   825,   828,   831,   834,
     837,   840,   843,   846,   849,   852,   855,   858,   861,   864,
     867,   870,   873,   876,   878,   880,   882,   885,   892,   909,
     922,   935,   946,   962,   986,  1014,  1051,  1055,  1059,  1062,
    1067,  1070,  1075,  1079,  1083,  1086,  1091,  1095,  1098,  1102,
    1104,  1106,  1108,  1110,  1112,  1114,  1118,  1121,  1123,  1132,
    1134,  1136,  1145,  1164,  1183
};
#endif

#if FFDEBUG || FFERROR_VERBOSE || 0
/* FFTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at FFNTOKENS, nonterminals.  */
static const char *const fftname[] =
{
  "$end", "error", "$undefined", "BOOLEAN", "LONG", "DOUBLE", "STRING",
  "BITSTR", "FUNCTION", "BFUNCTION", "IFUNCTION", "GTIFILTER",
  "GTIOVERLAP", "GTIFIND", "REGFILTER", "COLUMN", "BCOLUMN", "SCOLUMN",
  "BITCOL", "ROWREF", "NULLREF", "SNULLREF", "','", "'='", "':'", "'{'",
  "'}'", "'?'", "OR", "AND", "EQ", "NE", "'~'", "GT", "LT", "LTE", "GTE",
  "'+'", "'-'", "'%'", "'*'", "'/'", "'|'", "'&'", "XOR", "POWER", "NOT",
  "INTCAST", "FLTCAST", "UMINUS", "'['", "ACCUM", "DIFF", "'\\n'", "']'",
  "'('", "')'", "$accept", "lines", "line", "bvector", "vector", "expr",
  "bexpr", "bits", "sexpr", FF_NULLPTR
};
#endif

# ifdef FFPRINT
/* FFTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const fftype_uint16 fftoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,    44,    61,    58,   123,   125,    63,   277,   278,
     279,   280,   126,   281,   282,   283,   284,    43,    45,    37,
      42,    47,   124,    38,   285,   286,   287,   288,   289,   290,
      91,   291,   292,    10,    93,    40,    41
};
# endif

#define FFPACT_NINF -41

#define ffpact_value_is_default(Yystate) \
  (!!((Yystate) == (-41)))

#define FFTABLE_NINF -1

#define fftable_value_is_error(Yytable_value) \
  0

  /* FFPACT[STATE-NUM] -- Index in FFTABLE of the portion describing
     STATE-NUM.  */
static const fftype_int16 ffpact[] =
{
     -41,   319,   -41,   -40,   -41,   -41,   -41,   -41,   -41,   372,
     426,   426,    -5,    15,    -4,    27,    36,    38,    40,    41,
     -41,   -41,   -41,   426,   426,   426,   426,   426,   426,   -41,
     426,   -41,    -7,    10,  1201,    81,  1588,  1609,   -41,   -41,
     453,   151,   209,    12,   482,   175,  1651,   422,  1530,   116,
    1693,   -19,   -41,    13,   -18,   -41,     6,   426,   426,   426,
     426,  1530,   116,  1709,    17,    17,    19,    24,    17,    19,
      17,    19,   713,   251,   365,  1548,   426,   -41,   426,   -41,
     426,   426,   426,   426,   426,   426,   426,   426,   426,   426,
     426,   426,   426,   426,   426,   426,   426,   426,   -41,   426,
     426,   426,   426,   426,   426,   426,   -41,    -2,    -2,    -2,
      -2,    -2,    -2,    -2,    -2,    -2,   426,   -41,   426,   426,
     426,   426,   426,   426,   426,   -41,   426,   -41,   426,   -41,
     -41,   426,   -41,   426,   -41,   -41,   -41,   426,   426,   -41,
     426,   426,   -41,   426,   -41,  1392,  1415,  1438,  1461,   -41,
     -41,   -41,   -41,  1530,   116,  1530,   116,  1484,  1669,  1669,
    1669,  1610,  1610,  1610,  1610,   169,   169,   169,    28,    19,
      28,     5,     5,     5,     5,   826,  1507,  1562,  1701,   312,
     -20,    14,    14,    28,   851,    -2,    -2,   -25,   -25,   -25,
     -25,   -25,   -25,   -36,    24,    24,   876,   275,   275,    39,
      39,    39,    39,   -41,   511,   265,  1225,  1564,  1249,  1580,
     540,  1273,   569,  1297,   -41,   -41,   -41,   -41,   426,   426,
     -41,   426,   426,   426,   426,   -41,    24,   127,   426,   -41,
     426,   -41,   -41,   426,   -41,   426,   -41,    93,   -41,   426,
      94,   -41,   426,  1633,   901,  1633,   116,  1633,   116,  1709,
     926,   951,  1321,   741,   598,    79,   627,    80,   656,   426,
     -41,   426,   -41,   426,   -41,   426,   -41,   426,   -41,   101,
     107,   -41,   108,   110,   -41,   976,  1001,  1026,   769,  1345,
      61,    98,    62,    71,   426,   -41,   426,   -41,   426,   -41,
     -41,   426,   -41,   122,   -41,   -41,  1051,  1076,  1101,   685,
      77,   426,   -41,   426,   -41,   426,   -41,   426,   -41,   -41,
    1126,  1151,  1176,  1369,   -41,   -41,   -41,   426,   797,   -41
};

  /* FFDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when FFTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const fftype_uint8 ffdefact[] =
{
       2,     0,     1,     0,    71,    31,    32,   126,    18,     0,
       0,     0,     0,     0,     0,     0,    33,    72,   127,    19,
      35,    36,   129,     0,     0,     0,     0,     0,     0,     4,
       0,     3,     0,     0,     0,     0,     0,     0,     9,    54,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   106,     0,     0,   112,     0,     0,     0,     0,
       0,    12,    10,     0,    46,    47,   124,    29,    67,    68,
      69,    70,     0,     0,     0,     0,     0,    17,     0,    16,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     5,     0,
       0,     0,     0,     0,     0,     0,     6,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     8,     0,     0,
       0,     0,     0,     0,     0,     7,     0,    58,     0,    55,
      57,     0,    56,     0,    99,   100,   101,     0,     0,   107,
       0,     0,   113,     0,   116,     0,     0,     0,     0,    48,
     125,    30,   130,    15,    11,    13,    14,     0,    85,    86,
      84,    80,    81,    83,    82,    38,    39,    37,    40,    49,
      41,    43,    42,    44,    45,     0,     0,     0,     0,    94,
      93,    95,    96,    50,     0,     0,     0,    74,    75,    78,
      76,    77,    79,    23,    22,    21,     0,    87,    88,    89,
      91,    92,    90,   131,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    34,    73,   128,    20,     0,     0,
      62,     0,     0,     0,     0,   119,    29,     0,     0,    24,
       0,    60,   102,     0,   133,     0,    59,     0,   108,     0,
       0,   114,     0,    97,     0,    51,    53,    52,    98,   132,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      63,     0,   120,     0,    25,     0,   134,     0,   103,     0,
       0,   110,     0,     0,   117,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    64,     0,   121,     0,    26,
      61,     0,   109,     0,   115,   118,     0,     0,     0,     0,
       0,     0,    65,     0,   122,     0,    27,     0,   104,   111,
       0,     0,     0,     0,    66,   123,    28,     0,     0,   105
};

  /* FFPGOTO[NTERM-NUM].  */
static const fftype_int16 ffpgoto[] =
{
     -41,   -41,   -41,   -41,   -41,    -1,    96,   162,    30
};

  /* FFDEFGOTO[NTERM-NUM].  */
static const fftype_int8 ffdefgoto[] =
{
      -1,     1,    31,    32,    33,    48,    49,    46,    63
};

  /* FFTABLE[FFPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If FFTABLE_NINF, syntax error.  */
static const fftype_uint16 fftable[] =
{
      34,    51,    54,   138,   141,     8,   114,   115,    40,    44,
     102,   103,   113,    38,   116,    76,    19,   114,   115,    77,
     104,    53,    61,    64,    65,   116,    68,    70,   143,    72,
     105,    37,    78,    56,   131,   140,    79,   139,   142,    43,
      47,    50,   118,   119,   185,   120,   121,   122,   123,   124,
      96,    52,    55,   186,   104,    97,   145,   146,   147,   148,
      75,    57,   144,    58,   105,    59,    60,    97,   132,   105,
      93,    94,    95,    96,   116,   153,   124,   155,    97,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   170,   171,   172,   173,   174,   175,    35,   176,   255,
     257,   269,   272,   183,   184,    41,    45,   280,    99,   100,
     101,   102,   103,   281,   282,   196,   283,   292,   294,    62,
     293,   104,    66,    69,    71,   204,    73,   295,   300,   178,
     206,   105,   208,   309,   106,     0,     0,   210,     0,   211,
     212,     0,   213,    99,   100,   101,   102,   103,   197,   198,
     199,   200,   201,   202,   203,     0,   104,     0,     0,     0,
       0,   207,     0,    36,   113,     0,   105,   209,     0,   114,
     115,    42,   154,   128,   156,     0,     0,   116,    99,   100,
     101,   102,   103,   151,     0,     0,     0,   169,    67,     0,
       0,   104,    74,     0,     0,   177,   179,   180,   181,   182,
       0,   105,    99,   100,   101,   102,   103,   129,     0,    91,
      92,    93,    94,    95,    96,   104,     0,   243,   244,    97,
     245,   247,     0,   250,   205,   105,     0,   251,     0,   252,
       0,   135,   253,     0,   254,     0,     0,     0,   256,   107,
     108,   258,   109,   110,   111,   112,   113,     0,     0,     0,
       0,   114,   115,   249,     0,     0,     0,     0,   275,   116,
     276,     0,   277,     0,   278,   130,   279,     0,     0,   187,
     188,   189,   190,   191,   192,   193,   194,   195,    99,   100,
     101,   102,   103,   296,     0,   297,     0,   298,     0,     0,
     299,   104,    99,   100,   101,   102,   103,     0,     0,     0,
     310,   105,   311,     0,   312,   104,   313,   150,   120,   121,
     122,   123,   124,     0,     0,   105,   318,   246,   248,     2,
       3,   232,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,   101,   102,   103,    23,     0,     0,   226,   227,     0,
       0,     0,   104,     0,     0,     0,    24,    25,     0,     0,
       0,     0,   105,     0,     0,    26,    27,    28,     0,     0,
       0,     0,    29,     0,    30,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,     0,   107,   108,    23,   109,   110,
     111,   112,   113,     0,     0,     0,     0,   114,   115,    24,
      25,     0,     0,     0,     0,   116,     0,     0,    26,    27,
      28,   151,     0,     0,     0,     0,     0,    30,    39,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,     0,     0,
       0,    23,   118,   119,     0,   120,   121,   122,   123,   124,
       0,     0,     0,    24,    25,     0,     0,     0,     0,     0,
       0,     0,    26,    27,    28,   126,    80,     0,   136,     0,
       0,    30,     0,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,     0,
       0,     0,     0,    97,   133,    80,     0,     0,     0,   127,
       0,     0,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,     0,     0,
       0,     0,    97,   230,    80,     0,     0,     0,   134,     0,
       0,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,     0,     0,     0,
       0,    97,   237,    80,     0,     0,     0,   231,     0,     0,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,     0,     0,     0,     0,
      97,   240,    80,     0,     0,     0,   238,     0,     0,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,     0,     0,     0,     0,    97,
     267,    80,     0,     0,     0,   241,     0,     0,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,     0,     0,     0,     0,    97,   270,
      80,     0,     0,     0,   268,     0,     0,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,     0,     0,     0,     0,    97,   273,    80,
       0,     0,     0,   271,     0,     0,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,     0,     0,     0,     0,    97,   307,    80,     0,
       0,     0,   274,     0,     0,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,     0,     0,     0,     0,    97,    80,     0,     0,     0,
       0,   308,     0,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,     0,
       0,     0,     0,    97,    80,     0,     0,     0,     0,   149,
       0,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,     0,     0,     0,
       0,    97,    80,     0,     0,     0,     0,   266,     0,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,     0,     0,     0,     0,    97,
      80,     0,     0,     0,     0,   290,     0,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,     0,     0,     0,     0,    97,   219,    80,
       0,     0,     0,   319,     0,     0,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,     0,   224,    80,     0,    97,     0,     0,     0,
     220,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,     0,   228,    80,
       0,    97,     0,     0,     0,   225,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,     0,   259,    80,     0,    97,     0,     0,     0,
     229,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,     0,   261,    80,
       0,    97,     0,     0,     0,   260,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,     0,   263,    80,     0,    97,     0,     0,     0,
     262,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,     0,   284,    80,
       0,    97,     0,     0,     0,   264,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,     0,   286,    80,     0,    97,     0,     0,     0,
     285,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,     0,   288,    80,
       0,    97,     0,     0,     0,   287,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,     0,   301,    80,     0,    97,     0,     0,     0,
     289,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,     0,   303,    80,
       0,    97,     0,     0,     0,   302,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,     0,   305,    80,     0,    97,     0,     0,     0,
     304,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,     0,     0,    80,
       0,    97,     0,     0,     0,   306,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,     0,     0,    80,     0,    97,     0,     0,     0,
     314,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,     0,     0,    80,
       0,    97,     0,     0,     0,   315,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,     0,     0,    80,     0,    97,     0,     0,     0,
     316,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,   233,    80,     0,
       0,    97,     0,     0,    98,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,   235,    80,     0,     0,    97,     0,     0,     0,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,   239,    80,     0,     0,    97,
       0,     0,     0,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,   242,
      80,     0,     0,    97,     0,     0,     0,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,   265,    80,     0,     0,    97,     0,     0,
       0,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,   291,    80,     0,
       0,    97,     0,     0,     0,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,   317,    80,     0,     0,    97,     0,     0,     0,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    80,     0,     0,   214,    97,
       0,     0,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    80,     0,
       0,   215,    97,     0,     0,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    80,     0,     0,   216,    97,     0,     0,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    80,     0,     0,   217,    97,     0,
       0,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    80,   218,     0,
       0,    97,     0,     0,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      80,   221,     0,     0,    97,     0,     0,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    80,     0,     0,     0,    97,     0,     0,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,     0,     0,   118,   119,
      97,   120,   121,   122,   123,   124,   222,     0,     0,    99,
     100,   101,   102,   103,   118,   119,     0,   120,   121,   122,
     123,   124,   104,     0,   152,     0,     0,     0,     0,     0,
     118,   119,   105,   120,   121,   122,   123,   124,   107,   108,
     234,   109,   110,   111,   112,   113,     0,     0,     0,     0,
     114,   115,     0,     0,     0,     0,   236,     0,   116,   118,
     119,   117,   120,   121,   122,   123,   124,    88,    89,    90,
      91,    92,    93,    94,    95,    96,     0,     0,     0,     0,
      97,     0,   125,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,     0,
       0,   107,   108,    97,   109,   110,   111,   112,   113,     0,
       0,     0,     0,   114,   115,     0,     0,     0,     0,     0,
       0,   116,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,   137,     0,     0,     0,    97,
       0,     0,     0,   118,   119,   223,   120,   121,   122,   123,
     124,   118,   119,     0,   120,   121,   122,   123,   124,   118,
     119,     0,   120,   121,   122,   123,   124
};

static const fftype_int16 ffcheck[] =
{
       1,     6,     6,    22,    22,     7,    42,    43,     9,    10,
      30,    31,    37,    53,    50,    22,    18,    42,    43,    26,
      40,     6,    23,    24,    25,    50,    27,    28,    22,    30,
      50,     1,    22,     6,    22,    22,    26,    56,    56,     9,
      10,    11,    30,    31,    46,    33,    34,    35,    36,    37,
      45,    56,    56,    55,    40,    50,    57,    58,    59,    60,
      30,    25,    56,    25,    50,    25,    25,    50,    56,    50,
      42,    43,    44,    45,    50,    76,    37,    78,    50,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,     1,    99,     6,
       6,    22,    22,   104,   105,     9,    10,     6,    27,    28,
      29,    30,    31,     6,     6,   116,     6,    56,    56,    23,
      22,    40,    26,    27,    28,   126,    30,    56,     6,    99,
     131,    50,   133,    56,    53,    -1,    -1,   138,    -1,   140,
     141,    -1,   143,    27,    28,    29,    30,    31,   118,   119,
     120,   121,   122,   123,   124,    -1,    40,    -1,    -1,    -1,
      -1,   131,    -1,     1,    37,    -1,    50,   137,    -1,    42,
      43,     9,    76,    22,    78,    -1,    -1,    50,    27,    28,
      29,    30,    31,    56,    -1,    -1,    -1,    91,    26,    -1,
      -1,    40,    30,    -1,    -1,    99,   100,   101,   102,   103,
      -1,    50,    27,    28,    29,    30,    31,    56,    -1,    40,
      41,    42,    43,    44,    45,    40,    -1,   218,   219,    50,
     221,   222,    -1,   224,   128,    50,    -1,   228,    -1,   230,
      -1,    56,   233,    -1,   235,    -1,    -1,    -1,   239,    30,
      31,   242,    33,    34,    35,    36,    37,    -1,    -1,    -1,
      -1,    42,    43,   223,    -1,    -1,    -1,    -1,   259,    50,
     261,    -1,   263,    -1,   265,    56,   267,    -1,    -1,   107,
     108,   109,   110,   111,   112,   113,   114,   115,    27,    28,
      29,    30,    31,   284,    -1,   286,    -1,   288,    -1,    -1,
     291,    40,    27,    28,    29,    30,    31,    -1,    -1,    -1,
     301,    50,   303,    -1,   305,    40,   307,    56,    33,    34,
      35,    36,    37,    -1,    -1,    50,   317,   221,   222,     0,
       1,    56,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    29,    30,    31,    25,    -1,    -1,   185,   186,    -1,
      -1,    -1,    40,    -1,    -1,    -1,    37,    38,    -1,    -1,
      -1,    -1,    50,    -1,    -1,    46,    47,    48,    -1,    -1,
      -1,    -1,    53,    -1,    55,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    -1,    30,    31,    25,    33,    34,
      35,    36,    37,    -1,    -1,    -1,    -1,    42,    43,    37,
      38,    -1,    -1,    -1,    -1,    50,    -1,    -1,    46,    47,
      48,    56,    -1,    -1,    -1,    -1,    -1,    55,    56,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    -1,    -1,
      -1,    25,    30,    31,    -1,    33,    34,    35,    36,    37,
      -1,    -1,    -1,    37,    38,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    46,    47,    48,    22,    23,    -1,    56,    -1,
      -1,    55,    -1,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    -1,
      -1,    -1,    -1,    50,    22,    23,    -1,    -1,    -1,    56,
      -1,    -1,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    -1,    -1,
      -1,    -1,    50,    22,    23,    -1,    -1,    -1,    56,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    -1,    -1,    -1,
      -1,    50,    22,    23,    -1,    -1,    -1,    56,    -1,    -1,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    -1,    -1,    -1,    -1,
      50,    22,    23,    -1,    -1,    -1,    56,    -1,    -1,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    -1,    -1,    -1,    -1,    50,
      22,    23,    -1,    -1,    -1,    56,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    -1,    -1,    -1,    -1,    50,    22,
      23,    -1,    -1,    -1,    56,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    -1,    -1,    -1,    -1,    50,    22,    23,
      -1,    -1,    -1,    56,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    -1,    -1,    -1,    -1,    50,    22,    23,    -1,
      -1,    -1,    56,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    -1,    -1,    -1,    -1,    50,    23,    -1,    -1,    -1,
      -1,    56,    -1,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    -1,
      -1,    -1,    -1,    50,    23,    -1,    -1,    -1,    -1,    56,
      -1,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    -1,    -1,    -1,
      -1,    50,    23,    -1,    -1,    -1,    -1,    56,    -1,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    -1,    -1,    -1,    -1,    50,
      23,    -1,    -1,    -1,    -1,    56,    -1,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    -1,    -1,    -1,    -1,    50,    22,    23,
      -1,    -1,    -1,    56,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    -1,    22,    23,    -1,    50,    -1,    -1,    -1,
      54,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    -1,    22,    23,
      -1,    50,    -1,    -1,    -1,    54,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    -1,    22,    23,    -1,    50,    -1,    -1,    -1,
      54,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    -1,    22,    23,
      -1,    50,    -1,    -1,    -1,    54,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    -1,    22,    23,    -1,    50,    -1,    -1,    -1,
      54,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    -1,    22,    23,
      -1,    50,    -1,    -1,    -1,    54,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    -1,    22,    23,    -1,    50,    -1,    -1,    -1,
      54,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    -1,    22,    23,
      -1,    50,    -1,    -1,    -1,    54,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    -1,    22,    23,    -1,    50,    -1,    -1,    -1,
      54,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    -1,    22,    23,
      -1,    50,    -1,    -1,    -1,    54,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    -1,    22,    23,    -1,    50,    -1,    -1,    -1,
      54,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    -1,    -1,    23,
      -1,    50,    -1,    -1,    -1,    54,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    -1,    -1,    23,    -1,    50,    -1,    -1,    -1,
      54,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    -1,    -1,    23,
      -1,    50,    -1,    -1,    -1,    54,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    -1,    -1,    23,    -1,    50,    -1,    -1,    -1,
      54,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    22,    23,    -1,
      -1,    50,    -1,    -1,    53,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    22,    23,    -1,    -1,    50,    -1,    -1,    -1,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    22,    23,    -1,    -1,    50,
      -1,    -1,    -1,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    22,
      23,    -1,    -1,    50,    -1,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    22,    23,    -1,    -1,    50,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    22,    23,    -1,
      -1,    50,    -1,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    22,    23,    -1,    -1,    50,    -1,    -1,    -1,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    23,    -1,    -1,    26,    50,
      -1,    -1,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    23,    -1,
      -1,    26,    50,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    23,    -1,    -1,    26,    50,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    23,    -1,    -1,    26,    50,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    23,    24,    -1,
      -1,    50,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      23,    24,    -1,    -1,    50,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    23,    -1,    -1,    -1,    50,    -1,    -1,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    -1,    -1,    30,    31,
      50,    33,    34,    35,    36,    37,    24,    -1,    -1,    27,
      28,    29,    30,    31,    30,    31,    -1,    33,    34,    35,
      36,    37,    40,    -1,    56,    -1,    -1,    -1,    -1,    -1,
      30,    31,    50,    33,    34,    35,    36,    37,    30,    31,
      56,    33,    34,    35,    36,    37,    -1,    -1,    -1,    -1,
      42,    43,    -1,    -1,    -1,    -1,    56,    -1,    50,    30,
      31,    53,    33,    34,    35,    36,    37,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    -1,    -1,    -1,    -1,
      50,    -1,    53,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    -1,
      -1,    30,    31,    50,    33,    34,    35,    36,    37,    -1,
      -1,    -1,    -1,    42,    43,    -1,    -1,    -1,    -1,    -1,
      -1,    50,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    22,    -1,    -1,    -1,    50,
      -1,    -1,    -1,    30,    31,    24,    33,    34,    35,    36,
      37,    30,    31,    -1,    33,    34,    35,    36,    37,    30,
      31,    -1,    33,    34,    35,    36,    37
};

  /* FFSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const fftype_uint8 ffstos[] =
{
       0,    58,     0,     1,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    25,    37,    38,    46,    47,    48,    53,
      55,    59,    60,    61,    62,    63,    64,    65,    53,    56,
      62,    63,    64,    65,    62,    63,    64,    65,    62,    63,
      65,     6,    56,     6,     6,    56,     6,    25,    25,    25,
      25,    62,    63,    65,    62,    62,    63,    64,    62,    63,
      62,    63,    62,    63,    64,    65,    22,    26,    22,    26,
      23,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    50,    53,    27,
      28,    29,    30,    31,    40,    50,    53,    30,    31,    33,
      34,    35,    36,    37,    42,    43,    50,    53,    30,    31,
      33,    34,    35,    36,    37,    53,    22,    56,    22,    56,
      56,    22,    56,    22,    56,    56,    56,    22,    22,    56,
      22,    22,    56,    22,    56,    62,    62,    62,    62,    56,
      56,    56,    56,    62,    63,    62,    63,    62,    62,    62,
      62,    62,    62,    62,    62,    62,    62,    62,    62,    63,
      62,    62,    62,    62,    62,    62,    62,    63,    65,    63,
      63,    63,    63,    62,    62,    46,    55,    64,    64,    64,
      64,    64,    64,    64,    64,    64,    62,    65,    65,    65,
      65,    65,    65,    65,    62,    63,    62,    65,    62,    65,
      62,    62,    62,    62,    26,    26,    26,    26,    24,    22,
      54,    24,    24,    24,    22,    54,    64,    64,    22,    54,
      22,    56,    56,    22,    56,    22,    56,    22,    56,    22,
      22,    56,    22,    62,    62,    62,    63,    62,    63,    65,
      62,    62,    62,    62,    62,     6,    62,     6,    62,    22,
      54,    22,    54,    22,    54,    22,    56,    22,    56,    22,
      22,    56,    22,    22,    56,    62,    62,    62,    62,    62,
       6,     6,     6,     6,    22,    54,    22,    54,    22,    54,
      56,    22,    56,    22,    56,    56,    62,    62,    62,    62,
       6,    22,    54,    22,    54,    22,    54,    22,    56,    56,
      62,    62,    62,    62,    54,    54,    54,    22,    62,    56
};

  /* FFR1[FFN] -- Symbol number of symbol that rule FFN derives.  */
static const fftype_uint8 ffr1[] =
{
       0,    57,    58,    58,    59,    59,    59,    59,    59,    59,
      60,    60,    61,    61,    61,    61,    62,    63,    64,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    64,    64,
      64,    62,    62,    62,    62,    62,    62,    62,    62,    62,
      62,    62,    62,    62,    62,    62,    62,    62,    62,    62,
      62,    62,    62,    62,    62,    62,    62,    62,    62,    62,
      62,    62,    62,    62,    62,    62,    62,    62,    62,    62,
      62,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    65,    65,    65,    65,
      65,    65,    65,    65,    65
};

  /* FFR2[FFN] -- Number of symbols on the right hand side of rule FFN.  */
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
       7,    11,     2,     3,     5,     9,     3,     7,     9,     4,
       6,     8,    10,    12,     2,     3,     1,     1,     4,     1,
       3,     3,     5,     5,     7
};


#define fferrok         (fferrstatus = 0)
#define ffclearin       (ffchar = FFEMPTY)
#define FFEMPTY         (-2)
#define FFEOF           0

#define FFACCEPT        goto ffacceptlab
#define FFABORT         goto ffabortlab
#define FFERROR         goto fferrorlab


#define FFRECOVERING()  (!!fferrstatus)

#define FFBACKUP(Token, Value)                                  \
do                                                              \
  if (ffchar == FFEMPTY)                                        \
    {                                                           \
      ffchar = (Token);                                         \
      fflval = (Value);                                         \
      FFPOPSTACK (fflen);                                       \
      ffstate = *ffssp;                                         \
      goto ffbackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      fferror (FF_("syntax error: cannot back up")); \
      FFERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define FFTERROR        1
#define FFERRCODE       256



/* Enable debugging if requested.  */
#if FFDEBUG

# ifndef FFFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define FFFPRINTF fprintf
# endif

# define FFDPRINTF(Args)                        \
do {                                            \
  if (ffdebug)                                  \
    FFFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef FF_LOCATION_PRINT
# define FF_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define FF_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (ffdebug)                                                            \
    {                                                                     \
      FFFPRINTF (stderr, "%s ", Title);                                   \
      ff_symbol_print (stderr,                                            \
                  Type, Value); \
      FFFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on FFOUTPUT.  |
`----------------------------------------*/

static void
ff_symbol_value_print (FILE *ffoutput, int fftype, FFSTYPE const * const ffvaluep)
{
  FILE *ffo = ffoutput;
  FFUSE (ffo);
  if (!ffvaluep)
    return;
# ifdef FFPRINT
  if (fftype < FFNTOKENS)
    FFPRINT (ffoutput, fftoknum[fftype], *ffvaluep);
# endif
  FFUSE (fftype);
}


/*--------------------------------.
| Print this symbol on FFOUTPUT.  |
`--------------------------------*/

static void
ff_symbol_print (FILE *ffoutput, int fftype, FFSTYPE const * const ffvaluep)
{
  FFFPRINTF (ffoutput, "%s %s (",
             fftype < FFNTOKENS ? "token" : "nterm", fftname[fftype]);

  ff_symbol_value_print (ffoutput, fftype, ffvaluep);
  FFFPRINTF (ffoutput, ")");
}

/*------------------------------------------------------------------.
| ff_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
ff_stack_print (fftype_int16 *ffbottom, fftype_int16 *fftop)
{
  FFFPRINTF (stderr, "Stack now");
  for (; ffbottom <= fftop; ffbottom++)
    {
      int ffbot = *ffbottom;
      FFFPRINTF (stderr, " %d", ffbot);
    }
  FFFPRINTF (stderr, "\n");
}

# define FF_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (ffdebug)                                                  \
    ff_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the FFRULE is going to be reduced.  |
`------------------------------------------------*/

static void
ff_reduce_print (fftype_int16 *ffssp, FFSTYPE *ffvsp, int ffrule)
{
  unsigned long int fflno = ffrline[ffrule];
  int ffnrhs = ffr2[ffrule];
  int ffi;
  FFFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             ffrule - 1, fflno);
  /* The symbols being reduced.  */
  for (ffi = 0; ffi < ffnrhs; ffi++)
    {
      FFFPRINTF (stderr, "   $%d = ", ffi + 1);
      ff_symbol_print (stderr,
                       ffstos[ffssp[ffi + 1 - ffnrhs]],
                       &(ffvsp[(ffi + 1) - (ffnrhs)])
                                              );
      FFFPRINTF (stderr, "\n");
    }
}

# define FF_REDUCE_PRINT(Rule)          \
do {                                    \
  if (ffdebug)                          \
    ff_reduce_print (ffssp, ffvsp, Rule); \
} while (0)

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
#ifndef FFINITDEPTH
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
static FFSIZE_T
ffstrlen (const char *ffstr)
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
static char *
ffstpcpy (char *ffdest, const char *ffsrc)
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

/* Copy into *FFMSG, which is of size *FFMSG_ALLOC, an error message
   about the unexpected token FFTOKEN for the state stack whose top is
   FFSSP.

   Return 0 if *FFMSG was successfully written.  Return 1 if *FFMSG is
   not large enough to hold the message.  In that case, also set
   *FFMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
ffsyntax_error (FFSIZE_T *ffmsg_alloc, char **ffmsg,
                fftype_int16 *ffssp, int fftoken)
{
  FFSIZE_T ffsize0 = fftnamerr (FF_NULLPTR, fftname[fftoken]);
  FFSIZE_T ffsize = ffsize0;
  enum { FFERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *ffformat = FF_NULLPTR;
  /* Arguments of ffformat. */
  char const *ffarg[FFERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int ffcount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in ffchar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated ffchar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (fftoken != FFEMPTY)
    {
      int ffn = ffpact[*ffssp];
      ffarg[ffcount++] = fftname[fftoken];
      if (!ffpact_value_is_default (ffn))
        {
          /* Start FFX at -FFN if negative to avoid negative indexes in
             FFCHECK.  In other words, skip the first -FFN actions for
             this state because they are default actions.  */
          int ffxbegin = ffn < 0 ? -ffn : 0;
          /* Stay within bounds of both ffcheck and fftname.  */
          int ffchecklim = FFLAST - ffn + 1;
          int ffxend = ffchecklim < FFNTOKENS ? ffchecklim : FFNTOKENS;
          int ffx;

          for (ffx = ffxbegin; ffx < ffxend; ++ffx)
            if (ffcheck[ffx + ffn] == ffx && ffx != FFTERROR
                && !fftable_value_is_error (fftable[ffx + ffn]))
              {
                if (ffcount == FFERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    ffcount = 1;
                    ffsize = ffsize0;
                    break;
                  }
                ffarg[ffcount++] = fftname[ffx];
                {
                  FFSIZE_T ffsize1 = ffsize + fftnamerr (FF_NULLPTR, fftname[ffx]);
                  if (! (ffsize <= ffsize1
                         && ffsize1 <= FFSTACK_ALLOC_MAXIMUM))
                    return 2;
                  ffsize = ffsize1;
                }
              }
        }
    }

  switch (ffcount)
    {
# define FFCASE_(N, S)                      \
      case N:                               \
        ffformat = S;                       \
      break
      FFCASE_(0, FF_("syntax error"));
      FFCASE_(1, FF_("syntax error, unexpected %s"));
      FFCASE_(2, FF_("syntax error, unexpected %s, expecting %s"));
      FFCASE_(3, FF_("syntax error, unexpected %s, expecting %s or %s"));
      FFCASE_(4, FF_("syntax error, unexpected %s, expecting %s or %s or %s"));
      FFCASE_(5, FF_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef FFCASE_
    }

  {
    FFSIZE_T ffsize1 = ffsize + ffstrlen (ffformat);
    if (! (ffsize <= ffsize1 && ffsize1 <= FFSTACK_ALLOC_MAXIMUM))
      return 2;
    ffsize = ffsize1;
  }

  if (*ffmsg_alloc < ffsize)
    {
      *ffmsg_alloc = 2 * ffsize;
      if (! (ffsize <= *ffmsg_alloc
             && *ffmsg_alloc <= FFSTACK_ALLOC_MAXIMUM))
        *ffmsg_alloc = FFSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *ffp = *ffmsg;
    int ffi = 0;
    while ((*ffp = *ffformat) != '\0')
      if (*ffp == '%' && ffformat[1] == 's' && ffi < ffcount)
        {
          ffp += fftnamerr (ffp, ffarg[ffi++]);
          ffformat += 2;
        }
      else
        {
          ffp++;
          ffformat++;
        }
  }
  return 0;
}
#endif /* FFERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
ffdestruct (const char *ffmsg, int fftype, FFSTYPE *ffvaluep)
{
  FFUSE (ffvaluep);
  if (!ffmsg)
    ffmsg = "Deleting";
  FF_SYMBOL_PRINT (ffmsg, fftype, ffvaluep, fflocationp);

  FF_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  FFUSE (fftype);
  FF_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int ffchar;

/* The semantic value of the lookahead symbol.  */
FFSTYPE fflval;
/* Number of syntax errors so far.  */
int ffnerrs;


/*----------.
| ffparse.  |
`----------*/

int
ffparse (void)
{
    int ffstate;
    /* Number of tokens to shift before error messages enabled.  */
    int fferrstatus;

    /* The stacks and their tools:
       'ffss': related to states.
       'ffvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow ffoverflow
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
  int fftoken = 0;
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

  ffssp = ffss = ffssa;
  ffvsp = ffvs = ffvsa;
  ffstacksize = FFINITDEPTH;

  FFDPRINTF ((stderr, "Starting parse\n"));

  ffstate = 0;
  fferrstatus = 0;
  ffnerrs = 0;
  ffchar = FFEMPTY; /* Cause a token to be read.  */
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
  if (ffpact_value_is_default (ffn))
    goto ffdefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* FFCHAR is either FFEMPTY or FFEOF or a valid lookahead symbol.  */
  if (ffchar == FFEMPTY)
    {
      FFDPRINTF ((stderr, "Reading a token: "));
      ffchar = fflex ();
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
      if (fftable_value_is_error (ffn))
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
  FF_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++ffvsp = fflval;
  FF_IGNORE_MAYBE_UNINITIALIZED_END

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
     '$$ = $1'.

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
#line 257 "eval.y" /* yacc.c:1646  */
    {}
#line 1964 "y.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 259 "eval.y" /* yacc.c:1646  */
    { if( (ffvsp[-1].Node)<0 ) {
		     fferror("Couldn't build node structure: out of memory?");
		     FFERROR;  }
                  gParse.resultNode = (ffvsp[-1].Node);
		}
#line 1974 "y.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 265 "eval.y" /* yacc.c:1646  */
    { if( (ffvsp[-1].Node)<0 ) {
		     fferror("Couldn't build node structure: out of memory?");
		     FFERROR;  }
                  gParse.resultNode = (ffvsp[-1].Node);
		}
#line 1984 "y.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 271 "eval.y" /* yacc.c:1646  */
    { if( (ffvsp[-1].Node)<0 ) {
		     fferror("Couldn't build node structure: out of memory?");
		     FFERROR;  } 
                  gParse.resultNode = (ffvsp[-1].Node);
		}
#line 1994 "y.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 277 "eval.y" /* yacc.c:1646  */
    { if( (ffvsp[-1].Node)<0 ) {
		     fferror("Couldn't build node structure: out of memory?");
		     FFERROR;  }
                  gParse.resultNode = (ffvsp[-1].Node);
		}
#line 2004 "y.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 282 "eval.y" /* yacc.c:1646  */
    {  fferrok;  }
#line 2010 "y.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 286 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Vector( (ffvsp[0].Node) ); TEST((ffval.Node)); }
#line 2016 "y.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 288 "eval.y" /* yacc.c:1646  */
    {
                  if( gParse.Nodes[(ffvsp[-2].Node)].nSubNodes >= MAXSUBS ) {
		     (ffvsp[-2].Node) = Close_Vec( (ffvsp[-2].Node) ); TEST((ffvsp[-2].Node));
		     (ffval.Node) = New_Vector( (ffvsp[-2].Node) ); TEST((ffval.Node));
                  } else {
                     (ffval.Node) = (ffvsp[-2].Node);
                  }
		  gParse.Nodes[(ffval.Node)].SubNodes[ gParse.Nodes[(ffval.Node)].nSubNodes++ ]
		     = (ffvsp[0].Node);
                }
#line 2031 "y.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 301 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Vector( (ffvsp[0].Node) ); TEST((ffval.Node)); }
#line 2037 "y.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 303 "eval.y" /* yacc.c:1646  */
    {
                  if( TYPE((ffvsp[-2].Node)) < TYPE((ffvsp[0].Node)) )
                     TYPE((ffvsp[-2].Node)) = TYPE((ffvsp[0].Node));
                  if( gParse.Nodes[(ffvsp[-2].Node)].nSubNodes >= MAXSUBS ) {
		     (ffvsp[-2].Node) = Close_Vec( (ffvsp[-2].Node) ); TEST((ffvsp[-2].Node));
		     (ffval.Node) = New_Vector( (ffvsp[-2].Node) ); TEST((ffval.Node));
                  } else {
                     (ffval.Node) = (ffvsp[-2].Node);
                  }
		  gParse.Nodes[(ffval.Node)].SubNodes[ gParse.Nodes[(ffval.Node)].nSubNodes++ ]
		     = (ffvsp[0].Node);
                }
#line 2054 "y.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 316 "eval.y" /* yacc.c:1646  */
    {
                  if( gParse.Nodes[(ffvsp[-2].Node)].nSubNodes >= MAXSUBS ) {
		     (ffvsp[-2].Node) = Close_Vec( (ffvsp[-2].Node) ); TEST((ffvsp[-2].Node));
		     (ffval.Node) = New_Vector( (ffvsp[-2].Node) ); TEST((ffval.Node));
                  } else {
                     (ffval.Node) = (ffvsp[-2].Node);
                  }
		  gParse.Nodes[(ffval.Node)].SubNodes[ gParse.Nodes[(ffval.Node)].nSubNodes++ ]
		     = (ffvsp[0].Node);
                }
#line 2069 "y.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 327 "eval.y" /* yacc.c:1646  */
    {
                  TYPE((ffvsp[-2].Node)) = TYPE((ffvsp[0].Node));
                  if( gParse.Nodes[(ffvsp[-2].Node)].nSubNodes >= MAXSUBS ) {
		     (ffvsp[-2].Node) = Close_Vec( (ffvsp[-2].Node) ); TEST((ffvsp[-2].Node));
		     (ffval.Node) = New_Vector( (ffvsp[-2].Node) ); TEST((ffval.Node));
                  } else {
                     (ffval.Node) = (ffvsp[-2].Node);
                  }
		  gParse.Nodes[(ffval.Node)].SubNodes[ gParse.Nodes[(ffval.Node)].nSubNodes++ ]
		     = (ffvsp[0].Node);
                }
#line 2085 "y.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 341 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = Close_Vec( (ffvsp[-1].Node) ); TEST((ffval.Node)); }
#line 2091 "y.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 345 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = Close_Vec( (ffvsp[-1].Node) ); TEST((ffval.Node)); }
#line 2097 "y.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 349 "eval.y" /* yacc.c:1646  */
    {
                  (ffval.Node) = New_Const( BITSTR, (ffvsp[0].str), strlen((ffvsp[0].str))+1 ); TEST((ffval.Node));
		  SIZE((ffval.Node)) = strlen((ffvsp[0].str)); }
#line 2105 "y.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 353 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Column( (ffvsp[0].lng) ); TEST((ffval.Node)); }
#line 2111 "y.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 355 "eval.y" /* yacc.c:1646  */
    {
                  if( TYPE((ffvsp[-1].Node)) != LONG
		      || OPER((ffvsp[-1].Node)) != CONST_OP ) {
		     fferror("Offset argument must be a constant integer");
		     FFERROR;
		  }
                  (ffval.Node) = New_Offset( (ffvsp[-3].lng), (ffvsp[-1].Node) ); TEST((ffval.Node));
                }
#line 2124 "y.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 364 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_BinOp( BITSTR, (ffvsp[-2].Node), '&', (ffvsp[0].Node) ); TEST((ffval.Node));
                  SIZE((ffval.Node)) = ( SIZE((ffvsp[-2].Node))>SIZE((ffvsp[0].Node)) ? SIZE((ffvsp[-2].Node)) : SIZE((ffvsp[0].Node)) );  }
#line 2131 "y.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 367 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_BinOp( BITSTR, (ffvsp[-2].Node), '|', (ffvsp[0].Node) ); TEST((ffval.Node));
                  SIZE((ffval.Node)) = ( SIZE((ffvsp[-2].Node))>SIZE((ffvsp[0].Node)) ? SIZE((ffvsp[-2].Node)) : SIZE((ffvsp[0].Node)) );  }
#line 2138 "y.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 370 "eval.y" /* yacc.c:1646  */
    { 
		  if (SIZE((ffvsp[-2].Node))+SIZE((ffvsp[0].Node)) >= MAX_STRLEN) {
		    fferror("Combined bit string size exceeds " MAX_STRLEN_S " bits");
		    FFERROR;
		  }
		  (ffval.Node) = New_BinOp( BITSTR, (ffvsp[-2].Node), '+', (ffvsp[0].Node) ); TEST((ffval.Node));
                  SIZE((ffval.Node)) = SIZE((ffvsp[-2].Node)) + SIZE((ffvsp[0].Node)); 
		}
#line 2151 "y.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 379 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Deref( (ffvsp[-3].Node), 1, (ffvsp[-1].Node),  0,  0,  0,   0 ); TEST((ffval.Node)); }
#line 2157 "y.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 381 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Deref( (ffvsp[-5].Node), 2, (ffvsp[-3].Node), (ffvsp[-1].Node),  0,  0,   0 ); TEST((ffval.Node)); }
#line 2163 "y.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 383 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Deref( (ffvsp[-7].Node), 3, (ffvsp[-5].Node), (ffvsp[-3].Node), (ffvsp[-1].Node),  0,   0 ); TEST((ffval.Node)); }
#line 2169 "y.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 385 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Deref( (ffvsp[-9].Node), 4, (ffvsp[-7].Node), (ffvsp[-5].Node), (ffvsp[-3].Node), (ffvsp[-1].Node),   0 ); TEST((ffval.Node)); }
#line 2175 "y.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 387 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Deref( (ffvsp[-11].Node), 5, (ffvsp[-9].Node), (ffvsp[-7].Node), (ffvsp[-5].Node), (ffvsp[-3].Node), (ffvsp[-1].Node) ); TEST((ffval.Node)); }
#line 2181 "y.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 389 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Unary( BITSTR, NOT, (ffvsp[0].Node) ); TEST((ffval.Node));     }
#line 2187 "y.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 392 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = (ffvsp[-1].Node); }
#line 2193 "y.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 396 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Const( LONG,   &((ffvsp[0].lng)), sizeof(long)   ); TEST((ffval.Node)); }
#line 2199 "y.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 398 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Const( DOUBLE, &((ffvsp[0].dbl)), sizeof(double) ); TEST((ffval.Node)); }
#line 2205 "y.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 400 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Column( (ffvsp[0].lng) ); TEST((ffval.Node)); }
#line 2211 "y.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 402 "eval.y" /* yacc.c:1646  */
    {
                  if( TYPE((ffvsp[-1].Node)) != LONG
		      || OPER((ffvsp[-1].Node)) != CONST_OP ) {
		     fferror("Offset argument must be a constant integer");
		     FFERROR;
		  }
                  (ffval.Node) = New_Offset( (ffvsp[-3].lng), (ffvsp[-1].Node) ); TEST((ffval.Node));
                }
#line 2224 "y.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 411 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Func( LONG, row_fct,  0, 0, 0, 0, 0, 0, 0, 0 ); }
#line 2230 "y.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 413 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Func( LONG, null_fct, 0, 0, 0, 0, 0, 0, 0, 0 ); }
#line 2236 "y.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 415 "eval.y" /* yacc.c:1646  */
    { PROMOTE((ffvsp[-2].Node),(ffvsp[0].Node)); (ffval.Node) = New_BinOp( TYPE((ffvsp[-2].Node)), (ffvsp[-2].Node), '%', (ffvsp[0].Node) );
		  TEST((ffval.Node));                                                }
#line 2243 "y.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 418 "eval.y" /* yacc.c:1646  */
    { PROMOTE((ffvsp[-2].Node),(ffvsp[0].Node)); (ffval.Node) = New_BinOp( TYPE((ffvsp[-2].Node)), (ffvsp[-2].Node), '+', (ffvsp[0].Node) );
		  TEST((ffval.Node));                                                }
#line 2250 "y.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 421 "eval.y" /* yacc.c:1646  */
    { PROMOTE((ffvsp[-2].Node),(ffvsp[0].Node)); (ffval.Node) = New_BinOp( TYPE((ffvsp[-2].Node)), (ffvsp[-2].Node), '-', (ffvsp[0].Node) ); 
		  TEST((ffval.Node));                                                }
#line 2257 "y.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 424 "eval.y" /* yacc.c:1646  */
    { PROMOTE((ffvsp[-2].Node),(ffvsp[0].Node)); (ffval.Node) = New_BinOp( TYPE((ffvsp[-2].Node)), (ffvsp[-2].Node), '*', (ffvsp[0].Node) ); 
		  TEST((ffval.Node));                                                }
#line 2264 "y.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 427 "eval.y" /* yacc.c:1646  */
    { PROMOTE((ffvsp[-2].Node),(ffvsp[0].Node)); (ffval.Node) = New_BinOp( TYPE((ffvsp[-2].Node)), (ffvsp[-2].Node), '/', (ffvsp[0].Node) ); 
		  TEST((ffval.Node));                                                }
#line 2271 "y.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 430 "eval.y" /* yacc.c:1646  */
    { 
                   if (TYPE((ffvsp[-2].Node)) != LONG ||
		       TYPE((ffvsp[0].Node)) != LONG) {
                     fferror("Bitwise operations with incompatible types; only (bit OP bit) and (int OP int) are allowed");
                      FFERROR;
                   }
                   (ffval.Node) = New_BinOp( TYPE((ffvsp[-2].Node)), (ffvsp[-2].Node), '&', (ffvsp[0].Node) );
                }
#line 2284 "y.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 439 "eval.y" /* yacc.c:1646  */
    { 
                   if (TYPE((ffvsp[-2].Node)) != LONG ||
		       TYPE((ffvsp[0].Node)) != LONG) {
                     fferror("Bitwise operations with incompatible types; only (bit OP bit) and (int OP int) are allowed");
                      FFERROR;
                   }
                   (ffval.Node) = New_BinOp( TYPE((ffvsp[-2].Node)), (ffvsp[-2].Node), '|', (ffvsp[0].Node) );
                }
#line 2297 "y.tab.c" /* yacc.c:1646  */
    break;

  case 44:
#line 448 "eval.y" /* yacc.c:1646  */
    { 
                   if (TYPE((ffvsp[-2].Node)) != LONG ||
		       TYPE((ffvsp[0].Node)) != LONG) {
                     fferror("Bitwise operations with incompatible types; only (bit OP bit) and (int OP int) are allowed");
                      FFERROR;
                   }
                   (ffval.Node) = New_BinOp( TYPE((ffvsp[-2].Node)), (ffvsp[-2].Node), '^', (ffvsp[0].Node) );
                }
#line 2310 "y.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 457 "eval.y" /* yacc.c:1646  */
    { PROMOTE((ffvsp[-2].Node),(ffvsp[0].Node)); (ffval.Node) = New_BinOp( TYPE((ffvsp[-2].Node)), (ffvsp[-2].Node), POWER, (ffvsp[0].Node) );
		  TEST((ffval.Node));                                                }
#line 2317 "y.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 460 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = (ffvsp[0].Node); }
#line 2323 "y.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 462 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Unary( TYPE((ffvsp[0].Node)), UMINUS, (ffvsp[0].Node) ); TEST((ffval.Node)); }
#line 2329 "y.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 464 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = (ffvsp[-1].Node); }
#line 2335 "y.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 466 "eval.y" /* yacc.c:1646  */
    { (ffvsp[0].Node) = New_Unary( TYPE((ffvsp[-2].Node)), 0, (ffvsp[0].Node) );
                  (ffval.Node) = New_BinOp( TYPE((ffvsp[-2].Node)), (ffvsp[-2].Node), '*', (ffvsp[0].Node) ); 
		  TEST((ffval.Node));                                }
#line 2343 "y.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 470 "eval.y" /* yacc.c:1646  */
    { (ffvsp[-2].Node) = New_Unary( TYPE((ffvsp[0].Node)), 0, (ffvsp[-2].Node) );
                  (ffval.Node) = New_BinOp( TYPE((ffvsp[0].Node)), (ffvsp[-2].Node), '*', (ffvsp[0].Node) );
                  TEST((ffval.Node));                                }
#line 2351 "y.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 474 "eval.y" /* yacc.c:1646  */
    {
                  PROMOTE((ffvsp[-2].Node),(ffvsp[0].Node));
                  if( ! Test_Dims((ffvsp[-2].Node),(ffvsp[0].Node)) ) {
                     fferror("Incompatible dimensions in '?:' arguments");
		     FFERROR;
                  }
                  (ffval.Node) = New_Func( 0, ifthenelse_fct, 3, (ffvsp[-2].Node), (ffvsp[0].Node), (ffvsp[-4].Node),
                                 0, 0, 0, 0 );
                  TEST((ffval.Node));
                  if( SIZE((ffvsp[-2].Node))<SIZE((ffvsp[0].Node)) )  Copy_Dims((ffval.Node), (ffvsp[0].Node));
                  TYPE((ffvsp[-4].Node)) = TYPE((ffvsp[-2].Node));
                  if( ! Test_Dims((ffvsp[-4].Node),(ffval.Node)) ) {
                     fferror("Incompatible dimensions in '?:' condition");
		     FFERROR;
                  }
                  TYPE((ffvsp[-4].Node)) = BOOLEAN;
                  if( SIZE((ffval.Node))<SIZE((ffvsp[-4].Node)) )  Copy_Dims((ffval.Node), (ffvsp[-4].Node));
                }
#line 2374 "y.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 493 "eval.y" /* yacc.c:1646  */
    {
                  PROMOTE((ffvsp[-2].Node),(ffvsp[0].Node));
                  if( ! Test_Dims((ffvsp[-2].Node),(ffvsp[0].Node)) ) {
                     fferror("Incompatible dimensions in '?:' arguments");
		     FFERROR;
                  }
                  (ffval.Node) = New_Func( 0, ifthenelse_fct, 3, (ffvsp[-2].Node), (ffvsp[0].Node), (ffvsp[-4].Node),
                                 0, 0, 0, 0 );
                  TEST((ffval.Node));
                  if( SIZE((ffvsp[-2].Node))<SIZE((ffvsp[0].Node)) )  Copy_Dims((ffval.Node), (ffvsp[0].Node));
                  TYPE((ffvsp[-4].Node)) = TYPE((ffvsp[-2].Node));
                  if( ! Test_Dims((ffvsp[-4].Node),(ffval.Node)) ) {
                     fferror("Incompatible dimensions in '?:' condition");
		     FFERROR;
                  }
                  TYPE((ffvsp[-4].Node)) = BOOLEAN;
                  if( SIZE((ffval.Node))<SIZE((ffvsp[-4].Node)) )  Copy_Dims((ffval.Node), (ffvsp[-4].Node));
                }
#line 2397 "y.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 512 "eval.y" /* yacc.c:1646  */
    {
                  PROMOTE((ffvsp[-2].Node),(ffvsp[0].Node));
                  if( ! Test_Dims((ffvsp[-2].Node),(ffvsp[0].Node)) ) {
                     fferror("Incompatible dimensions in '?:' arguments");
		     FFERROR;
                  }
                  (ffval.Node) = New_Func( 0, ifthenelse_fct, 3, (ffvsp[-2].Node), (ffvsp[0].Node), (ffvsp[-4].Node),
                                 0, 0, 0, 0 );
                  TEST((ffval.Node));
                  if( SIZE((ffvsp[-2].Node))<SIZE((ffvsp[0].Node)) )  Copy_Dims((ffval.Node), (ffvsp[0].Node));
                  TYPE((ffvsp[-4].Node)) = TYPE((ffvsp[-2].Node));
                  if( ! Test_Dims((ffvsp[-4].Node),(ffval.Node)) ) {
                     fferror("Incompatible dimensions in '?:' condition");
		     FFERROR;
                  }
                  TYPE((ffvsp[-4].Node)) = BOOLEAN;
                  if( SIZE((ffval.Node))<SIZE((ffvsp[-4].Node)) )  Copy_Dims((ffval.Node), (ffvsp[-4].Node));
                }
#line 2420 "y.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 531 "eval.y" /* yacc.c:1646  */
    { if (FSTRCMP((ffvsp[-1].str),"RANDOM(") == 0) {  /* Scalar RANDOM() */
                     (ffval.Node) = New_Func( DOUBLE, rnd_fct, 0, 0, 0, 0, 0, 0, 0, 0 );
		  } else if (FSTRCMP((ffvsp[-1].str),"RANDOMN(") == 0) {/*Scalar RANDOMN()*/
		     (ffval.Node) = New_Func( DOUBLE, gasrnd_fct, 0, 0, 0, 0, 0, 0, 0, 0 );
                  } else {
                     fferror("Function() not supported");
		     FFERROR;
		  }
                  TEST((ffval.Node)); 
                }
#line 2435 "y.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 542 "eval.y" /* yacc.c:1646  */
    { if (FSTRCMP((ffvsp[-2].str),"SUM(") == 0) {
		     (ffval.Node) = New_Func( LONG, sum_fct, 1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
                  } else if (FSTRCMP((ffvsp[-2].str),"NELEM(") == 0) {
                     (ffval.Node) = New_Const( LONG, &( SIZE((ffvsp[-1].Node)) ), sizeof(long) );
                  } else if (FSTRCMP((ffvsp[-2].str),"ACCUM(") == 0) {
		    long zero = 0;
		    (ffval.Node) = New_BinOp( LONG , (ffvsp[-1].Node), ACCUM, New_Const( LONG, &zero, sizeof(zero) ));
		  } else {
                     fferror("Function(bool) not supported");
		     FFERROR;
		  }
                  TEST((ffval.Node)); 
		}
#line 2453 "y.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 556 "eval.y" /* yacc.c:1646  */
    { if (FSTRCMP((ffvsp[-2].str),"NELEM(") == 0) {
                     (ffval.Node) = New_Const( LONG, &( SIZE((ffvsp[-1].Node)) ), sizeof(long) );
		  } else if (FSTRCMP((ffvsp[-2].str),"NVALID(") == 0) {
		     (ffval.Node) = New_Func( LONG, nonnull_fct, 1, (ffvsp[-1].Node),
				    0, 0, 0, 0, 0, 0 );
		  } else {
                     fferror("Function(str) not supported");
		     FFERROR;
		  }
                  TEST((ffval.Node)); 
		}
#line 2469 "y.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 568 "eval.y" /* yacc.c:1646  */
    { if (FSTRCMP((ffvsp[-2].str),"NELEM(") == 0) {
                     (ffval.Node) = New_Const( LONG, &( SIZE((ffvsp[-1].Node)) ), sizeof(long) );
		} else if (FSTRCMP((ffvsp[-2].str),"NVALID(") == 0) { /* Bit arrays do not have NULL */
                     (ffval.Node) = New_Const( LONG, &( SIZE((ffvsp[-1].Node)) ), sizeof(long) );
		} else if (FSTRCMP((ffvsp[-2].str),"SUM(") == 0) {
		     (ffval.Node) = New_Func( LONG, sum_fct, 1, (ffvsp[-1].Node),
				    0, 0, 0, 0, 0, 0 );
		} else if (FSTRCMP((ffvsp[-2].str),"MIN(") == 0) {
		     (ffval.Node) = New_Func( TYPE((ffvsp[-1].Node)),  /* Force 1D result */
				    min1_fct, 1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
		     /* Note: $2 is a vector so the result can never
		        be a constant.  Therefore it will never be set
		        inside New_Func(), and it is safe to set SIZE() */
		     SIZE((ffval.Node)) = 1;
		} else if (FSTRCMP((ffvsp[-2].str),"ACCUM(") == 0) {
		    long zero = 0;
		    (ffval.Node) = New_BinOp( LONG , (ffvsp[-1].Node), ACCUM, New_Const( LONG, &zero, sizeof(zero) ));
		} else if (FSTRCMP((ffvsp[-2].str),"MAX(") == 0) {
		     (ffval.Node) = New_Func( TYPE((ffvsp[-1].Node)),  /* Force 1D result */
				    max1_fct, 1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
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
#line 2504 "y.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 599 "eval.y" /* yacc.c:1646  */
    { if (FSTRCMP((ffvsp[-2].str),"SUM(") == 0)
		     (ffval.Node) = New_Func( TYPE((ffvsp[-1].Node)), sum_fct, 1, (ffvsp[-1].Node),
				    0, 0, 0, 0, 0, 0 );
		  else if (FSTRCMP((ffvsp[-2].str),"AVERAGE(") == 0)
		     (ffval.Node) = New_Func( DOUBLE, average_fct, 1, (ffvsp[-1].Node),
				    0, 0, 0, 0, 0, 0 );
		  else if (FSTRCMP((ffvsp[-2].str),"STDDEV(") == 0)
		     (ffval.Node) = New_Func( DOUBLE, stddev_fct, 1, (ffvsp[-1].Node),
				    0, 0, 0, 0, 0, 0 );
		  else if (FSTRCMP((ffvsp[-2].str),"MEDIAN(") == 0)
		     (ffval.Node) = New_Func( TYPE((ffvsp[-1].Node)), median_fct, 1, (ffvsp[-1].Node),
				    0, 0, 0, 0, 0, 0 );
		  else if (FSTRCMP((ffvsp[-2].str),"NELEM(") == 0)
                     (ffval.Node) = New_Const( LONG, &( SIZE((ffvsp[-1].Node)) ), sizeof(long) );
		  else if (FSTRCMP((ffvsp[-2].str),"NVALID(") == 0)
		     (ffval.Node) = New_Func( LONG, nonnull_fct, 1, (ffvsp[-1].Node),
				    0, 0, 0, 0, 0, 0 );
		  else if   ((FSTRCMP((ffvsp[-2].str),"ACCUM(") == 0) && (TYPE((ffvsp[-1].Node)) == LONG)) {
		    long zero = 0;
		    (ffval.Node) = New_BinOp( LONG ,   (ffvsp[-1].Node), ACCUM, New_Const( LONG,   &zero, sizeof(zero) ));
		  } else if ((FSTRCMP((ffvsp[-2].str),"ACCUM(") == 0) && (TYPE((ffvsp[-1].Node)) == DOUBLE)) {
		    double zero = 0;
		    (ffval.Node) = New_BinOp( DOUBLE , (ffvsp[-1].Node), ACCUM, New_Const( DOUBLE, &zero, sizeof(zero) ));
		  } else if ((FSTRCMP((ffvsp[-2].str),"SEQDIFF(") == 0) && (TYPE((ffvsp[-1].Node)) == LONG)) {
		    long zero = 0;
		    (ffval.Node) = New_BinOp( LONG ,   (ffvsp[-1].Node), DIFF, New_Const( LONG,   &zero, sizeof(zero) ));
		  } else if ((FSTRCMP((ffvsp[-2].str),"SEQDIFF(") == 0) && (TYPE((ffvsp[-1].Node)) == DOUBLE)) {
		    double zero = 0;
		    (ffval.Node) = New_BinOp( DOUBLE , (ffvsp[-1].Node), DIFF, New_Const( DOUBLE, &zero, sizeof(zero) ));
		  } else if (FSTRCMP((ffvsp[-2].str),"ABS(") == 0)
		     (ffval.Node) = New_Func( 0, abs_fct, 1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
 		  else if (FSTRCMP((ffvsp[-2].str),"MIN(") == 0)
		     (ffval.Node) = New_Func( TYPE((ffvsp[-1].Node)),  /* Force 1D result */
				    min1_fct, 1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
		  else if (FSTRCMP((ffvsp[-2].str),"MAX(") == 0)
		     (ffval.Node) = New_Func( TYPE((ffvsp[-1].Node)),  /* Force 1D result */
				    max1_fct, 1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
		  else if (FSTRCMP((ffvsp[-2].str),"RANDOM(") == 0) { /* Vector RANDOM() */
                     (ffval.Node) = New_Func( 0, rnd_fct, 1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
		     TEST((ffval.Node));
		     TYPE((ffval.Node)) = DOUBLE;
		  } else if (FSTRCMP((ffvsp[-2].str),"RANDOMN(") == 0) {
		     (ffval.Node) = New_Func( 0, gasrnd_fct, 1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
		     TEST((ffval.Node));
		     TYPE((ffval.Node)) = DOUBLE;
                  } 
  		  else {  /*  These all take DOUBLE arguments  */
		     if( TYPE((ffvsp[-1].Node)) != DOUBLE ) (ffvsp[-1].Node) = New_Unary( DOUBLE, 0, (ffvsp[-1].Node) );
                     if (FSTRCMP((ffvsp[-2].str),"SIN(") == 0)
			(ffval.Node) = New_Func( 0, sin_fct,  1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[-2].str),"COS(") == 0)
			(ffval.Node) = New_Func( 0, cos_fct,  1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[-2].str),"TAN(") == 0)
			(ffval.Node) = New_Func( 0, tan_fct,  1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[-2].str),"ARCSIN(") == 0
			      || FSTRCMP((ffvsp[-2].str),"ASIN(") == 0)
			(ffval.Node) = New_Func( 0, asin_fct, 1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[-2].str),"ARCCOS(") == 0
			      || FSTRCMP((ffvsp[-2].str),"ACOS(") == 0)
			(ffval.Node) = New_Func( 0, acos_fct, 1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[-2].str),"ARCTAN(") == 0
			      || FSTRCMP((ffvsp[-2].str),"ATAN(") == 0)
			(ffval.Node) = New_Func( 0, atan_fct, 1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[-2].str),"SINH(") == 0)
			(ffval.Node) = New_Func( 0, sinh_fct,  1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[-2].str),"COSH(") == 0)
			(ffval.Node) = New_Func( 0, cosh_fct,  1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[-2].str),"TANH(") == 0)
			(ffval.Node) = New_Func( 0, tanh_fct,  1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[-2].str),"EXP(") == 0)
			(ffval.Node) = New_Func( 0, exp_fct,  1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[-2].str),"LOG(") == 0)
			(ffval.Node) = New_Func( 0, log_fct,  1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[-2].str),"LOG10(") == 0)
			(ffval.Node) = New_Func( 0, log10_fct, 1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[-2].str),"SQRT(") == 0)
			(ffval.Node) = New_Func( 0, sqrt_fct, 1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[-2].str),"ROUND(") == 0)
			(ffval.Node) = New_Func( 0, round_fct, 1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[-2].str),"FLOOR(") == 0)
			(ffval.Node) = New_Func( 0, floor_fct, 1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[-2].str),"CEIL(") == 0)
			(ffval.Node) = New_Func( 0, ceil_fct, 1, (ffvsp[-1].Node), 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP((ffvsp[-2].str),"RANDOMP(") == 0) {
		       (ffval.Node) = New_Func( 0, poirnd_fct, 1, (ffvsp[-1].Node), 
				      0, 0, 0, 0, 0, 0 );
		       TYPE((ffval.Node)) = LONG;
		     } else {
			fferror("Function(expr) not supported");
			FFERROR;
		     }
		  }
                  TEST((ffval.Node)); 
                }
#line 2603 "y.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 694 "eval.y" /* yacc.c:1646  */
    { 
		  if (FSTRCMP((ffvsp[-4].str),"STRSTR(") == 0) {
		    (ffval.Node) = New_Func( LONG, strpos_fct, 2, (ffvsp[-3].Node), (ffvsp[-1].Node), 0, 
				   0, 0, 0, 0 );
		    TEST((ffval.Node));
		  }
                }
#line 2615 "y.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 702 "eval.y" /* yacc.c:1646  */
    { 
		   if (FSTRCMP((ffvsp[-4].str),"DEFNULL(") == 0) {
		      if( SIZE((ffvsp[-3].Node))>=SIZE((ffvsp[-1].Node)) && Test_Dims( (ffvsp[-3].Node), (ffvsp[-1].Node) ) ) {
			 PROMOTE((ffvsp[-3].Node),(ffvsp[-1].Node));
			 (ffval.Node) = New_Func( 0, defnull_fct, 2, (ffvsp[-3].Node), (ffvsp[-1].Node), 0,
					0, 0, 0, 0 );
			 TEST((ffval.Node)); 
		      } else {
			 fferror("Dimensions of DEFNULL arguments "
				 "are not compatible");
			 FFERROR;
		      }
		   } else if (FSTRCMP((ffvsp[-4].str),"ARCTAN2(") == 0) {
		     if( TYPE((ffvsp[-3].Node)) != DOUBLE ) (ffvsp[-3].Node) = New_Unary( DOUBLE, 0, (ffvsp[-3].Node) );
		     if( TYPE((ffvsp[-1].Node)) != DOUBLE ) (ffvsp[-1].Node) = New_Unary( DOUBLE, 0, (ffvsp[-1].Node) );
		     if( Test_Dims( (ffvsp[-3].Node), (ffvsp[-1].Node) ) ) {
			(ffval.Node) = New_Func( 0, atan2_fct, 2, (ffvsp[-3].Node), (ffvsp[-1].Node), 0, 0, 0, 0, 0 );
			TEST((ffval.Node)); 
			if( SIZE((ffvsp[-3].Node))<SIZE((ffvsp[-1].Node)) ) Copy_Dims((ffval.Node), (ffvsp[-1].Node));
		     } else {
			fferror("Dimensions of arctan2 arguments "
				"are not compatible");
			FFERROR;
		     }
		   } else if (FSTRCMP((ffvsp[-4].str),"MIN(") == 0) {
		      PROMOTE( (ffvsp[-3].Node), (ffvsp[-1].Node) );
		      if( Test_Dims( (ffvsp[-3].Node), (ffvsp[-1].Node) ) ) {
			(ffval.Node) = New_Func( 0, min2_fct, 2, (ffvsp[-3].Node), (ffvsp[-1].Node), 0, 0, 0, 0, 0 );
			TEST((ffval.Node));
			if( SIZE((ffvsp[-3].Node))<SIZE((ffvsp[-1].Node)) ) Copy_Dims((ffval.Node), (ffvsp[-1].Node));
		      } else {
			fferror("Dimensions of min(a,b) arguments "
				"are not compatible");
			FFERROR;
		      }
		   } else if (FSTRCMP((ffvsp[-4].str),"MAX(") == 0) {
		      PROMOTE( (ffvsp[-3].Node), (ffvsp[-1].Node) );
		      if( Test_Dims( (ffvsp[-3].Node), (ffvsp[-1].Node) ) ) {
			(ffval.Node) = New_Func( 0, max2_fct, 2, (ffvsp[-3].Node), (ffvsp[-1].Node), 0, 0, 0, 0, 0 );
			TEST((ffval.Node));
			if( SIZE((ffvsp[-3].Node))<SIZE((ffvsp[-1].Node)) ) Copy_Dims((ffval.Node), (ffvsp[-1].Node));
		      } else {
			fferror("Dimensions of max(a,b) arguments "
				"are not compatible");
			FFERROR;
		      }
		   } else if (FSTRCMP((ffvsp[-4].str),"SETNULL(") == 0) {
		     if (OPER((ffvsp[-3].Node)) != CONST_OP
			 || SIZE((ffvsp[-3].Node)) != 1) {
		       fferror("SETNULL first argument must be a scalar constant");
		       FFERROR;
		     }
		     /* Make sure first arg is same type as second arg */
		     if ( TYPE((ffvsp[-3].Node)) != TYPE((ffvsp[-1].Node)) ) (ffvsp[-3].Node) = New_Unary( TYPE((ffvsp[-1].Node)), 0, (ffvsp[-3].Node) );
		     (ffval.Node) = New_Func( 0, setnull_fct, 2, (ffvsp[-1].Node), (ffvsp[-3].Node), 0, 0, 0, 0, 0 );
		   } else {
		      fferror("Function(expr,expr) not supported");
		      FFERROR;
		   }
                }
#line 2680 "y.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 763 "eval.y" /* yacc.c:1646  */
    { 
		  if (FSTRCMP((ffvsp[-8].str),"ANGSEP(") == 0) {
		    if( TYPE((ffvsp[-7].Node)) != DOUBLE ) (ffvsp[-7].Node) = New_Unary( DOUBLE, 0, (ffvsp[-7].Node) );
		    if( TYPE((ffvsp[-5].Node)) != DOUBLE ) (ffvsp[-5].Node) = New_Unary( DOUBLE, 0, (ffvsp[-5].Node) );
		    if( TYPE((ffvsp[-3].Node)) != DOUBLE ) (ffvsp[-3].Node) = New_Unary( DOUBLE, 0, (ffvsp[-3].Node) );
		    if( TYPE((ffvsp[-1].Node)) != DOUBLE ) (ffvsp[-1].Node) = New_Unary( DOUBLE, 0, (ffvsp[-1].Node) );
		    if( Test_Dims( (ffvsp[-7].Node), (ffvsp[-5].Node) ) && Test_Dims( (ffvsp[-5].Node), (ffvsp[-3].Node) ) && 
			Test_Dims( (ffvsp[-3].Node), (ffvsp[-1].Node) ) ) {
		      (ffval.Node) = New_Func( 0, angsep_fct, 4, (ffvsp[-7].Node), (ffvsp[-5].Node), (ffvsp[-3].Node), (ffvsp[-1].Node),0,0,0 );
		      TEST((ffval.Node)); 
		      if( SIZE((ffvsp[-7].Node))<SIZE((ffvsp[-5].Node)) ) Copy_Dims((ffval.Node), (ffvsp[-5].Node));
		      if( SIZE((ffvsp[-5].Node))<SIZE((ffvsp[-3].Node)) ) Copy_Dims((ffval.Node), (ffvsp[-3].Node));
		      if( SIZE((ffvsp[-3].Node))<SIZE((ffvsp[-1].Node)) ) Copy_Dims((ffval.Node), (ffvsp[-1].Node));
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
#line 2708 "y.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 787 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Deref( (ffvsp[-3].Node), 1, (ffvsp[-1].Node),  0,  0,  0,   0 ); TEST((ffval.Node)); }
#line 2714 "y.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 789 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Deref( (ffvsp[-5].Node), 2, (ffvsp[-3].Node), (ffvsp[-1].Node),  0,  0,   0 ); TEST((ffval.Node)); }
#line 2720 "y.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 791 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Deref( (ffvsp[-7].Node), 3, (ffvsp[-5].Node), (ffvsp[-3].Node), (ffvsp[-1].Node),  0,   0 ); TEST((ffval.Node)); }
#line 2726 "y.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 793 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Deref( (ffvsp[-9].Node), 4, (ffvsp[-7].Node), (ffvsp[-5].Node), (ffvsp[-3].Node), (ffvsp[-1].Node),   0 ); TEST((ffval.Node)); }
#line 2732 "y.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 795 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Deref( (ffvsp[-11].Node), 5, (ffvsp[-9].Node), (ffvsp[-7].Node), (ffvsp[-5].Node), (ffvsp[-3].Node), (ffvsp[-1].Node) ); TEST((ffval.Node)); }
#line 2738 "y.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 797 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Unary( LONG,   INTCAST, (ffvsp[0].Node) );  TEST((ffval.Node));  }
#line 2744 "y.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 799 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Unary( LONG,   INTCAST, (ffvsp[0].Node) );  TEST((ffval.Node));  }
#line 2750 "y.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 801 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Unary( DOUBLE, FLTCAST, (ffvsp[0].Node) );  TEST((ffval.Node));  }
#line 2756 "y.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 803 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Unary( DOUBLE, FLTCAST, (ffvsp[0].Node) );  TEST((ffval.Node));  }
#line 2762 "y.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 807 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Const( BOOLEAN, &((ffvsp[0].log)), sizeof(char) ); TEST((ffval.Node)); }
#line 2768 "y.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 809 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Column( (ffvsp[0].lng) ); TEST((ffval.Node)); }
#line 2774 "y.tab.c" /* yacc.c:1646  */
    break;

  case 73:
#line 811 "eval.y" /* yacc.c:1646  */
    {
                  if( TYPE((ffvsp[-1].Node)) != LONG
		      || OPER((ffvsp[-1].Node)) != CONST_OP ) {
		     fferror("Offset argument must be a constant integer");
		     FFERROR;
		  }
                  (ffval.Node) = New_Offset( (ffvsp[-3].lng), (ffvsp[-1].Node) ); TEST((ffval.Node));
                }
#line 2787 "y.tab.c" /* yacc.c:1646  */
    break;

  case 74:
#line 820 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), EQ,  (ffvsp[0].Node) ); TEST((ffval.Node));
		  SIZE((ffval.Node)) = 1;                                     }
#line 2794 "y.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 823 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), NE,  (ffvsp[0].Node) ); TEST((ffval.Node)); 
		  SIZE((ffval.Node)) = 1;                                     }
#line 2801 "y.tab.c" /* yacc.c:1646  */
    break;

  case 76:
#line 826 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), LT,  (ffvsp[0].Node) ); TEST((ffval.Node)); 
		  SIZE((ffval.Node)) = 1;                                     }
#line 2808 "y.tab.c" /* yacc.c:1646  */
    break;

  case 77:
#line 829 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), LTE, (ffvsp[0].Node) ); TEST((ffval.Node)); 
		  SIZE((ffval.Node)) = 1;                                     }
#line 2815 "y.tab.c" /* yacc.c:1646  */
    break;

  case 78:
#line 832 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), GT,  (ffvsp[0].Node) ); TEST((ffval.Node)); 
		  SIZE((ffval.Node)) = 1;                                     }
#line 2822 "y.tab.c" /* yacc.c:1646  */
    break;

  case 79:
#line 835 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), GTE, (ffvsp[0].Node) ); TEST((ffval.Node)); 
		  SIZE((ffval.Node)) = 1;                                     }
#line 2829 "y.tab.c" /* yacc.c:1646  */
    break;

  case 80:
#line 838 "eval.y" /* yacc.c:1646  */
    { PROMOTE((ffvsp[-2].Node),(ffvsp[0].Node)); (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), GT,  (ffvsp[0].Node) );
                  TEST((ffval.Node));                                               }
#line 2836 "y.tab.c" /* yacc.c:1646  */
    break;

  case 81:
#line 841 "eval.y" /* yacc.c:1646  */
    { PROMOTE((ffvsp[-2].Node),(ffvsp[0].Node)); (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), LT,  (ffvsp[0].Node) );
                  TEST((ffval.Node));                                               }
#line 2843 "y.tab.c" /* yacc.c:1646  */
    break;

  case 82:
#line 844 "eval.y" /* yacc.c:1646  */
    { PROMOTE((ffvsp[-2].Node),(ffvsp[0].Node)); (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), GTE, (ffvsp[0].Node) );
                  TEST((ffval.Node));                                               }
#line 2850 "y.tab.c" /* yacc.c:1646  */
    break;

  case 83:
#line 847 "eval.y" /* yacc.c:1646  */
    { PROMOTE((ffvsp[-2].Node),(ffvsp[0].Node)); (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), LTE, (ffvsp[0].Node) );
                  TEST((ffval.Node));                                               }
#line 2857 "y.tab.c" /* yacc.c:1646  */
    break;

  case 84:
#line 850 "eval.y" /* yacc.c:1646  */
    { PROMOTE((ffvsp[-2].Node),(ffvsp[0].Node)); (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), '~', (ffvsp[0].Node) );
                  TEST((ffval.Node));                                               }
#line 2864 "y.tab.c" /* yacc.c:1646  */
    break;

  case 85:
#line 853 "eval.y" /* yacc.c:1646  */
    { PROMOTE((ffvsp[-2].Node),(ffvsp[0].Node)); (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), EQ,  (ffvsp[0].Node) );
                  TEST((ffval.Node));                                               }
#line 2871 "y.tab.c" /* yacc.c:1646  */
    break;

  case 86:
#line 856 "eval.y" /* yacc.c:1646  */
    { PROMOTE((ffvsp[-2].Node),(ffvsp[0].Node)); (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), NE,  (ffvsp[0].Node) );
                  TEST((ffval.Node));                                               }
#line 2878 "y.tab.c" /* yacc.c:1646  */
    break;

  case 87:
#line 859 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), EQ,  (ffvsp[0].Node) ); TEST((ffval.Node));
                  SIZE((ffval.Node)) = 1; }
#line 2885 "y.tab.c" /* yacc.c:1646  */
    break;

  case 88:
#line 862 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), NE,  (ffvsp[0].Node) ); TEST((ffval.Node));
                  SIZE((ffval.Node)) = 1; }
#line 2892 "y.tab.c" /* yacc.c:1646  */
    break;

  case 89:
#line 865 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), GT,  (ffvsp[0].Node) ); TEST((ffval.Node));
                  SIZE((ffval.Node)) = 1; }
#line 2899 "y.tab.c" /* yacc.c:1646  */
    break;

  case 90:
#line 868 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), GTE, (ffvsp[0].Node) ); TEST((ffval.Node));
                  SIZE((ffval.Node)) = 1; }
#line 2906 "y.tab.c" /* yacc.c:1646  */
    break;

  case 91:
#line 871 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), LT,  (ffvsp[0].Node) ); TEST((ffval.Node));
                  SIZE((ffval.Node)) = 1; }
#line 2913 "y.tab.c" /* yacc.c:1646  */
    break;

  case 92:
#line 874 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), LTE, (ffvsp[0].Node) ); TEST((ffval.Node));
                  SIZE((ffval.Node)) = 1; }
#line 2920 "y.tab.c" /* yacc.c:1646  */
    break;

  case 93:
#line 877 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), AND, (ffvsp[0].Node) ); TEST((ffval.Node)); }
#line 2926 "y.tab.c" /* yacc.c:1646  */
    break;

  case 94:
#line 879 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), OR,  (ffvsp[0].Node) ); TEST((ffval.Node)); }
#line 2932 "y.tab.c" /* yacc.c:1646  */
    break;

  case 95:
#line 881 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), EQ,  (ffvsp[0].Node) ); TEST((ffval.Node)); }
#line 2938 "y.tab.c" /* yacc.c:1646  */
    break;

  case 96:
#line 883 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), NE,  (ffvsp[0].Node) ); TEST((ffval.Node)); }
#line 2944 "y.tab.c" /* yacc.c:1646  */
    break;

  case 97:
#line 886 "eval.y" /* yacc.c:1646  */
    { PROMOTE((ffvsp[-4].Node),(ffvsp[-2].Node)); PROMOTE((ffvsp[-4].Node),(ffvsp[0].Node)); PROMOTE((ffvsp[-2].Node),(ffvsp[0].Node));
		  (ffvsp[-2].Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), LTE, (ffvsp[-4].Node) );
                  (ffvsp[0].Node) = New_BinOp( BOOLEAN, (ffvsp[-4].Node), LTE, (ffvsp[0].Node) );
                  (ffval.Node) = New_BinOp( BOOLEAN, (ffvsp[-2].Node), AND, (ffvsp[0].Node) );
                  TEST((ffval.Node));                                         }
#line 2954 "y.tab.c" /* yacc.c:1646  */
    break;

  case 98:
#line 893 "eval.y" /* yacc.c:1646  */
    {
                  if( ! Test_Dims((ffvsp[-2].Node),(ffvsp[0].Node)) ) {
                     fferror("Incompatible dimensions in '?:' arguments");
		     FFERROR;
                  }
                  (ffval.Node) = New_Func( 0, ifthenelse_fct, 3, (ffvsp[-2].Node), (ffvsp[0].Node), (ffvsp[-4].Node),
                                 0, 0, 0, 0 );
                  TEST((ffval.Node));
                  if( SIZE((ffvsp[-2].Node))<SIZE((ffvsp[0].Node)) )  Copy_Dims((ffval.Node), (ffvsp[0].Node));
                  if( ! Test_Dims((ffvsp[-4].Node),(ffval.Node)) ) {
                     fferror("Incompatible dimensions in '?:' condition");
		     FFERROR;
                  }
                  if( SIZE((ffval.Node))<SIZE((ffvsp[-4].Node)) )  Copy_Dims((ffval.Node), (ffvsp[-4].Node));
                }
#line 2974 "y.tab.c" /* yacc.c:1646  */
    break;

  case 99:
#line 910 "eval.y" /* yacc.c:1646  */
    {
		   if (FSTRCMP((ffvsp[-2].str),"ISNULL(") == 0) {
		      (ffval.Node) = New_Func( 0, isnull_fct, 1, (ffvsp[-1].Node), 0, 0,
				     0, 0, 0, 0 );
		      TEST((ffval.Node)); 
                      /* Use expression's size, but return BOOLEAN */
		      TYPE((ffval.Node)) = BOOLEAN;
		   } else {
		      fferror("Boolean Function(expr) not supported");
		      FFERROR;
		   }
		}
#line 2991 "y.tab.c" /* yacc.c:1646  */
    break;

  case 100:
#line 923 "eval.y" /* yacc.c:1646  */
    {
		   if (FSTRCMP((ffvsp[-2].str),"ISNULL(") == 0) {
		      (ffval.Node) = New_Func( 0, isnull_fct, 1, (ffvsp[-1].Node), 0, 0,
				     0, 0, 0, 0 );
		      TEST((ffval.Node)); 
                      /* Use expression's size, but return BOOLEAN */
		      TYPE((ffval.Node)) = BOOLEAN;
		   } else {
		      fferror("Boolean Function(expr) not supported");
		      FFERROR;
		   }
		}
#line 3008 "y.tab.c" /* yacc.c:1646  */
    break;

  case 101:
#line 936 "eval.y" /* yacc.c:1646  */
    {
		   if (FSTRCMP((ffvsp[-2].str),"ISNULL(") == 0) {
		      (ffval.Node) = New_Func( BOOLEAN, isnull_fct, 1, (ffvsp[-1].Node), 0, 0,
				     0, 0, 0, 0 );
		      TEST((ffval.Node)); 
		   } else {
		      fferror("Boolean Function(expr) not supported");
		      FFERROR;
		   }
		}
#line 3023 "y.tab.c" /* yacc.c:1646  */
    break;

  case 102:
#line 947 "eval.y" /* yacc.c:1646  */
    {
		   if (FSTRCMP((ffvsp[-4].str),"DEFNULL(") == 0) {
		      if( SIZE((ffvsp[-3].Node))>=SIZE((ffvsp[-1].Node)) && Test_Dims( (ffvsp[-3].Node), (ffvsp[-1].Node) ) ) {
			 (ffval.Node) = New_Func( 0, defnull_fct, 2, (ffvsp[-3].Node), (ffvsp[-1].Node), 0,
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
#line 3043 "y.tab.c" /* yacc.c:1646  */
    break;

  case 103:
#line 963 "eval.y" /* yacc.c:1646  */
    {
		   if( TYPE((ffvsp[-5].Node)) != DOUBLE ) (ffvsp[-5].Node) = New_Unary( DOUBLE, 0, (ffvsp[-5].Node) );
		   if( TYPE((ffvsp[-3].Node)) != DOUBLE ) (ffvsp[-3].Node) = New_Unary( DOUBLE, 0, (ffvsp[-3].Node) );
		   if( TYPE((ffvsp[-1].Node)) != DOUBLE ) (ffvsp[-1].Node) = New_Unary( DOUBLE, 0, (ffvsp[-1].Node) );
		   if( ! (Test_Dims( (ffvsp[-5].Node), (ffvsp[-3].Node) ) && Test_Dims( (ffvsp[-3].Node), (ffvsp[-1].Node) ) ) ) {
		       fferror("Dimensions of NEAR arguments "
			       "are not compatible");
		       FFERROR;
		   } else {
		     if (FSTRCMP((ffvsp[-6].str),"NEAR(") == 0) {
		       (ffval.Node) = New_Func( BOOLEAN, near_fct, 3, (ffvsp[-5].Node), (ffvsp[-3].Node), (ffvsp[-1].Node),
				      0, 0, 0, 0 );
		     } else {
		       fferror("Boolean Function not supported");
		       FFERROR;
		     }
		     TEST((ffval.Node)); 

		     if( SIZE((ffval.Node))<SIZE((ffvsp[-5].Node)) )  Copy_Dims((ffval.Node), (ffvsp[-5].Node));
		     if( SIZE((ffvsp[-5].Node))<SIZE((ffvsp[-3].Node)) )  Copy_Dims((ffval.Node), (ffvsp[-3].Node));
		     if( SIZE((ffvsp[-3].Node))<SIZE((ffvsp[-1].Node)) )  Copy_Dims((ffval.Node), (ffvsp[-1].Node));
		   }
		}
#line 3071 "y.tab.c" /* yacc.c:1646  */
    break;

  case 104:
#line 987 "eval.y" /* yacc.c:1646  */
    {
		   if( TYPE((ffvsp[-9].Node)) != DOUBLE ) (ffvsp[-9].Node) = New_Unary( DOUBLE, 0, (ffvsp[-9].Node) );
		   if( TYPE((ffvsp[-7].Node)) != DOUBLE ) (ffvsp[-7].Node) = New_Unary( DOUBLE, 0, (ffvsp[-7].Node) );
		   if( TYPE((ffvsp[-5].Node)) != DOUBLE ) (ffvsp[-5].Node) = New_Unary( DOUBLE, 0, (ffvsp[-5].Node) );
		   if( TYPE((ffvsp[-3].Node)) != DOUBLE ) (ffvsp[-3].Node) = New_Unary( DOUBLE, 0, (ffvsp[-3].Node) );
		   if( TYPE((ffvsp[-1].Node))!= DOUBLE ) (ffvsp[-1].Node)= New_Unary( DOUBLE, 0, (ffvsp[-1].Node));
		   if( ! (Test_Dims( (ffvsp[-9].Node), (ffvsp[-7].Node) ) && Test_Dims( (ffvsp[-7].Node), (ffvsp[-5].Node) ) && 
			  Test_Dims( (ffvsp[-5].Node), (ffvsp[-3].Node) ) && Test_Dims( (ffvsp[-3].Node), (ffvsp[-1].Node) )) ) {
		     fferror("Dimensions of CIRCLE arguments "
			     "are not compatible");
		     FFERROR;
		   } else {
		     if (FSTRCMP((ffvsp[-10].str),"CIRCLE(") == 0) {
		       (ffval.Node) = New_Func( BOOLEAN, circle_fct, 5, (ffvsp[-9].Node), (ffvsp[-7].Node), (ffvsp[-5].Node), (ffvsp[-3].Node),
				      (ffvsp[-1].Node), 0, 0 );
		     } else {
		       fferror("Boolean Function not supported");
		       FFERROR;
		     }
		     TEST((ffval.Node)); 
		     if( SIZE((ffval.Node))<SIZE((ffvsp[-9].Node)) )  Copy_Dims((ffval.Node), (ffvsp[-9].Node));
		     if( SIZE((ffvsp[-9].Node))<SIZE((ffvsp[-7].Node)) )  Copy_Dims((ffval.Node), (ffvsp[-7].Node));
		     if( SIZE((ffvsp[-7].Node))<SIZE((ffvsp[-5].Node)) )  Copy_Dims((ffval.Node), (ffvsp[-5].Node));
		     if( SIZE((ffvsp[-5].Node))<SIZE((ffvsp[-3].Node)) )  Copy_Dims((ffval.Node), (ffvsp[-3].Node));
		     if( SIZE((ffvsp[-3].Node))<SIZE((ffvsp[-1].Node)) ) Copy_Dims((ffval.Node), (ffvsp[-1].Node));
		   }
		}
#line 3103 "y.tab.c" /* yacc.c:1646  */
    break;

  case 105:
#line 1015 "eval.y" /* yacc.c:1646  */
    {
		   if( TYPE((ffvsp[-13].Node)) != DOUBLE ) (ffvsp[-13].Node) = New_Unary( DOUBLE, 0, (ffvsp[-13].Node) );
		   if( TYPE((ffvsp[-11].Node)) != DOUBLE ) (ffvsp[-11].Node) = New_Unary( DOUBLE, 0, (ffvsp[-11].Node) );
		   if( TYPE((ffvsp[-9].Node)) != DOUBLE ) (ffvsp[-9].Node) = New_Unary( DOUBLE, 0, (ffvsp[-9].Node) );
		   if( TYPE((ffvsp[-7].Node)) != DOUBLE ) (ffvsp[-7].Node) = New_Unary( DOUBLE, 0, (ffvsp[-7].Node) );
		   if( TYPE((ffvsp[-5].Node))!= DOUBLE ) (ffvsp[-5].Node)= New_Unary( DOUBLE, 0, (ffvsp[-5].Node));
		   if( TYPE((ffvsp[-3].Node))!= DOUBLE ) (ffvsp[-3].Node)= New_Unary( DOUBLE, 0, (ffvsp[-3].Node));
		   if( TYPE((ffvsp[-1].Node))!= DOUBLE ) (ffvsp[-1].Node)= New_Unary( DOUBLE, 0, (ffvsp[-1].Node));
		   if( ! (Test_Dims( (ffvsp[-13].Node), (ffvsp[-11].Node) ) && Test_Dims( (ffvsp[-11].Node), (ffvsp[-9].Node) ) && 
			  Test_Dims( (ffvsp[-9].Node), (ffvsp[-7].Node) ) && Test_Dims( (ffvsp[-7].Node), (ffvsp[-5].Node) ) &&
			  Test_Dims((ffvsp[-5].Node),(ffvsp[-3].Node) ) && Test_Dims((ffvsp[-3].Node), (ffvsp[-1].Node) ) ) ) {
		     fferror("Dimensions of BOX or ELLIPSE arguments "
			     "are not compatible");
		     FFERROR;
		   } else {
		     if (FSTRCMP((ffvsp[-14].str),"BOX(") == 0) {
		       (ffval.Node) = New_Func( BOOLEAN, box_fct, 7, (ffvsp[-13].Node), (ffvsp[-11].Node), (ffvsp[-9].Node), (ffvsp[-7].Node),
				      (ffvsp[-5].Node), (ffvsp[-3].Node), (ffvsp[-1].Node) );
		     } else if (FSTRCMP((ffvsp[-14].str),"ELLIPSE(") == 0) {
		       (ffval.Node) = New_Func( BOOLEAN, elps_fct, 7, (ffvsp[-13].Node), (ffvsp[-11].Node), (ffvsp[-9].Node), (ffvsp[-7].Node),
				      (ffvsp[-5].Node), (ffvsp[-3].Node), (ffvsp[-1].Node) );
		     } else {
		       fferror("SAO Image Function not supported");
		       FFERROR;
		     }
		     TEST((ffval.Node)); 
		     if( SIZE((ffval.Node))<SIZE((ffvsp[-13].Node)) )  Copy_Dims((ffval.Node), (ffvsp[-13].Node));
		     if( SIZE((ffvsp[-13].Node))<SIZE((ffvsp[-11].Node)) )  Copy_Dims((ffval.Node), (ffvsp[-11].Node));
		     if( SIZE((ffvsp[-11].Node))<SIZE((ffvsp[-9].Node)) )  Copy_Dims((ffval.Node), (ffvsp[-9].Node));
		     if( SIZE((ffvsp[-9].Node))<SIZE((ffvsp[-7].Node)) )  Copy_Dims((ffval.Node), (ffvsp[-7].Node));
		     if( SIZE((ffvsp[-7].Node))<SIZE((ffvsp[-5].Node)) ) Copy_Dims((ffval.Node), (ffvsp[-5].Node));
		     if( SIZE((ffvsp[-5].Node))<SIZE((ffvsp[-3].Node)) ) Copy_Dims((ffval.Node), (ffvsp[-3].Node));
		     if( SIZE((ffvsp[-3].Node))<SIZE((ffvsp[-1].Node)) ) Copy_Dims((ffval.Node), (ffvsp[-1].Node));
		   }
		}
#line 3143 "y.tab.c" /* yacc.c:1646  */
    break;

  case 106:
#line 1052 "eval.y" /* yacc.c:1646  */
    { /* Use defaults for all elements */
		   (ffval.Node) = New_GTI(gtifilt_fct,  "", -99, -99, "*START*", "*STOP*" );
                   TEST((ffval.Node));                                        }
#line 3151 "y.tab.c" /* yacc.c:1646  */
    break;

  case 107:
#line 1056 "eval.y" /* yacc.c:1646  */
    { /* Use defaults for all except filename */
		  (ffval.Node) = New_GTI(gtifilt_fct,  (ffvsp[-1].str), -99, -99, "*START*", "*STOP*" );
                   TEST((ffval.Node));                                        }
#line 3159 "y.tab.c" /* yacc.c:1646  */
    break;

  case 108:
#line 1060 "eval.y" /* yacc.c:1646  */
    {  (ffval.Node) = New_GTI(gtifilt_fct,  (ffvsp[-3].str), (ffvsp[-1].Node), -99, "*START*", "*STOP*" );
                   TEST((ffval.Node));                                        }
#line 3166 "y.tab.c" /* yacc.c:1646  */
    break;

  case 109:
#line 1063 "eval.y" /* yacc.c:1646  */
    {  (ffval.Node) = New_GTI(gtifilt_fct,  (ffvsp[-7].str), (ffvsp[-5].Node), -99, (ffvsp[-3].str), (ffvsp[-1].str) );
                   TEST((ffval.Node));                                        }
#line 3173 "y.tab.c" /* yacc.c:1646  */
    break;

  case 110:
#line 1068 "eval.y" /* yacc.c:1646  */
    {  (ffval.Node) = New_GTI(gtiover_fct,  (ffvsp[-5].str), (ffvsp[-3].Node), (ffvsp[-1].Node), "*START*", "*STOP*");
                   TEST((ffval.Node));                                        }
#line 3180 "y.tab.c" /* yacc.c:1646  */
    break;

  case 111:
#line 1071 "eval.y" /* yacc.c:1646  */
    {  (ffval.Node) = New_GTI(gtiover_fct,  (ffvsp[-9].str), (ffvsp[-7].Node), (ffvsp[-5].Node), (ffvsp[-3].str), (ffvsp[-1].str) );
                   TEST((ffval.Node));                                        }
#line 3187 "y.tab.c" /* yacc.c:1646  */
    break;

  case 112:
#line 1076 "eval.y" /* yacc.c:1646  */
    { /* Use defaults for all elements */
		   (ffval.Node) = New_GTI(gtifind_fct,  "", -99, -99, "*START*", "*STOP*" );
                   TEST((ffval.Node));                                        }
#line 3195 "y.tab.c" /* yacc.c:1646  */
    break;

  case 113:
#line 1080 "eval.y" /* yacc.c:1646  */
    { /* Use defaults for all except filename */
		  (ffval.Node) = New_GTI(gtifind_fct,  (ffvsp[-1].str), -99, -99, "*START*", "*STOP*" );
                   TEST((ffval.Node));                                        }
#line 3203 "y.tab.c" /* yacc.c:1646  */
    break;

  case 114:
#line 1084 "eval.y" /* yacc.c:1646  */
    {  (ffval.Node) = New_GTI(gtifind_fct,  (ffvsp[-3].str), (ffvsp[-1].Node), -99, "*START*", "*STOP*" );
                   TEST((ffval.Node));                                        }
#line 3210 "y.tab.c" /* yacc.c:1646  */
    break;

  case 115:
#line 1087 "eval.y" /* yacc.c:1646  */
    {  (ffval.Node) = New_GTI(gtifind_fct,  (ffvsp[-7].str), (ffvsp[-5].Node), -99, (ffvsp[-3].str), (ffvsp[-1].str) );
                   TEST((ffval.Node));                                        }
#line 3217 "y.tab.c" /* yacc.c:1646  */
    break;

  case 116:
#line 1092 "eval.y" /* yacc.c:1646  */
    { /* Use defaults for all except filename */
                   (ffval.Node) = New_REG( (ffvsp[-1].str), -99, -99, "" );
                   TEST((ffval.Node));                                        }
#line 3225 "y.tab.c" /* yacc.c:1646  */
    break;

  case 117:
#line 1096 "eval.y" /* yacc.c:1646  */
    {  (ffval.Node) = New_REG( (ffvsp[-5].str), (ffvsp[-3].Node), (ffvsp[-1].Node), "" );
                   TEST((ffval.Node));                                        }
#line 3232 "y.tab.c" /* yacc.c:1646  */
    break;

  case 118:
#line 1099 "eval.y" /* yacc.c:1646  */
    {  (ffval.Node) = New_REG( (ffvsp[-7].str), (ffvsp[-5].Node), (ffvsp[-3].Node), (ffvsp[-1].str) );
                   TEST((ffval.Node));                                        }
#line 3239 "y.tab.c" /* yacc.c:1646  */
    break;

  case 119:
#line 1103 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Deref( (ffvsp[-3].Node), 1, (ffvsp[-1].Node),  0,  0,  0,   0 ); TEST((ffval.Node)); }
#line 3245 "y.tab.c" /* yacc.c:1646  */
    break;

  case 120:
#line 1105 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Deref( (ffvsp[-5].Node), 2, (ffvsp[-3].Node), (ffvsp[-1].Node),  0,  0,   0 ); TEST((ffval.Node)); }
#line 3251 "y.tab.c" /* yacc.c:1646  */
    break;

  case 121:
#line 1107 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Deref( (ffvsp[-7].Node), 3, (ffvsp[-5].Node), (ffvsp[-3].Node), (ffvsp[-1].Node),  0,   0 ); TEST((ffval.Node)); }
#line 3257 "y.tab.c" /* yacc.c:1646  */
    break;

  case 122:
#line 1109 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Deref( (ffvsp[-9].Node), 4, (ffvsp[-7].Node), (ffvsp[-5].Node), (ffvsp[-3].Node), (ffvsp[-1].Node),   0 ); TEST((ffval.Node)); }
#line 3263 "y.tab.c" /* yacc.c:1646  */
    break;

  case 123:
#line 1111 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Deref( (ffvsp[-11].Node), 5, (ffvsp[-9].Node), (ffvsp[-7].Node), (ffvsp[-5].Node), (ffvsp[-3].Node), (ffvsp[-1].Node) ); TEST((ffval.Node)); }
#line 3269 "y.tab.c" /* yacc.c:1646  */
    break;

  case 124:
#line 1113 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Unary( BOOLEAN, NOT, (ffvsp[0].Node) ); TEST((ffval.Node)); }
#line 3275 "y.tab.c" /* yacc.c:1646  */
    break;

  case 125:
#line 1115 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = (ffvsp[-1].Node); }
#line 3281 "y.tab.c" /* yacc.c:1646  */
    break;

  case 126:
#line 1119 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Const( STRING, (ffvsp[0].str), strlen((ffvsp[0].str))+1 ); TEST((ffval.Node));
                  SIZE((ffval.Node)) = strlen((ffvsp[0].str)); }
#line 3288 "y.tab.c" /* yacc.c:1646  */
    break;

  case 127:
#line 1122 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Column( (ffvsp[0].lng) ); TEST((ffval.Node)); }
#line 3294 "y.tab.c" /* yacc.c:1646  */
    break;

  case 128:
#line 1124 "eval.y" /* yacc.c:1646  */
    {
                  if( TYPE((ffvsp[-1].Node)) != LONG
		      || OPER((ffvsp[-1].Node)) != CONST_OP ) {
		     fferror("Offset argument must be a constant integer");
		     FFERROR;
		  }
                  (ffval.Node) = New_Offset( (ffvsp[-3].lng), (ffvsp[-1].Node) ); TEST((ffval.Node));
                }
#line 3307 "y.tab.c" /* yacc.c:1646  */
    break;

  case 129:
#line 1133 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = New_Func( STRING, null_fct, 0, 0, 0, 0, 0, 0, 0, 0 ); }
#line 3313 "y.tab.c" /* yacc.c:1646  */
    break;

  case 130:
#line 1135 "eval.y" /* yacc.c:1646  */
    { (ffval.Node) = (ffvsp[-1].Node); }
#line 3319 "y.tab.c" /* yacc.c:1646  */
    break;

  case 131:
#line 1137 "eval.y" /* yacc.c:1646  */
    { 
		  if (SIZE((ffvsp[-2].Node))+SIZE((ffvsp[0].Node)) >= MAX_STRLEN) {
		    fferror("Combined string size exceeds " MAX_STRLEN_S " characters");
		    FFERROR;
		  }
		  (ffval.Node) = New_BinOp( STRING, (ffvsp[-2].Node), '+', (ffvsp[0].Node) );  TEST((ffval.Node));
		  SIZE((ffval.Node)) = SIZE((ffvsp[-2].Node)) + SIZE((ffvsp[0].Node));
		}
#line 3332 "y.tab.c" /* yacc.c:1646  */
    break;

  case 132:
#line 1146 "eval.y" /* yacc.c:1646  */
    {
		  int outSize;
                  if( SIZE((ffvsp[-4].Node))!=1 ) {
                     fferror("Cannot have a vector string column");
		     FFERROR;
                  }
		  /* Since the output can be calculated now, as a constant
		     scalar, we must precalculate the output size, in
		     order to avoid an overflow. */
		  outSize = SIZE((ffvsp[-2].Node));
		  if (SIZE((ffvsp[0].Node)) > outSize) outSize = SIZE((ffvsp[0].Node));
                  (ffval.Node) = New_FuncSize( 0, ifthenelse_fct, 3, (ffvsp[-2].Node), (ffvsp[0].Node), (ffvsp[-4].Node),
				     0, 0, 0, 0, outSize);
		  
                  TEST((ffval.Node));
                  if( SIZE((ffvsp[-2].Node))<SIZE((ffvsp[0].Node)) )  Copy_Dims((ffval.Node), (ffvsp[0].Node));
                }
#line 3354 "y.tab.c" /* yacc.c:1646  */
    break;

  case 133:
#line 1165 "eval.y" /* yacc.c:1646  */
    { 
		  if (FSTRCMP((ffvsp[-4].str),"DEFNULL(") == 0) {
		     int outSize;
		     /* Since the output can be calculated now, as a constant
			scalar, we must precalculate the output size, in
			order to avoid an overflow. */
		     outSize = SIZE((ffvsp[-3].Node));
		     if (SIZE((ffvsp[-1].Node)) > outSize) outSize = SIZE((ffvsp[-1].Node));
		     
		     (ffval.Node) = New_FuncSize( 0, defnull_fct, 2, (ffvsp[-3].Node), (ffvsp[-1].Node), 0,
					0, 0, 0, 0, outSize );
		     TEST((ffval.Node)); 
		     if( SIZE((ffvsp[-1].Node))>SIZE((ffvsp[-3].Node)) ) SIZE((ffval.Node)) = SIZE((ffvsp[-1].Node));
		  } else {
		     fferror("Function(string,string) not supported");
		     FFERROR;
		  }
		}
#line 3377 "y.tab.c" /* yacc.c:1646  */
    break;

  case 134:
#line 1184 "eval.y" /* yacc.c:1646  */
    { 
		  if (FSTRCMP((ffvsp[-6].str),"STRMID(") == 0) {
		    int len;
		    if( TYPE((ffvsp[-3].Node)) != LONG || SIZE((ffvsp[-3].Node)) != 1 ||
			TYPE((ffvsp[-1].Node)) != LONG || SIZE((ffvsp[-1].Node)) != 1) {
		      fferror("When using STRMID(S,P,N), P and N must be integers (and not vector columns)");
		      FFERROR;
		    }
		    if (OPER((ffvsp[-1].Node)) == CONST_OP) {
		      /* Constant value: use that directly */
		      len = (gParse.Nodes[(ffvsp[-1].Node)].value.data.lng);
		    } else {
		      /* Variable value: use the maximum possible (from $2) */
		      len = SIZE((ffvsp[-5].Node));
		    }
		    if (len <= 0 || len >= MAX_STRLEN) {
		      fferror("STRMID(S,P,N), N must be 1-" MAX_STRLEN_S);
		      FFERROR;
		    }
		    (ffval.Node) = New_FuncSize( 0, strmid_fct, 3, (ffvsp[-5].Node), (ffvsp[-3].Node),(ffvsp[-1].Node),0,0,0,0,len);
		    TEST((ffval.Node));
		  } else {
		     fferror("Function(string,expr,expr) not supported");
		     FFERROR;
		  }
		}
#line 3408 "y.tab.c" /* yacc.c:1646  */
    break;


#line 3412 "y.tab.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter ffchar, and that requires
     that fftoken be updated with the new translation.  We take the
     approach of translating immediately before every use of fftoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     FFABORT, FFACCEPT, or FFERROR immediately after altering ffchar or
     if it invokes FFBACKUP.  In the case of FFABORT or FFACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of FFERROR or FFBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  FF_SYMBOL_PRINT ("-> $$ =", ffr1[ffn], &ffval, &ffloc);

  FFPOPSTACK (fflen);
  fflen = 0;
  FF_STACK_PRINT (ffss, ffssp);

  *++ffvsp = ffval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  ffn = ffr1[ffn];

  ffstate = ffpgoto[ffn - FFNTOKENS] + *ffssp;
  if (0 <= ffstate && ffstate <= FFLAST && ffcheck[ffstate] == *ffssp)
    ffstate = fftable[ffstate];
  else
    ffstate = ffdefgoto[ffn - FFNTOKENS];

  goto ffnewstate;


/*--------------------------------------.
| fferrlab -- here on detecting error.  |
`--------------------------------------*/
fferrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  fftoken = ffchar == FFEMPTY ? FFEMPTY : FFTRANSLATE (ffchar);

  /* If not already recovering from an error, report this error.  */
  if (!fferrstatus)
    {
      ++ffnerrs;
#if ! FFERROR_VERBOSE
      fferror (FF_("syntax error"));
#else
# define FFSYNTAX_ERROR ffsyntax_error (&ffmsg_alloc, &ffmsg, \
                                        ffssp, fftoken)
      {
        char const *ffmsgp = FF_("syntax error");
        int ffsyntax_error_status;
        ffsyntax_error_status = FFSYNTAX_ERROR;
        if (ffsyntax_error_status == 0)
          ffmsgp = ffmsg;
        else if (ffsyntax_error_status == 1)
          {
            if (ffmsg != ffmsgbuf)
              FFSTACK_FREE (ffmsg);
            ffmsg = (char *) FFSTACK_ALLOC (ffmsg_alloc);
            if (!ffmsg)
              {
                ffmsg = ffmsgbuf;
                ffmsg_alloc = sizeof ffmsgbuf;
                ffsyntax_error_status = 2;
              }
            else
              {
                ffsyntax_error_status = FFSYNTAX_ERROR;
                ffmsgp = ffmsg;
              }
          }
        fferror (ffmsgp);
        if (ffsyntax_error_status == 2)
          goto ffexhaustedlab;
      }
# undef FFSYNTAX_ERROR
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

  /* Do not reclaim the symbols of the rule whose action triggered
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
  fferrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      ffn = ffpact[ffstate];
      if (!ffpact_value_is_default (ffn))
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

  FF_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++ffvsp = fflval;
  FF_IGNORE_MAYBE_UNINITIALIZED_END


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

#if !defined ffoverflow || FFERROR_VERBOSE
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
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      fftoken = FFTRANSLATE (ffchar);
      ffdestruct ("Cleanup: discarding lookahead",
                  fftoken, &fflval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
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
  return ffresult;
}
#line 1213 "eval.y" /* yacc.c:1906  */


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

static int New_GTI( funcOp Op, char *fname, int Node1, int Node2, char *start, char *stop )
{
   fitsfile *fptr;
   Node *this, *that0, *that1, *that2;
   int  type,i,n, startCol, stopCol, Node0;
   int  hdutype, hdunum, evthdu, samefile, extvers, movetotype, tstat;
   char extname[100];
   long nrows;
   double timeZeroI[2], timeZeroF[2], dt, timeSpan;
   char xcol[20], xexpr[20];
   FFSTYPE colVal;

   if( (Op == gtifilt_fct || Op == gtifind_fct) && Node1==-99 ) {
      type = ffGetVariable( "TIME", &colVal );
      if( type==COLUMN ) {
	 Node1 = New_Column( (int)colVal.lng );
      } else {
	 fferror("Could not build TIME column for GTIFILTER/GTIFIND");
	 return(-1);
      }
   }

   if (Op == gtiover_fct) {
     if (Node1 == -99 || Node2 == -99) {
       fferror("startExpr and stopExpr values must be defined for GTIOVERLAP");
       return(-1);
     }
     /* Also case TIME_STOP to double precision */
     Node2 = New_Unary( DOUBLE, 0, Node2 );
     if (Node2 < 0) return(-1);

   }

   /* Type cast TIME to double precision */
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
	 fferror("Cannot use primary array for GTI filter / GTIFIND");
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
      this->SubNodes[1]    = Node1;
      this->operation      = (int) Op;
      if (Op == gtifilt_fct) {
	this->nSubNodes      = 2;
	this->DoOp           = Do_GTI;
	this->type           = BOOLEAN;
      } else if (Op == gtifind_fct) {
	this->nSubNodes      = 2;
	this->DoOp           = Do_GTI;
	this->type           = LONG;
      } else {
	this->nSubNodes      = 3;
	this->DoOp           = Do_GTI_Over;
	this->type           = DOUBLE;
      }
      that1                = gParse.Nodes + Node1;
      this->value.nelem    = that1->value.nelem;
      this->value.naxis    = that1->value.naxis;
      for( i=0; i < that1->value.naxis; i++ )
	 this->value.naxes[i] = that1->value.naxes[i];
      if (Op == gtiover_fct) {
	this->SubNodes[2]  = Node2;
	that2 = gParse.Nodes + Node2;
	if (that1->value.nelem != that2->value.nelem) {
	  fferror("Dimensions of TIME and TIME_STOP must match for GTIOVERLAP");
	  return(-1);
	}
      }

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

	 /* GTIOVERLAP() requires ordered GTI */
	 if (that0->type != 1 && Op == gtiover_fct) {
	   fferror("Input GTI must be time-ordered for GTIOVERLAP");
	   return(-1);
	 }
	 
	 /*  Handle TIMEZERO offset, if any  */
	 
	 dt = (timeZeroI[1] - timeZeroI[0]) + (timeZeroF[1] - timeZeroF[0]);
	 timeSpan = that0->value.data.dblptr[nrows+nrows-1]
	    - that0->value.data.dblptr[0];
	 if (timeSpan == 0) timeSpan = 1.0;
	 
	 if( fabs( dt / timeSpan ) > 1e-12 ) {
	    for( i=0; i<(nrows+nrows); i++ )
	       that0->value.data.dblptr[i] += dt;
	 }
      }
      /* If Node1 is constant (gtifilt_fct) or
	 Node1 and Node2 are constant (gtiover_fct), then evaluate now */
      if( OPER(Node1)==CONST_OP && (Op == gtifilt_fct || OPER(Node2)==CONST_OP)) {
	this->DoOp( this );
      }
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
   int dorow = (this->operation == gtifind_fct);

   theTimes = gParse.Nodes + this->SubNodes[0];
   theExpr  = gParse.Nodes + this->SubNodes[1];

   nGTI    = theTimes->value.nelem;
   start   = theTimes->value.data.dblptr;
   stop    = theTimes->value.data.dblptr + nGTI;
   ordered = theTimes->type;

   if( theExpr->operation==CONST_OP ) {
      gti = Search_GTI( theExpr->value.data.dbl, nGTI, start, stop, ordered, 0 );
      if (dorow) {
	this->value.data.lng = (gti >= 0) ? (gti+1) : -1;
      } else {
	this->value.data.log = (gti>=0);
      }
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
		 gti = Search_GTI( times[elem], nGTI, start, stop, ordered, 0 );
	       }
	       if (dorow) {
		 this->value.data.lngptr[elem] = ( gti >= 0 ) ? (gti + 1) : (-1);
		 this->value.undef[elem]  = ( gti >= 0 ) ? 0 : 1;
	       } else {
		 this->value.data.logptr[elem] = ( gti>=0 );
	       }
	    }
	 } else { /* nGTI == 0 */

	   if (dorow) { /* no good times so all values are undef */
	     while( elem-- ) {
	       this->value.undef[elem]       = 1;
	     }
	   } else {    /* no good times so all logicals are 0 */
	     while( elem-- ) {
	       this->value.data.logptr[elem] = 0;
	       this->value.undef[elem]       = 0;
	     }
	   }
	   
	 }
      }
   }

   if( theExpr->operation>0 )
      free( theExpr->value.data.ptr );
}

static void Do_GTI_Over( Node *this )
{
   Node *theTimes, *theStart, *theStop;
   double *gtiStart, *gtiStop;
   double *evtStart, *evtStop;
   long elem, nGTI, gti, nextGTI;
   int ordered;

   theTimes = gParse.Nodes + this->SubNodes[0]; /* GTI times */
   theStop  = gParse.Nodes + this->SubNodes[2]; /* User start time */
   theStart = gParse.Nodes + this->SubNodes[1]; /* User stop time */

   nGTI     = theTimes->value.nelem;
   gtiStart = theTimes->value.data.dblptr;        /* GTI start */
   gtiStop  = theTimes->value.data.dblptr + nGTI; /* GTI stop */

   if( theStart->operation==CONST_OP && theStop->operation==CONST_OP) {

      this->value.data.dbl = 
	(GTI_Over( theStart->value.data.dbl, theStop->value.data.dbl,
		   nGTI, gtiStart, gtiStop, &gti));
      this->operation      = CONST_OP;

   } else {
      char undefStart = 0, undefStop = 0; /* Input values are undef? */
      double uStart, uStop;       /* User start/stop values */
      if (theStart->operation==CONST_OP) uStart = theStart->value.data.dbl;
      if (theStop ->operation==CONST_OP) uStop  = theStop ->value.data.dbl;

      Allocate_Ptrs( this );

      evtStart = theStart->value.data.dblptr;
      evtStop  = theStop ->value.data.dblptr;
      if( !gParse.status ) {

	 elem = gParse.nRows * this->value.nelem;
	 if( nGTI ) {
	    double toverlap = 0.0;
	    gti = -1;
	    while( elem-- ) {
	      if (theStart->operation!=CONST_OP) {
		undefStart = theStart->value.undef[elem];
		uStart     = evtStart[elem];
	      }
	      if (theStop->operation!=CONST_OP) {
		undefStop  = theStop ->value.undef[elem];
		uStop      = evtStop[elem];
	      }
	      /* This works because at least one of the values is not const */
	      if( (this->value.undef[elem] = (undefStart||undefStop)) )
		  continue;

            /*  Before searching entire GTI, check the GTI found last time  */
	       if( gti<0 || 
		   uStart<gtiStart[gti] || uStart>gtiStop[gti] ||
		   uStop <gtiStart[gti] || uStop >gtiStop[gti]) {
		 /* Nope, need to recalculate */
		 toverlap = GTI_Over(uStart, uStop, 
				     nGTI, gtiStart, gtiStop, 
				     &gti);
	       } else {
		 /* We are in same GTI, the overlap is just stop-start of user range */
		 toverlap = (uStop-uStart);
	       }

	       /* This works because at least one of the values is not const */
	       this->value.data.dblptr[elem] = toverlap;
	    }
	 } else
	    /* nGTI == 0; there is no overlap so set all values to 0.0 */
	    while( elem-- ) {
	       this->value.data.dblptr[elem] = 0.0;
	       this->value.undef[elem]       = 0;
	    }
      }
   }

   if( theStart->operation>0 ) {
     free( theStart->value.data.ptr );
   }
   if( theStop->operation>0 ) {
     free( theStop->value.data.ptr );
   }
}

static double GTI_Over(double evtStart, double evtStop,
		       long nGTI, double *start, double *stop,
		       long *gtiout)
{
  long gti1, gti2, nextGTI1, nextGTI2;
  long gti, nMax;
  double overlap = 0.0;

  *gtiout = -1L;
  /* Zero or negative bin size */
  if (evtStop <= evtStart) return 0.0;

  /* Locate adjacent GTIs for evtStart and evtStop */
  gti1 = Search_GTI(evtStart, nGTI, start, stop, 1, &nextGTI1);
  gti2 = Search_GTI(evtStop,  nGTI, start, stop, 1, &nextGTI2);

  /* evtStart is in gti1, we return that for future processing */
  if (gti1 >= 0) *gtiout = gti1;

  /* Both evtStart/evtStop are beyond the last GTI */
  if (nextGTI1 < 0 && nextGTI2 < 0) return 0.0;

  /* Both evtStart/evtStop are in the same gap between GTIs */
  if (gti1 < 0 && gti2 < 0 && nextGTI1 == nextGTI2) return 0.0;

  /* Both evtStart/evtStop are in the same GTI */
  if (gti1 >= 0 && gti1 == gti2) return (evtStop-evtStart);

  /* Count through the remaining GTIs; there will be at least one */
  /* The largest GTI to consider is either nextGTI2-1, if it exists,
     or nGTI-1 */
  if (nextGTI2 < 0) nMax = nGTI-1;
  else if (gti2 >= 0) nMax = nextGTI2;
  else nMax = nextGTI2-1;
  for (gti = nextGTI1; gti <= nMax; gti++) {
    double starti = start[gti], stopi = stop[gti];
    /* Trim the GTI by actual evtStart/Stop times */
    if (evtStart > starti) starti = evtStart;
    if (evtStop  < stopi ) stopi  = evtStop;
    overlap += (stopi - starti);
  }
    
  return overlap;
}

/*
 * Search_GTI - search GTI for requested evtTime
 * 
 * double evtTime - requested event time
 * long nGTI - number of entries in start[] and stop[]
 * double start[], stop[] - start and stop of each GTI
 * int ordered - set to 1 if time-ordered
 * long *nextGTI0 - upon return, *nextGTI0 is either
 *                   the GTI evtTime is inside
 *                   the next GTI if evtTime is not inside
 *                   -1L if there is no next GTI
 *                   not set if nextGTI0 is a null pointer
 *
 * NOTE: for *nextGTI to be well-defined, the GTI must
 *   be ordered.  This is true when called by Do_GTI.
 *
 * RETURNS: gti index that evtTime is located inside, or -1L
 */
static long Search_GTI( double evtTime, long nGTI, double *start,
			double *stop, int ordered, long *nextGTI0 )
{
   long gti, nextGTI = -1L, step;
                             
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
		  nextGTI = gti+1;
		  gti = -1L;
		  break;
	       }
	    } else if( evtTime<start[gti] ) {
	       if( evtTime<=stop[gti-1] )
		  gti -= step;
	       else {
		  nextGTI = gti;
		  gti = -1L;
		  break;
	       }
	    } else {
	       nextGTI = gti;
	       break;
	    }
	 }
      } else {
	 if (start[0] > evtTime) nextGTI = 0;
	 gti = -1L;
      }
      
   } else { /*  Use "SLOW" linear search.  Not required to be 
	        ordered, so we have to search the whole table
		no matter what.
	    */
      gti = nGTI;
      while( gti-- ) {
	if( stop[gti] >= evtTime ) nextGTI = gti;
	if( evtTime>=start[gti] && evtTime<=stop[gti] )
	    break;
      }
   }

   if (nextGTI >= nGTI) nextGTI = -1;
   if (nextGTI0) *nextGTI0 = nextGTI;

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
