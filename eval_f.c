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

#include <limits.h>
#include "eval_defs.h"
#include "eval_tab.h"

typedef struct {
     int  datatype;   /* Data type to cast parse results into for user       */
     void *dataPtr;   /* Pointer to array of results, NULL if to use iterCol */
     void *nullPtr;   /* Pointer to nulval, use zero if NULL                 */
     long maxRows;    /* Max No. of rows to process, -1=all, 0=1 iteration   */
     int  anyNull;    /* Flag indicating at least 1 undef value encountered  */
} parseInfo;

ParseData gParse;     /* Global structure holding all parser information     */

/*---------------------------------------------------------------------------*/
int fffrow( fitsfile *fptr,         /* I - Input FITS file                   */
            char     *expr,         /* I - Boolean expression                */
            long     firstrow,      /* I - First row of table to eval        */
            long     nrows,         /* I - Number of rows to evaluate        */
            long     *n_good_rows,  /* O - Number of rows eval to True       */
            char     *row_status,   /* O - Array of boolean results          */
            int      *status )      /* O - Error status                      */
/*                                                                           */
/* Evaluate a boolean expression using the indicated rows, returning an      */
/* array of flags indicating which rows evaluated to TRUE/FALSE              */
/*---------------------------------------------------------------------------*/
{
   parseInfo Info;
   int naxis, constant;
   long nelem, naxes[MAXDIMS], elem;
   char result;

   if( *status ) return( *status );

   if( ffiprs( fptr, 0, expr, MAXDIMS, &Info.datatype, &nelem, &naxis,
               naxes, status ) ) {
      ffcprs();
      return( *status );
   }
   if( Info.datatype<0 ) {
      constant = 1;
      Info.datatype = -Info.datatype;
   } else
      constant = 0;

   if( Info.datatype!=TLOGICAL || nelem!=1 ) {
      ffcprs();
      ffpmsg("Expression does not evaluate to a logical scalar.");
      return( *status = PARSE_BAD_TYPE );
   }

   if( constant ) { /* No need to call parser... have result from ffiprs */
      result = gParse.Nodes[gParse.nNodes-1].value.data.log;
      *n_good_rows = nrows;
      for( elem=0; elem<nrows; elem++ )
	 row_status[elem] = result;
   } else {
      firstrow     = (firstrow>1 ? firstrow : 1);
      Info.dataPtr = row_status;
      Info.nullPtr = NULL;
      Info.maxRows = nrows;
   
      /*  Check whether there is at least 1 column referenced in   */
      /*  expression.  If not, setup the first column as a dummy.  */

      if( !gParse.nCols ) {
	 if( ffallocatecol( 0, status ) ) {
	    ffcprs();
	    return( *status );
	 }
	 fits_iter_set_by_num( gParse.colData, fptr, 1, 0, InputCol );
	 gParse.nCols++;
      }

      if( ffiter( gParse.nCols, gParse.colData, firstrow-1, 0,
		  parse_data, (void*)&Info, status ) == -1 )
	 *status = 0;  /* -1 indicates exitted without error before end... OK */

      if( *status ) {

	 /***********************/
	 /* Error... Do nothing */
	 /***********************/

      } else {

	 /***********************************/
	 /* Count number of good rows found */
	 /***********************************/

	 *n_good_rows = 0L;
	 for( elem=0; elem<Info.maxRows; elem++ ) {
	    if( row_status[elem]==1 ) ++*n_good_rows;
	 }
      }
   }

   ffcprs();
   return(*status);
}

/*--------------------------------------------------------------------------*/
int ffsrow( fitsfile *infptr,   /* I - Input FITS file                      */
            fitsfile *outfptr,  /* I - Output FITS file                     */
            char     *expr,     /* I - Boolean expression                   */
            int      *status )  /* O - Error status                         */
/*                                                                          */
/* Evaluate an expression on all rows of a table.  If the input and output  */
/* files are not the same, copy the TRUE rows to the output file.  If the   */
/* files are the same, delete the FALSE rows (preserve the TRUE rows).      */
/*--------------------------------------------------------------------------*/
{
   parseInfo Info;
   int naxis, constant;
   long nelem, naxes[MAXDIMS], nrows, rdlen, row, maxrows, nbuff;
   long inloc, outloc, ntodo;
   char *buffer, result;

   if( *status ) return( *status );

   if( ffiprs( infptr, 0, expr, MAXDIMS, &Info.datatype, &nelem, &naxis,
               naxes, status ) ) {
      ffcprs();
      return( *status );
   }
   if( Info.datatype<0 ) {
      constant = 1;
      Info.datatype = -Info.datatype;
   } else
      constant = 0;

   /**********************************************************************/
   /* Make sure expression evaluates to the right type... logical scalar */
   /**********************************************************************/

   if( Info.datatype!=TLOGICAL || nelem!=1 ) {
      ffcprs();
      ffpmsg("Expression does not evaluate to a logical scalar.");
      return( *status = PARSE_BAD_TYPE );
   }

   ffgkyj( infptr, "NAXIS1", &rdlen, NULL, status );
   ffgkyj( infptr, "NAXIS2", &nrows, NULL, status );
   if( *status ) {
      ffcprs();
      return( *status );
   }

   Info.dataPtr = (char *)malloc( nrows*sizeof(char) );
   Info.nullPtr = NULL;
   Info.maxRows = nrows;
   
   if( constant ) { /*  Set all rows to the same value from constant result  */

      result = gParse.Nodes[gParse.nNodes-1].value.data.log;
      for( ntodo = 0; ntodo<nrows; ntodo++ )
	 ((char*)Info.dataPtr)[ntodo] = result;

   } else {

      /*  Check whether there is at least 1 column referenced in   */
      /*  expression.  If not, setup the first column as a dummy.  */

      if( !gParse.nCols ) {
	 if( ffallocatecol( 0, status ) ) {
	    ffcprs();
	    return( *status );
	 }
	 fits_iter_set_by_num( gParse.colData, infptr, 1, 0, InputCol );
	 gParse.nCols++;
      }
      ffiter( gParse.nCols, gParse.colData, 0L, 0L,
	      parse_data, (void*)&Info, status );
   }

   if( *status ) {
      /* Error... Do nothing */
   } else {
      buffer  = (char *)malloc( 100000*sizeof(char) );
      if( buffer==NULL ) {
         ffcprs();
         return( *status=MEMORY_ALLOCATION );
      }
      maxrows = 100000L/rdlen;
      nbuff = 0;
      row = 0;
      if( infptr==outfptr )
         while( ((char*)Info.dataPtr)[row] ) row++;
      inloc  =  (infptr->Fptr)->datastart + row*rdlen;

      if( (outfptr->Fptr)->datastart<0 )
         /* rescan header if data pointer undefined */
         ffrdef(outfptr, status);
      outloc = (outfptr->Fptr)->datastart + row*rdlen;

      do {
         if( ((char*)Info.dataPtr)[row] ) {
            ffmbyt( infptr, inloc, REPORT_EOF,         status );
            ffgbyt( infptr, rdlen, buffer+rdlen*nbuff, status );
            nbuff++;
            if( nbuff==maxrows ) {
               ffmbyt( outfptr, outloc,      IGNORE_EOF, status );
               ffpbyt( outfptr, rdlen*nbuff, buffer,     status );
               outloc += nbuff*rdlen;
               nbuff = 0;
            }
         }
         row++;
         inloc += rdlen;
      } while( !*status && row<nrows );

      if( nbuff ) {
         ffmbyt( outfptr, outloc,      IGNORE_EOF, status );
         ffpbyt( outfptr, rdlen*nbuff, buffer,     status );
         outloc += nbuff*rdlen;
      }

      row = (outloc - (outfptr->Fptr)->datastart)/rdlen;
      if( infptr==outfptr ) {
         if( row<nrows ) ffdrow( infptr, row+1, nrows-row, status );
      } else if( (infptr->Fptr)->heapsize ) { /* Copy heap, if it exists */
         inloc = (infptr->Fptr)->datastart + (infptr->Fptr)->heapstart;
         ntodo = (infptr->Fptr)->heapsize;

         while ( ntodo ) {
            rdlen = (ntodo<100000 ? ntodo : 100000);
            ffmbyt( infptr,  inloc,  REPORT_EOF, status );
            ffgbyt( infptr,  rdlen,  buffer,     status );
            ffmbyt( outfptr, outloc, IGNORE_EOF, status );
            ffpbyt( outfptr, rdlen,  buffer,     status );
            inloc  += rdlen;
            outloc += rdlen;
            ntodo  -= rdlen;
         }
      }

      ffukyj( outfptr, "NAXIS2", row, "&", status );

      free(buffer);
   }

   free(Info.dataPtr);
   ffcprs();
   return(*status);
}

/*---------------------------------------------------------------------------*/
int ffcrow( fitsfile *fptr,      /* I - Input FITS file                      */
            int      datatype,   /* I - Datatype to return results as        */
            char     *expr,      /* I - Arithmetic expression                */
            long     firstrow,   /* I - First row to evaluate                */
            long     nelements,  /* I - Number of elements to return         */
            void     *nulval,    /* I - Ptr to value to use as UNDEF         */
            void     *array,     /* O - Array of results                     */
            int      *anynul,    /* O - Were any UNDEFs encountered?         */
            int      *status )   /* O - Error status                         */
/*                                                                           */
/* Calculate an expression for the indicated rows of a table, returning      */
/* the results, cast as datatype (TSHORT, TDOUBLE, etc), in array.  If       */
/* nulval==NULL, UNDEFs will be zeroed out.  For vector results, the number  */
/* of elements returned may be less than nelements if nelements is not an    */
/* even multiple of the result dimension.  Call fftexp to obtain the         */
/* dimensions of the results.                                                */
/*---------------------------------------------------------------------------*/
{
   parseInfo Info;
   int naxis;
   long nelem1, naxes[MAXDIMS];

   if( *status ) return( *status );

   if( ffiprs( fptr, 0, expr, MAXDIMS, &Info.datatype, &nelem1, &naxis,
               naxes, status ) ) {
      ffcprs();
      return( *status );
   }
   if( Info.datatype<0 ) Info.datatype = - Info.datatype;

   if( nelements<nelem1 ) {
      ffcprs();
      ffpmsg("Array not large enough to hold at least one row of data.");
      return( *status = PARSE_LRG_VECTOR );
   }

   firstrow = (firstrow>1 ? firstrow : 1);

   if( datatype ) Info.datatype = datatype;

   Info.dataPtr = array;
   Info.nullPtr = nulval;
   Info.maxRows = nelements / nelem1;
   
   /*  Check whether there is at least 1 column referenced in   */
   /*  expression.  If not, setup the first column as a dummy.  */

   if( !gParse.nCols ) {
      if( ffallocatecol( 0, status ) ) {
	 ffcprs();
	 return( *status );
      }
      fits_iter_set_by_num( gParse.colData, fptr, 1, 0, InputCol );
      gParse.nCols++;
   }

   if( ffiter( gParse.nCols, gParse.colData, firstrow-1, 0,
               parse_data, (void*)&Info, status ) == -1 )
      *status=0;  /* -1 indicates exitted without error before end... OK */

   *anynul = Info.anyNull;
   ffcprs();
   return( *status );
}

/*--------------------------------------------------------------------------*/
int ffccol( fitsfile *infptr,   /* I - Input FITS file                      */
            char     *expr,     /* I - Arithmetic expression                */
            fitsfile *outfptr,  /* I - Output fits file                     */
            char     *colname,  /* I - Name of output column                */
            int      *status )  /* O - Error status                         */
/*                                                                          */
/* Evaluate an expression for each row in the input FITS file and place     */
/* the results into the named column of the output fits file.  It is the    */
/* caller's responsibility to make sure the column exists and is of the     */
/* proper type to hold the results, although type conversions are handled   */
/* by CFITSIO.                                                              */
/*--------------------------------------------------------------------------*/
{
   parseInfo Info;
   int naxis, constant;
   long nelem, naxes[MAXDIMS];
   int col_cnt, colNo;
   Node *result;

   if( *status ) return( *status );
   
   if( ffiprs( infptr, 0, expr, MAXDIMS, &Info.datatype, &nelem, &naxis,
               naxes, status ) ) {
      ffcprs();
      return( *status );
   }
   if( Info.datatype<0 ) {
      constant = 1;
      Info.datatype = -Info.datatype;
   } else
      constant = 0;

   if( ffgcno( outfptr, CASEINSEN, colname, &colNo, status )==COL_NOT_FOUND ) {

      /*  Output column doesn't exist.  If a constant, put into keyword,
	  otherwise, abort with error. */

      if( constant ) {
	 *status = 0;
	 result  = gParse.Nodes + gParse.nNodes-1;
	 switch( Info.datatype ) {
	 case TDOUBLE:
	    ffukyd( outfptr, colname, result->value.data.dbl, 15,
		    NULL, status );
	    break;
	 case TLONG:
	    ffukyj( outfptr, colname, result->value.data.lng, NULL, status );
	    break;
	 case TLOGICAL:
	    ffukyl( outfptr, colname, result->value.data.log, NULL, status );
	    break;
	 case TBIT:
	 case TSTRING:
	    ffukys( outfptr, colname, result->value.data.str, NULL, status );
	    break;
	 }
      }

   } else if( ! *status ) {

      /*  Output column exists... put results into it  */

      /*************************************/
      /* Create new iterator Output Column */
      /*************************************/

      col_cnt = gParse.nCols;
      if( ffallocatecol( col_cnt, status ) ) {
	 ffcprs();
	 return( *status );
      }

      fits_iter_set_by_num( gParse.colData+col_cnt, outfptr,
			    colNo, 0, OutputCol );
      gParse.nCols++;

      Info.dataPtr = NULL;
      Info.maxRows = -1;    /*  Process all of the rows  */

      ffiter( gParse.nCols, gParse.colData, 0, 0,
	      parse_data, (void*)&Info, status );
   }

   ffcprs();
   return( *status );
}

/*--------------------------------------------------------------------------*/
int fftexp( fitsfile *fptr,      /* I - Input FITS file                     */
            char     *expr,      /* I - Arithmetic expression               */
            int      maxdim,     /* I - Max Dimension of naxes              */
            int      *datatype,  /* O - Data type of result                 */
            long     *nelem,     /* O - Vector length of result             */
            int      *naxis,     /* O - # of dimensions of result           */
            long     *naxes,     /* O - Size of each dimension              */
            int      *status )   /* O - Error status                        */
/*                                                                          */
/* Evaluate the given expression and return information on the result.      */
/*--------------------------------------------------------------------------*/
{
   ffiprs( fptr, 0, expr, maxdim, datatype, nelem, naxis, naxes, status );
   ffcprs();
   return( *status );
}

/*--------------------------------------------------------------------------*/
int ffiprs( fitsfile *fptr,      /* I - Input FITS file                     */
            int      compressed, /* I - Is FITS file hkunexpanded?          */
            char     *expr,      /* I - Arithmetic expression               */
            int      maxdim,     /* I - Max Dimension of naxes              */
            int      *datatype,  /* O - Data type of result                 */
            long     *nelem,     /* O - Vector length of result             */
            int      *naxis,     /* O - # of dimensions of result           */
            long     *naxes,     /* O - Size of each dimension              */
            int      *status )   /* O - Error status                        */
/*                                                                          */
/* Initialize the parser and determine what type of result the expression   */
/* produces.                                                                */
/*--------------------------------------------------------------------------*/
{
   FILE *fexpr;
   Node *result;
   char line[1024];
   int  i,lexpr,len,mlen;

   if( *status ) return( *status );

   /*  Initialize the Parser structure  */

   gParse.def_fptr   = fptr;
   gParse.compressed = compressed;
   gParse.nCols      = 0;
   gParse.colData    = NULL;
   gParse.colInfo    = NULL;
   gParse.colNulls   = NULL;
   gParse.Nodes      = (Node *)malloc(sizeof(Node)*100);
   if( gParse.Nodes ) {
      gParse.nNodesAlloc = 100;
      gParse.nNodes      =   0;
   } else
      return( *status = MEMORY_ALLOCATION );
   gParse.status     = 0;

   /*  Copy expression into parser... read from file if necessary  */

   if( expr[0]=='@' ) {
      lexpr = len = 0;
      gParse.expr = (char *)malloc(1024*sizeof(char));
      gParse.expr[0]='\0';
      mlen = 1024;
      if( (fexpr = fopen( expr+1, "r" ))==NULL ) {
         return 1;
      }
      while( fgets(line,1024,fexpr)!=NULL ) {
         len=strlen(line);

         if( line[len-1]=='\n' ) line[len-1] = ' ';
         if( lexpr+len+1>=mlen ) /* Add one in case need to add \n later */
            gParse.expr = (char *)realloc(gParse.expr, mlen+=1024);
         strcpy(gParse.expr+lexpr,line);
         lexpr+=len;
      }
      fclose(fexpr);
      if( gParse.expr[lexpr-1] != '\n' ) strcat(gParse.expr+lexpr++,"\n");
   } else {
      lexpr = strlen(expr) + 2;
      gParse.expr = (char*)malloc(lexpr*sizeof(char));
      strcpy(gParse.expr,expr);
      strcat(gParse.expr,"\n");
   }
   gParse.index    = 0;
   gParse.is_eobuf = 0;

   /*  Parse the expression, building the Nodes and determing  */
   /*  which columns are neded and what data type is returned  */

   ffrestart(NULL);
   ffparse();

   /*  Check results  */

   *status = gParse.status;
   if( *status ) return(*status);

   result = gParse.Nodes + gParse.nNodes-1;

   *naxis = result->value.naxis;
   *nelem = result->value.nelem;
   for( i=0; i<*naxis && i<maxdim; i++ )
      naxes[i] = result->value.naxes[i];

   switch( result->type ) {
   case BOOLEAN:
      *datatype = TLOGICAL;
      break;
   case LONG:
      *datatype = TLONG;
      break;
   case DOUBLE:
      *datatype = TDOUBLE;
      break;
   case BITSTR:
      *datatype = TBIT;
      break;
   case STRING:
      *datatype = TSTRING;
      break;
   default:
      *datatype = 0;
      ffpmsg("Bad return data type");
      *status = gParse.status = PARSE_BAD_TYPE;
      break;
   }
   gParse.datatype = *datatype;
   free(gParse.expr);

   if( result->operation==CONST_OP ) *datatype = - *datatype;
   return(*status);
}

/*--------------------------------------------------------------------------*/
void ffcprs( void )  /*  No parameters                                      */
/*                                                                          */
/* Clear the parser, making it ready to accept a new expression.            */
/*--------------------------------------------------------------------------*/
{
   int col, node;

   if( gParse.nCols > 0 ) {
      free( gParse.colData  );
      free( gParse.colInfo  );
      if( gParse.colNulls[0] )
         for( col=0; col<gParse.nCols; col++ ) {
            if( gParse.colInfo[col].type == BITSTR )
               free( ((char***)gParse.colNulls)[col][0] );
            free( gParse.colNulls[col] );
         }
      free( gParse.colNulls );
      gParse.nCols = 0;
   }

   if( gParse.nNodes > 0 ) {
      node = gParse.nNodes;
      while( node-- ) {
	 if( gParse.Nodes[node].operation==gtifilt_fct )
	    free( gParse.Nodes[ gParse.Nodes[node].SubNodes[0] ].value.data.ptr );
      }
      gParse.nNodes = 0;
   }
   if( gParse.Nodes ) free( gParse.Nodes );
   gParse.Nodes = NULL;
}

/*---------------------------------------------------------------------------*/
int parse_data( long        totalrows, /* I - Total rows to be processed     */
                long        offset,    /* I - Number of rows skipped at start*/
                long        firstrow,  /* I - First row of this iteration    */
                long        nrows,     /* I - Number of rows in this iter    */
                int         nCols,     /* I - Number of columns in use       */
                iteratorCol *colData,  /* IO- Column information/data        */
                void        *userPtr ) /* I - Data handling instructions     */
/*                                                                           */
/* Iterator work function which calls the parser and copies the results      */
/* into either an OutputCol or a data pointer supplied in the userPtr        */
/* structure.                                                                */
/*---------------------------------------------------------------------------*/
{
    int status, constant=0;
    long jj, kk, idx, remain, rowOffset, ntodo;
    Node *result;

    /* declare variables static to preserve their values between calls */
    static void *Data, *Null;
    static int  datasize;
    static long lastRow, jnull;
    static parseInfo *userInfo;
    static long zeros[4] = {0,0,0,0};

    /*--------------------------------------------------------*/
    /*  Initialization procedures: execute on the first call  */
    /*--------------------------------------------------------*/
    if (firstrow == offset+1)
    {
       userInfo = (parseInfo*)userPtr;

       if( userInfo->maxRows>0 )
          userInfo->maxRows = minvalue(totalrows,userInfo->maxRows);
       else if( userInfo->maxRows<0 )
          userInfo->maxRows = totalrows;
       else
          userInfo->maxRows = nrows;

       lastRow = firstrow + userInfo->maxRows - 1;

       if( userInfo->dataPtr==NULL ) {

          if( colData[nCols-1].iotype == InputCol ) {
             ffpmsg("Output column for parser results not found!");
             return( PARSE_NO_OUTPUT );
          }
          /* Data gets set later */
          Null = colData[nCols-1].array;
          userInfo->datatype = colData[nCols-1].datatype;

	  /* Check for a TNULL keyword for output column */

	  status = 0;
	  jnull = 0L;
	  ffgknj( colData[nCols-1].fptr, "TNULL", colData[nCols-1].colnum,
		  1, &jnull, (int*)&jj, &status );

       } else {

          Data = userInfo->dataPtr;
          Null = (userInfo->nullPtr ? userInfo->nullPtr : zeros);

       }

       /* Determine the size of each element of the returned result */

       switch( userInfo->datatype ) {
       case TBIT:       /*  Fall through to TBYTE  */
       case TLOGICAL:   /*  Fall through to TBYTE  */
       case TBYTE:     datasize = sizeof(char);     break;
       case TSHORT:    datasize = sizeof(short);    break;
       case TINT:      datasize = sizeof(int);      break;
       case TLONG:     datasize = sizeof(long);     break;
       case TFLOAT:    datasize = sizeof(float);    break;
       case TDOUBLE:   datasize = sizeof(double);   break;
       case TSTRING:   datasize = sizeof(char*);    break;
       }
    }

    /*-------------------------------------------*/
    /*  Main loop: process all the rows of data  */
    /*-------------------------------------------*/

    /*  If writing to output column, set first element to appropriate  */
    /*  null value.  If no NULLs encounter, zero out before returning. */

    userInfo->anyNull = 0;
    if( userInfo->dataPtr == NULL ) {
       /* First, reset Data pointer to start of output array */
       Data = (char*)colData[nCols-1].array + datasize;

       switch( userInfo->datatype ) {
       case TLOGICAL: *(char  *)Null = 'U';             break;
       case TBYTE:    *(char  *)Null = (char )jnull;    break;
       case TSHORT:   *(short *)Null = (short)jnull;    break;
       case TINT:     *(int   *)Null = (int  )jnull;    break;
       case TLONG:    *(long  *)Null = (long )jnull;    break;
       case TFLOAT:   *(float *)Null = FLOATNULLVALUE;  break;
       case TDOUBLE:  *(double*)Null = DOUBLENULLVALUE; break;
       case TSTRING:  *(char  *)Null = '\0';            break;
       }
    }

    /* Alter nrows in case calling routine didn't want to do all rows */

    nrows     = minvalue(nrows,lastRow-firstrow+1);

    /* Parser allocates arrays for each column and calculation it performs. */
    /* Limit number of rows processed during each pass to reduce memory     */
    /* requirements... In most cases, iterator will limit rows to less      */
    /* than 2500 rows per iteration, so this is really only relevant for    */
    /* hk-compressed files which must be decompressed in memory and sent    */
    /* whole to parse_data in a single iteration.                           */

    rowOffset = 0;
    remain    = nrows;
    while( remain ) {

       ntodo = minvalue(remain,2500);
       Reset_Parser ( firstrow, rowOffset, ntodo );
       Evaluate_Node( gParse.nNodes-1 );
       if( gParse.status ) break;

       rowOffset += ntodo;
       remain    -= ntodo;

       /*  Copy results into data array  */

       result = gParse.Nodes + gParse.nNodes-1;
       if( result->operation==CONST_OP ) constant = 1;

       switch( result->type ) {

       case BOOLEAN:
       case LONG:
       case DOUBLE:
	  if( constant ) {
	     char undef=0;
	     for( kk=0; kk<ntodo; kk++ )
		ffcvtn( gParse.datatype,
			&(result->value.data),
			&undef, result->value.nelem,
			userInfo->datatype, Null,
			(char*)Data+kk*datasize*result->value.nelem,
			&userInfo->anyNull, &gParse.status );
	  } else {
	     ffcvtn( gParse.datatype,
		     result->value.data.ptr,
		     result->value.undef,
		     result->value.nelem*ntodo,
		     userInfo->datatype, Null, Data,
		     &userInfo->anyNull, &gParse.status );
	     if( result->operation>0 ) {
		free( result->value.data.ptr );
	     }
	  }
          break;

       case BITSTR:
          switch( userInfo->datatype ) {
          case TBYTE:
             for( kk=0; kk<ntodo; kk++ )
                for( jj=0; jj<result->value.nelem; jj++ ) {
                   idx = ( jj + kk * result->value.nelem ) / 8;
                   if( jj%8 == 0 )
                      ((char*)Data)[idx] = 0;
		   if( constant ) {
		      if( result->value.data.str[jj]=='1' )
			 ((char*)Data)[idx] |= 128>>(jj%8);
		   } else {
		      if( result->value.data.strptr[kk][jj]=='1' )
			 ((char*)Data)[idx] |= 128>>(jj%8);
		   }
                }
             break;
          case TBIT:
          case TLOGICAL:
	     if( constant ) {
		for( kk=0; kk<ntodo; kk++ )
		   for( jj=0; jj<result->value.nelem; jj++ ) {
		      ((char*)Data)[ jj+kk*result->value.nelem ] =
			 ( result->value.data.str[jj]=='1' );
		   }
	     } else {
		for( kk=0; kk<ntodo; kk++ )
		   for( jj=0; jj<result->value.nelem; jj++ ) {
		      ((char*)Data)[ jj+kk*result->value.nelem ] =
			 ( result->value.data.strptr[kk][jj]=='1' );
		   }
	     }
             break; 
          default:
             ffpmsg("Cannot convert bit expression to desired type.");
             gParse.status = PARSE_BAD_TYPE;
             break;
          }
          if( result->operation>0 ) {
             free( result->value.data.strptr[0] );
             free( result->value.data.strptr );
          }
          break;

       case STRING:
          if( userInfo->datatype==TSTRING ) {
	     if( constant ) {
		for( jj=0; jj<ntodo; jj++ )
		   strcpy( ((char**)Data)[jj], result->value.data.str );
	     } else {
		for( jj=0; jj<ntodo; jj++ )
		   strcpy( ((char**)Data)[jj], result->value.data.strptr[jj] );
	     }
          } else {
             ffpmsg("Cannot convert string expression to desired type.");
             gParse.status = PARSE_BAD_TYPE;
          }
          if( result->operation>0 ) {
             free( result->value.data.strptr[0] );
             free( result->value.data.strptr );
          }
          break;
       }

       if( gParse.status ) break;

       /*  Increment Data to point to where the next block should go  */

       if( result->type==BITSTR && userInfo->datatype==TBYTE )
          Data = (char*)Data
                    + datasize * ( (result->value.nelem+7)/8 ) * ntodo;
       else if( result->type==STRING )
          Data = (char*)Data + datasize * ntodo;
       else
          Data = (char*)Data + datasize * ntodo * result->value.nelem;
    }

    /* If no NULLs encountered during this pass, set Null value to */
    /* zero to make the writing of the output column data faster   */

    if( userInfo->dataPtr == NULL && !userInfo->anyNull )
       memcpy( Null, zeros, datasize );

    /*-------------------------------------------------------*/
    /*  Clean up procedures:  after processing all the rows  */
    /*-------------------------------------------------------*/

    if( firstrow + nrows - 1 == lastRow ) {
       if( !gParse.status && userInfo->maxRows<totalrows ) return (-1);
    }

    return(gParse.status);  /* return successful status */
}

/*--------------------------------------------------------------------------*/
int ffcvtn( int   inputType,  /* I - Data type of input array               */
            void  *input,     /* I - Input array of type inputType          */
            char  *undef,     /* I - Array of flags indicating UNDEF elems  */
            long  ntodo,      /* I - Number of elements to process          */
            int   outputType, /* I - Data type of output array              */
            void  *nulval,    /* I - Ptr to value to use for UNDEF elements */
            void  *output,    /* O - Output array of type outputType        */
            int   *anynull,   /* O - Any nulls flagged?                     */
            int   *status )   /* O - Error status                           */
/*                                                                          */
/* Convert an array of any input data type to an array of any output        */
/* data type, using an array of UNDEF flags to assign nulvals to            */
/*--------------------------------------------------------------------------*/
{
   long i;

   switch( outputType ) {

   case TLOGICAL:
      switch( inputType ) {
      case TLOGICAL:
      case TBYTE:
         for( i=0; i<ntodo; i++ )
            if( ((unsigned char*)input)[i] )
                ((unsigned char*)output)[i] = 1;
            else
                ((unsigned char*)output)[i] = 0;
         break;
      case TSHORT:
         for( i=0; i<ntodo; i++ )
            if( ((short*)input)[i] )
                ((unsigned char*)output)[i] = 1;
            else
                ((unsigned char*)output)[i] = 0;
         break;
      case TLONG:
         for( i=0; i<ntodo; i++ )
            if( ((long*)input)[i] )
                ((unsigned char*)output)[i] = 1;
            else
                ((unsigned char*)output)[i] = 0;
         break;
      case TFLOAT:
         for( i=0; i<ntodo; i++ )
            if( ((float*)input)[i] )
                ((unsigned char*)output)[i] = 1;
            else
                ((unsigned char*)output)[i] = 0;
         break;
      case TDOUBLE:
         for( i=0; i<ntodo; i++ )
            if( ((double*)input)[i] )
                ((unsigned char*)output)[i] = 1;
            else
                ((unsigned char*)output)[i] = 0;
         break;
      default:
         *status = BAD_DATATYPE;
         break;
      }
      for(i=0;i<ntodo;i++) {
         if( undef[i] ) {
            ((unsigned char*)output)[i] = *(unsigned char*)nulval;
            *anynull = 1;
         }
      }
      break;

   case TBYTE:
      switch( inputType ) {
      case TLOGICAL:
      case TBYTE:
         for( i=0; i<ntodo; i++ )
            ((unsigned char*)output)[i] = ((unsigned char*)input)[i];
         break;
      case TSHORT:
         fffi2i1((short*)input,ntodo,1.,0.,0,0,0,NULL,NULL,(unsigned char*)output,status);
         break;
      case TLONG:
         for (i = 0; i < ntodo; i++) {
            if( ((long*)input)[i] < 0 ) {
               *status = OVERFLOW_ERR;
               ((unsigned char*)output)[i] = 0;
            } else if( ((long*)input)[i] > UCHAR_MAX ) {
               *status = OVERFLOW_ERR;
               ((unsigned char*)output)[i] = UCHAR_MAX;
            } else
               ((unsigned char*)output)[i] = (unsigned char) ((long*)input)[i];
         }
         break;
      case TFLOAT:
         fffr4i1((float*)input,ntodo,1.,0.,0,0,NULL,NULL,
		 (unsigned char*)output,status);
         break;
      case TDOUBLE:
         fffr8i1((double*)input,ntodo,1.,0.,0,0,NULL,NULL,
		 (unsigned char*)output,status);
         break;
      default:
         *status = BAD_DATATYPE;
         break;
      }
      for(i=0;i<ntodo;i++) {
         if( undef[i] ) {
            ((unsigned char*)output)[i] = *(unsigned char*)nulval;
            *anynull = 1;
         }
      }
      break;

   case TSHORT:
      switch( inputType ) {
      case TLOGICAL:
      case TBYTE:
         for( i=0; i<ntodo; i++ )
            ((short*)output)[i] = ((unsigned char*)input)[i];
         break;
      case TSHORT:
         for( i=0; i<ntodo; i++ )
            ((short*)output)[i] = ((short*)input)[i];
         break;
      case TLONG:
         for (i = 0; i < ntodo; i++) {
            if( ((long*)input)[i] < SHRT_MIN ) {
               *status = OVERFLOW_ERR;
               ((short*)output)[i] = SHRT_MIN;
            } else if ( ((long*)input)[i] > SHRT_MAX ) {
               *status = OVERFLOW_ERR;
               ((short*)output)[i] = SHRT_MAX;
            } else
               ((short*)output)[i] = (short) ((long*)input)[i];
         }
         break;
      case TFLOAT:
         fffr4i2((float*)input,ntodo,1.,0.,0,0,NULL,NULL,
		 (short*)output,status);
         break;
      case TDOUBLE:
         fffr8i2((double*)input,ntodo,1.,0.,0,0,NULL,NULL,
		 (short*)output,status);
         break;
      default:
         *status = BAD_DATATYPE;
         break;
      }
      for(i=0;i<ntodo;i++) {
         if( undef[i] ) {
            ((short*)output)[i] = *(short*)nulval;
            *anynull = 1;
         }
      }
      break;

   case TINT:
      switch( inputType ) {
      case TLOGICAL:
      case TBYTE:
         for( i=0; i<ntodo; i++ )
            ((int*)output)[i] = ((unsigned char*)input)[i];
         break;
      case TSHORT:
         for( i=0; i<ntodo; i++ )
            ((int*)output)[i] = ((short*)input)[i];
         break;
      case TLONG:
         for( i=0; i<ntodo; i++ )
            ((int*)output)[i] = ((long*)input)[i];
         break;
      case TFLOAT:
         fffr4int((float*)input,ntodo,1.,0.,0,0,NULL,NULL,
		  (int*)output,status);
         break;
      case TDOUBLE:
         fffr8int((double*)input,ntodo,1.,0.,0,0,NULL,NULL,
		  (int*)output,status);
         break;
      default:
         *status = BAD_DATATYPE;
         break;
      }
      for(i=0;i<ntodo;i++) {
         if( undef[i] ) {
            ((int*)output)[i] = *(int*)nulval;
            *anynull = 1;
         }
      }
      break;

   case TLONG:
      switch( inputType ) {
      case TLOGICAL:
      case TBYTE:
         for( i=0; i<ntodo; i++ )
            ((long*)output)[i] = ((unsigned char*)input)[i];
         break;
      case TSHORT:
         for( i=0; i<ntodo; i++ )
            ((long*)output)[i] = ((short*)input)[i];
         break;
      case TLONG:
         for( i=0; i<ntodo; i++ )
            ((long*)output)[i] = ((long*)input)[i];
         break;
      case TFLOAT:
         fffr4i4((float*)input,ntodo,1.,0.,0,0,NULL,NULL,
		 (long*)output,status);
         break;
      case TDOUBLE:
         fffr8i4((double*)input,ntodo,1.,0.,0,0,NULL,NULL,
		 (long*)output,status);
         break;
      default:
         *status = BAD_DATATYPE;
         break;
      }
      for(i=0;i<ntodo;i++) {
         if( undef[i] ) {
            ((long*)output)[i] = *(long*)nulval;
            *anynull = 1;
         }
      }
      break;

   case TFLOAT:
      switch( inputType ) {
      case TLOGICAL:
      case TBYTE:
         for( i=0; i<ntodo; i++ )
            ((float*)output)[i] = ((unsigned char*)input)[i];
         break;
      case TSHORT:
         for( i=0; i<ntodo; i++ )
            ((float*)output)[i] = ((short*)input)[i];
         break;
      case TLONG:
         for( i=0; i<ntodo; i++ )
            ((float*)output)[i] = ((long*)input)[i];
         break;
      case TFLOAT:
         for( i=0; i<ntodo; i++ )
            ((float*)output)[i] = ((float*)input)[i];
         break;
      case TDOUBLE:
         fffr8r4((double*)input,ntodo,1.,0.,0,0,NULL,NULL,
		 (float*)output,status);
         break;
      default:
         *status = BAD_DATATYPE;
         break;
      }
      for(i=0;i<ntodo;i++) {
         if( undef[i] ) {
            ((float*)output)[i] = *(float*)nulval;
            *anynull = 1;
         }
      }
      break;

   case TDOUBLE:
      switch( inputType ) {
      case TLOGICAL:
      case TBYTE:
         for( i=0; i<ntodo; i++ )
            ((double*)output)[i] = ((unsigned char*)input)[i];
         break;
      case TSHORT:
         for( i=0; i<ntodo; i++ )
            ((double*)output)[i] = ((short*)input)[i];
         break;
      case TLONG:
         for( i=0; i<ntodo; i++ )
            ((double*)output)[i] = ((long*)input)[i];
         break;
      case TFLOAT:
         for( i=0; i<ntodo; i++ )
            ((double*)output)[i] = ((float*)input)[i];
         break;
      case TDOUBLE:
         for( i=0; i<ntodo; i++ )
            ((double*)output)[i] = ((double*)input)[i];
         break;
      default:
         *status = BAD_DATATYPE;
         break;
      }
      for(i=0;i<ntodo;i++) {
         if( undef[i] ) {
            ((double*)output)[i] = *(double*)nulval;
            *anynull = 1;
         }
      }
      break;

   default:
      *status = BAD_DATATYPE;
      break;
   }
   return ( *status );
}

/*---------------------------------------------------------------------------*/
int fffrwc( fitsfile *fptr,        /* I - Input FITS file                    */
            char     *expr,        /* I - Boolean expression                 */
            char     *timeCol,     /* I - Name of time column                */
            char     *parCol,      /* I - Name of parameter column           */
            char     *valCol,      /* I - Name of value column               */
            long     ntimes,       /* I - Number of distinct times in file   */
            double   *times,       /* O - Array of times in file             */
            char     *time_status, /* O - Array of boolean results           */
            int      *status )     /* O - Error status                       */
/*                                                                           */
/* Evaluate a boolean expression for each time in a compressed file,         */
/* returning an array of flags indicating which times evaluated to TRUE/FALSE*/
/*---------------------------------------------------------------------------*/
{
   parseInfo Info;
   long alen, width;
   int parNo, typecode;
   int naxis, constant, nCol=0;
   long nelem, naxes[MAXDIMS], elem;
   char result;

   if( *status ) return( *status );

   fits_get_colnum( fptr, CASEINSEN, timeCol, &gParse.timeCol, status );
   fits_get_colnum( fptr, CASEINSEN, parCol,  &gParse.parCol , status );
   fits_get_colnum( fptr, CASEINSEN, valCol,  &gParse.valCol, status );
   if( *status ) return( *status );
   
   if( ffiprs( fptr, 1, expr, MAXDIMS, &Info.datatype, &nelem,
               &naxis, naxes, status ) ) {
      ffcprs();
      return( *status );
   }
   if( Info.datatype<0 ) {
      constant = 1;
      Info.datatype = -Info.datatype;
      nCol = gParse.nCols;
      gParse.nCols = 0;    /*  Ignore all column references  */
   } else
      constant = 0;

   if( Info.datatype!=TLOGICAL || nelem!=1 ) {
      ffcprs();
      ffpmsg("Expression does not evaluate to a logical scalar.");
      return( *status = PARSE_BAD_TYPE );
   }

   /*******************************************/
   /* Allocate data arrays for each parameter */
   /*******************************************/
   
   parNo = gParse.nCols;
   while( parNo-- ) {
      switch( gParse.colData[parNo].datatype ) {
      case TLONG:
	 if( (gParse.colData[parNo].array =
	      (long *)malloc( (ntimes+1)*sizeof(long) )) )
	    ((long*)gParse.colData[parNo].array)[0] = 1234554321;
	 else
	    *status = MEMORY_ALLOCATION;
	 break;
      case TDOUBLE:
	 if( (gParse.colData[parNo].array =
	      (double *)malloc( (ntimes+1)*sizeof(double) )) )
	    ((double*)gParse.colData[parNo].array)[0] = DOUBLENULLVALUE;
	 else
	    *status = MEMORY_ALLOCATION;
	 break;
      case TSTRING:
	 if( !fits_get_coltype( fptr, gParse.valCol, &typecode,
				&alen, &width, status ) ) {
	    alen++;
	    if( (gParse.colData[parNo].array =
		 (char **)malloc( (ntimes+1)*sizeof(char*) )) ) {
	       if( (((char **)gParse.colData[parNo].array)[0] =
		    (char *)malloc( (ntimes+1)*sizeof(char)*alen )) ) {
		  for( elem=1; elem<=ntimes; elem++ )
		     ((char **)gParse.colData[parNo].array)[elem] =
			((char **)gParse.colData[parNo].array)[elem-1]+alen;
		  ((char **)gParse.colData[parNo].array)[0][0] = '\0';
	       } else {
		  free( gParse.colData[parNo].array );
		  *status = MEMORY_ALLOCATION;
	       }
	    } else {
	       *status = MEMORY_ALLOCATION;
	    }
	 }
	 break;
      }
      if( *status ) {
	 while( parNo-- ) {
	    if( gParse.colData[parNo].datatype==TSTRING )
	       free( ((char **)gParse.colData[parNo].array)[0] );
	    free( gParse.colData[parNo].array );
	 }
	 return( *status );
      }
   }
   
   /**********************************************************************/
   /* Read data from columns needed for the expression and then parse it */
   /**********************************************************************/
   
   if( !uncompress_hkdata( fptr, ntimes, times, status ) ) {
      if( constant ) {
	 result = gParse.Nodes[gParse.nNodes-1].value.data.log;
	 elem = ntimes;
	 while( elem-- ) time_status[elem] = result;
      } else {
	 Info.dataPtr  = time_status;
	 Info.nullPtr  = NULL;
	 Info.maxRows  = ntimes;
	 *status       = parse_data( ntimes, 0, 1, ntimes, gParse.nCols,
				     gParse.colData, (void*)&Info );
      }
   }
   
   /************/
   /* Clean up */
   /************/
   
   parNo = gParse.nCols;
   while ( parNo-- ) {
      if( gParse.colData[parNo].datatype==TSTRING )
	 free( ((char **)gParse.colData[parNo].array)[0] );
      free( gParse.colData[parNo].array );
   }
   
   if( constant ) gParse.nCols = nCol;

   ffcprs();
   return(*status);
}

/*---------------------------------------------------------------------------*/
int uncompress_hkdata( fitsfile *fptr,
                       long     ntimes,
                       double   *times,
                       int      *status )
/*                                                                           */
/* description                                                               */
/*---------------------------------------------------------------------------*/
{
   char parName[256], *sPtr[1], found[1000];
   int parNo, anynul;
   long naxis2, row, currelem;
   double currtime, newtime;

   sPtr[0] = parName;
   currelem = 0;
   currtime = -1e38;

   parNo=gParse.nCols;
   while( parNo-- ) found[parNo] = 0;

   if( ffgkyj( fptr, "NAXIS2", &naxis2, NULL, status ) ) return( *status );

   for( row=1; row<=naxis2; row++ ) {
      if( ffgcvd( fptr, gParse.timeCol, row, 1L, 1L, 0.0,
                  &newtime, &anynul, status ) ) return( *status );
      if( newtime != currtime ) {
         /*  New time encountered... propogate parameters to next row  */
         if( newtime<currtime ) {
            ffpmsg("Time stamps out of order");
            return( *status = PARSE_BAD_COL );
         }
         if( currelem==ntimes ) {
            ffpmsg("Found more unique time stamps than caller indicated");
            return( *status = PARSE_BAD_COL );
         }
         times[currelem++] = currtime = newtime;
	 parNo = gParse.nCols;
         while( parNo-- ) {
            switch( gParse.colData[parNo].datatype ) {
            case TLONG:
               ((long*)gParse.colData[parNo].array)[currelem] =
                  ((long*)gParse.colData[parNo].array)[currelem-1];
               break;
            case TDOUBLE:
               ((double*)gParse.colData[parNo].array)[currelem] =
                  ((double*)gParse.colData[parNo].array)[currelem-1];
               break;
            case TSTRING:
               strcpy( ((char **)gParse.colData[parNo].array)[currelem],
                       ((char **)gParse.colData[parNo].array)[currelem-1] );
               break;
            }
         }
      }

      if( ffgcvs( fptr, gParse.parCol, row, 1L, 1L, "",
                  sPtr, &anynul, status ) ) return( *status );
      parNo = gParse.nCols;
      while( parNo-- )
	 if( !strcasecmp( parName, gParse.colData[parNo].colname ) ) break;

      if( parNo>=0 ) {
	 found[parNo] = 1; /* Flag this parameter as found */
         switch( gParse.colData[parNo].datatype ) {
         case TLONG:
            ffgcvj( fptr, gParse.valCol, row, 1L, 1L,
                    ((long*)gParse.colData[parNo].array)[0],
                    ((long*)gParse.colData[parNo].array)+currelem,
                    &anynul, status );
            break;
         case TDOUBLE:
            ffgcvd( fptr, gParse.valCol, row, 1L, 1L,
                    ((double*)gParse.colData[parNo].array)[0],
                    ((double*)gParse.colData[parNo].array)+currelem,
                    &anynul, status );
            break;
         case TSTRING:
            ffgcvs( fptr, gParse.valCol, row, 1L, 1L,
                    ((char**)gParse.colData[parNo].array)[0],
                    ((char**)gParse.colData[parNo].array)+currelem,
                    &anynul, status );
            break;
         }
         if( *status ) return( *status );
      }
   }

   if( currelem<ntimes ) {
      ffpmsg("Found fewer unique time stamps than caller indicated");
      return( *status = PARSE_BAD_COL );
   }

   /*  Check for any parameters which were not located in the table  */
   parNo = gParse.nCols;
   while( parNo-- )
      if( !found[parNo] ) {
	 sprintf( parName, "Parameter not found: %-30s", 
		  gParse.colData[parNo].colname );
	 ffpmsg( parName );
	 *status = PARSE_SYNTAX_ERR;
      }
   return( *status );
}
