#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "fitsio2.h"

/* ======================================================================
This file contains stubs for the AIPS WCS routines that are 
contained in the source file wcsutil.c.  The routines in wcsutil.c
should only be used by software that adheres to the terms of
the GNU General Public License.  Users who want to use CFITSIO but are
unwilling to release their code under the terms of the GNU General
Public License should replace the wcsutil.c file with this current
file before building the CFITSIO library.   This alternate version of
CFITSIO will behave the same as the standard version, except that it
will not support the ffwldp and ffxypx routines that calculate 
image coordinate transformation from pixel coordinates to world 
coordinates (e.g. Right Ascension and Declination) and vise versa.
======================================================================== */

/*--------------------------------------------------------------------------*/
int ffgics(fitsfile *fptr,    /* I - FITS file pointer           */
           double *xrval,     /* O - X reference value           */
           double *yrval,     /* O - Y reference value           */
           double *xrpix,     /* O - X reference pixel           */
           double *yrpix,     /* O - Y reference pixel           */
           double *xinc,      /* O - X increment per pixel       */
           double *yinc,      /* O - Y increment per pixel       */
           double *rot,       /* O - rotation angle (degrees)    */
           char *type,        /* O - type of projection ('-tan') */
           int *status)       /* IO - error status               */
/*
       read the values of the celestial coordinate system keywords.
       These values may be used as input to the subroutines that
       calculate celestial coordinates. (ffxypx, ffwldp)

       Modified in Nov 1999 to convert the CD matrix keywords back
       to the old CDELTn form, and to swap the axes if the dec-like
       axis is given first, and to assume default values if any of the
       keywords are not present.
*/
{
    int tstat = 0, cd_exists = 0, pc_exists = 0;
    char ctype[FLEN_VALUE];
    double cd11 = 0.0, cd21 = 0.0, cd22 = 0.0, cd12 = 0.0;
    double pc11 = 1.0, pc21 = 0.0, pc22 = 1.0, pc12 = 0.0;
    double pi =  3.1415926535897932;
    double phia, phib, temp;
    double toler = .0002;  /* tolerance for angles to agree (radians) */
                           /*   (= approximately 0.01 degrees) */

    if (*status > 0)
       return(*status);

    tstat = 0;
    if (ffgkyd(fptr, "CRVAL1", xrval, NULL, &tstat))
       *xrval = 0.;

    tstat = 0;
    if (ffgkyd(fptr, "CRVAL2", yrval, NULL, &tstat))
       *yrval = 0.;

    tstat = 0;
    if (ffgkyd(fptr, "CRPIX1", xrpix, NULL, &tstat))
        *xrpix = 0.;

    tstat = 0;
    if (ffgkyd(fptr, "CRPIX2", yrpix, NULL, &tstat))
        *yrpix = 0.;

    /* look for CDELTn first, then CDi_j keywords */
    tstat = 0;
    if (ffgkyd(fptr, "CDELT1", xinc, NULL, &tstat))
    {
        /* CASE 1: no CDELTn keyword, so look for the CD matrix */
        tstat = 0;
        if (ffgkyd(fptr, "CD1_1", &cd11, NULL, &tstat))
            tstat = 0;  /* reset keyword not found error */
        else
            cd_exists = 1;  /* found at least 1 CD_ keyword */

        if (ffgkyd(fptr, "CD2_1", &cd21, NULL, &tstat))
            tstat = 0;  /* reset keyword not found error */
        else
            cd_exists = 1;  /* found at least 1 CD_ keyword */

        if (ffgkyd(fptr, "CD1_2", &cd12, NULL, &tstat))
            tstat = 0;  /* reset keyword not found error */
        else
            cd_exists = 1;  /* found at least 1 CD_ keyword */

        if (ffgkyd(fptr, "CD2_2", &cd22, NULL, &tstat))
            tstat = 0;  /* reset keyword not found error */
        else
            cd_exists = 1;  /* found at least 1 CD_ keyword */

        if (cd_exists)  /* convert CDi_j back to CDELTn */
        {
            /* there are 2 ways to compute the angle: */
            phia = atan2( cd21, cd11);
            phib = atan2(-cd12, cd22);

            /* ensure that phia <= phib */
            temp = minvalue(phia, phib);
            phib = maxvalue(phia, phib);
            phia = temp;

            /* there is a possible 180 degree ambiguity in the angles */
            /* so add 180 degress to the smaller value if the values  */
            /* differ by more than 90 degrees = pi/2 radians.         */
            /* (Later, we may decide to take the other solution by    */
            /* subtracting 180 degrees from the larger value).        */

            if ((phib - phia) > (pi / 2.))
               phia += pi;

            if (fabs(phia - phib) > toler) 
            {
               /* angles don't agree, so looks like there is some skewness */
               /* between the axes.  Return with an error to be safe. */
               *status = APPROX_WCS_KEY;
            }
      
            phia = (phia + phib) /2.;  /* use the average of the 2 values */
            *xinc = cd11 / cos(phia);
            *yinc = cd22 / cos(phia);
            *rot = phia * 180. / pi;

            /* common usage is to have a positive yinc value.  If it is */
            /* negative, then subtract 180 degrees from rot and negate  */
            /* both xinc and yinc.  */

            if (*yinc < 0)
            {
                *xinc = -(*xinc);
                *yinc = -(*yinc);
                *rot = *rot - 180.;
            }
        }
        else   /* no CD matrix keywords either */
        {
            *xinc = 1.;

            /* there was no CDELT1 keyword, but check for CDELT2 just in case */
            tstat = 0;
            if (ffgkyd(fptr, "CDELT2", yinc, NULL, &tstat))
                *yinc = 1.;

            tstat = 0;
            if (ffgkyd(fptr, "CROTA2", rot, NULL, &tstat))
                *rot=0.;
        }
    }
    else  /* Case 2: CDELTn + optional PC matrix */
    {
        if (ffgkyd(fptr, "CDELT2", yinc, NULL, &tstat))
            *yinc = 1.;

        tstat = 0;
        if (ffgkyd(fptr, "CROTA2", rot, NULL, &tstat))
        {
            *rot=0.;

            /* no CROTA2 keyword, so look for the PC matrix */
            tstat = 0;
            if (ffgkyd(fptr, "PC1_1", &pc11, NULL, &tstat))
                tstat = 0;  /* reset keyword not found error */
            else
                pc_exists = 1;  /* found at least 1 PC_ keyword */

            if (ffgkyd(fptr, "PC2_1", &pc21, NULL, &tstat))
                tstat = 0;  /* reset keyword not found error */
            else
                pc_exists = 1;  /* found at least 1 PC_ keyword */

            if (ffgkyd(fptr, "PC1_2", &pc12, NULL, &tstat))
                tstat = 0;  /* reset keyword not found error */
            else
                pc_exists = 1;  /* found at least 1 PC_ keyword */

            if (ffgkyd(fptr, "PC2_2", &pc22, NULL, &tstat))
                tstat = 0;  /* reset keyword not found error */
            else
                pc_exists = 1;  /* found at least 1 PC_ keyword */

            if (pc_exists)  /* convert PCi_j back to CDELTn */
            {
                /* there are 2 ways to compute the angle: */
                phia = atan2( pc21, pc11);
                phib = atan2(-pc12, pc22);

                /* ensure that phia <= phib */
                temp = minvalue(phia, phib);
                phib = maxvalue(phia, phib);
                phia = temp;

                /* there is a possible 180 degree ambiguity in the angles */
                /* so add 180 degress to the smaller value if the values  */
                /* differ by more than 90 degrees = pi/2 radians.         */
                /* (Later, we may decide to take the other solution by    */
                /* subtracting 180 degrees from the larger value).        */

                if ((phib - phia) > (pi / 2.))
                   phia += pi;

                if (fabs(phia - phib) > toler) 
                {
                  /* angles don't agree, so looks like there is some skewness */
                  /* between the axes.  Return with an error to be safe. */
                  *status = APPROX_WCS_KEY;
                }
      
                phia = (phia + phib) /2.;  /* use the average of the 2 values */
                *rot = phia * 180. / pi;
            }
        }
    }

    /* get the type of projection, if any */
    tstat = 0;
    if (ffgkys(fptr, "CTYPE1", ctype, NULL, &tstat))
         type[0] = '\0';
    else
    {
        /* copy the projection type string */
        strncpy(type, &ctype[4], 4);
        type[4] = '\0';

        /* check if RA and DEC are inverted */
        if (!strncmp(ctype, "DEC-", 4) || !strncmp(ctype+1, "LAT", 3))
        {
            /* the latitudinal axis is given first, so swap them */

/*
 this case was removed on 12/9.  Apparently not correct.

            if ((*xinc / *yinc) < 0. )  
                *rot = -90. - (*rot);
            else
*/
            *rot = 90. - (*rot);

            /* Empirical tests with ds9 show the y-axis sign must be negated */
            /* and the xinc and yinc values must NOT be swapped. */
            *yinc = -(*yinc);

            temp = *xrval;
            *xrval = *yrval;
            *yrval = temp;
        }   
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgtcs(fitsfile *fptr,    /* I - FITS file pointer           */
           int xcol,          /* I - column containing the RA coordinate  */
           int ycol,          /* I - column containing the DEC coordinate */
           double *xrval,     /* O - X reference value           */
           double *yrval,     /* O - Y reference value           */
           double *xrpix,     /* O - X reference pixel           */
           double *yrpix,     /* O - Y reference pixel           */
           double *xinc,      /* O - X increment per pixel       */
           double *yinc,      /* O - Y increment per pixel       */
           double *rot,       /* O - rotation angle (degrees)    */
           char *type,        /* O - type of projection ('-sin') */
           int *status)       /* IO - error status               */
/*
       read the values of the celestial coordinate system keywords
       from a FITS table where the X and Y or RA and DEC coordinates
       are stored in separate column.  
       These values may be used as input to the subroutines that
       calculate celestial coordinates. (ffxypx, ffwldp)
*/
{
    char comm[FLEN_COMMENT],ctype[FLEN_VALUE],keynam[FLEN_KEYWORD];
    int tstatus = 0;

    if (*status > 0)
       return(*status);

    ffkeyn("TCRVL",xcol,keynam,status);
    ffgkyd(fptr,keynam,xrval,comm,status);

    ffkeyn("TCRVL",ycol,keynam,status);
    ffgkyd(fptr,keynam,yrval,comm,status);

    ffkeyn("TCRPX",xcol,keynam,status);
    ffgkyd(fptr,keynam,xrpix,comm,status);

    ffkeyn("TCRPX",ycol,keynam,status);
    ffgkyd(fptr,keynam,yrpix,comm,status);

    ffkeyn("TCDLT",xcol,keynam,status);
    ffgkyd(fptr,keynam,xinc,comm,status);

    ffkeyn("TCDLT",ycol,keynam,status);
    ffgkyd(fptr,keynam,yinc,comm,status);

    ffkeyn("TCTYP",xcol,keynam,status);
    ffgkys(fptr,keynam,ctype,comm,status);

    if (*status > 0)
    {
      ffpmsg
      ("ffgtcs could not find all the celestial coordinate keywords");
      return(*status = NO_WCS_KEY); 
    }

    /* copy the projection type string */
    strncpy(type, &ctype[4], 4);
    type[4] = '\0';

    *rot=0.;   /* default rotation is 0  */
    ffkeyn("TCROT",ycol,keynam,status);
    ffgkyd(fptr,keynam,rot,comm,&tstatus);  /* keyword may not exist */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffwldp(double xpix, double ypix, double xref, double yref,
      double xrefpix, double yrefpix, double xinc, double yinc, double rot,
      char *type, double *xpos, double *ypos, int *status)
{
    if (*status > 0)
        return(*status);

    ffpmsg("This non-GNU version of CFITSIO does not support");
    ffpmsg(" celestial coordinate transformations.");
    return(*status = 503);
}  
/*--------------------------------------------------------------------------*/
int ffxypx(double xpos, double ypos, double xref, double yref, 
      double xrefpix, double yrefpix, double xinc, double yinc, double rot,
      char *type, double *xpix, double *ypix, int *status)
{
    if (*status > 0)
        return(*status);

    ffpmsg("This non-GNU version of CFITSIO does not support");
    ffpmsg(" celestial coordinate transformations.");
    return(*status = 503);
} 
