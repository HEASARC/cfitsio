#include "fitsio2.h"
#include "f77_wrap.h"

/* Use a simple ellipse prototype for Fwork_fn to satisfy finicky compilers */
typedef struct {
   void *userData;
   void (*Fwork_fn)(PLONG_cfTYPE *total_n, ...);
} FtnUserData;

/******************************************************************/
/*  Cffiter is the wrapper for CFITSIO's ffiter which takes most  */
/*  of its arguments via a structure, iteratorCol.  This routine  */
/*  takes a list of arrays and converts them into a single array  */
/*  of type iteratorCol and passes it to CFITSIO.  Because ffiter */
/*  will be passing control to a Fortran work function, the C     */
/*  wrapper, Cwork_fn, must be passed in its place which then     */
/*  calls the Fortran routine after the necessary data            */
/*  manipulation.  The Fortran routine is passed via the user-    */
/*  supplied parameter pointer.                                   */
/******************************************************************/

void Cffiter( int n_cols, int *units, int *colnum, char *colname[], 
	      int *datatype, int *iotype,
              long offset, long n_per_loop, void *Fwork_fn,
	      void *userData, int *status)
{
   iteratorCol *cols;
   int i;
   FtnUserData FuserData;
   extern Cwork_fn();

   FuserData.Fwork_fn = (void(*)(PLONG_cfTYPE *,...))Fwork_fn;
   FuserData.userData = userData;

   cols = malloc( n_cols*sizeof(iteratorCol) );
   if( cols==NULL ) printf ("Bad Pointer!\n");
   for(i=0;i<n_cols;i++) {
      cols[i].fptr     = gFitsFiles[ units[i] ];
      cols[i].colnum   = colnum[i];
      strncpy(cols[i].colname,colname[i],70);
      cols[i].datatype = datatype[i];
      cols[i].iotype   = iotype[i];
   }

   ffiter( n_cols, cols, offset, n_per_loop, Cwork_fn, 
	   (void*)&FuserData, status );
   free(cols);
}
#define ftiter_STRV_A4 NUM_ELEM_ARG(1)
FCALLSCSUB11(Cffiter,FTITER,ftiter,INT,INTV,INTV,STRINGV,INTV,INTV,LONG,LONG,PVOID,PVOID,PINT)

/*-----------------------------------------------------------------*/
/*  This function is called by CFITSIO's ffiter and serves as the  */
/*  wrapper for the Fortran work function which is passed in the   */
/*  extra user-supplied pointer.  It breaks up C's iteratorCol     */
/*  into several separate arrays.  Because we cannot send an       */
/*  array of pointers for the column data, we instead send *many*  */
/*  arrays as final parameters.                                    */
/*-----------------------------------------------------------------*/

int Cwork_fn( long total_n, long offset,       long first_n,    long n_values,
	      int n_cols,   iteratorCol *cols, void *FuserData )
{
   int  *units, *colnum, *datatype, *iotype, *repeat;
   char **ptrs;
   int  i,j,status=0;

   /*  Allocate memory for all the arrays.  Grab all the int's  */
   /*  at once and divide up among parameters                   */

   ptrs  = (char**)malloc(n_cols*sizeof(char*));
   units = (int*)malloc(5*n_cols*sizeof(int));
   colnum   = units + 1 * n_cols;
   datatype = units + 2 * n_cols;
   iotype   = units + 3 * n_cols;
   repeat   = units + 4 * n_cols;

   for(i=0;i<n_cols;i++) {
      for(j=0;j<MAXFITSFILES;j++)
	 if( cols[i].fptr==gFitsFiles[j] )
	    units[i] = j;
      colnum[i]   = cols[i].colnum;
      datatype[i] = cols[i].datatype;
      iotype[i]   = cols[i].iotype;
      repeat[i]   = cols[i].repeat;

      if( datatype[i]==TLOGICAL ) {
	 /*  Don't forget first element is null value  */
	 ptrs[i] = malloc( (n_values*repeat[i]+1)*4 );
	 for( j=0;j<=n_values*repeat[i]; j++ )
	    ((int*)ptrs[i])[j] = C2FLOGICAL( ((char*)cols[i].array)[j]);
      } else
	 ptrs[i] = cols[i].array;
   }

   if(!status) {
              /*  Handle Fortran function call manually...  */
	      /*  cfortran.h cannot handle all the desired  */
              /*  'ptrs' nor the indirect function call.    */

      PLONG_cfTYPE a1,a2,a3,a4;    /* Do this in case longs are */ 
      FtnUserData *f;              /* not the same size as ints */

      a1 = total_n;
      a2 = offset;
      a3 = first_n;
      a4 = n_values;
      f = FuserData;

      f->Fwork_fn(&a1,&a2,&a3,&a4,&n_cols,units,colnum,datatype,
		  iotype,repeat,&status,f->userData,
		  ptrs[ 0], ptrs[ 1], ptrs[ 2], ptrs[ 3], ptrs[ 4],
		  ptrs[ 5], ptrs[ 6], ptrs[ 7], ptrs[ 8], ptrs[ 9],
		  ptrs[10], ptrs[11], ptrs[12], ptrs[13], ptrs[14],
		  ptrs[15], ptrs[16], ptrs[17], ptrs[18], ptrs[19] );
   }

   /*  Check whether there are any LOGICAL columns being outputted  */
   for( i=0;i<n_cols;i++ ) {
      if( iotype[i]!=InputCol ) {
	 if( datatype[i]==TLOGICAL ) {
	    for( j=0;j<=n_values*repeat[i];j++ )
	       ((char*)cols[i].array)[j] = F2CLOGICAL( ((int*)ptrs[i])[j] );
	    free(ptrs[i]);
	 }
      }
   }

   free(ptrs);
   free(units);
   return(status);
}

/******************************************************************/
/*  To allow the Fortran work function to have access to string   */
/*  arrays via the passed pointers, special routines are required */
/*  for going between the pointers and actual Fortran strings:    */
/*        FTGISTR (Get Iterator STRing)                           */
/*        FTPISTR (Put Iterator STRing)                           */
/*  To retain knowledge of the size of the Fortran string being   */
/*  passed, the routines are written via once-expanded cfortran   */
/*  macros.                                                       */
/******************************************************************/

CFextern VOID_cfF(FTGISTR,ftgistr)
CFARGT14(NCF,DCF,ABSOFT_cf2(VOID),PVOID,INT,PSTRING,CF_0,CF_0,CF_0,CF_0,CF_0,CF_0,CF_0,CF_0,CF_0,CF_0,CF_0))
{
   QCF(PVOID,1)
   QCF(INT,2)
   QCF(PSTRING,3)
   char **p;
   int Fstrlen,elemNum;
   char *str;

   p       = TCF(ftgistr,PVOID,1,0);
   elemNum = TCF(ftgistr,INT,2,0);
   str     = TCF(ftgistr,PSTRING,3,0);

#ifdef vmsFortran
   Fstrlen = A3->dsc$w_length;
#else
#ifdef CRAYFortran
   Fstrlen = _fcdlen(A3);
#else
   Fstrlen = C3;
#endif
#endif

   memcpy(str,p[elemNum-1],Fstrlen);

   RCF(PVOID,1)
   RCF(INT,2)
   RCF(PSTRING,3)  /*  This takes care of adding trailing spaces as needed  */
}

CFextern VOID_cfF(FTPISTR,ftpistr)
CFARGT14(NCF,DCF,ABSOFT_cf2(VOID),PVOID,INT,STRING,CF_0,CF_0,CF_0,CF_0,CF_0,CF_0,CF_0,CF_0,CF_0,CF_0,CF_0))
{
   QCF(PVOID,1)
   QCF(INT,2)
   QCF(STRING,3)
   char **p;
   int Cstrlen, elemNum;
   char *str;

   p       = TCF(ftpistr,PVOID,1,0);
   elemNum = TCF(ftpistr,INT,2,0);
   str     = TCF(ftpistr,STRING,3,0);

   /*  CFITSIO guarantees that there will always be two pointers  */
   /*  in the array (nullval + 1 element), so get string length   */
   /*  by subtracting these two pointers.                         */

   Cstrlen = p[1]-p[0];

   memcpy(p[elemNum-1],str,Cstrlen);

                    /* Put 0 in in case str is longer than Cstrlen */
   p[elemNum-1][Cstrlen-1]='\0';

   RCF(PVOID,1)
   RCF(INT,2)
   RCF(STRING,3)
}
