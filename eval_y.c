
/*  A Bison parser, made from eval.y
 by  GNU Bison version 1.25
  */

#define FFBISON 1  /* Identify Bison output.  */

#define	BOOLEAN	258
#define	LONG	259
#define	DOUBLE	260
#define	STRING	261
#define	BITSTR	262
#define	FUNCTION	263
#define	BFUNCTION	264
#define	FILTER	265
#define	COLUMN	266
#define	BCOLUMN	267
#define	SCOLUMN	268
#define	BITCOL	269
#define	ROWREF	270
#define	OR	271
#define	AND	272
#define	EQ	273
#define	NE	274
#define	GT	275
#define	LT	276
#define	LTE	277
#define	GTE	278
#define	POWER	279
#define	NOT	280
#define	INTCAST	281
#define	FLTCAST	282
#define	UMINUS	283

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
/*                                                                      */
/************************************************************************/

#define  APPROX 1.0e-7
#include "eval_defs.h"

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

static int  New_Const ( int returnType, void *value, long len );
static int  New_Column( int ColNum );
static int  New_Unary ( int returnType, int Op, int Node1 );
static int  New_BinOp ( int returnType, int Node1, int Op, int Node2 );
static int  New_Func  ( int returnType, funcOp Op, int nNodes,
			int Node1, int Node2, int Node3, int Node4, 
			int Node5, int Node6, int Node7 );
static int  New_Deref ( int Var,  int nDim,
			int Dim1, int Dim2, int Dim3, int Dim4, int Dim5 );
static int  New_GTI   ( char *fname, int Node1, char *start, char *stop );
static int  Test_Dims ( int Node1, int Node2 );

static void Allocate_Ptrs( Node *this );
static void Do_Unary     ( Node *this );
static void Do_BinOp_bit ( Node *this );
static void Do_BinOp_str ( Node *this );
static void Do_BinOp_log ( Node *this );
static void Do_BinOp_lng ( Node *this );
static void Do_BinOp_dbl ( Node *this );
static void Do_Func      ( Node *this );
static void Do_Deref     ( Node *this );
static void Do_GTI       ( Node *this );

static char  saobox (double xcen, double ycen, double xwid, double ywid,
		     double rot,  double xcol, double ycol);
static char  ellipse(double xcen, double ycen, double xrad, double yrad,
		     double rot, double xcol, double ycol);
static char  circle (double xcen, double ycen, double rad,
		     double xcol, double ycol);
static char  near   (double x, double y, double tolerance);
static char  bitcmp (char *bitstrm1, char *bitstrm2);
static char  bitlgte(char *bits1, int oper, char *bits2);

static void  bitand(char *result, char *bitstrm1, char *bitstrm2);
static void  bitor (char *result, char *bitstrm1, char *bitstrm2);
static void  bitnot(char *result, char *bits);

static void  fferror(char *msg);

#ifdef __cplusplus
    }
#endif


#line 137 "eval.y"
typedef union {
    int    Node;        /* Index of Node */
    double dbl;         /* real value    */
    long   lng;         /* integer value */
    char   log;         /* logical value */
    char   str[256];    /* string value  */
} FFSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	FFFINAL		205
#define	FFFLAG		-32768
#define	FFNTBASE	45

#define FFTRANSLATE(x) ((unsigned)(x) <= 283 ? fftranslate[x] : 51)

static const char fftranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    41,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,    30,    34,     2,    42,
    43,    31,    28,    16,    29,     2,    32,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    18,     2,     2,
    17,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    40,     2,    44,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    33,     2,    23,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    19,    20,    21,    22,    24,    25,    26,    27,    35,    36,
    37,    38,    39
};

#if FFDEBUG != 0
static const short ffprhs[] = {     0,
     0,     1,     4,     6,     9,    12,    15,    18,    21,    23,
    25,    29,    33,    37,    40,    44,    46,    48,    50,    52,
    56,    60,    64,    68,    72,    76,    79,    83,    87,    91,
    94,    98,   102,   106,   110,   116,   121,   128,   137,   148,
   161,   164,   167,   170,   173,   175,   177,   181,   185,   189,
   193,   197,   201,   205,   209,   213,   217,   221,   225,   229,
   233,   237,   241,   245,   249,   253,   259,   263,   267,   271,
   277,   285,   297,   313,   316,   320,   326,   336,   341,   348,
   357,   368,   381,   384,   388,   390,   392,   396,   400
};

static const short ffrhs[] = {    -1,
    45,    46,     0,    41,     0,    48,    41,     0,    49,    41,
     0,    50,    41,     0,    47,    41,     0,     1,    41,     0,
     7,     0,    14,     0,    47,    34,    47,     0,    47,    33,
    47,     0,    47,    28,    47,     0,    36,    47,     0,    42,
    47,    43,     0,     4,     0,     5,     0,    11,     0,    15,
     0,    48,    30,    48,     0,    48,    28,    48,     0,    48,
    29,    48,     0,    48,    31,    48,     0,    48,    32,    48,
     0,    48,    35,    48,     0,    29,    48,     0,    42,    48,
    43,     0,    48,    31,    49,     0,    49,    31,    48,     0,
     8,    43,     0,     8,    49,    43,     0,     8,    50,    43,
     0,     8,    47,    43,     0,     8,    48,    43,     0,     8,
    48,    16,    48,    43,     0,    48,    40,    48,    44,     0,
    48,    40,    48,    16,    48,    44,     0,    48,    40,    48,
    16,    48,    16,    48,    44,     0,    48,    40,    48,    16,
    48,    16,    48,    16,    48,    44,     0,    48,    40,    48,
    16,    48,    16,    48,    16,    48,    16,    48,    44,     0,
    37,    48,     0,    37,    49,     0,    38,    48,     0,    38,
    49,     0,     3,     0,    12,     0,    47,    21,    47,     0,
    47,    22,    47,     0,    47,    25,    47,     0,    47,    26,
    47,     0,    47,    24,    47,     0,    47,    27,    47,     0,
    48,    24,    48,     0,    48,    25,    48,     0,    48,    27,
    48,     0,    48,    26,    48,     0,    48,    23,    48,     0,
    48,    21,    48,     0,    48,    22,    48,     0,    50,    21,
    50,     0,    50,    22,    50,     0,    49,    20,    49,     0,
    49,    19,    49,     0,    49,    21,    49,     0,    49,    22,
    49,     0,    48,    17,    48,    18,    48,     0,     9,    48,
    43,     0,     9,    49,    43,     0,     9,    50,    43,     0,
     8,    49,    16,    49,    43,     0,     9,    48,    16,    48,
    16,    48,    43,     0,     9,    48,    16,    48,    16,    48,
    16,    48,    16,    48,    43,     0,     9,    48,    16,    48,
    16,    48,    16,    48,    16,    48,    16,    48,    16,    48,
    43,     0,    10,    43,     0,    10,     6,    43,     0,    10,
     6,    16,    48,    43,     0,    10,     6,    16,    48,    16,
     6,    16,     6,    43,     0,    49,    40,    48,    44,     0,
    49,    40,    48,    16,    48,    44,     0,    49,    40,    48,
    16,    48,    16,    48,    44,     0,    49,    40,    48,    16,
    48,    16,    48,    16,    48,    44,     0,    49,    40,    48,
    16,    48,    16,    48,    16,    48,    16,    48,    44,     0,
    36,    49,     0,    42,    49,    43,     0,     6,     0,    13,
     0,    42,    50,    43,     0,    50,    28,    50,     0,     8,
    50,    16,    50,    43,     0
};

#endif

#if FFDEBUG != 0
static const short ffrline[] = { 0,
   180,   181,   184,   185,   190,   195,   200,   205,   208,   213,
   215,   218,   221,   224,   226,   230,   232,   234,   236,   238,
   241,   244,   247,   250,   253,   256,   258,   260,   264,   268,
   278,   289,   298,   307,   344,   381,   383,   385,   387,   389,
   391,   393,   395,   397,   401,   403,   405,   408,   411,   414,
   417,   420,   423,   426,   429,   432,   435,   438,   441,   444,
   447,   450,   452,   454,   456,   459,   466,   479,   492,   503,
   519,   537,   558,   584,   594,   604,   622,   640,   642,   644,
   646,   648,   650,   652,   656,   659,   661,   663,   666
};
#endif


#if FFDEBUG != 0 || defined (FFERROR_VERBOSE)

static const char * const fftname[] = {   "$","error","$undefined.","BOOLEAN",
"LONG","DOUBLE","STRING","BITSTR","FUNCTION","BFUNCTION","FILTER","COLUMN","BCOLUMN",
"SCOLUMN","BITCOL","ROWREF","','","'='","':'","OR","AND","EQ","NE","'~'","GT",
"LT","LTE","GTE","'+'","'-'","'%'","'*'","'/'","'|'","'&'","POWER","NOT","INTCAST",
"FLTCAST","UMINUS","'['","'\\n'","'('","')'","']'","lines","line","bits","expr",
"bexpr","sexpr", NULL
};
#endif

static const short ffr1[] = {     0,
    45,    45,    46,    46,    46,    46,    46,    46,    47,    47,
    47,    47,    47,    47,    47,    48,    48,    48,    48,    48,
    48,    48,    48,    48,    48,    48,    48,    48,    48,    48,
    48,    48,    48,    48,    48,    48,    48,    48,    48,    48,
    48,    48,    48,    48,    49,    49,    49,    49,    49,    49,
    49,    49,    49,    49,    49,    49,    49,    49,    49,    49,
    49,    49,    49,    49,    49,    49,    49,    49,    49,    49,
    49,    49,    49,    49,    49,    49,    49,    49,    49,    49,
    49,    49,    49,    49,    50,    50,    50,    50,    50
};

static const short ffr2[] = {     0,
     0,     2,     1,     2,     2,     2,     2,     2,     1,     1,
     3,     3,     3,     2,     3,     1,     1,     1,     1,     3,
     3,     3,     3,     3,     3,     2,     3,     3,     3,     2,
     3,     3,     3,     3,     5,     4,     6,     8,    10,    12,
     2,     2,     2,     2,     1,     1,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     5,     3,     3,     3,     5,
     7,    11,    15,     2,     3,     5,     9,     4,     6,     8,
    10,    12,     2,     3,     1,     1,     3,     3,     5
};

static const short ffdefact[] = {     1,
     0,     0,    45,    16,    17,    85,     9,     0,     0,     0,
    18,    46,    86,    10,    19,     0,     0,     0,     0,     3,
     0,     2,     0,     0,     0,     0,     8,    30,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    74,    26,     0,
     0,    14,     0,    83,    41,    42,    43,    44,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     7,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     4,     0,     0,
     0,     0,     0,     0,     5,     0,     0,     0,     6,    33,
     0,    34,     0,    31,     0,    32,     0,    67,    68,    69,
     0,    75,    15,    27,    84,    87,     0,     0,    47,    48,
    51,    49,    50,    52,    13,    12,    11,     0,    58,    59,
    57,    53,    54,    56,    55,    21,    22,    20,    23,    28,
    24,    25,     0,    63,    62,    64,    65,    29,     0,     0,
     0,    60,    61,    88,     0,     0,     0,     0,     0,    14,
     0,     0,     0,    36,     0,    78,     0,     0,    35,    70,
    89,     0,     0,    76,    66,     0,     0,     0,     0,     0,
    37,     0,    79,     0,    71,     0,     0,     0,     0,     0,
     0,    38,     0,    80,     0,    77,     0,     0,     0,     0,
    39,     0,    81,     0,    72,     0,     0,     0,    40,    82,
     0,     0,    73,     0,     0
};

static const short ffdefgoto[] = {     1,
    22,    33,    43,    40,    41
};

static const short ffpact[] = {-32768,
   198,   -32,-32768,-32768,-32768,-32768,-32768,   238,   279,    -5,
-32768,-32768,-32768,-32768,-32768,   279,   279,   279,   279,-32768,
   279,-32768,   898,   746,   156,    56,-32768,-32768,   195,   502,
   127,     9,   953,   527,   824,     0,   -14,-32768,   -26,   143,
    70,-32768,   881,     2,   -26,     2,   -26,     2,   235,   674,
   829,    66,    12,    12,    12,    12,    12,    12,    12,    12,
    12,-32768,   279,   279,   279,   279,   279,   279,   279,   279,
   279,   279,   279,   279,   279,   279,   279,-32768,   279,   279,
   279,   279,   279,   279,-32768,    -3,    -3,    -3,-32768,-32768,
   279,-32768,   279,-32768,    -3,-32768,   279,-32768,-32768,-32768,
   279,-32768,-32768,-32768,-32768,-32768,    12,    12,    27,    27,
    27,    27,    27,    27,    11,-32768,-32768,   861,   936,   936,
   936,   960,   960,   960,   960,   110,   110,   110,   -29,     2,
   -29,   -29,   302,   280,    86,   -27,   -27,   -29,   327,    -3,
    -3,     7,     7,-32768,   698,   837,   -16,   767,   552,-32768,
    87,   279,   279,-32768,   279,-32768,    18,     4,-32768,-32768,
-32768,   279,    50,-32768,   919,   352,   377,   577,    41,   279,
-32768,   279,-32768,   279,-32768,    52,   402,   427,   787,    16,
   279,-32768,   279,-32768,   279,-32768,   452,   477,   602,   279,
-32768,   279,-32768,   279,-32768,   626,   650,   807,-32768,-32768,
   279,   722,-32768,    80,-32768
};

static const short ffpgoto[] = {-32768,
-32768,    78,    -1,    32,    15
};


#define	FFLAST		1000


static const short fftable[] = {    24,
    37,   101,     6,    83,   140,    76,    30,    34,    27,    13,
    77,    88,    84,    77,    39,    26,    45,    47,     7,    50,
    86,    87,    32,    36,    95,    14,   161,    88,   102,    86,
    87,    88,    25,    95,    88,    52,    88,    38,   141,    31,
    35,    84,   100,    60,    61,    88,   106,   107,    44,    46,
    48,    96,    51,   108,    59,   169,   176,   180,   186,    60,
    61,   118,   119,   120,   121,   122,   123,   124,   125,   126,
   127,   128,   129,   131,   132,   133,    86,    87,    23,   205,
     0,   138,   139,    88,     0,    29,    86,    87,     0,   145,
    86,    87,     0,    88,    42,   148,    89,    88,    49,   149,
   142,   143,   144,     0,     0,   130,    81,    82,   106,   147,
   134,   135,   136,   137,    59,     0,    83,     0,     0,    60,
    61,     0,     0,     0,   146,    84,     0,     0,     0,   103,
   109,   110,   111,   112,   113,   114,   115,   116,   117,     0,
    74,    75,    93,     0,    76,    79,    80,    81,    82,    77,
   165,   166,     0,   167,   157,   158,     0,    83,     0,     0,
   168,    79,    80,    81,    82,     0,    84,     0,   177,    94,
   178,     0,   179,    83,    79,    80,    81,    82,     0,   187,
     0,   188,    84,   189,   150,   151,    83,     0,   196,     0,
   197,     0,   198,     0,     0,    84,    85,   204,     2,   202,
     3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
    13,    14,    15,     0,     0,    53,    54,     0,    55,    56,
    57,    58,    59,     0,     0,     0,    16,    60,    61,     0,
     0,     0,     0,    17,    18,    19,     0,    90,    20,    21,
     3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
    13,    14,    15,     0,     0,    53,    54,     0,    55,    56,
    57,    58,    59,     0,     0,     0,    16,    60,    61,     0,
     0,     0,     0,    17,    18,    19,     0,   103,     0,    21,
    28,     3,     4,     5,     6,     7,     8,     9,    10,    11,
    12,    13,    14,    15,     0,     0,     0,     0,     0,    80,
    81,    82,     0,     0,     0,     0,     0,    16,     0,     0,
    83,     0,     0,     0,    17,    18,    19,   153,    63,    84,
    21,     0,    64,    65,    66,    67,    68,    69,    70,    71,
    72,    73,    74,    75,     0,     0,    76,     0,     0,     0,
     0,    77,   155,    63,     0,   154,     0,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,     0,
     0,    76,     0,     0,     0,     0,    77,   170,    63,     0,
   156,     0,    64,    65,    66,    67,    68,    69,    70,    71,
    72,    73,    74,    75,     0,     0,    76,     0,     0,     0,
     0,    77,   172,    63,     0,   171,     0,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,     0,
     0,    76,     0,     0,     0,     0,    77,   181,    63,     0,
   173,     0,    64,    65,    66,    67,    68,    69,    70,    71,
    72,    73,    74,    75,     0,     0,    76,     0,     0,     0,
     0,    77,   183,    63,     0,   182,     0,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,     0,
     0,    76,     0,     0,     0,     0,    77,   190,    63,     0,
   184,     0,    64,    65,    66,    67,    68,    69,    70,    71,
    72,    73,    74,    75,     0,     0,    76,     0,     0,     0,
     0,    77,   192,    63,     0,   191,     0,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,     0,
     0,    76,     0,     0,     0,     0,    77,    91,    63,     0,
   193,     0,    64,    65,    66,    67,    68,    69,    70,    71,
    72,    73,    74,    75,     0,     0,    76,     0,     0,     0,
     0,    77,    97,    63,    92,     0,     0,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,     0,
     0,    76,     0,     0,     0,     0,    77,   163,    63,    98,
     0,     0,    64,    65,    66,    67,    68,    69,    70,    71,
    72,    73,    74,    75,     0,     0,    76,     0,     0,     0,
     0,    77,   174,    63,   164,     0,     0,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,     0,
     0,    76,     0,     0,     0,     0,    77,   194,    63,   175,
     0,     0,    64,    65,    66,    67,    68,    69,    70,    71,
    72,    73,    74,    75,     0,     0,    76,     0,     0,     0,
     0,    77,    63,     0,   195,     0,    64,    65,    66,    67,
    68,    69,    70,    71,    72,    73,    74,    75,     0,     0,
    76,     0,     0,     0,     0,    77,    63,     0,     0,   199,
    64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
    74,    75,     0,     0,    76,     0,     0,     0,     0,    77,
    63,     0,     0,   200,    64,    65,    66,    67,    68,    69,
    70,    71,    72,    73,    74,    75,     0,     0,    76,     0,
     0,     0,     0,    77,    63,     0,   104,     0,    64,    65,
    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
     0,     0,    76,     0,     0,     0,     0,    77,    63,     0,
   159,     0,    64,    65,    66,    67,    68,    69,    70,    71,
    72,    73,    74,    75,     0,     0,    76,     0,     0,     0,
     0,    77,    63,     0,   203,     0,    64,    65,    66,    67,
    68,    69,    70,    71,    72,    73,    74,    75,     0,     0,
    76,     0,   162,    63,     0,    77,    78,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,     0,
     0,    76,   185,    63,     0,     0,    77,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,     0,
     0,    76,   201,    63,     0,     0,    77,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,     0,
     0,    76,    79,    80,    81,    82,    77,    79,    80,    81,
    82,     0,     0,     0,    83,    79,    80,    81,    82,    83,
     0,     0,     0,    84,     0,     0,    99,    83,    84,     0,
     0,   105,     0,     0,     0,     0,    84,    63,   152,   160,
     0,    64,    65,    66,    67,    68,    69,    70,    71,    72,
    73,    74,    75,     0,     0,    76,     0,    63,     0,     0,
    77,    64,    65,    66,    67,    68,    69,    70,    71,    72,
    73,    74,    75,     0,     0,    76,     0,     0,    53,    54,
    77,    55,    56,    57,    58,    59,     0,     0,     0,     0,
    60,    61,     0,     0,     0,     0,     0,     0,    62,    64,
    65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
    75,     0,     0,    76,     0,     0,     0,     0,    77,    67,
    68,    69,    70,    71,    72,    73,    74,    75,     0,     0,
    76,     0,     0,    53,    54,    77,    55,    56,    57,    58,
    59,     0,     0,     0,     0,    60,    61,    71,    72,    73,
    74,    75,     0,     0,    76,     0,     0,     0,     0,    77
};

static const short ffcheck[] = {     1,
     6,    16,     6,    31,     8,    35,     8,     9,    41,    13,
    40,    28,    40,    40,    16,     1,    18,    19,     7,    21,
    21,    22,     8,     9,    16,    14,    43,    28,    43,    21,
    22,    28,     1,    16,    28,    21,    28,    43,    42,     8,
     9,    40,    43,    33,    34,    28,    43,    36,    17,    18,
    19,    43,    21,    42,    28,     6,    16,     6,    43,    33,
    34,    63,    64,    65,    66,    67,    68,    69,    70,    71,
    72,    73,    74,    75,    76,    77,    21,    22,     1,     0,
    -1,    83,    84,    28,    -1,     8,    21,    22,    -1,    91,
    21,    22,    -1,    28,    17,    97,    41,    28,    21,   101,
    86,    87,    88,    -1,    -1,    74,    21,    22,    43,    95,
    79,    80,    81,    82,    28,    -1,    31,    -1,    -1,    33,
    34,    -1,    -1,    -1,    93,    40,    -1,    -1,    -1,    43,
    53,    54,    55,    56,    57,    58,    59,    60,    61,    -1,
    31,    32,    16,    -1,    35,    19,    20,    21,    22,    40,
   152,   153,    -1,   155,   140,   141,    -1,    31,    -1,    -1,
   162,    19,    20,    21,    22,    -1,    40,    -1,   170,    43,
   172,    -1,   174,    31,    19,    20,    21,    22,    -1,   181,
    -1,   183,    40,   185,   107,   108,    31,    -1,   190,    -1,
   192,    -1,   194,    -1,    -1,    40,    41,     0,     1,   201,
     3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
    13,    14,    15,    -1,    -1,    21,    22,    -1,    24,    25,
    26,    27,    28,    -1,    -1,    -1,    29,    33,    34,    -1,
    -1,    -1,    -1,    36,    37,    38,    -1,    43,    41,    42,
     3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
    13,    14,    15,    -1,    -1,    21,    22,    -1,    24,    25,
    26,    27,    28,    -1,    -1,    -1,    29,    33,    34,    -1,
    -1,    -1,    -1,    36,    37,    38,    -1,    43,    -1,    42,
    43,     3,     4,     5,     6,     7,     8,     9,    10,    11,
    12,    13,    14,    15,    -1,    -1,    -1,    -1,    -1,    20,
    21,    22,    -1,    -1,    -1,    -1,    -1,    29,    -1,    -1,
    31,    -1,    -1,    -1,    36,    37,    38,    16,    17,    40,
    42,    -1,    21,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    31,    32,    -1,    -1,    35,    -1,    -1,    -1,
    -1,    40,    16,    17,    -1,    44,    -1,    21,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
    -1,    35,    -1,    -1,    -1,    -1,    40,    16,    17,    -1,
    44,    -1,    21,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    31,    32,    -1,    -1,    35,    -1,    -1,    -1,
    -1,    40,    16,    17,    -1,    44,    -1,    21,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
    -1,    35,    -1,    -1,    -1,    -1,    40,    16,    17,    -1,
    44,    -1,    21,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    31,    32,    -1,    -1,    35,    -1,    -1,    -1,
    -1,    40,    16,    17,    -1,    44,    -1,    21,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
    -1,    35,    -1,    -1,    -1,    -1,    40,    16,    17,    -1,
    44,    -1,    21,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    31,    32,    -1,    -1,    35,    -1,    -1,    -1,
    -1,    40,    16,    17,    -1,    44,    -1,    21,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
    -1,    35,    -1,    -1,    -1,    -1,    40,    16,    17,    -1,
    44,    -1,    21,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    31,    32,    -1,    -1,    35,    -1,    -1,    -1,
    -1,    40,    16,    17,    43,    -1,    -1,    21,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
    -1,    35,    -1,    -1,    -1,    -1,    40,    16,    17,    43,
    -1,    -1,    21,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    31,    32,    -1,    -1,    35,    -1,    -1,    -1,
    -1,    40,    16,    17,    43,    -1,    -1,    21,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
    -1,    35,    -1,    -1,    -1,    -1,    40,    16,    17,    43,
    -1,    -1,    21,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    31,    32,    -1,    -1,    35,    -1,    -1,    -1,
    -1,    40,    17,    -1,    43,    -1,    21,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    31,    32,    -1,    -1,
    35,    -1,    -1,    -1,    -1,    40,    17,    -1,    -1,    44,
    21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    32,    -1,    -1,    35,    -1,    -1,    -1,    -1,    40,
    17,    -1,    -1,    44,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    -1,    -1,    35,    -1,
    -1,    -1,    -1,    40,    17,    -1,    43,    -1,    21,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
    -1,    -1,    35,    -1,    -1,    -1,    -1,    40,    17,    -1,
    43,    -1,    21,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    31,    32,    -1,    -1,    35,    -1,    -1,    -1,
    -1,    40,    17,    -1,    43,    -1,    21,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    31,    32,    -1,    -1,
    35,    -1,    16,    17,    -1,    40,    41,    21,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
    -1,    35,    16,    17,    -1,    -1,    40,    21,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
    -1,    35,    16,    17,    -1,    -1,    40,    21,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
    -1,    35,    19,    20,    21,    22,    40,    19,    20,    21,
    22,    -1,    -1,    -1,    31,    19,    20,    21,    22,    31,
    -1,    -1,    -1,    40,    -1,    -1,    43,    31,    40,    -1,
    -1,    43,    -1,    -1,    -1,    -1,    40,    17,    18,    43,
    -1,    21,    22,    23,    24,    25,    26,    27,    28,    29,
    30,    31,    32,    -1,    -1,    35,    -1,    17,    -1,    -1,
    40,    21,    22,    23,    24,    25,    26,    27,    28,    29,
    30,    31,    32,    -1,    -1,    35,    -1,    -1,    21,    22,
    40,    24,    25,    26,    27,    28,    -1,    -1,    -1,    -1,
    33,    34,    -1,    -1,    -1,    -1,    -1,    -1,    41,    21,
    22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
    32,    -1,    -1,    35,    -1,    -1,    -1,    -1,    40,    24,
    25,    26,    27,    28,    29,    30,    31,    32,    -1,    -1,
    35,    -1,    -1,    21,    22,    40,    24,    25,    26,    27,
    28,    -1,    -1,    -1,    -1,    33,    34,    28,    29,    30,
    31,    32,    -1,    -1,    35,    -1,    -1,    -1,    -1,    40
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/local/share/bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

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
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define fferrok		(fferrstatus = 0)
#define ffclearin	(ffchar = FFEMPTY)
#define FFEMPTY		-2
#define FFEOF		0
#define FFACCEPT	return(0)
#define FFABORT 	return(1)
#define FFERROR		goto fferrlab1
/* Like FFERROR except do call fferror.
   This remains here temporarily to ease the
   transition to the new meaning of FFERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define FFFAIL		goto fferrlab
#define FFRECOVERING()  (!!fferrstatus)
#define FFBACKUP(token, value) \
do								\
  if (ffchar == FFEMPTY && fflen == 1)				\
    { ffchar = (token), fflval = (value);			\
      ffchar1 = FFTRANSLATE (ffchar);				\
      FFPOPSTACK;						\
      goto ffbackup;						\
    }								\
  else								\
    { fferror ("syntax error: cannot back up"); FFERROR; }	\
while (0)

#define FFTERROR	1
#define FFERRCODE	256

#ifndef FFPURE
#define FFLEX		fflex()
#endif

#ifdef FFPURE
#ifdef FFLSP_NEEDED
#ifdef FFLEX_PARAM
#define FFLEX		fflex(&fflval, &fflloc, FFLEX_PARAM)
#else
#define FFLEX		fflex(&fflval, &fflloc)
#endif
#else /* not FFLSP_NEEDED */
#ifdef FFLEX_PARAM
#define FFLEX		fflex(&fflval, FFLEX_PARAM)
#else
#define FFLEX		fflex(&fflval)
#endif
#endif /* not FFLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef FFPURE

int	ffchar;			/*  the lookahead symbol		*/
FFSTYPE	fflval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef FFLSP_NEEDED
FFLTYPE fflloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int ffnerrs;			/*  number of parse errors so far       */
#endif  /* not FFPURE */

#if FFDEBUG != 0
int ffdebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  FFINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	FFINITDEPTH
#define FFINITDEPTH 200
#endif

/*  FFMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if FFMAXDEPTH == 0
#undef FFMAXDEPTH
#endif

#ifndef FFMAXDEPTH
#define FFMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int ffparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __ff_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__ff_memcpy (to, from, count)
     char *to;
     char *from;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__ff_memcpy (char *to, char *from, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 196 "/usr/local/share/bison.simple"

/* The user can define FFPARSE_PARAM as the name of an argument to be passed
   into ffparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef FFPARSE_PARAM
#ifdef __cplusplus
#define FFPARSE_PARAM_ARG void *FFPARSE_PARAM
#define FFPARSE_PARAM_DECL
#else /* not __cplusplus */
#define FFPARSE_PARAM_ARG FFPARSE_PARAM
#define FFPARSE_PARAM_DECL void *FFPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not FFPARSE_PARAM */
#define FFPARSE_PARAM_ARG
#define FFPARSE_PARAM_DECL
#endif /* not FFPARSE_PARAM */

int
ffparse(FFPARSE_PARAM_ARG)
     FFPARSE_PARAM_DECL
{
  register int ffstate;
  register int ffn;
  register short *ffssp;
  register FFSTYPE *ffvsp;
  int fferrstatus;	/*  number of tokens to shift before error messages enabled */
  int ffchar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	ffssa[FFINITDEPTH];	/*  the state stack			*/
  FFSTYPE ffvsa[FFINITDEPTH];	/*  the semantic value stack		*/

  short *ffss = ffssa;		/*  refer to the stacks thru separate pointers */
  FFSTYPE *ffvs = ffvsa;	/*  to allow ffoverflow to reallocate them elsewhere */

#ifdef FFLSP_NEEDED
  FFLTYPE fflsa[FFINITDEPTH];	/*  the location stack			*/
  FFLTYPE *ffls = fflsa;
  FFLTYPE *fflsp;

#define FFPOPSTACK   (ffvsp--, ffssp--, fflsp--)
#else
#define FFPOPSTACK   (ffvsp--, ffssp--)
#endif

  int ffstacksize = FFINITDEPTH;

#ifdef FFPURE
  int ffchar;
  FFSTYPE fflval;
  int ffnerrs;
#ifdef FFLSP_NEEDED
  FFLTYPE fflloc;
#endif
#endif

  FFSTYPE ffval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int fflen;

#if FFDEBUG != 0
  if (ffdebug)
    fprintf(stderr, "Starting parse\n");
#endif

  ffstate = 0;
  fferrstatus = 0;
  ffnerrs = 0;
  ffchar = FFEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  ffssp = ffss - 1;
  ffvsp = ffvs;
#ifdef FFLSP_NEEDED
  fflsp = ffls;
#endif

/* Push a new state, which is found in  ffstate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
ffnewstate:

  *++ffssp = ffstate;

  if (ffssp >= ffss + ffstacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      FFSTYPE *ffvs1 = ffvs;
      short *ffss1 = ffss;
#ifdef FFLSP_NEEDED
      FFLTYPE *ffls1 = ffls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = ffssp - ffss + 1;

#ifdef ffoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef FFLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if ffoverflow is a macro.  */
      ffoverflow("parser stack overflow",
		 &ffss1, size * sizeof (*ffssp),
		 &ffvs1, size * sizeof (*ffvsp),
		 &ffls1, size * sizeof (*fflsp),
		 &ffstacksize);
#else
      ffoverflow("parser stack overflow",
		 &ffss1, size * sizeof (*ffssp),
		 &ffvs1, size * sizeof (*ffvsp),
		 &ffstacksize);
#endif

      ffss = ffss1; ffvs = ffvs1;
#ifdef FFLSP_NEEDED
      ffls = ffls1;
#endif
#else /* no ffoverflow */
      /* Extend the stack our own way.  */
      if (ffstacksize >= FFMAXDEPTH)
	{
	  fferror("parser stack overflow");
	  return 2;
	}
      ffstacksize *= 2;
      if (ffstacksize > FFMAXDEPTH)
	ffstacksize = FFMAXDEPTH;
      ffss = (short *) alloca (ffstacksize * sizeof (*ffssp));
      __ff_memcpy ((char *)ffss, (char *)ffss1, size * sizeof (*ffssp));
      ffvs = (FFSTYPE *) alloca (ffstacksize * sizeof (*ffvsp));
      __ff_memcpy ((char *)ffvs, (char *)ffvs1, size * sizeof (*ffvsp));
#ifdef FFLSP_NEEDED
      ffls = (FFLTYPE *) alloca (ffstacksize * sizeof (*fflsp));
      __ff_memcpy ((char *)ffls, (char *)ffls1, size * sizeof (*fflsp));
#endif
#endif /* no ffoverflow */

      ffssp = ffss + size - 1;
      ffvsp = ffvs + size - 1;
#ifdef FFLSP_NEEDED
      fflsp = ffls + size - 1;
#endif

#if FFDEBUG != 0
      if (ffdebug)
	fprintf(stderr, "Stack size increased to %d\n", ffstacksize);
#endif

      if (ffssp >= ffss + ffstacksize - 1)
	FFABORT;
    }

#if FFDEBUG != 0
  if (ffdebug)
    fprintf(stderr, "Entering state %d\n", ffstate);
#endif

  goto ffbackup;
 ffbackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* ffresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  ffn = ffpact[ffstate];
  if (ffn == FFFLAG)
    goto ffdefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* ffchar is either FFEMPTY or FFEOF
     or a valid token in external form.  */

  if (ffchar == FFEMPTY)
    {
#if FFDEBUG != 0
      if (ffdebug)
	fprintf(stderr, "Reading a token: ");
#endif
      ffchar = FFLEX;
    }

  /* Convert token to internal form (in ffchar1) for indexing tables with */

  if (ffchar <= 0)		/* This means end of input. */
    {
      ffchar1 = 0;
      ffchar = FFEOF;		/* Don't call FFLEX any more */

#if FFDEBUG != 0
      if (ffdebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      ffchar1 = FFTRANSLATE(ffchar);

#if FFDEBUG != 0
      if (ffdebug)
	{
	  fprintf (stderr, "Next token is %d (%s", ffchar, fftname[ffchar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef FFPRINT
	  FFPRINT (stderr, ffchar, fflval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  ffn += ffchar1;
  if (ffn < 0 || ffn > FFLAST || ffcheck[ffn] != ffchar1)
    goto ffdefault;

  ffn = fftable[ffn];

  /* ffn is what to do for this token type in this state.
     Negative => reduce, -ffn is rule number.
     Positive => shift, ffn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (ffn < 0)
    {
      if (ffn == FFFLAG)
	goto fferrlab;
      ffn = -ffn;
      goto ffreduce;
    }
  else if (ffn == 0)
    goto fferrlab;

  if (ffn == FFFINAL)
    FFACCEPT;

  /* Shift the lookahead token.  */

#if FFDEBUG != 0
  if (ffdebug)
    fprintf(stderr, "Shifting token %d (%s), ", ffchar, fftname[ffchar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (ffchar != FFEOF)
    ffchar = FFEMPTY;

  *++ffvsp = fflval;
#ifdef FFLSP_NEEDED
  *++fflsp = fflloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (fferrstatus) fferrstatus--;

  ffstate = ffn;
  goto ffnewstate;

/* Do the default action for the current state.  */
ffdefault:

  ffn = ffdefact[ffstate];
  if (ffn == 0)
    goto fferrlab;

/* Do a reduction.  ffn is the number of a rule to reduce with.  */
ffreduce:
  fflen = ffr2[ffn];
  if (fflen > 0)
    ffval = ffvsp[1-fflen]; /* implement default value of the action */

#if FFDEBUG != 0
  if (ffdebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       ffn, ffrline[ffn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = ffprhs[ffn]; ffrhs[i] > 0; i++)
	fprintf (stderr, "%s ", fftname[ffrhs[i]]);
      fprintf (stderr, " -> %s\n", fftname[ffr1[ffn]]);
    }
#endif


  switch (ffn) {

case 3:
#line 184 "eval.y"
{;
    break;}
case 4:
#line 186 "eval.y"
{ if( ffvsp[-1].Node<0 ) {
		     fferror("Couldn't build node structure: out of memory?");
		     FFERROR;  }
		;
    break;}
case 5:
#line 191 "eval.y"
{ if( ffvsp[-1].Node<0 ) {
		     fferror("Couldn't build node structure: out of memory?");
		     FFERROR;  }
		;
    break;}
case 6:
#line 196 "eval.y"
{ if( ffvsp[-1].Node<0 ) {
		     fferror("Couldn't build node structure: out of memory?");
		     FFERROR;  } 
		;
    break;}
case 7:
#line 201 "eval.y"
{ if( ffvsp[-1].Node<0 ) {
		     fferror("Couldn't build node structure: out of memory?");
		     FFERROR;  }
		;
    break;}
case 8:
#line 205 "eval.y"
{  fferrok;  ;
    break;}
case 9:
#line 209 "eval.y"
{
                  ffval.Node = New_Const( BITSTR, ffvsp[0].str, strlen(ffvsp[0].str)+1 ); TEST(ffval.Node);
		  SIZE(ffval.Node) = strlen(ffvsp[0].str);
		;
    break;}
case 10:
#line 214 "eval.y"
{ ffval.Node = New_Column( ffvsp[0].lng ); TEST(ffval.Node); ;
    break;}
case 11:
#line 216 "eval.y"
{ ffval.Node = New_BinOp( BITSTR, ffvsp[-2].Node, '&', ffvsp[0].Node ); TEST(ffval.Node);
                  SIZE(ffval.Node) = ( SIZE(ffvsp[-2].Node)>SIZE(ffvsp[0].Node) ? SIZE(ffvsp[-2].Node) : SIZE(ffvsp[0].Node) );  ;
    break;}
case 12:
#line 219 "eval.y"
{ ffval.Node = New_BinOp( BITSTR, ffvsp[-2].Node, '|', ffvsp[0].Node ); TEST(ffval.Node);
                  SIZE(ffval.Node) = ( SIZE(ffvsp[-2].Node)>SIZE(ffvsp[0].Node) ? SIZE(ffvsp[-2].Node) : SIZE(ffvsp[0].Node) );  ;
    break;}
case 13:
#line 222 "eval.y"
{ ffval.Node = New_BinOp( BITSTR, ffvsp[-2].Node, '+', ffvsp[0].Node ); TEST(ffval.Node);
                  SIZE(ffval.Node) = SIZE(ffvsp[-2].Node) + SIZE(ffvsp[0].Node);                          ;
    break;}
case 14:
#line 225 "eval.y"
{ ffval.Node = New_Unary( BITSTR, NOT, ffvsp[0].Node ); TEST(ffval.Node);     ;
    break;}
case 15:
#line 227 "eval.y"
{ ffval.Node = ffvsp[-1].Node; ;
    break;}
case 16:
#line 231 "eval.y"
{ ffval.Node = New_Const( LONG,   &(ffvsp[0].lng), sizeof(long)   ); TEST(ffval.Node); ;
    break;}
case 17:
#line 233 "eval.y"
{ ffval.Node = New_Const( DOUBLE, &(ffvsp[0].dbl), sizeof(double) ); TEST(ffval.Node); ;
    break;}
case 18:
#line 235 "eval.y"
{ ffval.Node = New_Column( ffvsp[0].lng ); TEST(ffval.Node); ;
    break;}
case 19:
#line 237 "eval.y"
{ ffval.Node = New_Func( LONG, row_fct, 0, 0, 0, 0, 0, 0, 0, 0 ); ;
    break;}
case 20:
#line 239 "eval.y"
{ PROMOTE(ffvsp[-2].Node,ffvsp[0].Node); ffval.Node = New_BinOp( TYPE(ffvsp[-2].Node), ffvsp[-2].Node, '%', ffvsp[0].Node );
		  TEST(ffval.Node);                                                ;
    break;}
case 21:
#line 242 "eval.y"
{ PROMOTE(ffvsp[-2].Node,ffvsp[0].Node); ffval.Node = New_BinOp( TYPE(ffvsp[-2].Node), ffvsp[-2].Node, '+', ffvsp[0].Node );
		  TEST(ffval.Node);                                                ;
    break;}
case 22:
#line 245 "eval.y"
{ PROMOTE(ffvsp[-2].Node,ffvsp[0].Node); ffval.Node = New_BinOp( TYPE(ffvsp[-2].Node), ffvsp[-2].Node, '-', ffvsp[0].Node ); 
		  TEST(ffval.Node);                                                ;
    break;}
case 23:
#line 248 "eval.y"
{ PROMOTE(ffvsp[-2].Node,ffvsp[0].Node); ffval.Node = New_BinOp( TYPE(ffvsp[-2].Node), ffvsp[-2].Node, '*', ffvsp[0].Node ); 
		  TEST(ffval.Node);                                                ;
    break;}
case 24:
#line 251 "eval.y"
{ PROMOTE(ffvsp[-2].Node,ffvsp[0].Node); ffval.Node = New_BinOp( TYPE(ffvsp[-2].Node), ffvsp[-2].Node, '/', ffvsp[0].Node ); 
		  TEST(ffval.Node);                                                ;
    break;}
case 25:
#line 254 "eval.y"
{ PROMOTE(ffvsp[-2].Node,ffvsp[0].Node); ffval.Node = New_BinOp( TYPE(ffvsp[-2].Node), ffvsp[-2].Node, POWER, ffvsp[0].Node );
		  TEST(ffval.Node);                                                ;
    break;}
case 26:
#line 257 "eval.y"
{ ffval.Node = New_Unary( TYPE(ffvsp[0].Node), UMINUS, ffvsp[0].Node ); TEST(ffval.Node); ;
    break;}
case 27:
#line 259 "eval.y"
{ ffval.Node = ffvsp[-1].Node; ;
    break;}
case 28:
#line 261 "eval.y"
{ ffvsp[0].Node = New_Unary( TYPE(ffvsp[-2].Node), 0, ffvsp[0].Node );
                  ffval.Node = New_BinOp( TYPE(ffvsp[-2].Node), ffvsp[-2].Node, '*', ffvsp[0].Node ); 
		  TEST(ffval.Node);                                ;
    break;}
case 29:
#line 265 "eval.y"
{ ffvsp[-2].Node = New_Unary( TYPE(ffvsp[0].Node), 0, ffvsp[-2].Node );
                  ffval.Node = New_BinOp( TYPE(ffvsp[0].Node), ffvsp[-2].Node, '*', ffvsp[0].Node );
                  TEST(ffval.Node);                                ;
    break;}
case 30:
#line 269 "eval.y"
{ if (FSTRCMP(ffvsp[-1].str,"RANDOM(") == 0)
                    ffval.Node = New_Func( DOUBLE, rnd_fct, 0, 0, 0, 0, 0, 0, 0, 0 );
                  else
		    {
                     fferror("Function() not supported");
		     FFERROR;
		    }
                  TEST(ffval.Node); 
                ;
    break;}
case 31:
#line 279 "eval.y"
{ if (FSTRCMP(ffvsp[-2].str,"SUM(") == 0) {
		     ffval.Node = New_Func( LONG, sum_fct, 1, ffvsp[-1].Node, 0, 0, 0, 0, 0, 0 );
                  } else if (FSTRCMP(ffvsp[-2].str,"NELEM(") == 0) {
                     ffval.Node = New_Const( LONG, &( SIZE(ffvsp[-1].Node) ), sizeof(long) );
		  } else {
                     fferror("Function(bool) not supported");
		     FFERROR;
		  }
                  TEST(ffval.Node); 
		;
    break;}
case 32:
#line 290 "eval.y"
{ if (FSTRCMP(ffvsp[-2].str,"NELEM(") == 0) {
                     ffval.Node = New_Const( LONG, &( SIZE(ffvsp[-1].Node) ), sizeof(long) );
		  } else {
                     fferror("Function(str) not supported");
		     FFERROR;
		  }
                  TEST(ffval.Node); 
		;
    break;}
case 33:
#line 299 "eval.y"
{ if (FSTRCMP(ffvsp[-2].str,"NELEM(") == 0) {
                     ffval.Node = New_Const( LONG, &( SIZE(ffvsp[-1].Node) ), sizeof(long) );
		  } else {
                     fferror("Function(bits) not supported");
		     FFERROR;
		  }
                  TEST(ffval.Node); 
		;
    break;}
case 34:
#line 308 "eval.y"
{ if (FSTRCMP(ffvsp[-2].str,"SUM(") == 0)
		     ffval.Node = New_Func( TYPE(ffvsp[-1].Node), sum_fct, 1, ffvsp[-1].Node,
				    0, 0, 0, 0, 0, 0 );
		  else if (FSTRCMP(ffvsp[-2].str,"NELEM(") == 0)
                     ffval.Node = New_Const( LONG, &( SIZE(ffvsp[-1].Node) ), sizeof(long) );
		  else if (FSTRCMP(ffvsp[-2].str,"ABS(") == 0)
		     ffval.Node = New_Func( 0, abs_fct, 1, ffvsp[-1].Node, 0, 0, 0, 0, 0, 0 );
                  else {
		     if( TYPE(ffvsp[-1].Node) != DOUBLE ) ffvsp[-1].Node = New_Unary( DOUBLE, 0, ffvsp[-1].Node );
                     if (FSTRCMP(ffvsp[-2].str,"SIN(") == 0)
			ffval.Node = New_Func( 0, sin_fct,  1, ffvsp[-1].Node, 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP(ffvsp[-2].str,"COS(") == 0)
			ffval.Node = New_Func( 0, cos_fct,  1, ffvsp[-1].Node, 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP(ffvsp[-2].str,"TAN(") == 0)
			ffval.Node = New_Func( 0, tan_fct,  1, ffvsp[-1].Node, 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP(ffvsp[-2].str,"ARCSIN(") == 0)
			ffval.Node = New_Func( 0, asin_fct, 1, ffvsp[-1].Node, 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP(ffvsp[-2].str,"ARCCOS(") == 0)
			ffval.Node = New_Func( 0, acos_fct, 1, ffvsp[-1].Node, 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP(ffvsp[-2].str,"ARCTAN(") == 0)
			ffval.Node = New_Func( 0, atan_fct, 1, ffvsp[-1].Node, 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP(ffvsp[-2].str,"EXP(") == 0)
			ffval.Node = New_Func( 0, exp_fct,  1, ffvsp[-1].Node, 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP(ffvsp[-2].str,"LOG(") == 0)
			ffval.Node = New_Func( 0, log_fct,  1, ffvsp[-1].Node, 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP(ffvsp[-2].str,"LOG10(") == 0)
			ffval.Node = New_Func( 0, log10_fct, 1, ffvsp[-1].Node, 0, 0, 0, 0, 0, 0 );
		     else if (FSTRCMP(ffvsp[-2].str,"SQRT(") == 0)
			ffval.Node = New_Func( 0, sqrt_fct, 1, ffvsp[-1].Node, 0, 0, 0, 0, 0, 0 );
		     else {
			fferror("Function(expr) not supported");
			FFERROR;
		     }
		  }
                  TEST(ffval.Node); 
                ;
    break;}
case 35:
#line 345 "eval.y"
{ 
		   if (FSTRCMP(ffvsp[-4].str,"DEFNULL(") == 0) {
		      if( SIZE(ffvsp[-3].Node)>=SIZE(ffvsp[-1].Node) && Test_Dims( ffvsp[-3].Node, ffvsp[-1].Node ) ) {
			 PROMOTE(ffvsp[-3].Node,ffvsp[-1].Node);
			 ffval.Node = New_Func( 0, defnull_fct, 2, ffvsp[-3].Node, ffvsp[-1].Node, 0,
					0, 0, 0, 0 );
			 TEST(ffval.Node); 
		      } else {
			 fferror("Dimensions of DEFNULL arguments are not compatible");
			 FFERROR;
		      }
		   } else if (FSTRCMP(ffvsp[-4].str,"ARCTAN2(") == 0) {
		     if( TYPE(ffvsp[-3].Node) != DOUBLE ) ffvsp[-3].Node = New_Unary( DOUBLE, 0, ffvsp[-3].Node );
		     if( TYPE(ffvsp[-1].Node) != DOUBLE ) ffvsp[-1].Node = New_Unary( DOUBLE, 0, ffvsp[-1].Node );
		     if( Test_Dims( ffvsp[-3].Node, ffvsp[-1].Node ) ) {
			ffval.Node = New_Func( 0, atan2_fct, 2, ffvsp[-3].Node, ffvsp[-1].Node, 0, 0, 0, 0, 0 );
			TEST(ffval.Node); 
			if( SIZE(ffvsp[-3].Node)<SIZE(ffvsp[-1].Node) ) {
			   int i;
			   gParse.Nodes[ffval.Node].value.nelem =
			      gParse.Nodes[ffvsp[-1].Node].value.nelem;
			   gParse.Nodes[ffval.Node].value.naxis =
			      gParse.Nodes[ffvsp[-1].Node].value.naxis;
			   for( i=0; i<gParse.Nodes[ffvsp[-1].Node].value.naxis; i++ )
			      gParse.Nodes[ffval.Node].value.naxes[i] =
				 gParse.Nodes[ffvsp[-1].Node].value.naxes[i];
			}
		     } else {
			fferror("Dimensions of arctan2 arguments are not compatible");
			FFERROR;
		     }
		  } else {
                     fferror("Function(expr,expr) not supported");
		     FFERROR;
		  }
                ;
    break;}
case 36:
#line 382 "eval.y"
{ ffval.Node = New_Deref( ffvsp[-3].Node, 1, ffvsp[-1].Node,  0,  0,  0,   0 ); TEST(ffval.Node); ;
    break;}
case 37:
#line 384 "eval.y"
{ ffval.Node = New_Deref( ffvsp[-5].Node, 2, ffvsp[-3].Node, ffvsp[-1].Node,  0,  0,   0 ); TEST(ffval.Node); ;
    break;}
case 38:
#line 386 "eval.y"
{ ffval.Node = New_Deref( ffvsp[-7].Node, 3, ffvsp[-5].Node, ffvsp[-3].Node, ffvsp[-1].Node,  0,   0 ); TEST(ffval.Node); ;
    break;}
case 39:
#line 388 "eval.y"
{ ffval.Node = New_Deref( ffvsp[-9].Node, 4, ffvsp[-7].Node, ffvsp[-5].Node, ffvsp[-3].Node, ffvsp[-1].Node,   0 ); TEST(ffval.Node); ;
    break;}
case 40:
#line 390 "eval.y"
{ ffval.Node = New_Deref( ffvsp[-11].Node, 5, ffvsp[-9].Node, ffvsp[-7].Node, ffvsp[-5].Node, ffvsp[-3].Node, ffvsp[-1].Node ); TEST(ffval.Node); ;
    break;}
case 41:
#line 392 "eval.y"
{ ffval.Node = New_Unary( LONG,   INTCAST, ffvsp[0].Node );  TEST(ffval.Node);  ;
    break;}
case 42:
#line 394 "eval.y"
{ ffval.Node = New_Unary( LONG,   INTCAST, ffvsp[0].Node );  TEST(ffval.Node);  ;
    break;}
case 43:
#line 396 "eval.y"
{ ffval.Node = New_Unary( DOUBLE, FLTCAST, ffvsp[0].Node );  TEST(ffval.Node);  ;
    break;}
case 44:
#line 398 "eval.y"
{ ffval.Node = New_Unary( DOUBLE, FLTCAST, ffvsp[0].Node );  TEST(ffval.Node);  ;
    break;}
case 45:
#line 402 "eval.y"
{ ffval.Node = New_Const( BOOLEAN, &(ffvsp[0].log), sizeof(char) ); TEST(ffval.Node); ;
    break;}
case 46:
#line 404 "eval.y"
{ ffval.Node = New_Column( ffvsp[0].lng ); TEST(ffval.Node); ;
    break;}
case 47:
#line 406 "eval.y"
{ ffval.Node = New_BinOp( BOOLEAN, ffvsp[-2].Node, EQ,  ffvsp[0].Node ); TEST(ffval.Node);
		  SIZE(ffval.Node) = 1;                                     ;
    break;}
case 48:
#line 409 "eval.y"
{ ffval.Node = New_BinOp( BOOLEAN, ffvsp[-2].Node, NE,  ffvsp[0].Node ); TEST(ffval.Node); 
		  SIZE(ffval.Node) = 1;                                     ;
    break;}
case 49:
#line 412 "eval.y"
{ ffval.Node = New_BinOp( BOOLEAN, ffvsp[-2].Node, LT,  ffvsp[0].Node ); TEST(ffval.Node); 
		  SIZE(ffval.Node) = 1;                                     ;
    break;}
case 50:
#line 415 "eval.y"
{ ffval.Node = New_BinOp( BOOLEAN, ffvsp[-2].Node, LTE, ffvsp[0].Node ); TEST(ffval.Node); 
		  SIZE(ffval.Node) = 1;                                     ;
    break;}
case 51:
#line 418 "eval.y"
{ ffval.Node = New_BinOp( BOOLEAN, ffvsp[-2].Node, GT,  ffvsp[0].Node ); TEST(ffval.Node); 
		  SIZE(ffval.Node) = 1;                                     ;
    break;}
case 52:
#line 421 "eval.y"
{ ffval.Node = New_BinOp( BOOLEAN, ffvsp[-2].Node, GTE, ffvsp[0].Node ); TEST(ffval.Node); 
		  SIZE(ffval.Node) = 1;                                     ;
    break;}
case 53:
#line 424 "eval.y"
{ PROMOTE(ffvsp[-2].Node,ffvsp[0].Node); ffval.Node = New_BinOp( BOOLEAN, ffvsp[-2].Node, GT,  ffvsp[0].Node );
                  TEST(ffval.Node);                                               ;
    break;}
case 54:
#line 427 "eval.y"
{ PROMOTE(ffvsp[-2].Node,ffvsp[0].Node); ffval.Node = New_BinOp( BOOLEAN, ffvsp[-2].Node, LT,  ffvsp[0].Node );
                  TEST(ffval.Node);                                               ;
    break;}
case 55:
#line 430 "eval.y"
{ PROMOTE(ffvsp[-2].Node,ffvsp[0].Node); ffval.Node = New_BinOp( BOOLEAN, ffvsp[-2].Node, GTE, ffvsp[0].Node );
                  TEST(ffval.Node);                                               ;
    break;}
case 56:
#line 433 "eval.y"
{ PROMOTE(ffvsp[-2].Node,ffvsp[0].Node); ffval.Node = New_BinOp( BOOLEAN, ffvsp[-2].Node, LTE, ffvsp[0].Node );
                  TEST(ffval.Node);                                               ;
    break;}
case 57:
#line 436 "eval.y"
{ PROMOTE(ffvsp[-2].Node,ffvsp[0].Node); ffval.Node = New_BinOp( BOOLEAN, ffvsp[-2].Node, '~', ffvsp[0].Node );
                  TEST(ffval.Node);                                               ;
    break;}
case 58:
#line 439 "eval.y"
{ PROMOTE(ffvsp[-2].Node,ffvsp[0].Node); ffval.Node = New_BinOp( BOOLEAN, ffvsp[-2].Node, EQ,  ffvsp[0].Node );
                  TEST(ffval.Node);                                               ;
    break;}
case 59:
#line 442 "eval.y"
{ PROMOTE(ffvsp[-2].Node,ffvsp[0].Node); ffval.Node = New_BinOp( BOOLEAN, ffvsp[-2].Node, NE,  ffvsp[0].Node );
                  TEST(ffval.Node);                                               ;
    break;}
case 60:
#line 445 "eval.y"
{ ffval.Node = New_BinOp( BOOLEAN, ffvsp[-2].Node, EQ,  ffvsp[0].Node ); TEST(ffval.Node);
                  SIZE(ffval.Node) = 1; ;
    break;}
case 61:
#line 448 "eval.y"
{ ffval.Node = New_BinOp( BOOLEAN, ffvsp[-2].Node, NE,  ffvsp[0].Node ); TEST(ffval.Node);
                  SIZE(ffval.Node) = 1; ;
    break;}
case 62:
#line 451 "eval.y"
{ ffval.Node = New_BinOp( BOOLEAN, ffvsp[-2].Node, AND, ffvsp[0].Node ); TEST(ffval.Node); ;
    break;}
case 63:
#line 453 "eval.y"
{ ffval.Node = New_BinOp( BOOLEAN, ffvsp[-2].Node, OR,  ffvsp[0].Node ); TEST(ffval.Node); ;
    break;}
case 64:
#line 455 "eval.y"
{ ffval.Node = New_BinOp( BOOLEAN, ffvsp[-2].Node, EQ,  ffvsp[0].Node ); TEST(ffval.Node); ;
    break;}
case 65:
#line 457 "eval.y"
{ ffval.Node = New_BinOp( BOOLEAN, ffvsp[-2].Node, NE,  ffvsp[0].Node ); TEST(ffval.Node); ;
    break;}
case 66:
#line 460 "eval.y"
{ PROMOTE(ffvsp[-4].Node,ffvsp[-2].Node); PROMOTE(ffvsp[-4].Node,ffvsp[0].Node); PROMOTE(ffvsp[-2].Node,ffvsp[0].Node);
		  ffvsp[-2].Node = New_BinOp( BOOLEAN, ffvsp[-2].Node, LTE, ffvsp[-4].Node );
                  ffvsp[0].Node = New_BinOp( BOOLEAN, ffvsp[-4].Node, LTE, ffvsp[0].Node );
                  ffval.Node = New_BinOp( BOOLEAN, ffvsp[-2].Node, AND, ffvsp[0].Node );
                  TEST(ffval.Node);                                         ;
    break;}
case 67:
#line 467 "eval.y"
{
		   if (FSTRCMP(ffvsp[-2].str,"ISNULL(") == 0) {
		      ffval.Node = New_Func( 0, isnull_fct, 1, ffvsp[-1].Node, 0, 0,
				     0, 0, 0, 0 );
		      TEST(ffval.Node); 
                      /* Use expression's size, but return BOOLEAN */
		      TYPE(ffval.Node) = BOOLEAN;
		   } else {
		      fferror("Boolean Function(expr) not supported");
		      FFERROR;
		   }
		;
    break;}
case 68:
#line 480 "eval.y"
{
		   if (FSTRCMP(ffvsp[-2].str,"ISNULL(") == 0) {
		      ffval.Node = New_Func( 0, isnull_fct, 1, ffvsp[-1].Node, 0, 0,
				     0, 0, 0, 0 );
		      TEST(ffval.Node); 
                      /* Use expression's size, but return BOOLEAN */
		      TYPE(ffval.Node) = BOOLEAN;
		   } else {
		      fferror("Boolean Function(expr) not supported");
		      FFERROR;
		   }
		;
    break;}
case 69:
#line 493 "eval.y"
{
		   if (FSTRCMP(ffvsp[-2].str,"ISNULL(") == 0) {
		      ffval.Node = New_Func( BOOLEAN, isnull_fct, 1, ffvsp[-1].Node, 0, 0,
				     0, 0, 0, 0 );
		      TEST(ffval.Node); 
		   } else {
		      fferror("Boolean Function(expr) not supported");
		      FFERROR;
		   }
		;
    break;}
case 70:
#line 504 "eval.y"
{
		   if (FSTRCMP(ffvsp[-4].str,"DEFNULL(") == 0) {
		      if( SIZE(ffvsp[-3].Node)>=SIZE(ffvsp[-1].Node) && Test_Dims( ffvsp[-3].Node, ffvsp[-1].Node ) ) {
			 ffval.Node = New_Func( 0, defnull_fct, 2, ffvsp[-3].Node, ffvsp[-1].Node, 0,
					0, 0, 0, 0 );
			 TEST(ffval.Node); 
		      } else {
			 fferror("Dimensions of DEFNULL arguments are not compatible");
			 FFERROR;
		      }
		   } else {
		      fferror("Boolean Function(expr,expr) not supported");
		      FFERROR;
		   }
		;
    break;}
case 71:
#line 520 "eval.y"
{
		   if( SIZE(ffvsp[-5].Node)>1 || SIZE(ffvsp[-3].Node)>1 || SIZE(ffvsp[-1].Node)>1 ) {
		      fferror("Cannot use array as function argument");
		      FFERROR;
		   }
		   if( TYPE(ffvsp[-5].Node) != DOUBLE ) ffvsp[-5].Node = New_Unary( DOUBLE, 0, ffvsp[-5].Node );
		   if( TYPE(ffvsp[-3].Node) != DOUBLE ) ffvsp[-3].Node = New_Unary( DOUBLE, 0, ffvsp[-3].Node );
		   if( TYPE(ffvsp[-1].Node) != DOUBLE ) ffvsp[-1].Node = New_Unary( DOUBLE, 0, ffvsp[-1].Node );
		   if (FSTRCMP(ffvsp[-6].str,"NEAR(") == 0)
		      ffval.Node = New_Func( BOOLEAN, near_fct, 3, ffvsp[-5].Node, ffvsp[-3].Node, ffvsp[-1].Node,
				     0, 0, 0, 0 );
		   else {
		      fferror("Boolean Function not supported");
		      FFERROR;
		   }
                   TEST(ffval.Node); 
		;
    break;}
case 72:
#line 538 "eval.y"
{
		   if( SIZE(ffvsp[-9].Node)>1 || SIZE(ffvsp[-7].Node)>1 || SIZE(ffvsp[-5].Node)>1 || SIZE(ffvsp[-3].Node)>1
		       || SIZE(ffvsp[-1].Node)>1 ) {
		      fferror("Cannot use array as function argument");
		      FFERROR;
		   }
		   if( TYPE(ffvsp[-9].Node) != DOUBLE ) ffvsp[-9].Node = New_Unary( DOUBLE, 0, ffvsp[-9].Node );
		   if( TYPE(ffvsp[-7].Node) != DOUBLE ) ffvsp[-7].Node = New_Unary( DOUBLE, 0, ffvsp[-7].Node );
		   if( TYPE(ffvsp[-5].Node) != DOUBLE ) ffvsp[-5].Node = New_Unary( DOUBLE, 0, ffvsp[-5].Node );
		   if( TYPE(ffvsp[-3].Node) != DOUBLE ) ffvsp[-3].Node = New_Unary( DOUBLE, 0, ffvsp[-3].Node );
		   if( TYPE(ffvsp[-1].Node)!= DOUBLE ) ffvsp[-1].Node= New_Unary( DOUBLE, 0, ffvsp[-1].Node);
                   if (FSTRCMP(ffvsp[-10].str,"CIRCLE(") == 0)
		      ffval.Node = New_Func( BOOLEAN, circle_fct, 5, ffvsp[-9].Node, ffvsp[-7].Node, ffvsp[-5].Node, ffvsp[-3].Node,
				     ffvsp[-1].Node, 0, 0 );
		   else {
		      fferror("Boolean Function not supported");
		      FFERROR;
		   }
                   TEST(ffval.Node); 
		;
    break;}
case 73:
#line 559 "eval.y"
{
		   if( SIZE(ffvsp[-13].Node)>1 || SIZE(ffvsp[-11].Node)>1 || SIZE(ffvsp[-9].Node)>1 || SIZE(ffvsp[-7].Node)>1
		       || SIZE(ffvsp[-5].Node)>1 || SIZE(ffvsp[-3].Node)>1 || SIZE(ffvsp[-1].Node)>1 ) {
		      fferror("Cannot use array as function argument");
		      FFERROR;
		   }
		   if( TYPE(ffvsp[-13].Node) != DOUBLE ) ffvsp[-13].Node = New_Unary( DOUBLE, 0, ffvsp[-13].Node );
		   if( TYPE(ffvsp[-11].Node) != DOUBLE ) ffvsp[-11].Node = New_Unary( DOUBLE, 0, ffvsp[-11].Node );
		   if( TYPE(ffvsp[-9].Node) != DOUBLE ) ffvsp[-9].Node = New_Unary( DOUBLE, 0, ffvsp[-9].Node );
		   if( TYPE(ffvsp[-7].Node) != DOUBLE ) ffvsp[-7].Node = New_Unary( DOUBLE, 0, ffvsp[-7].Node );
		   if( TYPE(ffvsp[-5].Node)!= DOUBLE ) ffvsp[-5].Node= New_Unary( DOUBLE, 0, ffvsp[-5].Node);
		   if( TYPE(ffvsp[-3].Node)!= DOUBLE ) ffvsp[-3].Node= New_Unary( DOUBLE, 0, ffvsp[-3].Node);
		   if( TYPE(ffvsp[-1].Node)!= DOUBLE ) ffvsp[-1].Node= New_Unary( DOUBLE, 0, ffvsp[-1].Node);
		   if (FSTRCMP(ffvsp[-14].str,"BOX(") == 0)
		      ffval.Node = New_Func( BOOLEAN, box_fct, 7, ffvsp[-13].Node, ffvsp[-11].Node, ffvsp[-9].Node, ffvsp[-7].Node,
				      ffvsp[-5].Node, ffvsp[-3].Node, ffvsp[-1].Node );
		   else if (FSTRCMP(ffvsp[-14].str,"ELLIPSE(") == 0)
		      ffval.Node = New_Func( BOOLEAN, elps_fct, 7, ffvsp[-13].Node, ffvsp[-11].Node, ffvsp[-9].Node, ffvsp[-7].Node,
				      ffvsp[-5].Node, ffvsp[-3].Node, ffvsp[-1].Node );
		   else {
		      fferror("SAO Image Function not supported");
		      FFERROR;
		   }
                   TEST(ffval.Node); 
		;
    break;}
case 74:
#line 585 "eval.y"
{ /* Use defaults for all except filename */
                   if (FSTRCMP(ffvsp[-1].str,"GTIFILTER(") == 0) {
                      ffval.Node = New_GTI( "", -99, "*START*", "*STOP*" );
                   } else {
                      fferror("Filter Function not supported");
		      FFERROR;
		   }
                   TEST(ffval.Node); 
                ;
    break;}
case 75:
#line 595 "eval.y"
{ /* Use defaults for all except filename */
                   if (FSTRCMP(ffvsp[-2].str,"GTIFILTER(") == 0) {
                      ffval.Node = New_GTI( ffvsp[-1].str, -99, "*START*", "*STOP*" );
                   } else {
                      fferror("Filter Function not supported");
		      FFERROR;
		   }
                   TEST(ffval.Node); 
                ;
    break;}
case 76:
#line 605 "eval.y"
{
                   if (FSTRCMP(ffvsp[-4].str,"GTIFILTER(") == 0) {
                      if( SIZE(ffvsp[-1].Node)>1 ) {
                         fferror("Cannot filter on a vector column");
                         FFERROR;
                      }
		      if( gParse.Nodes[ffvsp[-1].Node].operation==-1000 ) {
                         fferror("Cannot filter on a constant");
                         FFERROR;
                      }
                      ffval.Node = New_GTI( ffvsp[-3].str, ffvsp[-1].Node, "*START*", "*STOP*" );
                   } else {
                      fferror("Filter Function not supported");
		      FFERROR;
		   }
                   TEST(ffval.Node); 
                ;
    break;}
case 77:
#line 623 "eval.y"
{
                   if (FSTRCMP(ffvsp[-8].str,"GTIFILTER(") == 0) {
                      if( SIZE(ffvsp[-5].Node)>1 ) {
                         fferror("Cannot filter on a vector column");
                         FFERROR;
                      }
		      if( gParse.Nodes[ffvsp[-5].Node].operation==-1000 ) {
                         fferror("Cannot filter on a constant");
                         FFERROR;
                      }
                      ffval.Node = New_GTI( ffvsp[-7].str, ffvsp[-5].Node, ffvsp[-3].str, ffvsp[-1].str );
                   } else {
                      fferror("Filter Function not supported");
		      FFERROR;
		   }
                   TEST(ffval.Node); 
                ;
    break;}
case 78:
#line 641 "eval.y"
{ ffval.Node = New_Deref( ffvsp[-3].Node, 1, ffvsp[-1].Node,  0,  0,  0,   0 ); TEST(ffval.Node); ;
    break;}
case 79:
#line 643 "eval.y"
{ ffval.Node = New_Deref( ffvsp[-5].Node, 2, ffvsp[-3].Node, ffvsp[-1].Node,  0,  0,   0 ); TEST(ffval.Node); ;
    break;}
case 80:
#line 645 "eval.y"
{ ffval.Node = New_Deref( ffvsp[-7].Node, 3, ffvsp[-5].Node, ffvsp[-3].Node, ffvsp[-1].Node,  0,   0 ); TEST(ffval.Node); ;
    break;}
case 81:
#line 647 "eval.y"
{ ffval.Node = New_Deref( ffvsp[-9].Node, 4, ffvsp[-7].Node, ffvsp[-5].Node, ffvsp[-3].Node, ffvsp[-1].Node,   0 ); TEST(ffval.Node); ;
    break;}
case 82:
#line 649 "eval.y"
{ ffval.Node = New_Deref( ffvsp[-11].Node, 5, ffvsp[-9].Node, ffvsp[-7].Node, ffvsp[-5].Node, ffvsp[-3].Node, ffvsp[-1].Node ); TEST(ffval.Node); ;
    break;}
case 83:
#line 651 "eval.y"
{ ffval.Node = New_Unary( BOOLEAN, NOT, ffvsp[0].Node ); TEST(ffval.Node); ;
    break;}
case 84:
#line 653 "eval.y"
{ ffval.Node = ffvsp[-1].Node; ;
    break;}
case 85:
#line 657 "eval.y"
{ ffval.Node = New_Const( STRING, ffvsp[0].str, strlen(ffvsp[0].str)+1 ); TEST(ffval.Node);
                  SIZE(ffval.Node) = strlen(ffvsp[0].str);                            ;
    break;}
case 86:
#line 660 "eval.y"
{ ffval.Node = New_Column( ffvsp[0].lng ); TEST(ffval.Node); ;
    break;}
case 87:
#line 662 "eval.y"
{ ffval.Node = ffvsp[-1].Node; ;
    break;}
case 88:
#line 664 "eval.y"
{ ffval.Node = New_BinOp( STRING, ffvsp[-2].Node, '+', ffvsp[0].Node );  TEST(ffval.Node);
		  SIZE(ffval.Node) = SIZE(ffvsp[-2].Node) + SIZE(ffvsp[0].Node);                   ;
    break;}
case 89:
#line 667 "eval.y"
{ 
		  if (FSTRCMP(ffvsp[-4].str,"DEFNULL(") == 0) {
		     ffval.Node = New_Func( 0, defnull_fct, 2, ffvsp[-3].Node, ffvsp[-1].Node, 0,
				    0, 0, 0, 0 );
		     TEST(ffval.Node); 
		     if( SIZE(ffvsp[-1].Node)>SIZE(ffvsp[-3].Node) ) SIZE(ffval.Node) = SIZE(ffvsp[-1].Node);
		  }
		;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 498 "/usr/local/share/bison.simple"

  ffvsp -= fflen;
  ffssp -= fflen;
#ifdef FFLSP_NEEDED
  fflsp -= fflen;
#endif

#if FFDEBUG != 0
  if (ffdebug)
    {
      short *ssp1 = ffss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != ffssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++ffvsp = ffval;

#ifdef FFLSP_NEEDED
  fflsp++;
  if (fflen == 0)
    {
      fflsp->first_line = fflloc.first_line;
      fflsp->first_column = fflloc.first_column;
      fflsp->last_line = (fflsp-1)->last_line;
      fflsp->last_column = (fflsp-1)->last_column;
      fflsp->text = 0;
    }
  else
    {
      fflsp->last_line = (fflsp+fflen-1)->last_line;
      fflsp->last_column = (fflsp+fflen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  ffn = ffr1[ffn];

  ffstate = ffpgoto[ffn - FFNTBASE] + *ffssp;
  if (ffstate >= 0 && ffstate <= FFLAST && ffcheck[ffstate] == *ffssp)
    ffstate = fftable[ffstate];
  else
    ffstate = ffdefgoto[ffn - FFNTBASE];

  goto ffnewstate;

fferrlab:   /* here on detecting error */

  if (! fferrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++ffnerrs;

#ifdef FFERROR_VERBOSE
      ffn = ffpact[ffstate];

      if (ffn > FFFLAG && ffn < FFLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -ffn if nec to avoid negative indexes in ffcheck.  */
	  for (x = (ffn < 0 ? -ffn : 0);
	       x < (sizeof(fftname) / sizeof(char *)); x++)
	    if (ffcheck[x + ffn] == x)
	      size += strlen(fftname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (ffn < 0 ? -ffn : 0);
		       x < (sizeof(fftname) / sizeof(char *)); x++)
		    if (ffcheck[x + ffn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, fftname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      fferror(msg);
	      free(msg);
	    }
	  else
	    fferror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* FFERROR_VERBOSE */
	fferror("parse error");
    }

  goto fferrlab1;
fferrlab1:   /* here on error raised explicitly by an action */

  if (fferrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (ffchar == FFEOF)
	FFABORT;

#if FFDEBUG != 0
      if (ffdebug)
	fprintf(stderr, "Discarding token %d (%s).\n", ffchar, fftname[ffchar1]);
#endif

      ffchar = FFEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  fferrstatus = 3;		/* Each real token shifted decrements this */

  goto fferrhandle;

fferrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  ffn = ffdefact[ffstate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (ffn) goto ffdefault;
#endif

fferrpop:   /* pop the current state because it cannot handle the error token */

  if (ffssp == ffss) FFABORT;
  ffvsp--;
  ffstate = *--ffssp;
#ifdef FFLSP_NEEDED
  fflsp--;
#endif

#if FFDEBUG != 0
  if (ffdebug)
    {
      short *ssp1 = ffss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != ffssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

fferrhandle:

  ffn = ffpact[ffstate];
  if (ffn == FFFLAG)
    goto fferrdefault;

  ffn += FFTERROR;
  if (ffn < 0 || ffn > FFLAST || ffcheck[ffn] != FFTERROR)
    goto fferrdefault;

  ffn = fftable[ffn];
  if (ffn < 0)
    {
      if (ffn == FFFLAG)
	goto fferrpop;
      ffn = -ffn;
      goto ffreduce;
    }
  else if (ffn == 0)
    goto fferrpop;

  if (ffn == FFFINAL)
    FFACCEPT;

#if FFDEBUG != 0
  if (ffdebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++ffvsp = fflval;
#ifdef FFLSP_NEEDED
  *++fflsp = fflloc;
#endif

  ffstate = ffn;
  goto ffnewstate;
}
#line 677 "eval.y"


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
      this->operation  = -1000;             /* Flag a constant */
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
      this->type        = gParse.colInfo[ColNum].type;
      this->value.nelem = gParse.colInfo[ColNum].nelem;
      this->value.naxis = gParse.colInfo[ColNum].naxis;
      for( i=0; i<gParse.colInfo[ColNum].naxis; i++ )
	 this->value.naxes[i] = gParse.colInfo[ColNum].naxes[i];
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

   if( that->operation==-1000 ) {  /* Operating on a constant! */
      switch( Op ) {
      case DOUBLE:
      case FLTCAST:
	 if( that->type==LONG )
	    that->value.data.dbl = (double)that->value.data.lng;
	 else if( that->type==BOOLEAN )
	    that->value.data.dbl = ( that->value.data.log ? 1.0 : 0.0 );
	 that->type=DOUBLE;
	 return(Node1);
	 break;
      case LONG:
      case INTCAST:
	 if( that->type==DOUBLE )
	    that->value.data.lng = (long)that->value.data.dbl;
	 else if( that->type==BOOLEAN )
	    that->value.data.lng = ( that->value.data.log ? 1L : 0L );
	 that->type=LONG;
	 return(Node1);
	 break;
      case BOOLEAN:
	 if( that->type==DOUBLE )
	    that->value.data.log = ( that->value.data.dbl != 0.0 );
	 else if( that->type==LONG )
	    that->value.data.log = ( that->value.data.lng != 0L );
	 that->type=BOOLEAN;
	 return(Node1);
	 break;
      case UMINUS:
	 if( that->type==DOUBLE )
	    that->value.data.dbl = - that->value.data.dbl;
	 else if( that->type==LONG )
	    that->value.data.lng = - that->value.data.lng;
	 return(Node1);
	 break;
      case NOT:
	 if( that->type==BOOLEAN )
	    that->value.data.log = ( ! that->value.data.log );
	 else if( that->type==BITSTR )
	    bitnot( that->value.data.str, that->value.data.str );
	 return(Node1);
	 break;
      }
   }
   
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
   }
   return( n );
}

static int New_BinOp( int returnType, int Node1, int Op, int Node2 )
{
   Node *this,*that1,*that2;
   int  n,i;

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
      if( that1->type!=STRING && that1->type!=BITSTR )
	 if( !Test_Dims( Node1, Node2 ) ) {
	    Free_Last_Node();
	    ffpmsg("Array sizes/dims do not match for binary operator");
	    return(-1);
	 }
      if( that1->value.nelem == 1 ) that1 = that2;

      this->value.nelem = that1->value.nelem;
      this->value.naxis = that1->value.naxis;
      for( i=0; i<that1->value.naxis; i++ )
	 this->value.naxes[i] = that1->value.naxes[i];

	 /*  Both subnodes should be of same time  */
      switch( that1->type ) {
      case BITSTR:  this->DoOp = Do_BinOp_bit;  break;
      case STRING:  this->DoOp = Do_BinOp_str;  break;
      case BOOLEAN: this->DoOp = Do_BinOp_log;  break;
      case LONG:    this->DoOp = Do_BinOp_lng;  break;
      case DOUBLE:  this->DoOp = Do_BinOp_dbl;  break;
      }
   }
   return( n );
}

static int New_Func( int returnType, funcOp Op, int nNodes,
		     int Node1, int Node2, int Node3, int Node4, 
		     int Node5, int Node6, int Node7 )
/* If returnType==0 , use Node1's type and vector sizes as returnType, */
/* else return a single value of type returnType                       */
{
   Node *this, *that;
   int  i,n;

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
   }
   return( n );
}

static int New_Deref( int Var,  int nDim,
		      int Dim1, int Dim2, int Dim3, int Dim4, int Dim5 )
{
   int n, idx;
   long elem=0;
   Node *this, *theVar, *theDim[MAXDIMS];

   if( Var<0 || Dim1<0 || Dim2<0 || Dim3<0 || Dim4<0 || Dim5<0 ) return(-1);

   theVar = gParse.Nodes + Var;
   if( theVar->operation==-1000 || theVar->value.nelem==1 ) {
      ffpmsg("Cannot index a scalar value");
      gParse.status = PARSE_SYNTAX_ERR;
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

      for( idx=0; idx<nDim; idx++ )
	 if( theDim[idx]->value.nelem>1 ) {
	    Free_Last_Node();
	    ffpmsg("Cannot use an array as an index value");
	    gParse.status = PARSE_SYNTAX_ERR;
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
   }
   return(n);
}

static int New_GTI( char *fname, int Node1, char *start, char *stop )
{
   fitsfile *fptr;
   Node *this, *that0;
   int  type,i,n, startCol, stopCol, Node0;
   int  hdutype, hdunum, evthdu, samefile, extvers, movetotype, tstat;
   char extname[100];
   long colnum, nrows;
   double timeZeroI[2], timeZeroF[2], dt, timeSpan;

   if( Node1==-99 ) {
      type = ffbuildcolumn( "TIME", &colnum );
      if( (type==LONG || type==DOUBLE) && gParse.colInfo[colnum].nelem==1 ) {
	 Node1 = New_Column( colnum );
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
		 &gParse.status );
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
      this->value.nelem    = 1;
      this->value.naxis    = 1;
      this->value.naxes[0] = 1;

      /* Init START/STOP node to be treated as a "constant" */

      this->SubNodes[0]    = Node0;
      that0                = gParse.Nodes + Node0;
      that0->operation     = -1000;
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
	 
	 /*  Handle TIMEZERO offset, if any  */
	 
	 dt = (timeZeroI[1] - timeZeroI[0]) + (timeZeroF[1] - timeZeroF[0]);
	 timeSpan = that0->value.data.dblptr[nrows+nrows-1]
	    - that0->value.data.dblptr[0];
	 
	 if( fabs( dt / timeSpan ) > 1e-12 ) {
	    for( i=0; i<(nrows+nrows); i++ )
	       that0->value.data.dblptr[i] += dt;
	 }
      }
   }

   if( samefile )
      ffmahd( fptr, evthdu, &hdutype, &gParse.status );
   else
      ffclos( fptr, &gParse.status );
   return( n );
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

/********************************************************************/
/*    Routines for actually evaluating the expression start here    */
/********************************************************************/

void Evaluate_Node( int thisNode )
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

void Reset_Parser( long firstRow, long rowOffset, long nRows )
    /***********************************************************************/
    /*  Reset the parser for processing another batch of data...           */
    /*    firstRow:  Row number of the first element of the iterCol.array  */
    /*    rowOffset: How many rows of iterCol.array should be skipped      */
    /*    nRows:     Number of rows to be processed                        */
    /*  Then, allocate and initialize the necessary UNDEF arrays for each  */
    /*  column used by the parser.  Finally, initialize each COLUMN node   */
    /*  so that its UNDEF and DATA pointers point to the appropriate       */
    /*  column arrays.                                                     */
    /***********************************************************************/
{
   int     i, column;
   long    nelem, len, row, offset, idx;
   char  **bitStrs;
   char  **sptr;
   char   *barray;
   long   *iarray;
   double *rarray;

   gParse.nRows    = nRows;
   gParse.firstRow = firstRow + rowOffset;

   /*  Resize and fill in UNDEF arrays for each column  */

   for( i=0; i<gParse.nCols; i++ ) {
      if( gParse.colData[i].iotype == OutputCol ) continue;

      nelem  = gParse.colInfo[i].nelem;
      len    = nelem * nRows;
      offset = nelem * rowOffset + 1; /* Skip initial NULLVAL in [0] elem */

      switch ( gParse.colInfo[i].type ) {
      case BITSTR:
      /* No need for UNDEF array, but must make string DATA array */
	 len = (nelem+1)*nRows;   /* Count '\0' */
	 bitStrs = ((char***)gParse.colNulls)[i];
	 if( bitStrs ) free( bitStrs[0] );
	 free( bitStrs );
	 bitStrs = (char**)malloc( nRows*sizeof(char*) );
	 if( bitStrs==NULL ) {
	    gParse.status = MEMORY_ALLOCATION;
	    break;
	 }
	 bitStrs[0] = (char*)malloc( len*sizeof(char) );
	 if( bitStrs[0]==NULL ) {
	    free( bitStrs );
	    gParse.colNulls[i] = NULL;
	    gParse.status = MEMORY_ALLOCATION;
	    break;
	 }

	 for( row=0; row<gParse.nRows; row++ ) {
	    bitStrs[row] = bitStrs[0] + row*(nelem+1);
	    idx = (row+rowOffset)*( (nelem+7)/8 ) + 1;
	    for(len=0; len<nelem; len++) {
	       if( ((char*)gParse.colData[i].array)[idx] & (1<<(7-len%8)) )
		  bitStrs[row][len] = '1';
	       else
		  bitStrs[row][len] = '0';
	       if( len%8==7 ) idx++;
	    }
	    bitStrs[row][len] = '\0';
	 }
	 gParse.colNulls[i] = (char*)bitStrs;
	 break;
      case STRING:
	 sptr = (char**)gParse.colData[i].array;
	 free( gParse.colNulls[i] );
	 gParse.colNulls[i] = (char*)malloc( nRows*sizeof(char) );
	 if( gParse.colNulls[i]==NULL ) {
	    gParse.status = MEMORY_ALLOCATION;
	    break;
	 }
	 for( row=0; row<nRows; row++ ) {
	    if( **sptr != '\0' && FSTRCMP( sptr[0], sptr[row+rowOffset+1] )==0 )
	       gParse.colNulls[i][row] = 1;
	    else
	       gParse.colNulls[i][row] = 0;
	 }
	 break;
      case BOOLEAN:
	 barray = (char*)gParse.colData[i].array;
	 free( gParse.colNulls[i] );
	 gParse.colNulls[i] = (char*)malloc( len*sizeof(char) );
	 if( gParse.colNulls[i]==NULL ) {
	    gParse.status = MEMORY_ALLOCATION;
	    break;
	 }
	 while( len-- ) {
	    gParse.colNulls[i][len] = 
	       ( barray[0]!=0 && barray[0]==barray[len+offset] );
	 }
	 break;
      case LONG:
	 iarray = (long*)gParse.colData[i].array;
	 free( gParse.colNulls[i] );
	 gParse.colNulls[i] = (char*)malloc( len*sizeof(char) );
	 if( gParse.colNulls[i]==NULL ) {
	    gParse.status = MEMORY_ALLOCATION;
	    break;
	 }
	 while( len-- ) {
	    gParse.colNulls[i][len] = 
	       ( iarray[0]!=0L && iarray[0]==iarray[len+offset] );
	 }
	 break;
      case DOUBLE:
	 rarray = (double*)gParse.colData[i].array;
	 free( gParse.colNulls[i] );
	 gParse.colNulls[i] = (char*)malloc( len*sizeof(char) );
	 if( gParse.colNulls[i]==NULL ) {
	    gParse.status = MEMORY_ALLOCATION;
	    break;
	 }
	 while( len-- ) {
	    gParse.colNulls[i][len] = 
	       ( rarray[0]!=0.0 && rarray[0]==rarray[len+offset]);
	 }
	 break;
      }
      if( gParse.status ) {  /*  Deallocate NULL arrays of previous columns */
	 while( i-- ) {
	    if( gParse.colInfo[i].type==BITSTR )
	       free( ((char***)gParse.colNulls)[i][0] );
	    free( gParse.colNulls[i] );
	    gParse.colNulls[i] = NULL;
	 }
	 return;
      }
   }

   /*  Reset Column Nodes' pointers to point to right data and UNDEF arrays  */

   for( i=0; i<gParse.nNodes; i++ ) {
      if(    gParse.Nodes[i].operation >  0
	  || gParse.Nodes[i].operation == -1000 ) continue;

      column = -gParse.Nodes[i].operation;
      offset = gParse.colInfo[column].nelem * rowOffset + 1;

      gParse.Nodes[i].value.undef = gParse.colNulls[column];

      switch( gParse.Nodes[i].type ) {
      case BITSTR:
	 gParse.Nodes[i].value.data.strptr = ((char***)gParse.colNulls)[column];
	 gParse.Nodes[i].value.undef       = NULL;
	 break;
      case STRING:
	 gParse.Nodes[i].value.data.strptr = 
	    ((char**)gParse.colData[column].array)+rowOffset+1;
	 break;
      case BOOLEAN:
	 gParse.Nodes[i].value.data.logptr = 
	    ((char*)gParse.colData[column].array)+offset;
	 break;
      case LONG:
	 gParse.Nodes[i].value.data.lngptr = 
	    ((long*)gParse.colData[column].array)+offset;
	 break;
      case DOUBLE:
	 gParse.Nodes[i].value.data.dblptr = 
	    ((double*)gParse.colData[column].array)+offset;
	 break;
      }
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

      this->value.data.ptr = malloc( elem*(size+1) );
      
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

   /*  New_Unary pre-evaluates operations on constants,  */
   /*  so no need to worry about that case               */

   that = gParse.Nodes + this->SubNodes[0];

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

   if( that->operation>0 ) {
      free( that->value.data.ptr );
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

   const1 = ( that1->operation==-1000 );
   const2 = ( that2->operation==-1000 );
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
      }
      this->operation = -1000;

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

   const1 = ( that1->operation==-1000 );
   const2 = ( that2->operation==-1000 );
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

	 /*  Concat Strings  */

      case '+':
	 strcpy( this->value.data.str, sptr1 );
	 strcat( this->value.data.str, sptr2 );
	 break;
      }
      this->operation = -1000;

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

   vector1 = ( that1->operation!=-1000 );
   if( vector1 )
      vector1 = that1->value.nelem;
   else {
      val1  = that1->value.data.log;
   }

   vector2 = ( that2->operation!=-1000 );
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
      }
      this->operation=-1000;
   } else {
      rows  = gParse.nRows;
      nelem = this->value.nelem;
      elem  = this->value.nelem * rows;

      Allocate_Ptrs( this );

      if( !gParse.status ) {
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
		  this->value.data.logptr[elem] = (val1 && val2);
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

   vector1 = ( that1->operation!=-1000 );
   if( vector1 )
      vector1 = that1->value.nelem;
   else {
      val1  = that1->value.data.lng;
   }

   vector2 = ( that2->operation!=-1000 );
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
      }
      this->operation=-1000;

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

	    case '%':   
	       if( val2 ) this->value.data.lngptr[elem] = (val1 % val2);
	       else {
		  fferror("Divide by Zero");
		  free( this->value.data.ptr );
	       }
	       break;
	    case '/': 
	       if( val2 ) this->value.data.lngptr[elem] = (val1 / val2); 
	       else {
		  fferror("Divide by Zero");
		  free( this->value.data.ptr );
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

   vector1 = ( that1->operation!=-1000 );
   if( vector1 )
      vector1 = that1->value.nelem;
   else {
      val1  = that1->value.data.dbl;
   }

   vector2 = ( that2->operation!=-1000 );
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
      }
      this->operation=-1000;

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
		  fferror("Divide by Zero");
		  free( this->value.data.ptr );
	       }
	       break;
	    case '/': 
	       if( val2 ) this->value.data.dblptr[elem] = (val1 / val2); 
	       else {
		  fferror("Divide by Zero");
		  free( this->value.data.ptr );
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

static void Do_Func( Node *this )
{
   Node *theParams[MAXSUBS];
   int  vector[MAXSUBS], allConst;
   lval pVals[MAXSUBS];
   char pNull[MAXSUBS];
   long   ival;
   double dval;
   int  i;
   long row, elem, nelem;

   i = this->nSubNodes;
   allConst = 1;
   while( i-- ) {
      theParams[i] = gParse.Nodes + this->SubNodes[i];
      vector[i]   = ( theParams[i]->operation!=-1000 );
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

   if( allConst ) {

      switch( this->operation ) {

	    /* Non-Trig single-argument functions */

	 case sum_fct:
	    if( theParams[0]->type==BOOLEAN )
	       this->value.data.lng = ( pVals[0].data.log ? 1 : 0 );
	    else if( theParams[0]->type==LONG )
	       this->value.data.lng = pVals[0].data.lng;
	    else
	       this->value.data.dbl = pVals[0].data.dbl;
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

	    /* Trig functions with 1 double argument */

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
	    dval = pVals[0].data.dbl;
	    this->value.data.dbl = atan( dval );
	    break;
	 case exp_fct:
	    dval = pVals[0].data.dbl;
	    this->value.data.dbl = exp( dval );
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

	    /* Two-argument Trig Functions */

	 case atan2_fct:
	    this->value.data.dbl =
	       atan2( pVals[0].data.dbl, pVals[1].data.dbl );
	    break;

	    /* Boolean SAO region Functions... all arguments scalar dbls */

	 case near_fct:
	    this->value.data.log = near( pVals[0].data.dbl, pVals[1].data.dbl,
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
      }
      this->operation = -1000;

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
	 case rnd_fct:
	    while( row-- ) {
	       this->value.data.dblptr[row] = (double)rand() / 2147483647; 
	       this->value.undef[row] = 0;
	    }
	    break;

	    /* Non-Trig single-argument functions */
	    
	 case sum_fct:
	    elem = row * theParams[0]->value.nelem;
	    if( theParams[0]->type==BOOLEAN ) {
	       while( row-- ) {
		  this->value.data.lngptr[row] = 0;
		  this->value.undef[row] = 0;
		  nelem = theParams[0]->value.nelem;
		  while( nelem-- ) {
		     elem--;
		     this->value.data.lngptr[row] +=
			( theParams[0]->value.data.logptr[elem] ? 1 : 0 );
		     this->value.undef[row] |=
			  theParams[0]->value.undef[elem];
		  }
	       }		  
	    } else if( theParams[0]->type==LONG ) {
	       while( row-- ) {
		  this->value.data.lngptr[row] = 0;
		  this->value.undef[row] = 0;
		  nelem = theParams[0]->value.nelem;
		  while( nelem-- ) {
		     elem--;
		     this->value.data.lngptr[row] +=
			theParams[0]->value.data.lngptr[elem];
		     this->value.undef[row] |=
			  theParams[0]->value.undef[elem];
		  }
	       }		  
	    } else {
	       while( row-- ) {
		  this->value.data.dblptr[row] = 0.0;
		  this->value.undef[row] = 0;
		  nelem = theParams[0]->value.nelem;
		  while( nelem-- ) {
		     elem--;
		     this->value.data.dblptr[row] +=
			theParams[0]->value.data.dblptr[elem];
		     this->value.undef[row] |=
			  theParams[0]->value.undef[elem];
		  }
	       }		  
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
		     this->value.undef[elem] = pNull[1];
		     strcpy(this->value.data.strptr[elem],pVals[1].data.str);
		  } else {
		     this->value.undef[elem] = 0;
		     strcpy(this->value.data.strptr[elem],pVals[0].data.str);
		  }
	       }
	    }
	    break;

	    /* Trig functions with 1 double argument */

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
		     fferror("Out of range argument to arcsin");
		     free( this->value.data.ptr );
		     break;
		  } else
		     this->value.data.dblptr[elem] = asin( dval );
	       }
	    break;
	 case acos_fct:
	    while( elem-- )
	       if( !(this->value.undef[elem] = theParams[0]->value.undef[elem]) ) {
		  dval = theParams[0]->value.data.dblptr[elem];
		  if( dval<-1.0 || dval>1.0 ) {
		     fferror("Out of range argument to arccos");
		     free( this->value.data.ptr );
		     break;
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
		     fferror("Out of range argument to log");
		     free( this->value.data.ptr );
		     break;
		  } else
		     this->value.data.dblptr[elem] = log( dval );
	       }
	    break;
	 case log10_fct:
	    while( elem-- )
	       if( !(this->value.undef[elem] = theParams[0]->value.undef[elem]) ) {
		  dval = theParams[0]->value.data.dblptr[elem];
		  if( dval<=0.0 ) {
		     fferror("Out of range argument to log10");
		     free( this->value.data.ptr );
		     break;
		  } else
		     this->value.data.dblptr[elem] = log10( dval );
	       }
	    break;
	 case sqrt_fct:
	    while( elem-- )
	       if( !(this->value.undef[elem] = theParams[0]->value.undef[elem]) ) {
		  dval = theParams[0]->value.data.dblptr[elem];
		  if( dval<0.0 ) {
		     fferror("Out of range argument to sqrt");
		     free( this->value.data.ptr );
		     break;
		  } else
		     this->value.data.dblptr[elem] = sqrt( dval );
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

	    /* Boolean SAO region Functions... all arguments scalar dbls */

	 case near_fct:
	    while( row-- ) {
	       this->value.undef[row] = 0;
	       i=3; while( i-- )
		  if( vector[i] ) {
		     pVals[i].data.dbl = theParams[i]->value.data.dblptr[row];
		     this->value.undef[row] |= theParams[i]->value.undef[row];
		  }
	       if( !(this->value.undef[row]) )
		  this->value.data.logptr[row] =
		     near( pVals[0].data.dbl, pVals[1].data.dbl,
			   pVals[2].data.dbl );
	    }
	    break;
	 case circle_fct:
	    while( row-- ) {
	       this->value.undef[row] = 0;
	       i=5; while( i-- )
		  if( vector[i] ) {
		     pVals[i].data.dbl = theParams[i]->value.data.dblptr[row];
		     this->value.undef[row] |= theParams[i]->value.undef[row];
		  }
	       if( !(this->value.undef[row]) )
		  this->value.data.logptr[row] =
		     circle( pVals[0].data.dbl, pVals[1].data.dbl,
			     pVals[2].data.dbl, pVals[3].data.dbl,
			     pVals[4].data.dbl );
	    }
	    break;
	 case box_fct:
	    while( row-- ) {
	       this->value.undef[row] = 0;
	       i=7; while( i-- )
		  if( vector[i] ) {
		     pVals[i].data.dbl = theParams[i]->value.data.dblptr[row];
		     this->value.undef[row] |= theParams[i]->value.undef[row];
		  }
	       if( !(this->value.undef[row]) )
		  this->value.data.logptr[row] =
		     saobox( pVals[0].data.dbl, pVals[1].data.dbl,
			     pVals[2].data.dbl, pVals[3].data.dbl,
			     pVals[4].data.dbl, pVals[5].data.dbl,
			     pVals[6].data.dbl );
	    }
	    break;
	 case elps_fct:
	    while( row-- ) {
	       this->value.undef[row] = 0;
	       i=7; while( i-- )
		  if( vector[i] ) {
		     pVals[i].data.dbl = theParams[i]->value.data.dblptr[row];
		     this->value.undef[row] |= theParams[i]->value.undef[row];
		  }
	       if( !(this->value.undef[row]) )
		  this->value.data.logptr[row] =
		     ellipse( pVals[0].data.dbl, pVals[1].data.dbl,
			      pVals[2].data.dbl, pVals[3].data.dbl,
			      pVals[4].data.dbl, pVals[5].data.dbl,
			      pVals[6].data.dbl );
	    }
	    break;
	 }
      }
   }

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
      isConst[i] = ( theDims[i]->operation==-1000 );
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
	       this->value.undef[row] = theVar->value.undef[elem];
	       if( this->type==DOUBLE )
		  this->value.data.dblptr[row] = 
		     theVar->value.data.dblptr[elem];
	       else if( this->type==LONG )
		  this->value.data.lngptr[row] = 
		     theVar->value.data.lngptr[elem];
	       else
		  this->value.data.logptr[row] = 
		     theVar->value.data.logptr[elem];
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
	       this->value.undef[row] = theVar->value.undef[elem];
	       if( this->type==DOUBLE )
		  this->value.data.dblptr[row] = 
		     theVar->value.data.dblptr[elem];
	       else if( this->type==LONG )
		  this->value.data.lngptr[row] = 
		     theVar->value.data.lngptr[elem];
	       else
		  this->value.data.logptr[row] = 
		     theVar->value.data.logptr[elem];
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
   long row, nGTI, i;

   theTimes = gParse.Nodes + this->SubNodes[0];
   theExpr  = gParse.Nodes + this->SubNodes[1];

   nGTI  = theTimes->value.nelem;
   start = theTimes->value.data.dblptr;
   stop  = theTimes->value.data.dblptr + nGTI;

   times = theExpr->value.data.dblptr;

   Allocate_Ptrs( this );

   if( !gParse.status ) {

      i = 0;
      row = gParse.nRows;
      if( nGTI )
	 while( row-- ) {
	    this->value.undef[row] = theExpr->value.undef[row];

	    /*  Before searching entire GTI, check the GTI found last time  */
	    if( i<0 || times[row]<start[i] || times[row]>stop[i] ) {
	       i = nGTI;
	       while( i-- )
		  if( times[row]>=start[i] && times[row]<=stop[i] )
		     break;
	    }
	    this->value.data.logptr[row] = ( i>=0 );
	 }
      else
	 while( row-- ) {
	    this->value.data.logptr[row] = 0;
	    this->value.undef[row]       = 0;
	 }

   }

   if( theExpr->operation>0 )
      free( theExpr->value.data.ptr );
}

/*****************************************************************************/
/*  Utility routines which perform the calculations on bits and SAO regions  */
/*****************************************************************************/

#define myPI  3.1415926535897932385

static char bitlgte(char *bits1, int oper, char *bits2)
{
 int val1, val2, nextbit;
 char result;
 int i, l1, l2, length, ldiff;
 char stream[256];
 char chr1, chr2;

 l1 = strlen(bits1);
 l2 = strlen(bits2);
 if (l1 < l2)
   {
    length = l2;
    ldiff = l2 - l1;
    i=0;
    while( ldiff-- ) stream[i++] = '0';
    while( l1--    ) stream[i++] = *(bits1++);
    stream[i] = '\0';
    bits1 = stream;
   }
 else if (l2 < l1)
   {
    length = l1;
    ldiff = l1 - l2;
    i=0;
    while( ldiff-- ) stream[i++] = '0';
    while( l2--    ) stream[i++] = *(bits2++);
    stream[i] = '\0';
    bits2 = stream;
   }
 else
    length = l1;

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
 return (result);
}

static void bitand(char *result,char *bitstrm1,char *bitstrm2)
{
 int i, l1, l2, ldiff;
 char stream[256];
 char chr1, chr2;

 l1 = strlen(bitstrm1);
 l2 = strlen(bitstrm2);
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
 *result = '\0';
}

static void bitor(char *result,char *bitstrm1,char *bitstrm2)
{
 int i, l1, l2, ldiff;
 char stream[256];
 char chr1, chr2;

 l1 = strlen(bitstrm1);
 l2 = strlen(bitstrm2);
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
 int i, l1, l2, ldiff;
 char stream[256];
 char chr1, chr2;

 l1 = strlen(bitstrm1);
 l2 = strlen(bitstrm2);
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
	  return( 0 );
    }
 return( 1 );
}

static char near(double x, double y, double tolerance)
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

static void fferror(char *s)
{
    char msg[80];

    if( !gParse.status ) gParse.status = PARSE_SYNTAX_ERR;

    strncpy(msg, s, 80);
    msg[79] = '\0';
    ffpmsg(msg);
}

