/*  This file, convert.c, contains routines to do datatype conversion,     */
/*  linear scaling, and null value detection when reading or writing data  */
/*  values in a FITS file.                                                 */

/*  The FITSIO software was written by William Pence at the High Energy    */
/*  Astrophysic Science Archive Research Center (HEASARC) at the NASA      */
/*  Goddard Space Flight Center.  Users shall not, without prior written   */
/*  permission of the U.S. Government,  establish a claim to statutory     */
/*  copyright.  The Government and others acting on its behalf, shall have */
/*  a royalty-free, non-exclusive, irrevocable,  worldwide license for     */
/*  Government purposes to publish, distribute, translate, copy, exhibit,  */
/*  and perform such material.                                             */

#include <stdio.h>
#include <limits.h>
#ifndef _FITSIO2_H
#include "fitsio2.h"
#endif

#if MACHINE == ALPHAVMS
  static float testfloat = TESTFLOAT;  /* use to test floating pt format */
#endif

/*--------------------------------------------------------------------------*/
int ffi1fi1(unsigned char *input,  /* I - array of values to be converted  */
            long ntodo,            /* I - number of elements in the array  */
            double scale,          /* I - FITS TSCALn or BSCALE value      */
            double zero,           /* I - FITS TZEROn or BZERO  value      */
            unsigned char *output, /* O - output array of converted values */
            int *status)           /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required
*/
{
    long ii;
    double dvalue;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
            output[ii] = input[ii];   /* just copy input to output */
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
        {
            dvalue = ( ((double) input[ii]) - zero) / scale;

            if (dvalue < DUCHAR_MIN)
            {
                *status = OVERFLOW_ERR;
                output[ii] = 0;
            }
            else if (dvalue > DUCHAR_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = UCHAR_MAX;
            }
            else
                output[ii] = (unsigned char) dvalue;
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffi2fi1(short *input,          /* I - array of values to be converted  */
            long ntodo,            /* I - number of elements in the array  */
            double scale,          /* I - FITS TSCALn or BSCALE value      */
            double zero,           /* I - FITS TZEROn or BZERO  value      */
            unsigned char *output, /* O - output array of converted values */
            int *status)           /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required
*/
{
    long ii;
    double dvalue;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
        {
            if (input[ii] < 0)
            {
                *status = OVERFLOW_ERR;
                output[ii] = 0;
            }
            else if (input[ii] > UCHAR_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = UCHAR_MAX;
            }
            else
                output[ii] = input[ii];
        }
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
        {
            dvalue = (input[ii] - zero) / scale;

            if (dvalue < DUCHAR_MIN)
            {
                *status = OVERFLOW_ERR;
                output[ii] = 0;
            }
            else if (dvalue > DUCHAR_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = UCHAR_MAX;
            }
            else
                output[ii] = (unsigned char) dvalue;
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffi4fi1(long *input,           /* I - array of values to be converted  */
            long ntodo,            /* I - number of elements in the array  */
            double scale,          /* I - FITS TSCALn or BSCALE value      */
            double zero,           /* I - FITS TZEROn or BZERO  value      */
            unsigned char *output, /* O - output array of converted values */
            int *status)           /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required.
*/
{
    long ii;
    double dvalue;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
        {
            if (input[ii] < 0)
            {
                *status = OVERFLOW_ERR;
                output[ii] = 0;
            }
            else if (input[ii] > UCHAR_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = UCHAR_MAX;
            }
            else
                output[ii] = input[ii];
        }
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
        {
            dvalue = (input[ii] - zero) / scale;

            if (dvalue < DUCHAR_MIN)
            {
                *status = OVERFLOW_ERR;
                output[ii] = 0;
            }
            else if (dvalue > DUCHAR_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = UCHAR_MAX;
            }
            else
                output[ii] = (unsigned char) dvalue;
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffr4fi1(float *input,          /* I - array of values to be converted  */
            long ntodo,            /* I - number of elements in the array  */
            double scale,          /* I - FITS TSCALn or BSCALE value      */
            double zero,           /* I - FITS TZEROn or BZERO  value      */
            unsigned char *output, /* O - output array of converted values */
            int *status)           /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required.
*/
{
    long ii;
    double dvalue;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
        {
            if (input[ii] < DUCHAR_MIN)
            {
                *status = OVERFLOW_ERR;
                output[ii] = 0;
            }
            else if (input[ii] > DUCHAR_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = UCHAR_MAX;
            }
            else
                output[ii] = (unsigned char) input[ii];
        }
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
        {
            dvalue = (input[ii] - zero) / scale;

            if (dvalue < DUCHAR_MIN)
            {
                *status = OVERFLOW_ERR;
                output[ii] = 0;
            }
            else if (dvalue > DUCHAR_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = UCHAR_MAX;
            }
            else
                output[ii] = (unsigned char) dvalue;
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffr8fi1(double *input,         /* I - array of values to be converted  */
            long ntodo,            /* I - number of elements in the array  */
            double scale,          /* I - FITS TSCALn or BSCALE value      */
            double zero,           /* I - FITS TZEROn or BZERO  value      */
            unsigned char *output, /* O - output array of converted values */
            int *status)           /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required.
*/
{
    long ii;
    double dvalue;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
        {
            if (input[ii] < DUCHAR_MIN)
            {
                *status = OVERFLOW_ERR;
                output[ii] = 0;
            }
            else if (input[ii] > DUCHAR_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = UCHAR_MAX;
            }
            else
                output[ii] = (unsigned char) input[ii];
        }
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
        {
            dvalue = (input[ii] - zero) / scale;

            if (dvalue < DUCHAR_MIN)
            {
                *status = OVERFLOW_ERR;
                output[ii] = 0;
            }
            else if (dvalue > DUCHAR_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = UCHAR_MAX;
            }
            else
                output[ii] = (unsigned char) dvalue;
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffi1fi2(unsigned char *input,  /* I - array of values to be converted  */
            long ntodo,            /* I - number of elements in the array  */
            double scale,          /* I - FITS TSCALn or BSCALE value      */
            double zero,           /* I - FITS TZEROn or BZERO  value      */
            short *output,         /* O - output array of converted values */
            int *status)           /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required
*/
{
    long ii;
    double dvalue;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
            output[ii] = input[ii];   /* just copy input to output */
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
        {
            dvalue = (((double) input[ii]) - zero) / scale;

            if (dvalue < DSHRT_MIN)
            {
                *status = OVERFLOW_ERR;
                output[ii] = SHRT_MIN;
            }
            else if (dvalue > DSHRT_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = SHRT_MAX;
            }
            else
                output[ii] = (short) dvalue;
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffi2fi2(short *input,       /* I - array of values to be converted  */
            long ntodo,         /* I - number of elements in the array  */
            double scale,       /* I - FITS TSCALn or BSCALE value      */
            double zero,        /* I - FITS TZEROn or BZERO  value      */
            short *output,      /* O - output array of converted values */
            int *status)        /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required
*/
{
    long ii;
    double dvalue;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
            output[ii] = input[ii];   /* just copy input to output */
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
        {
            dvalue = (input[ii] - zero) / scale;

            if (dvalue < DSHRT_MIN)
            {
                *status = OVERFLOW_ERR;
                output[ii] = SHRT_MIN;
            }
            else if (dvalue > DSHRT_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = SHRT_MAX;
            }
            else
                output[ii] = (short) dvalue;
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffi4fi2(long *input,       /* I - array of values to be converted  */
            long ntodo,        /* I - number of elements in the array  */
            double scale,      /* I - FITS TSCALn or BSCALE value      */
            double zero,       /* I - FITS TZEROn or BZERO  value      */
            short *output,     /* O - output array of converted values */
            int *status)       /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required.
*/
{
    long ii;
    double dvalue;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
        {
            if (input[ii] < SHRT_MIN)
            {
                *status = OVERFLOW_ERR;
                output[ii] = SHRT_MIN;
            }
            else if (input[ii] > SHRT_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = SHRT_MAX;
            }
            else
                output[ii] = input[ii];
        }
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
        {
            dvalue = (input[ii] - zero) / scale;

            if (dvalue < DSHRT_MIN)
            {
                *status = OVERFLOW_ERR;
                output[ii] = SHRT_MIN;
            }
            else if (dvalue > DSHRT_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = SHRT_MAX;
            }
            else
                output[ii] = (short) dvalue;
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffr4fi2(float *input,      /* I - array of values to be converted  */
            long ntodo,        /* I - number of elements in the array  */
            double scale,      /* I - FITS TSCALn or BSCALE value      */
            double zero,       /* I - FITS TZEROn or BZERO  value      */
            short *output,     /* O - output array of converted values */
            int *status)       /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required.
*/
{
    long ii;
    double dvalue;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
        {
            if (input[ii] < DSHRT_MIN)
            {
                *status = OVERFLOW_ERR;
                output[ii] = SHRT_MIN;
            }
            else if (input[ii] > DSHRT_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = SHRT_MAX;
            }
            else
                output[ii] = (short) input[ii];
        }
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
        {
            dvalue = (input[ii] - zero) / scale;

            if (dvalue < DSHRT_MIN)
            {
                *status = OVERFLOW_ERR;
                output[ii] = SHRT_MIN;
            }
            else if (dvalue > DSHRT_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = SHRT_MAX;
            }
            else
                output[ii] = (short) dvalue;
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffr8fi2(double *input,     /* I - array of values to be converted  */
            long ntodo,        /* I - number of elements in the array  */
            double scale,      /* I - FITS TSCALn or BSCALE value      */
            double zero,       /* I - FITS TZEROn or BZERO  value      */
            short *output,     /* O - output array of converted values */
            int *status)       /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required.
*/
{
    long ii;
    double dvalue;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
        {
            if (input[ii] < DSHRT_MIN)
            {
                *status = OVERFLOW_ERR;
                output[ii] = SHRT_MIN;
            }
            else if (input[ii] > DSHRT_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = SHRT_MAX;
            }
            else
                output[ii] = (short) input[ii];
        }
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
        {
            dvalue = (input[ii] - zero) / scale;

            if (dvalue < DSHRT_MIN)
            {
                *status = OVERFLOW_ERR;
                output[ii] = SHRT_MIN;
            }
            else if (dvalue > DSHRT_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = SHRT_MAX;
            }
            else
                output[ii] = (short) dvalue;
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffi1fi4(unsigned char *input,  /* I - array of values to be converted  */
            long ntodo,            /* I - number of elements in the array  */
            double scale,          /* I - FITS TSCALn or BSCALE value      */
            double zero,           /* I - FITS TZEROn or BZERO  value      */
            long *output,          /* O - output array of converted values */
            int *status)           /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required
*/
{
    long ii;
    double dvalue;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
            output[ii] = (long) input[ii];   /* just copy input to output */
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
        {
            dvalue = (((double) input[ii]) - zero) / scale;

            if (dvalue < DLONG_MIN)
            {
                *status = OVERFLOW_ERR;
                output[ii] = LONG_MIN;
            }
            else if (dvalue > DLONG_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = LONG_MAX;
            }
            else
                output[ii] = (long) dvalue;
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffi2fi4(short *input,      /* I - array of values to be converted  */
            long ntodo,        /* I - number of elements in the array  */
            double scale,      /* I - FITS TSCALn or BSCALE value      */
            double zero,       /* I - FITS TZEROn or BZERO  value      */
            long *output,      /* O - output array of converted values */
            int *status)       /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required
*/
{
    long ii;
    double dvalue;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
            output[ii] = (long) input[ii];   /* just copy input to output */
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
        {
            dvalue = (input[ii] - zero) / scale;

            if (dvalue < DLONG_MIN)
            {
                *status = OVERFLOW_ERR;
                output[ii] = LONG_MIN;
            }
            else if (dvalue > DLONG_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = LONG_MAX;
            }
            else
                output[ii] = (long) dvalue;
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffi4fi4(long *input,       /* I - array of values to be converted  */
            long ntodo,        /* I - number of elements in the array  */
            double scale,      /* I - FITS TSCALn or BSCALE value      */
            double zero,       /* I - FITS TZEROn or BZERO  value      */
            long *output,      /* O - output array of converted values */
            int *status)       /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required
*/
{
    long ii;
    double dvalue;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
            output[ii] = (long) input[ii];   /* just copy input to output */
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
        {
            dvalue = (input[ii] - zero) / scale;

            if (dvalue < DLONG_MIN)
            {
                *status = OVERFLOW_ERR;
                output[ii] = LONG_MIN;
            }
            else if (dvalue > DLONG_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = LONG_MAX;
            }
            else
                output[ii] = (long) dvalue;
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffr4fi4(float *input,      /* I - array of values to be converted  */
            long ntodo,        /* I - number of elements in the array  */
            double scale,      /* I - FITS TSCALn or BSCALE value      */
            double zero,       /* I - FITS TZEROn or BZERO  value      */
            long *output,      /* O - output array of converted values */
            int *status)       /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required.
*/
{
    long ii;
    double dvalue;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
        {
            if (input[ii] < DLONG_MIN)
            {
                *status = OVERFLOW_ERR;
                output[ii] = LONG_MIN;
            }
            else if (input[ii] > DLONG_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = LONG_MAX;
            }
            else
                output[ii] = (long) input[ii];
        }
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
        {
            dvalue = (input[ii] - zero) / scale;

            if (dvalue < DLONG_MIN)
            {
                *status = OVERFLOW_ERR;
                output[ii] = LONG_MIN;
            }
            else if (dvalue > DLONG_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = LONG_MAX;
            }
            else
                output[ii] = (long) dvalue;
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffr8fi4(double *input,     /* I - array of values to be converted  */
            long ntodo,        /* I - number of elements in the array  */
            double scale,      /* I - FITS TSCALn or BSCALE value      */
            double zero,       /* I - FITS TZEROn or BZERO  value      */
            long *output,      /* O - output array of converted values */
            int *status)       /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required.
*/
{
    long ii;
    double dvalue;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
        {
            if (input[ii] < DLONG_MIN)
            {
                *status = OVERFLOW_ERR;
                output[ii] = LONG_MIN;
            }
            else if (input[ii] > DLONG_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = LONG_MAX;
            }
            else
                output[ii] = (long) input[ii];
        }
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
        {
            dvalue = (input[ii] - zero) / scale;

            if (dvalue < DLONG_MIN)
            {
                *status = OVERFLOW_ERR;
                output[ii] = LONG_MIN;
            }
            else if (dvalue > DLONG_MAX)
            {
                *status = OVERFLOW_ERR;
                output[ii] = LONG_MAX;
            }
            else
                output[ii] = (long) dvalue;
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffi1fr4(unsigned char *input,  /* I - array of values to be converted  */
            long ntodo,            /* I - number of elements in the array  */
            double scale,          /* I - FITS TSCALn or BSCALE value      */
            double zero,           /* I - FITS TZEROn or BZERO  value      */
            float *output,         /* O - output array of converted values */
            int *status)           /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required.
*/
{
    long ii;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
                output[ii] = (float) input[ii];
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
            output[ii] = ( ( (double) input[ii] ) - zero) / scale;
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffi2fr4(short *input,      /* I - array of values to be converted  */
            long ntodo,        /* I - number of elements in the array  */
            double scale,      /* I - FITS TSCALn or BSCALE value      */
            double zero,       /* I - FITS TZEROn or BZERO  value      */
            float *output,     /* O - output array of converted values */
            int *status)       /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required.
*/
{
    long ii;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
                output[ii] = (float) input[ii];
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
            output[ii] = (input[ii] - zero) / scale;
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffi4fr4(long *input,       /* I - array of values to be converted  */
            long ntodo,        /* I - number of elements in the array  */
            double scale,      /* I - FITS TSCALn or BSCALE value      */
            double zero,       /* I - FITS TZEROn or BZERO  value      */
            float *output,     /* O - output array of converted values */
            int *status)       /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required.
*/
{
    long ii;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
                output[ii] = (float) input[ii];
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
            output[ii] = (input[ii] - zero) / scale;
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffr4fr4(float *input,      /* I - array of values to be converted  */
            long ntodo,        /* I - number of elements in the array  */
            double scale,      /* I - FITS TSCALn or BSCALE value      */
            double zero,       /* I - FITS TZEROn or BZERO  value      */
            float *output,     /* O - output array of converted values */
            int *status)       /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required.
*/
{
    long ii;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
                output[ii] = input[ii];
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
            output[ii] = (input[ii] - zero) / scale;
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffr8fr4(double *input,     /* I - array of values to be converted  */
            long ntodo,        /* I - number of elements in the array  */
            double scale,      /* I - FITS TSCALn or BSCALE value      */
            double zero,       /* I - FITS TZEROn or BZERO  value      */
            float *output,     /* O - output array of converted values */
            int *status)       /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required.
*/
{
    long ii;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
                output[ii] = (float) input[ii];
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
            output[ii] = (input[ii] - zero) / scale;
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffi1fr8(unsigned char *input,  /* I - array of values to be converted  */
            long ntodo,            /* I - number of elements in the array  */
            double scale,          /* I - FITS TSCALn or BSCALE value      */
            double zero,           /* I - FITS TZEROn or BZERO  value      */
            double *output,        /* O - output array of converted values */
            int *status)           /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required.
*/
{
    long ii;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
                output[ii] = (double) input[ii];
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
            output[ii] = ( ( (double) input[ii] ) - zero) / scale;
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffi2fr8(short *input,      /* I - array of values to be converted  */
            long ntodo,        /* I - number of elements in the array  */
            double scale,      /* I - FITS TSCALn or BSCALE value      */
            double zero,       /* I - FITS TZEROn or BZERO  value      */
            double *output,    /* O - output array of converted values */
            int *status)       /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required.
*/
{
    long ii;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
                output[ii] = (double) input[ii];
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
            output[ii] = (input[ii] - zero) / scale;
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffi4fr8(long *input,       /* I - array of values to be converted  */
            long ntodo,        /* I - number of elements in the array  */
            double scale,      /* I - FITS TSCALn or BSCALE value      */
            double zero,       /* I - FITS TZEROn or BZERO  value      */
            double *output,    /* O - output array of converted values */
            int *status)       /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required.
*/
{
    long ii;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
                output[ii] = (double) input[ii];
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
            output[ii] = (input[ii] - zero) / scale;
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffr4fr8(float *input,      /* I - array of values to be converted  */
            long ntodo,        /* I - number of elements in the array  */
            double scale,      /* I - FITS TSCALn or BSCALE value      */
            double zero,       /* I - FITS TZEROn or BZERO  value      */
            double *output,    /* O - output array of converted values */
            int *status)       /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required.
*/
{
    long ii;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
                output[ii] = (double) input[ii];
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
            output[ii] = (input[ii] - zero) / scale;
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffr8fr8(double *input,     /* I - array of values to be converted  */
            long ntodo,        /* I - number of elements in the array  */
            double scale,      /* I - FITS TSCALn or BSCALE value      */
            double zero,       /* I - FITS TZEROn or BZERO  value      */
            double *output,    /* O - output array of converted values */
            int *status)       /* IO - error status                    */
/*
  Copy input to output prior to writing output to a FITS file.
  Do datatype conversion and scaling if required.
*/
{
    long ii;

    if (scale == 1. && zero == 0.)
    {       
        for (ii = 0; ii < ntodo; ii++)
                output[ii] = input[ii];
    }
    else
    {
        for (ii = 0; ii < ntodo; ii++)
            output[ii] = (input[ii] - zero) / scale;
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffi1i1(unsigned char *input, /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            unsigned char tnull,  /* I - value of FITS TNULLn keyword if any */
            unsigned char nullval,/* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            unsigned char *output,/* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to tnull.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;
    double dvalue;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
                output[ii] = input[ii];  
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                dvalue = input[ii] * scale + zero;

                if (dvalue < DUCHAR_MIN)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = 0;
                }
                else if (dvalue > DUCHAR_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = UCHAR_MAX;
                }
                else
                    output[ii] = (unsigned char) dvalue;
            }
        }
    }
    else        /* must check for null values */
    {
        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                    output[ii] = input[ii];
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                {
                    dvalue = input[ii] * scale + zero;

                    if (dvalue < DUCHAR_MIN)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = 0;
                    }
                    else if (dvalue > DUCHAR_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = UCHAR_MAX;
                    }
                    else
                        output[ii] = (unsigned char) dvalue;
                }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffi2i1(short *input,         /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            short tnull,          /* I - value of FITS TNULLn keyword if any */
            unsigned char nullval,/* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            unsigned char *output,/* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to tnull.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;
    double dvalue;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] < 0)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = 0;
                }
                else if (input[ii] > UCHAR_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = UCHAR_MAX;
                }
                else
                    output[ii] = (unsigned char) input[ii];
            }
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                dvalue = input[ii] * scale + zero;

                if (dvalue < DUCHAR_MIN)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = 0;
                }
                else if (dvalue > DUCHAR_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = UCHAR_MAX;
                }
                else
                    output[ii] = (unsigned char) dvalue;
            }
        }
    }
    else        /* must check for null values */
    {
        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }

                else
                {
                    if (input[ii] < 0)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = 0;
                    }
                    else if (input[ii] > UCHAR_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = UCHAR_MAX;
                    }
                    else
                        output[ii] = (unsigned char) input[ii];
                }
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                {
                    dvalue = input[ii] * scale + zero;

                    if (dvalue < DUCHAR_MIN)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = 0;
                    }
                    else if (dvalue > DUCHAR_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = UCHAR_MAX;
                    }
                    else
                        output[ii] = (unsigned char) dvalue;
                }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffi4i1(long *input,          /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            long tnull,           /* I - value of FITS TNULLn keyword if any */
            unsigned char nullval,/* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            unsigned char *output,/* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to tnull.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;
    double dvalue;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] < 0)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = 0;
                }
                else if (input[ii] > UCHAR_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = UCHAR_MAX;
                }
                else
                    output[ii] = (unsigned char) input[ii];
            }
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                dvalue = input[ii] * scale + zero;

                if (dvalue < DUCHAR_MIN)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = 0;
                }
                else if (dvalue > DUCHAR_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = UCHAR_MAX;
                }
                else
                    output[ii] = (unsigned char) dvalue;
            }
        }
    }
    else        /* must check for null values */
    {
        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                {
                    if (input[ii] < 0)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = 0;
                    }
                    else if (input[ii] > UCHAR_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = UCHAR_MAX;
                    }
                    else
                        output[ii] = (unsigned char) input[ii];
                }
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                {
                    dvalue = input[ii] * scale + zero;

                    if (dvalue < DUCHAR_MIN)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = 0;
                    }
                    else if (dvalue > DUCHAR_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = UCHAR_MAX;
                    }
                    else
                        output[ii] = (unsigned char) dvalue;
                }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffr4i1(float *input,         /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            unsigned char nullval,/* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            unsigned char *output,/* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to NaN.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;
    double dvalue;
    short *sptr, iret;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] < 0)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = 0;
                }
                else if (input[ii] > UCHAR_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = UCHAR_MAX;
                }
                else
                    output[ii] = (unsigned char) input[ii];
            }
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                dvalue = input[ii] * scale + zero;

                if (dvalue < DUCHAR_MIN)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = 0;
                }
                else if (dvalue > DUCHAR_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = UCHAR_MAX;
                }
                else
                    output[ii] = (unsigned char) dvalue;
            }
        }
    }
    else        /* must check for null values */
    {
        sptr = (short *) input;
#if MACHINE == VAXVMS
   /* do nothing */

#elif MACHINE == ALPHAVMS
        if (*(short *) &testfloat == IEEEFLOAT)
            sptr++;       /* point to MSBs */

#elif BYTESWAPPED == TRUE 
        sptr++;       /* point to MSBs */
#endif

        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++, sptr += 2)
            {
              /* use redundant boolean logic in following statement */
              /* to suppress irritating Borland compiler warning message */
              if (0 != (iret = fnan(*sptr) ) )  /* test for NaN or underflow */
              {
                  if (iret == 1)  /* is it a NaN? */
                  {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                  }
                  else            /* it's an underflow */
                     output[ii] = 0;
              }
              else
                {
                    if (input[ii] < 0)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = 0;
                    }
                    else if (input[ii] > UCHAR_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = UCHAR_MAX;
                    }
                    else
                        output[ii] = (unsigned char) input[ii];
                }
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++, sptr += 2)
            {
              if (0 != (iret = fnan(*sptr) ) )  /* test for NaN or underflow */
              {
                  if (iret == 1)  /* is it a NaN? */
                  {  
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                  }
                  else            /* it's an underflow */
                     output[ii] = (unsigned char) zero;
              }
              else
                {
                    dvalue = input[ii] * scale + zero;

                    if (dvalue < DUCHAR_MIN)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = 0;
                    }
                    else if (dvalue > DUCHAR_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = UCHAR_MAX;
                    }
                    else
                        output[ii] = (unsigned char) dvalue;
                }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffr8i1(double *input,        /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            unsigned char nullval,/* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            unsigned char *output,/* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to NaN.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;
    double dvalue;
    short *sptr, iret;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] < 0)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = 0;
                }
                else if (input[ii] > UCHAR_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = UCHAR_MAX;
                }
                else
                    output[ii] = (unsigned char) input[ii];
            }
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                dvalue = input[ii] * scale + zero;

                if (dvalue < DUCHAR_MIN)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = 0;
                }
                else if (dvalue > DUCHAR_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = UCHAR_MAX;
                }
                else
                    output[ii] = (unsigned char) dvalue;
            }
        }
    }
    else        /* must check for null values */
    {
        sptr = (short *) input;

#if MACHINE == VAXVMS
   /* do nothing */

#elif MACHINE == ALPHAVMS
        if (*(short *) &testfloat == IEEEFLOAT)
            sptr += 3;       /* point to MSBs */

#elif BYTESWAPPED == TRUE
        sptr += 3;       /* point to MSBs */
#endif
        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++, sptr += 4)
            {
              if (0 != (iret = dnan(*sptr)) )  /* test for NaN or underflow */
              {
                  if (iret == 1)  /* is it a NaN? */
                  {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                  }
                  else            /* it's an underflow */
                     output[ii] = 0;
              }
              else
                {
                    if (input[ii] < 0)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = 0;
                    }
                    else if (input[ii] > UCHAR_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = UCHAR_MAX;
                    }
                    else
                        output[ii] = (unsigned char) input[ii];
                }
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++, sptr += 4)
            {
              if (0 != (iret = dnan(*sptr)) )  /* test for NaN or underflow */
              {
                  if (iret == 1)  /* is it a NaN? */
                  {  
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                  }
                  else            /* it's an underflow */
                     output[ii] = (unsigned char) zero;
              }
              else
                {
                    dvalue = input[ii] * scale + zero;

                    if (dvalue < DUCHAR_MIN)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = 0;
                    }
                    else if (dvalue > DUCHAR_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = UCHAR_MAX;
                    }
                    else
                        output[ii] = (unsigned char) dvalue;
                }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffi1i2(unsigned char *input, /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            unsigned char tnull,  /* I - value of FITS TNULLn keyword if any */
            short nullval,        /* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            short *output,        /* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to tnull.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;
    double dvalue;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
                output[ii] = (short) input[ii];  /* copy input to output */
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                dvalue = input[ii] * scale + zero;

                if (dvalue < DSHRT_MIN)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = SHRT_MIN;
                }
                else if (dvalue > DSHRT_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = SHRT_MAX;
                }
                else
                    output[ii] = (short) dvalue;
            }
        }
    }
    else        /* must check for null values */
    {
        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                    output[ii] = (short) input[ii];
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                {
                    dvalue = input[ii] * scale + zero;

                    if (dvalue < DSHRT_MIN)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = SHRT_MIN;
                    }
                    else if (dvalue > DSHRT_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = SHRT_MAX;
                    }
                    else
                        output[ii] = (short) dvalue;
                }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffi2i2(short *input,         /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            short tnull,          /* I - value of FITS TNULLn keyword if any */
            short nullval,        /* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            short *output,        /* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to tnull.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;
    double dvalue;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
                output[ii] = input[ii];  /* copy input to output */
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                dvalue = input[ii] * scale + zero;

                if (dvalue < DSHRT_MIN)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = SHRT_MIN;
                }
                else if (dvalue > DSHRT_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = SHRT_MAX;
                }
                else
                    output[ii] = (short) dvalue;
            }
        }
    }
    else        /* must check for null values */
    {
        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                    output[ii] = input[ii];
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                {
                    dvalue = input[ii] * scale + zero;

                    if (dvalue < DSHRT_MIN)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = SHRT_MIN;
                    }
                    else if (dvalue > DSHRT_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = SHRT_MAX;
                    }
                    else
                        output[ii] = (short) dvalue;
                }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffi4i2(long *input,          /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            long tnull,           /* I - value of FITS TNULLn keyword if any */
            short nullval,        /* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            short *output,        /* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to tnull.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;
    double dvalue;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] < SHRT_MIN)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = SHRT_MIN;
                }
                else if (input[ii] > SHRT_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = SHRT_MAX;
                }
                else
                    output[ii] = (short) input[ii];
            }
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                dvalue = input[ii] * scale + zero;

                if (dvalue < DSHRT_MIN)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = SHRT_MIN;
                }
                else if (dvalue > DSHRT_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = SHRT_MAX;
                }
                else
                    output[ii] = (short) dvalue;
            }
        }
    }
    else        /* must check for null values */
    {
        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                {
                    if (input[ii] < SHRT_MIN)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = SHRT_MIN;
                    }
                    else if (input[ii] > SHRT_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = SHRT_MAX;
                    }
                    else
                        output[ii] = (short) input[ii];
                }
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                {
                    dvalue = input[ii] * scale + zero;

                    if (dvalue < DSHRT_MIN)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = SHRT_MIN;
                    }
                    else if (dvalue > DSHRT_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = SHRT_MAX;
                    }
                    else
                        output[ii] = (short) dvalue;
                }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffr4i2(float *input,         /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            short nullval,        /* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            short *output,        /* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to NaN.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;
    double dvalue;
    short *sptr, iret;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] < DSHRT_MIN)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = SHRT_MIN;
                }
                else if (input[ii] > DSHRT_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = SHRT_MAX;
                }
                else
                    output[ii] = (short) input[ii];
            }
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                dvalue = input[ii] * scale + zero;

                if (dvalue < DSHRT_MIN)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = SHRT_MIN;
                }
                else if (dvalue > DSHRT_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = SHRT_MAX;
                }
                else
                    output[ii] = (short) dvalue;
            }
        }
    }
    else        /* must check for null values */
    {
        sptr = (short *) input;

#if MACHINE == VAXVMS
   /* do nothing */

#elif MACHINE == ALPHAVMS
        if (*(short *) &testfloat == IEEEFLOAT)
            sptr++;       /* point to MSBs */

#elif BYTESWAPPED == TRUE
        sptr++;       /* point to MSBs */
#endif

        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++, sptr += 2)
            {
              if (0 != (iret = fnan(*sptr) ) )  /* test for NaN or underflow */
              {
                  if (iret == 1)  /* is it a NaN? */
                  {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                  }
                  else            /* it's an underflow */
                     output[ii] = 0;
              }
              else
                {
                    if (input[ii] < DSHRT_MIN)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = SHRT_MIN;
                    }
                    else if (input[ii] > DSHRT_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = SHRT_MAX;
                    }
                    else
                        output[ii] = (short) input[ii];
                }
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++, sptr += 2)
            {
              if (0 != (iret = fnan(*sptr) ) )  /* test for NaN or underflow */
              {
                  if (iret == 1)  /* is it a NaN? */
                  {  
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                  }
                  else            /* it's an underflow */
                     output[ii] = (short) zero;
              }
              else
                {
                    dvalue = input[ii] * scale + zero;

                    if (dvalue < DSHRT_MIN)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = SHRT_MIN;
                    }
                    else if (dvalue > DSHRT_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = SHRT_MAX;
                    }
                    else
                        output[ii] = (short) dvalue;
                }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffr8i2(double *input,        /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            short nullval,        /* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            short *output,        /* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to NaN.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;
    double dvalue;
    short *sptr, iret;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] < DSHRT_MIN)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = SHRT_MIN;
                }
                else if (input[ii] > DSHRT_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = SHRT_MAX;
                }
                else
                    output[ii] = (short) input[ii];
            }
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                dvalue = input[ii] * scale + zero;

                if (dvalue < DSHRT_MIN)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = SHRT_MIN;
                }
                else if (dvalue > DSHRT_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = SHRT_MAX;
                }
                else
                    output[ii] = (short) dvalue;
            }
        }
    }
    else        /* must check for null values */
    {
        sptr = (short *) input;

#if MACHINE == VAXVMS
   /* do nothing */

#elif MACHINE == ALPHAVMS
        if (*(short *) &testfloat == IEEEFLOAT)
            sptr += 3;       /* point to MSBs */

#elif BYTESWAPPED == TRUE
        sptr += 3;       /* point to MSBs */
#endif
        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++, sptr += 4)
            {
              if (0 != (iret = dnan(*sptr)) )  /* test for NaN or underflow */
              {
                  if (iret == 1)  /* is it a NaN? */
                  {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                  }
                  else            /* it's an underflow */
                     output[ii] = 0;
              }
              else
                {
                    if (input[ii] < DSHRT_MIN)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = SHRT_MIN;
                    }
                    else if (input[ii] > DSHRT_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = SHRT_MAX;
                    }
                    else
                        output[ii] = (short) input[ii];
                }
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++, sptr += 4)
            {
              if (0 != (iret = dnan(*sptr)) )  /* test for NaN or underflow */
              {
                  if (iret == 1)  /* is it a NaN? */
                  {  
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                  }
                  else            /* it's an underflow */
                     output[ii] = (short) zero;
              }
              else
                {
                    dvalue = input[ii] * scale + zero;

                    if (dvalue < DSHRT_MIN)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = SHRT_MIN;
                    }
                    else if (dvalue > DSHRT_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = SHRT_MAX;
                    }
                    else
                        output[ii] = (short) dvalue;
                }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffi1i4(unsigned char *input, /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            unsigned char tnull,  /* I - value of FITS TNULLn keyword if any */
            long nullval,         /* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            long *output,         /* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to tnull.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;
    double dvalue;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
                output[ii] = (long) input[ii];  /* copy input to output */
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                dvalue = input[ii] * scale + zero;

                if (dvalue < DLONG_MIN)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = LONG_MIN;
                }
                else if (dvalue > DLONG_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = LONG_MAX;
                }
                else
                    output[ii] = (long) dvalue;
            }
        }
    }
    else        /* must check for null values */
    {
        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                    output[ii] = (long) input[ii];
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                {
                    dvalue = input[ii] * scale + zero;

                    if (dvalue < DLONG_MIN)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = LONG_MIN;
                    }
                    else if (dvalue > DLONG_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = LONG_MAX;
                    }
                    else
                        output[ii] = (long) dvalue;
                }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffi2i4(short *input,         /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            short tnull,          /* I - value of FITS TNULLn keyword if any */
            long nullval,         /* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            long *output,         /* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to tnull.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;
    double dvalue;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
                output[ii] = (long) input[ii];   /* copy input to output */
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                dvalue = input[ii] * scale + zero;

                if (dvalue < DLONG_MIN)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = LONG_MIN;
                }
                else if (dvalue > DLONG_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = LONG_MAX;
                }
                else
                    output[ii] = (long) dvalue;
            }
        }
    }
    else        /* must check for null values */
    {
        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                    output[ii] = (long) input[ii];
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                {
                    dvalue = input[ii] * scale + zero;

                    if (dvalue < DLONG_MIN)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = LONG_MIN;
                    }
                    else if (dvalue > DLONG_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = LONG_MAX;
                    }
                    else
                        output[ii] = (long) dvalue;
                }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffi4i4(long *input,          /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            long tnull,           /* I - value of FITS TNULLn keyword if any */
            long nullval,         /* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            long *output,         /* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to tnull.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;
    double dvalue;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
                output[ii] = input[ii];   /* copy input to output */
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                dvalue = input[ii] * scale + zero;

                if (dvalue < DLONG_MIN)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = LONG_MIN;
                }
                else if (dvalue > DLONG_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = LONG_MAX;
                }
                else
                    output[ii] = (long) dvalue;
            }
        }
    }
    else        /* must check for null values */
    {
        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                    output[ii] = input[ii];

            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                {
                    dvalue = input[ii] * scale + zero;

                    if (dvalue < DLONG_MIN)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = LONG_MIN;
                    }
                    else if (dvalue > DLONG_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = LONG_MAX;
                    }
                    else
                        output[ii] = (long) dvalue;
                }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffr4i4(float *input,         /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            long nullval,         /* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            long *output,         /* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to NaN.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;
    double dvalue;
    short *sptr, iret;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] < DLONG_MIN)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = LONG_MIN;
                }
                else if (input[ii] > DLONG_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = LONG_MAX;
                }
                else
                    output[ii] = (long) input[ii];
            }
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                dvalue = input[ii] * scale + zero;

                if (dvalue < DLONG_MIN)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = LONG_MIN;
                }
                else if (dvalue > DLONG_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = LONG_MAX;
                }
                else
                    output[ii] = (long) dvalue;
            }
        }
    }
    else        /* must check for null values */
    {
        sptr = (short *) input;

#if MACHINE == VAXVMS
   /* do nothing */

#elif MACHINE == ALPHAVMS
        if (*(short *) &testfloat == IEEEFLOAT)
            sptr++;       /* point to MSBs */

#elif BYTESWAPPED == TRUE
        sptr++;       /* point to MSBs */
#endif

        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++, sptr += 2)
            {
              if (0 != (iret = fnan(*sptr) ) )  /* test for NaN or underflow */
              {
                  if (iret == 1)  /* is it a NaN? */
                  {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                  }
                  else            /* it's an underflow */
                     output[ii] = 0;
              }
              else
                {
                    if (input[ii] < DLONG_MIN)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = LONG_MIN;
                    }
                    else if (input[ii] > DLONG_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = LONG_MAX;
                    }
                    else
                        output[ii] = (long) input[ii];
                }
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++, sptr += 2)
            {
              if (0 != (iret = fnan(*sptr) ) )  /* test for NaN or underflow */
              {
                  if (iret == 1)  /* is it a NaN? */
                  {  
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                  }
                  else            /* it's an underflow */
                     output[ii] = (long) zero;
              }
              else
                {
                    dvalue = input[ii] * scale + zero;

                    if (dvalue < DLONG_MIN)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = LONG_MIN;
                    }
                    else if (dvalue > DLONG_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = LONG_MAX;
                    }
                    else
                        output[ii] = (long) dvalue;
                }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffr8i4(double *input,        /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            long nullval,         /* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            long *output,         /* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to NaN.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;
    double dvalue;
    short *sptr, iret;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] < DLONG_MIN)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = LONG_MIN;
                }
                else if (input[ii] > DLONG_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = LONG_MAX;
                }
                else
                    output[ii] = (long) input[ii];
            }
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                dvalue = input[ii] * scale + zero;

                if (dvalue < DLONG_MIN)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = LONG_MIN;
                }
                else if (dvalue > DLONG_MAX)
                {
                    *status = OVERFLOW_ERR;
                    output[ii] = LONG_MAX;
                }
                else
                    output[ii] = (long) dvalue;
            }
        }
    }
    else        /* must check for null values */
    {
        sptr = (short *) input;

#if MACHINE == VAXVMS
   /* do nothing */

#elif MACHINE == ALPHAVMS
        if (*(short *) &testfloat == IEEEFLOAT)
            sptr += 3;       /* point to MSBs */

#elif BYTESWAPPED == TRUE
        sptr += 3;       /* point to MSBs */
#endif
        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++, sptr += 4)
            {
              if (0 != (iret = dnan(*sptr)) )  /* test for NaN or underflow */
              {
                  if (iret == 1)  /* is it a NaN? */
                  {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                  }
                  else            /* it's an underflow */
                     output[ii] = 0;
              }
              else
                {
                    if (input[ii] < DLONG_MIN)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = LONG_MIN;
                    }
                    else if (input[ii] > DLONG_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = LONG_MAX;
                    }
                    else
                        output[ii] = (long) input[ii];
                }
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++, sptr += 4)
            {
              if (0 != (iret = dnan(*sptr)) )  /* test for NaN or underflow */
              {
                  if (iret == 1)  /* is it a NaN? */
                  {  
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                  }
                  else            /* it's an underflow */
                     output[ii] = (long) zero;
              }
              else
                {
                    dvalue = input[ii] * scale + zero;

                    if (dvalue < DLONG_MIN)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = LONG_MIN;
                    }
                    else if (dvalue > DLONG_MAX)
                    {
                        *status = OVERFLOW_ERR;
                        output[ii] = LONG_MAX;
                    }
                    else
                        output[ii] = (long) dvalue;
                }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffi1r4(unsigned char *input, /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            unsigned char tnull,  /* I - value of FITS TNULLn keyword if any */
            float nullval,        /* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            float *output,        /* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to tnull.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
                output[ii] = (float) input[ii];  /* copy input to output */
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                output[ii] = ( (double) input[ii] ) * scale + zero;
            }
        }
    }
    else        /* must check for null values */
    {
        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                    output[ii] = (float) input[ii];
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                {
                    output[ii] = ( (double) input[ii] ) * scale + zero;
                }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffi2r4(short *input,         /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            short tnull,          /* I - value of FITS TNULLn keyword if any */
            float nullval,        /* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            float *output,        /* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to tnull.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
                output[ii] = (float) input[ii];  /* copy input to output */
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                output[ii] = input[ii] * scale + zero;
            }
        }
    }
    else        /* must check for null values */
    {
        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                    output[ii] = (float) input[ii];
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                {
                    output[ii] = input[ii] * scale + zero;
                }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffi4r4(long *input,          /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            long tnull,           /* I - value of FITS TNULLn keyword if any */
            float nullval,        /* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            float *output,        /* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to tnull.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
                output[ii] = (float) input[ii];  /* copy input to output */
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                output[ii] = input[ii] * scale + zero;
            }
        }
    }
    else        /* must check for null values */
    {
        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                    output[ii] = (float) input[ii];
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                {
                    output[ii] = input[ii] * scale + zero;
                }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffr4r4(float *input,         /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            float nullval,        /* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            float *output,        /* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to NaN.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;
    short *sptr, iret;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
                output[ii] = input[ii];   /* just copy input to output */
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                output[ii] = input[ii] * scale + zero;
            }
        }
    }
    else        /* must check for null values */
    {
        sptr = (short *) input;

#if MACHINE == VAXVMS
   /* do nothing */

#elif MACHINE == ALPHAVMS
        if (*(short *) &testfloat == IEEEFLOAT)
            sptr++;       /* point to MSBs */

#elif BYTESWAPPED == TRUE
        sptr++;       /* point to MSBs */
#endif

        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++, sptr += 2)
            {
              if (0 != (iret = fnan(*sptr) ) )  /* test for NaN or underflow */
              {
                  if (iret == 1)  /* is it a NaN? */
                  {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                  }
                  else            /* it's an underflow */
                     output[ii] = 0;
              }
              else
                output[ii] = input[ii];
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++, sptr += 2)
            {
              if (0 != (iret = fnan(*sptr) ) )  /* test for NaN or underflow */
              {
                  if (iret == 1)  /* is it a NaN? */
                  {  
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                  }
                  else            /* it's an underflow */
                     output[ii] = zero;
              }
              else
                  output[ii] = input[ii] * scale + zero;
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffr8r4(double *input,        /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            float nullval,        /* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            float *output,        /* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to NaN.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;
    short *sptr, iret;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
                output[ii] = (double) input[ii]; /* copy input to output */
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                output[ii] = input[ii] * scale + zero;
            }
        }
    }
    else        /* must check for null values */
    {
        sptr = (short *) input;

#if MACHINE == VAXVMS
   /* do nothing */

#elif MACHINE == ALPHAVMS
        if (*(short *) &testfloat == IEEEFLOAT)
            sptr += 3;       /* point to MSBs */

#elif BYTESWAPPED == TRUE
        sptr += 3;       /* point to MSBs */
#endif
        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++, sptr += 4)
            {
              if (0 != (iret = dnan(*sptr)) )  /* test for NaN or underflow */
              {
                  if (iret == 1)  /* is it a NaN? */
                  {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                  }
                  else            /* it's an underflow */
                     output[ii] = 0;
              }
              else
                  output[ii] = (double) input[ii];
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++, sptr += 4)
            {
              if (0 != (iret = dnan(*sptr)) )  /* test for NaN or underflow */
              {
                  if (iret == 1)  /* is it a NaN? */
                  {  
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                  }
                  else            /* it's an underflow */
                     output[ii] = zero;
              }
              else
                  output[ii] = input[ii] * scale + zero;
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffi1r8(unsigned char *input, /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            unsigned char tnull,  /* I - value of FITS TNULLn keyword if any */
            double nullval,       /* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            double *output,       /* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to tnull.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
                output[ii] = (double) input[ii]; /* copy input to output */
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                output[ii] = input[ii] * scale + zero;
            }
        }
    }
    else        /* must check for null values */
    {
        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                    output[ii] = (double) input[ii];
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                {
                    output[ii] = input[ii] * scale + zero;
                }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffi2r8(short *input,         /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            short tnull,          /* I - value of FITS TNULLn keyword if any */
            double nullval,       /* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            double *output,       /* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to tnull.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
                output[ii] = (double) input[ii]; /* copy input to output */
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                output[ii] = input[ii] * scale + zero;
            }
        }
    }
    else        /* must check for null values */
    {
        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                    output[ii] = (double) input[ii];
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                {
                    output[ii] = input[ii] * scale + zero;
                }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffi4r8(long *input,          /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            long tnull,           /* I - value of FITS TNULLn keyword if any */
            double nullval,       /* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            double *output,       /* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to tnull.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
                output[ii] = (double) input[ii]; /* copy input to output */
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                output[ii] = input[ii] * scale + zero;
            }
        }
    }
    else        /* must check for null values */
    {
        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                    output[ii] = (double) input[ii];
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                if (input[ii] == tnull)
                {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                }
                else
                {
                    output[ii] = input[ii] * scale + zero;
                }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffr4r8(float *input,         /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            double nullval,       /* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            double *output,       /* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to NaN.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;
    short *sptr, iret;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
                output[ii] = (double) input[ii]; /* copy input to output */
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                output[ii] = input[ii] * scale + zero;
            }
        }
    }
    else        /* must check for null values */
    {
        sptr = (short *) input;

#if MACHINE == VAXVMS
   /* do nothing */

#elif MACHINE == ALPHAVMS
        if (*(short *) &testfloat == IEEEFLOAT)
            sptr++;       /* point to MSBs */

#elif BYTESWAPPED == TRUE
        sptr++;       /* point to MSBs */
#endif
        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++, sptr += 2)
            {
              if (0 != (iret = fnan(*sptr) ) )  /* test for NaN or underflow */
              {
                  if (iret == 1)  /* is it a NaN? */
                  {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                  }
                  else            /* it's an underflow */
                     output[ii] = 0;
              }
              else
                output[ii] = (double) input[ii];
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++, sptr += 2)
            {
              if (0 != (iret = fnan(*sptr) ) )  /* test for NaN or underflow */
              {
                  if (iret == 1)  /* is it a NaN? */
                  {  
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                  }
                  else            /* it's an underflow */
                     output[ii] = zero;
              }
              else
                  output[ii] = input[ii] * scale + zero;
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffr8r8(double *input,        /* I - array of values to be converted     */
            long ntodo,           /* I - number of elements in the array     */
            double scale,         /* I - FITS TSCALn or BSCALE value         */
            double zero,          /* I - FITS TZEROn or BZERO  value         */
            int nullcheck,        /* I - null checking code; 0 = don't check */
                                  /*     1:set null pixels = nullval         */
                                  /*     2: if null pixel, set nullarray = 1 */
            double nullval,       /* I - set null pixels, if nullcheck = 1   */
            char *nullarray,      /* I - bad pixel array, if nullcheck = 2   */
            int  *anynull,        /* O - set to 1 if any pixels are null     */
            double *output,       /* O - array of converted pixels           */
            int *status)          /* IO - error status                       */
/*
  Copy input to output following reading of the input from a FITS file.
  Check for null values and do datatype conversion and scaling if required.
  The nullcheck code value determines how any null values in the input array
  are treated.  A null value is an input pixel that is equal to NaN.  If 
  nullcheck = 0, then no checking for nulls is performed and any null values
  will be transformed just like any other pixel.  If nullcheck = 1, then the
  output pixel will be set = nullval if the corresponding input pixel is null.
  If nullcheck = 2, then if the pixel is null then the corresponding value of
  nullarray will be set to 1; the value of nullarray for non-null pixels 
  will = 0.  The anynull parameter will be set = 1 if any of the returned
  pixels are null, otherwise anynull will be returned with a value = 0;
*/
{
    long ii;
    short *sptr, iret;

    if (nullcheck == 0)     /* no null checking required */
    {
        if (scale == 1. && zero == 0.)      /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++)
                output[ii] = input[ii];   /* just copy input to output */
        }
        else             /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++)
            {
                output[ii] = input[ii] * scale + zero;
            }
        }
    }
    else        /* must check for null values */
    {
        sptr = (short *) input;

#if MACHINE == VAXVMS
   /* do nothing */

#elif MACHINE == ALPHAVMS
        if (*(short *) &testfloat == IEEEFLOAT)
            sptr += 3;       /* point to MSBs */

#elif BYTESWAPPED == TRUE
        sptr += 3;       /* point to MSBs */
#endif

        if (scale == 1. && zero == 0.)  /* no scaling */
        {       
            for (ii = 0; ii < ntodo; ii++, sptr += 4)
            {
              if (0 != (iret = dnan(*sptr)) )  /* test for NaN or underflow */
              {
                  if (iret == 1)  /* is it a NaN? */
                  {
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                  }
                  else            /* it's an underflow */
                     output[ii] = 0;
              }
              else
                  output[ii] = input[ii];
            }
        }
        else                  /* must scale the data */
        {
            for (ii = 0; ii < ntodo; ii++, sptr += 4)
            {
              if (0 != (iret = dnan(*sptr)) )  /* test for NaN or underflow */
              {
                  if (iret == 1)  /* is it a NaN? */
                  {  
                    *anynull = 1;
                    if (nullcheck == 1)
                        output[ii] = nullval;
                    else
                        nullarray[ii] = 1;
                  }
                  else            /* it's an underflow */
                     output[ii] = zero;
              }
              else
                  output[ii] = input[ii] * scale + zero;
            }
        }
    }
    return(*status);
}
