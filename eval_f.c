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
int fits_find_rows( fitsfile *fptr,        /* I - Input FITS file            */
                    char     *expr,        /* I - Boolean expression         */
                    long     firstrow,     /* I - First row of table to eval */
                    long     nrows,        /* I - Number of rows to evaluate */
                    long     *n_good_rows, /* O - Number of rows eval to True*/
                    char     *row_status,  /* O - Array of boolean results   */
                    int      *status )     /* O - Error status               */
/*                                                                           */
/* Evaluate a boolean expression using the indicated rows, returning an      */
/* array of flags indicating which rows evaluated to TRUE/FALSE              */
/*---------------------------------------------------------------------------*/
{
   parseInfo Info;
   int naxis;
   long nelem, naxes[MAXDIMS], elem;

   if( *status ) return( *status );

   if( ffiprs( fptr, 0, expr, MAXDIMS, &Info.datatype, &nelem, &naxis,
               naxes, status ) ) {
      ffcprs();
      return( *status );
   }

   if( Info.datatype!=TLOGICAL || nelem!=1 ) {
      ffcprs();
      ffpmsg("Expression does not evaluate to a logical scalar.");
      return( *status = PARSE_BAD_TYPE );
   }

   firstrow     = (firstrow>1 ? firstrow : 1);
   Info.dataPtr = row_status;
   Info.nullPtr = NULL;
   Info.maxRows = nrows;
   
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

   ffcprs();
   return(*status);
}

/*--------------------------------------------------------------------------*/
int fits_select_rows( fitsfile *infptr,  /* I - Input FITS file             */
                      fitsfile *outfptr, /* I - Output FITS file            */
                      char     *expr,    /* I - Boolean expression          */
                      int      *status ) /* O - Error status                */
/*                                                                          */
/* Evaluate an expression on all rows of a table.  If the input and output  */
/* files are not the same, copy the TRUE rows to the output file.  If the   */
/* files are the same, delete the FALSE rows (preserve the TRUE rows).      */
/*--------------------------------------------------------------------------*/
{
   parseInfo Info;
   int naxis;
   long nelem, naxes[MAXDIMS], nrows, rdlen, row, maxrows, nbuff;
   long inloc, outloc, ntodo;
   char *buffer;

   if( *status ) return( *status );

   if( ffiprs( infptr, 0, expr, MAXDIMS, &Info.datatype, &nelem, &naxis,
               naxes, status ) ) {
      ffcprs();
      return( *status );
   }

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
   
   if( ffiter( gParse.nCols, gParse.colData, 0L, 0L,
               parse_data, (void*)&Info, status )  ) {
      /* Error... Do nothing */
   } else {
      buffer  = (char *)malloc( 50000*sizeof(char) );
      if( buffer==NULL ) {
         ffcprs();
         return( *status=MEMORY_ALLOCATION );
      }
      maxrows = 50000L/rdlen;
      nbuff = 0;
      row = 0;
      if( infptr==outfptr )
         while( ((char*)Info.dataPtr)[row] ) row++;
      inloc  =  infptr->datastart + row*rdlen;

      if( outfptr->datastart<0 )  /* rescan header if data pointer undefined */
         ffrdef(outfptr, status);
      outloc = outfptr->datastart + row*rdlen;

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
      } while( !*status && row<=nrows );

      if( nbuff ) {
         ffmbyt( outfptr, outloc,      IGNORE_EOF, status );
         ffpbyt( outfptr, rdlen*nbuff, buffer,     status );
         outloc += nbuff*rdlen;
      }

      row = (outloc - outfptr->datastart)/rdlen;
      if( infptr==outfptr ) {
         if( row<nrows ) ffdrow( infptr, row+1, nrows-row, status );
      } else if( infptr->heapsize ) { /* Copy heap, if it exists */
         inloc = infptr->datastart + infptr->heapstart;
         ntodo = infptr->heapsize;

         while ( ntodo ) {
            rdlen = (ntodo<50000 ? ntodo : 50000);
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
int fits_calc_rows( fitsfile *fptr,     /* I - Input FITS file               */
                    int      datatype,  /* I - Datatype to return results as */
                    char     *expr,     /* I - Arithmetic expression         */
                    long     firstrow,  /* I - First row to evaluate         */
                    long     nelements, /* I - Number of elements to return  */
                    void     *nulval,   /* I - Ptr to value to use as UNDEF  */
                    void     *array,    /* O - Array of results              */
                    int      *anynul,   /* O - Were any UNDEFs encountered?  */
                    int      *status )  /* O - Error status                  */
/*                                                                           */
/* Calculate an expression for the indicated rows of a table, returning      */
/* the results, cast as datatype (TSHORT, TDOUBLE, etc), in array.  If       */
/* nulval==NULL, UNDEFs will be zeroed out.  For vector results, the number  */
/* of elements returned may be less than nelements if nelements is not an    */
/* even multiple of the result dimension.  Call fits_test_expr to obtain     */
/* the dimensions of the results.                                            */
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
   
   if( ffiter( gParse.nCols, gParse.colData, firstrow-1, 0,
               parse_data, (void*)&Info, status ) == -1 )
      *status=0;  /* -1 indicates exitted without error before end... OK */

   *anynul = Info.anyNull;
   ffcprs();
   return( *status );
}

/*--------------------------------------------------------------------------*/
int fits_calc_col( fitsfile *infptr,  /* I - Input FITS file                */
                   char     *expr,    /* I - Arithmetic expression          */
                   fitsfile *outfptr, /* I - Output fits file               */
                   char     *colname, /* I - Name of output column          */
                   int      *status ) /* O - Error status                   */
/*                                                                          */
/* Evaluate an expression for each row in the input FITS file and place     */
/* the results into the named column of the output fits file.  It is the    */
/* caller's responsibility to make sure the column exists and is of the     */
/* proper type to hold the results, although type conversions are handled   */
/* by CFITSIO.                                                              */
/*--------------------------------------------------------------------------*/
{
   parseInfo Info;
   int naxis;
   long nelem, naxes[MAXDIMS];
   int col_cnt;

   if( *status ) return( *status );
   
   if( ffiprs( infptr, 0, expr, MAXDIMS, &Info.datatype, &nelem, &naxis,
               naxes, status ) ) {
      ffcprs();
      return( *status );
   }

   /*************************************/
   /* Create new iterator Output Column */
   /*************************************/

   col_cnt = gParse.nCols;
   if( (col_cnt%25)==0 ) {
      gParse.colData = 
         (iteratorCol*)realloc( gParse.colData,
                                (col_cnt+25)*sizeof(iteratorCol) );
      gParse.colInfo =
         (DataInfo*)realloc( gParse.colInfo,
                             (col_cnt+25)*sizeof(DataInfo) );
      if(    gParse.colData == NULL
          || gParse.colInfo == NULL    ) {
         if( gParse.colData ) free(gParse.colData);
         if( gParse.colInfo ) free(gParse.colInfo);
         ffcprs();
         return( *status = MEMORY_ALLOCATION );
      }
   }
   gParse.nCols++;
   gParse.colData[col_cnt].fptr     = outfptr;
   gParse.colData[col_cnt].colnum   = 0;
   gParse.colData[col_cnt].iotype   = OutputCol;
   gParse.colData[col_cnt].datatype = 0;
   strcpy(gParse.colData[col_cnt].colname,colname);

   Info.dataPtr = NULL;
   Info.maxRows = -1;    /*  Process all of the rows  */

   ffiter( gParse.nCols, gParse.colData, 0, 0,
           parse_data, (void*)&Info, status );

   ffcprs();
   return( *status );
}

/*--------------------------------------------------------------------------*/
int fits_test_expr( fitsfile *fptr,     /* I - Input FITS file              */
                    char     *expr,     /* I - Arithmetic expression        */
                    int      maxdim,    /* I - Max Dimension of naxes       */
                    int      *datatype, /* O - Data type of result          */
                    long     *nelem,    /* O - Vector length of result      */
                    int      *naxis,    /* O - # of dimensions of result    */
                    long     *naxes,    /* O - Size of each dimension       */
                    int      *status )  /* O - Error status                 */
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
   char line[1024];
   int  i,lexpr,len,mlen,type;

   if( *status ) return( *status );

   /*  Initialize the Parser structure  */

   gParse.init_flag  = 1;
   gParse.def_fptr   = fptr;
   gParse.compressed = compressed;
   gParse.nCols      = 0;
   gParse.colData    = NULL;
   gParse.colInfo    = NULL;
   gParse.firstRow   = 1;
   gParse.currRow    = 1;
   gParse.currCol    = 0;
   gParse.byteloc    = 0;
   gParse.nbytes     = 0;
   gParse.bytecodes  = NULL;
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

   /*  Parse the expression with dummy data to locate all  */
   /*  the needed keys and columns                         */

   gParse.undef = 0;
   ffrestart(NULL);
   ffparse();
   gParse.init_flag = 0;

   /*  Check results  */

   *status = gParse.status;
   if( *status ) return(*status);

   type = gParse.resultInfo.type;
   if( type==BOOLEAN || type==INTEGER || type==REAL
             || type==STRING ) {
      *nelem = *naxis = naxes[0] = gParse.resultInfo.nelem = 
         gParse.resultInfo.naxis = gParse.resultInfo.naxes[0] = 1;
   } else if( type==BITSTR ) {
      *naxis = gParse.resultInfo.naxis = 1;
      *nelem = naxes[0] = gParse.resultInfo.nelem = 
               gParse.resultInfo.naxes[0] = strlen(gParse.resultData.string);
   } else {
      *naxis = gParse.resultInfo.naxis = gParse.resultData.intvec.naxis;
      *nelem = gParse.resultInfo.nelem = gParse.resultData.intvec.nelem;
      for( i=0; i<*naxis && i<maxdim; i++ )
         naxes[i] = gParse.resultInfo.naxes[i] =
            gParse.resultData.intvec.naxes[i];
      free(gParse.resultData.intvec.iptr);
      free(gParse.resultData.intvec.undef);
   }

   switch( type ) {
   case BOOLEANV:
   case BOOLEAN:
      *datatype = TLOGICAL;
      break;
   case INTEGERV:
   case INTEGER:
      *datatype = TLONG;
      break;
   case REALV:
   case REAL:
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
   return(*status);
}

/*--------------------------------------------------------------------------*/
void ffcprs( void )  /*  No parameters                                      */
/*                                                                          */
/* Clear the parser, making it ready to accept a new expression.            */
/*--------------------------------------------------------------------------*/
{
   if( gParse.nCols > 0 ) {
      free( gParse.colData );
      free( gParse.colInfo );
      gParse.nCols = 0;
   }
   free(gParse.expr);
   free(gParse.bytecodes);
   gParse.expr = NULL;
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
    int ii, jj;

    /* declare variables static to preserve their values between calls */
    static void *Data, *Null;
    static int  datasize;
    static long lastRow;
    static parseInfo *userInfo;
    static long zeros[4] = {0,0,0,0};

    /*--------------------------------------------------------*/
    /*  Initialization procedures: execute on the first call  */
    /*--------------------------------------------------------*/
    if (firstrow == offset+1)
    {
       userInfo = (parseInfo*)userPtr;
       userInfo->anyNull = 0;

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
       } else {
          Data = userInfo->dataPtr;
          Null = (userInfo->nullPtr ? userInfo->nullPtr : zeros);
       }

       /* Determine the size of each element of the returned result */

       switch( userInfo->datatype ) {
       case TBIT:       /*  Fall through to TBYTE  */
       case TLOGICAL:   /*  Fall through to TBYTE  */
       case TBYTE:     datasize = sizeof(char);   break;
       case TSHORT:    datasize = sizeof(short);  break;
       case TLONG:     datasize = sizeof(long);   break;
       case TDOUBLE:   datasize = sizeof(double); break;
       case TSTRING:   datasize = sizeof(char*);  break;
       }
    }

    /*-------------------------------------------*/
    /*  Main loop: process all the rows of data  */
    /*-------------------------------------------*/

    /*  NOTE: 1st element of array is the null pixel value!  */

    gParse.firstRow = firstrow;
    nrows = minvalue(nrows,lastRow-firstrow+1);
    if( userInfo->dataPtr == NULL )
       Data = (char*)colData[nCols-1].array + datasize;

    for (ii = 0; ii < nrows; ii++)
    {
       gParse.undef = 0;
       gParse.currRow = firstrow + ii;
       gParse.byteloc = 0;
       ffparse();

       if( gParse.status ) break;

       /*  Copy results into data array  */

       switch( gParse.resultInfo.type ) {
       case BOOLEAN:
       case INTEGER:
       case REAL:
          ffcvtn( gParse.datatype,
                  &gParse.resultData,
                  &gParse.undef,
                  1,
                  userInfo->datatype, Null, Data,
                  &userInfo->anyNull, &gParse.status );
          break;
       case BOOLEANV:
       case INTEGERV:
       case REALV:
          if( gParse.undef ) memset( gParse.resultData.boolvec.undef, 1, 
                                     gParse.resultInfo.nelem );
          ffcvtn( gParse.datatype,
                  gParse.resultData.boolvec.bptr,
                  gParse.resultData.boolvec.undef,
                  gParse.resultInfo.nelem,
                  userInfo->datatype, Null, Data,
                  &userInfo->anyNull, &gParse.status );
          free( gParse.resultData.boolvec.bptr );
          free( gParse.resultData.boolvec.undef );
          break;
       case BITSTR:
          switch( userInfo->datatype ) {
          case TBYTE:
             for( jj=0; jj<gParse.resultInfo.nelem; jj++ ) {
                if( jj%8 == 0 ) ((char*)Data)[jj/8] = 0;
                if( gParse.resultData.string[jj]=='1' )
                   ((char*)Data)[jj/8] |= 128>>(jj%8);
             }
             break;
          case TBIT:
          case TLOGICAL:
             for( jj=0; jj<gParse.resultInfo.nelem; jj++ ) {
                ((char*)Data)[jj] = (gParse.resultData.string[jj]=='1');
             }
             break; 
          default:
             ffpmsg("Cannot convert bit expression to desired type.");
             gParse.status = PARSE_BAD_TYPE;
             break;
          }
          break;
       case STRING:
          if( userInfo->datatype==TSTRING ) {
             strcpy( *(char**)Data, gParse.resultData.string );
          } else {
             ffpmsg("Cannot convert string expression to desired type.");
             gParse.status = PARSE_BAD_TYPE;
          }
          break;
       }
       if( gParse.status ) break;

       /*  Increment Data to point to where the next row should go  */

       if( gParse.resultInfo.type==BITSTR && userInfo->datatype==TBYTE )
          Data = (char*)Data + datasize*( (gParse.resultInfo.nelem+7)/8 );
       else
          Data = (char*)Data + datasize*gParse.resultInfo.nelem;
    }

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
         fffr4i1((float*)input,ntodo,1.,0.,0,0,NULL,NULL,(unsigned char*)output,status);
         break;
      case TDOUBLE:
         fffr8i1((double*)input,ntodo,1.,0.,0,0,NULL,NULL,(unsigned char*)output,status);
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
         fffr4i2((float*)input,ntodo,1.,0.,0,0,NULL,NULL,(short*)output,status);
         break;
      case TDOUBLE:
         fffr8i2((double*)input,ntodo,1.,0.,0,0,NULL,NULL,(short*)output,status);
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
         fffr4i4((float*)input,ntodo,1.,0.,0,0,NULL,NULL,(long*)output,status);
         break;
      case TDOUBLE:
         fffr8i4((double*)input,ntodo,1.,0.,0,0,NULL,NULL,(long*)output,status);
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
         fffr8r4((double*)input,ntodo,1.,0.,0,0,NULL,NULL,(float*)output,status);
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
int fits_find_rows_cmp(           /*                                         */
           fitsfile *fptr,        /* I - Input FITS file                     */
           char     *expr,        /* I - Boolean expression                  */
           char     *timeCol,     /* I - Name to time column                 */
           char     *parCol,      /* I - Name to parameter column            */
           char     *valCol,      /* I - Name to value column                */
           long     ntimes,       /* I - Number of distinct times in file    */
           double   *times,       /* O - Array of times in file              */
           char     *time_status, /* O - Array of boolean results            */
           int      *status )     /* O - Error status                        */
/*                                                                           */
/* Evaluate a boolean expression for each time in a compressed file,         */
/* returning an array of flags indicating which times evaluated to TRUE/FALSE*/
/*---------------------------------------------------------------------------*/
{
   parseInfo Info;
   long alen, width;
   int parNo, typecode;
   int naxis;
   long nelem, naxes[MAXDIMS], elem;

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

   if( Info.datatype!=TLOGICAL || nelem!=1 ) {
      ffcprs();
      ffpmsg("Expression does not evaluate to a logical scalar.");
      return( *status = PARSE_BAD_TYPE );
   }

   /*******************************************/
   /* Allocate data arrays for each parameter */
   /*******************************************/
   
   for( parNo=0; parNo<gParse.nCols; parNo++ ) {
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
      Info.dataPtr  = time_status;
      Info.nullPtr  = NULL;
      Info.maxRows  = ntimes;
      *status       = parse_data( ntimes, 0, 1, ntimes, gParse.nCols,
                                  gParse.colData, (void*)&Info );
   }
   
   /************/
   /* Clean up */
   /************/
   
   for( parNo=0; parNo<gParse.nCols; parNo++ ) {
      if( gParse.colData[parNo].datatype==TSTRING )
         free( ((char **)gParse.colData[parNo].array)[0] );
      free( gParse.colData[parNo].array );
   }
   
   ffcprs();
   return(*status);
}

/*---------------------------------------------------------------------------*/
int uncompress_hkdata( fitsfile *fptr,    /* I - FITS file pointer           */
                       long     ntimes,   /* I - Number of time stampts      */
                       double   *times,   /* O - Time stamps in file         */
                       int      *status ) /* O - CFITSIO error status        */
/*                                                                           */
/* Search compressed hk file for the parameters referenced in gParse.expr    */
/*---------------------------------------------------------------------------*/
{
   char parName[256], *sPtr[1];
   int parNo, anynul;
   long naxis2, row, currelem;
   double currtime, newtime;

   sPtr[0] = parName;
   currelem = 0;
   currtime = -1e38;

   if( ffgkyj( fptr, "NAXIS2", &naxis2, NULL, status ) ) return( *status );

   for( row=1; row<=naxis2; row++ ) {
      if( ffgcvd( fptr, gParse.timeCol, row, 1L, 1L, 0.0,
                  &newtime, &anynul, status ) ) return( *status );
      if( newtime != currtime ) {
         /*  New time encountered... propogate parameters to next row  */
         if( newtime<currtime ) {
            /* ERROR */
            ffpmsg("Time stamps out of order");
            return( *status = PARSE_BAD_COL );
         }
         if( currelem==ntimes ) {
            /* ERROR */
            ffpmsg("Found more unique time stamps than caller indicated");
            return( *status = PARSE_BAD_COL );
         }
         times[currelem++] = currtime = newtime;
         for( parNo=0; parNo<gParse.nCols; parNo++ ) {
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
      parNo = 0;
      while( parNo<gParse.nCols
             && strcasecmp( parName, gParse.colData[parNo].colname ) ) parNo++;

      if( parNo<gParse.nCols ) {
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
      /* ERROR */
      ffpmsg("Found fewer unique time stamps than caller indicated");
      return( *status = PARSE_BAD_COL );
   }
   return( *status );
}
