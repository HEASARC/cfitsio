%{
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
/*                                                                      */
/************************************************************************/

#include <math.h>
#include <string.h>
#include <ctype.h>
#ifdef sparc
#include <malloc.h>
#else
#include <stdlib.h>
#endif
#include "eval_defs.h"
#include "eval_tab.h"

/*****  Internal functions  *****/

static int find_keywd( char *key, YYSTYPE *thelval );
static int build_column_array( char *colname, YYSTYPE *thelval );
static int find_column( char *colname );
static int expr_read( char *buf, int nbytes );

/*****  Definitions  *****/

#define YY_NO_UNPUT   /*  Don't include YYUNPUT function  */

#define MAXCHR 256
#define MAXBIT 128
#define ERROR  -1

#define OCT_0 "000"
#define OCT_1 "001"
#define OCT_2 "010"
#define OCT_3 "011"
#define OCT_4 "100"
#define OCT_5 "101"
#define OCT_6 "110"
#define OCT_7 "111"
#define OCT_X "xxx"

#define HEX_0 "0000"
#define HEX_1 "0001"
#define HEX_2 "0010"
#define HEX_3 "0011"
#define HEX_4 "0100"
#define HEX_5 "0101"
#define HEX_6 "0110"
#define HEX_7 "0111"
#define HEX_8 "1000"
#define HEX_9 "1001"
#define HEX_A "1010"
#define HEX_B "1011"
#define HEX_C "1100"
#define HEX_D "1101"
#define HEX_E "1110"
#define HEX_F "1111"
#define HEX_X "xxxx"

/* 
   MJT - 13 June 1996
   read from buffer instead of stdin
   (as per old ftools.skel)
*/
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
        if ( (result = expr_read( (char *) buf, max_size )) < 0 ) \
            YY_FATAL_ERROR( "read() in flex scanner failed" );

%}
bit		([bB][01xX]+)
oct		([oO][01234567xX]+)
hex		([hH][0123456789aAbBcCdDeEfFxX]+)
integer		[0-9]+
boolean         (t|f|T|F)
real		([0-9]*"."[0-9]+)|([0-9]*"."*[0-9]+[eEdD][+-]?[0-9]+)|([0-9]*".")
constant        ("#pi"|"#PI"|"#e"|"#E"|"#deg"|"#DEG"|"#row"|"#ROW")
keyword		("#"[a-zA-Z0-9_]+)|("#""$"[^\n]*"$")
string		[\"\'][^\"\'\n]*[\"\']
variable	([a-zA-Z_][a-zA-Z0-9_]*)|("$"[^$\n]*"$")
function	[a-zA-Z][a-zA-Z0-9]+"("
intcast		("(int)"|"(INT)")
fltcast		("(float)"|"(FLOAT)"|"(double)"|"(DOUBLE)")
power		("^"|"**")
not             ("!"|".not."|".NOT."|"not."|"NOT.")
or              ("||"|".or."|".OR."|"or."|"OR.")
and             ("&&"|".and."|".AND."|"and."|"AND.")
equal		("=="|".eq."|".EQ."|"eq."|"EQ.")
not_equal	("!="|".ne."|".NE."|"ne."|"NE.")
greater         (">"|".gt."|".GT."|"gt."|"GT.")
lesser          ("<"|".lt."|".LT."|"lt."|"LT.")
greater_eq	(">="|"=>"|".ge."|".GE."|"ge."|"GE.")
lesser_eq	("<="|"=<"|".le."|".LE."|"le."|"LE.")
nl		\n

%%

[ \t]+     ;
{bit}		{
                  int len;
                  len = strlen(yytext);
		  while (yytext[len] == ' ')
			len--;
                  len = len - 1;
		  strncpy(yylval.str,&yytext[1],len);
		  yylval.str[len] = '\0';
		  return( BITSTR );
		}
{oct}		{
                  int len;
                  char tmpstring[256];
                  char bitstring[256];
                  len = strlen(yytext);
		  while (yytext[len] == ' ')
			len--;
                  len = len - 1;
		  strncpy(tmpstring,&yytext[1],len);
                  tmpstring[len] = '\0';
                  bitstring[0] = '\0';
		  len = 0;
                  while ( tmpstring[len] != '\0')
                       {
			switch ( tmpstring[len] )
			      {
			       case '0':
					strcat(bitstring,OCT_0);
					break;
			       case '1':
					strcat(bitstring,OCT_1);
					break;
			       case '2':
					strcat(bitstring,OCT_2);
					break;
			       case '3':
					strcat(bitstring,OCT_3);
					break;
			       case '4':
					strcat(bitstring,OCT_4);
					break;
			       case '5':
					strcat(bitstring,OCT_5);
					break;
			       case '6':
					strcat(bitstring,OCT_6);
					break;
			       case '7':
					strcat(bitstring,OCT_7);
					break;
			       case 'x':
			       case 'X':
					strcat(bitstring,OCT_X);
					break;
			      }
			len++;
                       }
                  strcpy( yylval.str, bitstring );
		  return( BITSTR );
		}
{hex}		{
                  int len;
                  char tmpstring[256];
                  char bitstring[256];
                  len = strlen(yytext);
		  while (yytext[len] == ' ')
			len--;
                  len = len - 1;
		  strncpy(tmpstring,&yytext[1],len);
                  tmpstring[len] = '\0';
                  bitstring[0] = '\0';
		  len = 0;
                  while ( tmpstring[len] != '\0')
                       {
			switch ( tmpstring[len] )
			      {
			       case '0':
					strcat(bitstring,HEX_0);
					break;
			       case '1':
					strcat(bitstring,HEX_1);
					break;
			       case '2':
					strcat(bitstring,HEX_2);
					break;
			       case '3':
					strcat(bitstring,HEX_3);
					break;
			       case '4':
					strcat(bitstring,HEX_4);
					break;
			       case '5':
					strcat(bitstring,HEX_5);
					break;
			       case '6':
					strcat(bitstring,HEX_6);
					break;
			       case '7':
					strcat(bitstring,HEX_7);
					break;
			       case '8':
					strcat(bitstring,HEX_8);
					break;
			       case '9':
					strcat(bitstring,HEX_9);
					break;
			       case 'a':
			       case 'A':
					strcat(bitstring,HEX_A);
					break;
			       case 'b':
			       case 'B':
					strcat(bitstring,HEX_B);
					break;
			       case 'c':
			       case 'C':
					strcat(bitstring,HEX_C);
					break;
			       case 'd':
			       case 'D':
					strcat(bitstring,HEX_D);
					break;
			       case 'e':
			       case 'E':
					strcat(bitstring,HEX_E);
					break;
			       case 'f':
			       case 'F':
					strcat(bitstring,HEX_F);
					break;
			       case 'x':
			       case 'X':
					strcat(bitstring,HEX_X);
					break;
			      }
			len++;
                       }

                  strcpy( yylval.str, bitstring );
		  return( BITSTR );
		}
{integer}	{
                  yylval.lng = atol(yytext);
		  return( LONG );
		}
{boolean}	{
                  if ((yytext[0] == 't') || (yytext[0] == 'T'))
		    yylval.log = 1;
		  else
		    yylval.log = 0;
		  return( BOOLEAN );
		}
{real}		{
                  yylval.dbl = atof(yytext);
		  return( DOUBLE );
		}
{constant}	{
                  if ((strcmp(yytext,"#pi")==0)||(strcmp(yytext,"#PI")==0)) {
		     yylval.dbl = (double)(4) * atan((double)(1));
		     return( DOUBLE );
		  }
                  else if ((strcmp(yytext,"#e")==0)||(strcmp(yytext,"#E")==0)) 
		  {
		     yylval.dbl = exp((double)(1));
		     return( DOUBLE );
		  }
                  else if ((strcmp(yytext,"#deg") == 0)||
			   (strcmp(yytext,"#DEG") == 0))
		  {
		     yylval.dbl = ((double)4)*atan((double)1)/((double)180);
		     return( DOUBLE );
		  }
                  else if ((strcmp(yytext,"#row") == 0)||
			   (strcmp(yytext,"#ROW") == 0))
		  {
		     return( ROWREF );
		  }
		}
{keyword}	{
                 int len; 
		 if (yytext[1] == '$')
		   {
		    len = strlen(yytext) - 3;
		    strncpy(yylval.str,&yytext[2],len);
		    yylval.str[len] = '\0';
		    yytext = yylval.str;
		   }
		 else
		   {
		    len = strlen(yytext) - 1;
		    strncpy(yylval.str,&yytext[1],len);
		    yylval.str[len] = '\0';
		    yytext = yylval.str;
		   }
		 return( find_keywd(yytext, &yylval) );
                }
{string}	{
                  int len;
                  len = strlen(yytext) - 2;
		  while (yytext[len] == ' ')
			len--;
		  strncpy(yylval.str,&yytext[1],len);
		  yylval.str[len] = '\0';
		  return( STRING );
		}
{variable}	{
                 /* copy from FITS table based on type */
		 int    len,type;

                 if (yytext[0] == '$') {
		    len = strlen(yytext) - 2;
		    strncpy(yylval.str,&yytext[1],len);
		    yylval.str[len] = '\0';
		    yytext = yylval.str;
		 } 
		 switch( build_column_array(yytext, &yylval) ) {
		    case LONG:
		    case DOUBLE:   type =  COLUMN;  break;
		    case BOOLEAN:  type = BCOLUMN;  break;
		    case STRING:   type = SCOLUMN;  break;
		    case BITSTR:   type =  BITCOL;  break;
                    default:
                         type = find_keywd(yytext, &yylval);
                         if( type!=ERROR )
                            gParse.status = 0;
                         break;
		 }
                 return( type );
		}
{function}	{
                  char *fname;
		  int len=0;
                  fname = &yylval.str[0];
		  while( (fname[len]=toupper(yytext[len])) ) len++;

                  if(      FSTRCMP(fname,"BOX(")==0 
                        || FSTRCMP(fname,"CIRCLE(")==0 
                        || FSTRCMP(fname,"ELLIPSE(")==0 
                        || FSTRCMP(fname,"NEAR(")==0 
                        || FSTRCMP(fname,"ISNULL(")==0 
                         )
                     /* Return type is always boolean  */
		     return( BFUNCTION );
                  else if (FSTRCMP(fname,"GTIFILTER(")==0
                         )
                     return( FILTER    );
                  else 
		     return( FUNCTION  );
		}
{intcast}	{ return( INTCAST ); }
{fltcast}	{ return( FLTCAST ); }
{power}		{ return( POWER   ); }
{not}		{ return( NOT     ); }
{or}		{ return( OR      ); }
{and}		{ return( AND     ); }
{equal}		{ return( EQ      ); }
{not_equal}	{ return( NE      ); }
{greater}	{ return( GT      ); }
{lesser}	{ return( LT      ); }
{greater_eq}	{ return( GTE     ); }
{lesser_eq}	{ return( LTE     ); }
{nl}		{ return( '\n'    ); }
.		{ return( yytext[0] ); }
%%

int yywrap()
{
  /* MJT -- 13 June 1996
     Supplied for compatibility with
     pre-2.5.1 versions of flex which
     do not recognize %option noyywrap 
  */
  return(1);
}

/* 
   expr_read is lifted from old ftools.skel. 
   Now we can use any version of flex with
   no .skel file necessary! MJT - 13 June 1996

   keep a memory of how many bytes have been
   read previously, so that an unlimited-sized
   buffer can be supported. PDW - 28 Feb 1998
*/

static int expr_read(char *buf, int nbytes)
{
 int n;
 
 n = 0;
 if( !gParse.is_eobuf ) {
     do {
        buf[n++] = gParse.expr[gParse.index++];
       } while ((n<nbytes)&&(gParse.expr[gParse.index] != '\0'));
     if( gParse.expr[gParse.index] == '\0' ) gParse.is_eobuf = 1;
 }
 buf[n] = '\0';
 return(n);
}

static int find_column(char *colname)
{
   int i;
 
   if( gParse.nCols )
      for( i=0; i<gParse.nCols; i++ ) {
         if( strcasecmp(gParse.colData[i].colname,colname) == 0 ) {
            return( i );
         }
      }
   return( -1 );
}

/* Public entry point for calls from eval.y */
int yybuildcolumn( char *ColName, long *ColNum )
{
   YYSTYPE thelval;
   int type;

   type = build_column_array( ColName, &thelval );
   *ColNum = thelval.lng;
   return( type );
}

int yyallocatecol( int nCol, int *status )
{
   if( (nCol%25)==0 ) {
      if( nCol ) {
	 gParse.colData  = (iteratorCol*) realloc( gParse.colData,
                                              (nCol+25)*sizeof(iteratorCol) );
	 gParse.colInfo  = (DataInfo   *) realloc( gParse.colInfo,
                                              (nCol+25)*sizeof(DataInfo)    );
	 gParse.colNulls = (char      **) realloc( gParse.colNulls,
					      (nCol+25)*sizeof(char*)       );
      } else {
	 gParse.colData  = (iteratorCol*) malloc( 25*sizeof(iteratorCol) );
	 gParse.colInfo  = (DataInfo   *) malloc( 25*sizeof(DataInfo)    );
	 gParse.colNulls = (char      **) malloc( 25*sizeof(char*)       );
      }
      if(    gParse.colData  == NULL
          || gParse.colInfo  == NULL
          || gParse.colNulls == NULL    ) {
	 if( gParse.colData  ) free(gParse.colData);
	 if( gParse.colInfo  ) free(gParse.colInfo);
	 if( gParse.colNulls ) free(gParse.colNulls);
	 return( *status = MEMORY_ALLOCATION );
      }
   }
   gParse.colNulls[nCol] = NULL;
   return 0;
}

static int build_column_array( char *colname, YYSTYPE *thelval )
{
   int col_cnt, status;
   int colnum, typecode;
   long repeat, width;
   fitsfile *fptr;

   if( strlen(colname)>69 ) colname[69]='\0';

   col_cnt = find_column(colname);
   if( col_cnt<0 ) {

      fptr = gParse.def_fptr;
      col_cnt = gParse.nCols;
      if( yyallocatecol( col_cnt, &gParse.status ) ) return ERROR;

      status = 0;
      if( gParse.compressed )
         colnum = gParse.valCol;
      else
         if( fits_get_colnum( fptr, CASEINSEN, colname, &colnum, &status ) ) {
	    gParse.status = status;
	    return ERROR;
         }

      if( fits_get_coltype( fptr, colnum, &typecode,
			    &repeat, &width, &status ) ) {
	 gParse.status = status;
	 return ERROR;
      }

      fits_iter_set_by_num( gParse.colData+col_cnt, fptr, colnum, 0, InputCol );
      strcpy(gParse.colData[col_cnt].colname,colname);

      switch( typecode ) {
      case TBIT:
	 gParse.colInfo[col_cnt].type = BITSTR;
	 gParse.colData[col_cnt].datatype = TBYTE;
         break;
      case TBYTE:
      case TSHORT:
      case TLONG:
	 gParse.colInfo[col_cnt].type = LONG;
	 gParse.colData[col_cnt].datatype = TLONG;
	 break;
      case TFLOAT:
      case TDOUBLE:
	 gParse.colInfo[col_cnt].type = DOUBLE;
	 gParse.colData[col_cnt].datatype = TDOUBLE;
	 break;
      case TLOGICAL:
	 gParse.colInfo[col_cnt].type = BOOLEAN;
	 gParse.colData[col_cnt].datatype = TLOGICAL;
	 break;
      case TSTRING:
	 gParse.colInfo[col_cnt].type = STRING ;
	 gParse.colData[col_cnt].datatype = TSTRING;
	 break;
      }
      gParse.colInfo[col_cnt].nelem = repeat;
      if( repeat>1 && typecode!=TSTRING ) {
	 if( fits_read_tdim( fptr, colnum, MAXDIMS,
			     &gParse.colInfo[col_cnt].naxis,
			     &gParse.colInfo[col_cnt].naxes[0], &status )
	     ) {
	    gParse.status = status;
	    return ERROR;
	 }
      } else {
	 gParse.colInfo[col_cnt].naxis = 1;
	 gParse.colInfo[col_cnt].naxes[0] = 1;
      }
      gParse.nCols++;
   }
   thelval->lng = col_cnt;
   return( gParse.colInfo[col_cnt].type );
}

static int find_keywd(char *keyname, YYSTYPE *thelval )
{
   int status, type;
   char keyvalue[71], dtype;
   fitsfile *fptr;
   double rval;
   int bval;
   long ival;

   if( strlen(keyname)>8 ) keyname[8]='\0';

   status = 0;
   fptr = gParse.def_fptr;
   if( fits_read_keyword( fptr, keyname, keyvalue, NULL, &status ) ) {
      gParse.status = status;
      return( ERROR );
   }
      
   if( fits_get_keytype( keyvalue, &dtype, &status ) ) {
      gParse.status = status;
      return( ERROR );
   }
      
   switch( dtype ) {
   case 'C':
      fits_read_key_str( fptr, keyname, keyvalue, NULL, &status );
      type = STRING;
      strcpy( thelval->str , keyvalue );
      break;
   case 'L':
      fits_read_key_log( fptr, keyname, &bval, NULL, &status );
      type = BOOLEAN;
      thelval->log = bval;
      break;
   case 'I':
      fits_read_key_lng( fptr, keyname, &ival, NULL, &status );
      type = LONG;
      thelval->lng = ival;
      break;
   case 'F':
      fits_read_key_dbl( fptr, keyname, &rval, NULL, &status );
      type = DOUBLE;
      thelval->dbl = rval;
      break;
   default:
      type = ERROR;
      break;
   }

   if( status ) {
      gParse.status=status;
      return ERROR;
   }

   return( type );
}

#if defined(vms) || defined(__vms)

/* A very dumb patch for VMS, which lacks a strcasecmp */
int strcasecmp(char *s1, char *s2)
{
 int i,l;
 char st1[1024],st2[1014];

 l = strlen(s1);
 for (i=0;i<=l;i++)
    st1[i] = toupper(s1[i]);
 l = strlen(s2);
 for (i=0;i<=l;i++)
    st2[i] = toupper(s2[i]);
 return ( strcmp(st1,st2) );
}
#endif
