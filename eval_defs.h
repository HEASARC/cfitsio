#include <stdio.h>
#include <math.h>
#include <string.h>
#ifdef __sgi
#include <alloca.h>
#endif
#ifdef sparc
#include <malloc.h>
#else
#include <stdlib.h>
#endif
#include "fitsio2.h"

#define MAXDIMS 5
#define MAXSEQ 10

typedef struct {
                  long nelem;
                  int  naxis;
                  long naxes[MAXDIMS];
                  long *iptr;
                  char *undef;
                                } ivec;
typedef struct {
                  long nelem;
                  int  naxis;
                  long naxes[MAXDIMS];
                  double *rptr;
                  char   *undef;
                                } rvec;
typedef struct {
                  long nelem;
                  int  naxis;
                  long naxes[MAXDIMS];
                  char *bptr;
                  char *undef;
                                } bvec;

typedef struct {
                  int  nelem;
                  long val[MAXSEQ];
                                } iseq;

typedef struct {
                  int    nelem;
                  double val[MAXSEQ];
                                } rseq;

typedef struct {
                  int  type;
                  int  nelem;
                  int  naxis;
                  long naxes[MAXDIMS];
                                } DataInfo;

typedef struct {
                  fitsfile    *def_fptr;

                  int         compressed;
                  int         timeCol;
                  int         parCol;
                  int         valCol;

                  char        *expr;
                  int         index;
                  int         is_eobuf;

                  int         byteloc;
                  int         nbytes;
                  short       *bytecodes;

                  int         init_flag;
                  long        firstRow;
                  long        currRow;

                  int         nCols;
                  int         currCol;
                  iteratorCol *colData;
                  DataInfo    *colInfo;

                  char        undef;
                  DataInfo    resultInfo;
                  union {
                       double real;
                       long   integer;
                       char   boolean;
                       bvec   boolvec;
                       ivec   intvec;
                       rvec   realvec;
                       char   string[256];
                        }     resultData;
                  int         datatype;

                  int         status;
                                } ParseData;


extern ParseData gParse;

int  ffparse(void);
int  fflex(void);
int  fflex_real(void);
void ffrestart(FILE*);

