#include "fitsio2.h"
#include "f77_wrap.h"

/* Use a simple ellipse prototype for Fwork_fn to satisfy finicky compilers */
typedef struct {
   void *userData;
   void (*Fwork_fn)(PLONG_cfTYPE *total_n, ...);
} FtnUserData;

/*        Declare protoypes to make C++ happy       */
int Cwork_fn(long, long, long, long, int, iteratorCol *, void *);
void Cffiter( int n_cols, int *units, int *colnum, char *colname[], 
	      int *datatype, int *iotype,
              long offset, long n_per_loop, void *Fwork_fn,
	      void *userData, int *status);

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

   FuserData.Fwork_fn = (void(*)(PLONG_cfTYPE *,...))Fwork_fn;
   FuserData.userData = userData;

   cols = (iteratorCol *)malloc( n_cols*sizeof(iteratorCol) );
   if( cols==NULL ) {
      *status = 1;
      return;
   }
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
   char **sptr;
   void **ptrs;
   int  i,j,k,nstr,status=0;
   long *slen;

#ifdef vmsFortran
   /*  Passing strings under VMS require a special structure  */
   fstringvector *vmsStrs;
#endif

   /*  Allocate memory for all the arrays.  Grab all the int's  */
   /*  at once and divide up among parameters                   */

   ptrs  = (void**)malloc(2*n_cols*sizeof(void*));
   units = (int*)malloc(5*n_cols*sizeof(int));
   colnum   = units + 1 * n_cols;
   datatype = units + 2 * n_cols;
   iotype   = units + 3 * n_cols;
   repeat   = units + 4 * n_cols;

   nstr = 0;
   slen = (long*)(ptrs+n_cols);
#ifdef vmsFortran
   vmsStrs = (fstringvector *)calloc(sizeof(fstringvector),n_cols);
#endif

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
	 ptrs[i] = (void *)malloc( (n_values*repeat[i]+1)*4 );
	 for( j=0;j<=n_values*repeat[i]; j++ )
	    ((int*)ptrs[i])[j] = C2FLOGICAL( ((char*)cols[i].array)[j]);
      } else if ( datatype[i]==TSTRING ) {
	 sptr = (char**)cols[i].array;
	 slen[nstr] = sptr[1] - sptr[0];
	 for(j=0;j<=n_values;j++)
	    for(k=strlen( sptr[j] );k<slen[nstr];k++)
	       sptr[j][k] = ' ';
#ifdef vmsFortran
	 vmsStrs[nstr].dsc$a_pointer         = sptr[0];
	 vmsStrs[nstr].dsc$w_length          = slen[nstr];
	 vmsStrs[nstr].dsc$l_m[0]            = n_values+1;
	 vmsStrs[nstr].dsc$l_arsize          = slen[nstr] * (n_values+1);
	 vmsStrs[nstr].dsc$bounds[0].dsc$l_u = n_values+1;
	 vmsStrs[nstr].dsc$a_a0              = sptr[0] - slen[nstr];
	 ptrs[i] = (void *)(vmsStrs+nstr);
#else
	 ptrs[i] = (void *)sptr[0];
#endif
	 nstr++;
      } else
	 ptrs[i] = (void *)cols[i].array;
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
      f  = (FtnUserData *)FuserData;

      f->Fwork_fn(&a1,&a2,&a3,&a4,&n_cols,units,colnum,datatype,
		  iotype,repeat,&status,f->userData,
		  ptrs[ 0], ptrs[ 1], ptrs[ 2], ptrs[ 3], ptrs[ 4],
		  ptrs[ 5], ptrs[ 6], ptrs[ 7], ptrs[ 8], ptrs[ 9],
		  ptrs[10], ptrs[11], ptrs[12], ptrs[13], ptrs[14],
		  ptrs[15], ptrs[16], ptrs[17], ptrs[18], ptrs[19],
		  ptrs[20], ptrs[21], ptrs[22], ptrs[23], ptrs[24] );
   }

   /*  Check whether there are any LOGICAL or STRING columns being outputted  */
   nstr=0;
   for( i=0;i<n_cols;i++ ) {
      if( iotype[i]!=InputCol ) {
	 if( datatype[i]==TLOGICAL ) {
	    for( j=0;j<=n_values*repeat[i];j++ )
	       ((char*)cols[i].array)[j] = F2CLOGICAL( ((int*)ptrs[i])[j] );
	    free(ptrs[i]);
	 } else if( datatype[i]==TSTRING ) {
	    for( j=0;j<=n_values;j++ )
	       ((char**)cols[i].array)[j][slen[nstr]-1] = '\0';
	 }
      }
      if( datatype[i]==TSTRING ) nstr++;
   }

   free(ptrs);
   free(units);
#ifdef vmsFortran
   free(vmsStrs);
#endif
   return(status);
}
