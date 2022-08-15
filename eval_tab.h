/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

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
#line 199 "eval.y" /* yacc.c:1909  */

    int    Node;        /* Index of Node */
    double dbl;         /* real value    */
    long   lng;         /* integer value */
    char   log;         /* logical value */
    char   str[MAX_STRLEN];    /* string value  */

#line 136 "y.tab.h" /* yacc.c:1909  */
};

typedef union FFSTYPE FFSTYPE;
# define FFSTYPE_IS_TRIVIAL 1
# define FFSTYPE_IS_DECLARED 1
#endif


extern FFSTYPE fflval;

int ffparse (void);

#endif /* !FF_FF_Y_TAB_H_INCLUDED  */
