/*  This file, swapproc.c, contains general utility routines that are      */
/*  used by other FITSIO routines to swap bytes.                           */

/*  The FITSIO software was written by William Pence at the High Energy    */
/*  Astrophysic Science Archive Research Center (HEASARC) at the NASA      */
/*  Goddard Space Flight Center.  Users shall not, without prior written   */
/*  permission of the U.S. Government,  establish a claim to statutory     */
/*  copyright.  The Government and others acting on its behalf, shall have */
/*  a royalty-free, non-exclusive, irrevocable,  worldwide license for     */
/*  Government purposes to publish, distribute, translate, copy, exhibit,  */
/*  and perform such material.                                             */

/*
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
*/
#include "fitsio2.h"
/*--------------------------------------------------------------------------*/
void ffswap2(short *svalues,  /* IO - pointer to shorts to be swapped       */
             long nvals)      /* I  - number of shorts to be swapped        */
/*
  swap the bytes in the input short integers: ( 0 1 -> 1 0 )
*/
{
    register char *cvalues;
    register long ii;

    union u_tag {
        char cvals[2];   /* equivalence an array of 4 bytes with */
        short sval;      /* a short */
    } u;

    cvalues = (char *) svalues;      /* copy the initial pointer value */

    for (ii = 0; ii < nvals;)
    {
        u.sval = svalues[ii++];  /* copy next short to temporary buffer */

        *cvalues++ = u.cvals[1]; /* copy the 2 bytes to output in turn */
        *cvalues++ = u.cvals[0];
    }
    return;
}
/*--------------------------------------------------------------------------*/
void ffswaplong(long *lvalues, /* IO - pointer to longs to be swapped       */
                long nvals)    /* I  - number of 4-byte words to be swapped */
/*
  swap the bytes in the input long integers: ( 0 1 2 3 -> 3 2 1 0 )
  On machines which use 8-byte words to store the long value (e.g. Alpha OSF)
  then this routine also packs the 4-bytes from each long into a contiguous
  array of bytes.
*/
{
    register char *cvalues;
    register long ii;

    union u_tag {
        char cvals[4];   /* equivalence an array of 4 bytes with */
        long lval;      /* a long */
    } u;

    cvalues = (char *) lvalues;    /* copy the initial pointer value */

    for (ii = 0; ii < nvals;)
    {
        u.lval = lvalues[ii++];  /* copy next long to temporary buffer */

        *cvalues++ = u.cvals[3]; /* copy the 4 bytes in turn */
        *cvalues++ = u.cvals[2];
        *cvalues++ = u.cvals[1];
        *cvalues++ = u.cvals[0]; 
    }
    return;
}
/*--------------------------------------------------------------------------*/
void ffunswaplong(long *lvalues, /* IO - pointer to longs to be swapped     */
                  long nvals)    /* I  - number of longs to be swapped      */
/*
  unswap 4 bytes in each long integer:  ( 0 1 2 3 -> 3 2 1 0 )
  On machines which use 8-byte words to store the long value (e.g. Alpha OSF)
  then this routine also unpacks the 4-bytes from the contiguous array of
  bytes into 8-byte words.
*/
{
    register unsigned char *cvalues;
    register long ii;

    union u_tag {
        unsigned char cvals[4];   /* equivalence an array of 4 bytes */
        long lval;      /* with a long */
    } u;

    /* set pointer to last byte of last long to be swapped */
    cvalues = (unsigned char *) lvalues + (nvals * 4) - 1;  

    for (ii = nvals - 1; ii >= 0;)
    {

#if MACHINE == ALPHA_OSF

        /* if the 4-byte long is stored in an 8 byte word, we must */
        /* initialize the word with the sign bit which is in the   */
        /* most sig. byte, i.e. at location *(cvalues-3)           */
        if ( *(cvalues-3) >  127)
            u.lval = -1; 
        else
            u.lval = 0;

#endif

        u.cvals[0] =  *cvalues--;  /* copy the 4 bytes in turn */
        u.cvals[1] =  *cvalues--;
        u.cvals[2] =  *cvalues--;
        u.cvals[3] =  *cvalues--;

        lvalues[ii--] = u.lval; /* copy swapped value back to the array */
    }
    return;
}
/*--------------------------------------------------------------------------*/
void ffswapfloat(float *fvalues, /* IO - pointer to floats to be swapped    */
                 long nvals)     /* I  - number of floats to be swapped     */
/*
  swap the bytes in the input floats: ( 0 1 2 3 -> 3 2 1 0 )
  Currently, this routine is idential to ffswaplong except for type casts.
  Future ports to any machines which store a 4-byte float in an 8-byte word
  may require separate ffswapfloat and ffunswapfloat routines, similar
  to the ffswaplong and ffunswaplong routines.
*/
{
    register char *cvalues;
    register long ii;

    union u_tag {
        char cvals[4];   /* equivalence an array of 4 bytes with */
        float fval;      /* a float */
    } u;

    cvalues = (char *) fvalues;   /* copy the initial pointer value */

    for (ii = 0; ii < nvals;)
    {
        u.fval = fvalues[ii++];  /* copy next float to buffer */

        *cvalues++ = u.cvals[3]; /* copy the 4 bytes in turn */
        *cvalues++ = u.cvals[2];
        *cvalues++ = u.cvals[1];
        *cvalues++ = u.cvals[0]; 
    }
    return;
}
/*--------------------------------------------------------------------------*/
void ffswap8(double *dvalues,  /* IO - pointer to doubles to be swapped     */
             long nvals)       /* I  - number of doubles to be swapped      */
/*
  swap the bytes in the input doubles: ( 01234567  -> 76543210 )
*/
{
    register char *cvalues;
    register long ii;

    union u_tag {
        char cvals[8];   /* equivalence an array of 8 bytes with */
        double dval;      /* a double */
    } u;

    cvalues = (char *) dvalues;      /* copy the pointer value */

    for (ii = 0; ii < nvals;)
    {
        u.dval = dvalues[ii++];  /* copy next double to buffer */

        *cvalues++ = u.cvals[7]; /* copy the 8 bytes in turn */
        *cvalues++ = u.cvals[6];
        *cvalues++ = u.cvals[5];
        *cvalues++ = u.cvals[4];
        *cvalues++ = u.cvals[3];
        *cvalues++ = u.cvals[2];
        *cvalues++ = u.cvals[1];
        *cvalues++ = u.cvals[0];
    }
    return;
}

