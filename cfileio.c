/*  This file, cfileio.c, contains the low-level C language file access    */
/*  routines used by the FITSIO software.                                  */

/*  The FITSIO software was written by William Pence at the High Energy    */
/*  Astrophysic Science Archive Research Center (HEASARC) at the NASA      */
/*  Goddard Space Flight Center.  Users shall not, without prior written   */
/*  permission of the U.S. Government,  establish a claim to statutory     */
/*  copyright.  The Government and others acting on its behalf, shall have */
/*  a royalty-free, non-exclusive, irrevocable,  worldwide license for     */
/*  Government purposes to publish, distribute, translate, copy, exhibit,  */
/*  and perform such material.                                             */

#include <string.h>
#include "fitsio2.h"
/*--------------------------------------------------------------------------*/
int ffopenx(FILE **diskfile, /* O - pointer to  file descriptor             */
            const char *filename,  /* I - name of file to open or create    */
            int newfile,     /* I - 0=open existing file; 1=create new file */
            int readwrite,   /* I - 0 = open readonly; 1 = open read/write  */
            long *filesize,  /* O - size of file (bytes); needed when       */
                             /*     opening an existing file with write mode*/
            int *status)     /* O - error status: 0 = success, 1 = error    */
/*
  lowest-level routine to open or create a file;  If creating a new file,
  make sure a file with the same name doesn't already exist.  If opening 
  an existing file then must also return the current size of the file.
*/
{
    char mode[4];

/* temporarily add test for correct byteswapping.  Remove this after
   cfitsio is more fully tested on all machines.
*/
   union u_tag {
     short ival;
     char cval[2];
   } u;

   u.ival = 1;
   if  ((BYTESWAPPED == TRUE && u.cval[0] != 1) ||
        (BYTESWAPPED == FALSE && u.cval[1] != 1) )
   {
   printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
   printf(" Byteswapping is not being done correctly on this system.\n");
   printf(" Check the MACHINE and BYTESWAPPED definitions in fitsio2.h\n");
   printf(" Please report this problem to the author at\n");
   printf("     pence@tetra.gsfc.nasa.gov\n");
   printf(  "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
   }

/* end of temporary byteswap checking code */

    *filesize = 0;

    if (newfile)
    {
      strcpy(mode, "w+b");    /* create new file with read, write access */

      *diskfile = fopen(filename, "r");  /* file already exist? */

      if (*diskfile)
      {
        fclose(*diskfile);         /* close file and exit with error */
        *diskfile = 0;             /* return null pointer */
        return(*status = 1); 
      }
    }

    else if (readwrite)
      strcpy(mode, "r+b");    /* open existing file with read-write */
    else
      strcpy(mode, "rb");     /* open existing file readonly */

    /* open the file */

#if MACHINE == ALPHAVMS || MACHINE == VAXVMS

    /* specify VMS record structure: fixed format, 2880 byte records */
    /* but force stream mode access to enable random I/O access      */

    *diskfile =  fopen(filename,mode,"rfm=fix","mrs=2880","ctx=stm"); 

#else

    *diskfile =  fopen(filename,mode); 

#endif

    if (*diskfile)
    {
        if (!newfile)
        {
            fseek(*diskfile, 0, 2);   /* move to end of the existing file */
            *filesize = ftell(*diskfile);  /* position = size of file */
            fseek(*diskfile, 0, 0);   /* move back to beginning of file */
        }

        return(*status = 0);
    }
    else
        return(*status = 1); 
}
/*--------------------------------------------------------------------------*/
int ffclosex(FILE *diskfile, /* I - file descriptor                        */
             const char *filename, /* I - name of the file                 */
             int keep,       /* I - 0=discard the file, else keep the file */
             int *status)    /* O - error status                           */

/*
  Low-level, system dependent routine to close the disk file.
  If keep = 0, then the file will also be deleted from disk.
*/
{
    *status = 0;

    if (fclose(diskfile))     /* close the disk file */
        *status = 1;

    if (keep == 0)
        remove(filename);    /* delete the file */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffflushx( FILE *diskfile )   /* I - file descriptor                */
/*
  lowest level system-dependent routine to flush file buffers to disk.
*/
{
    return(fflush(diskfile));
}
/*--------------------------------------------------------------------------*/
int ffseek( FILE *diskfile,   /* I - file descriptor                */
            long position)    /* I - byte position to seek to       */
/*
  lowest level system-dependent routine to seek to a position in a file.
  Return non-zero on error.
*/
{
    return(fseek(diskfile, position, 0));
}
/*--------------------------------------------------------------------------*/
int ffwrite( FILE *diskfile,   /* I - file descriptor                */
             long nbytes,      /* I - number of bytes to write       */
             void *buffer,     /* I - buffer to write                */
             int *status)      /* O - error status                   */
/*
  lowest level system-dependent routine to write bytes to a file.
  Return status = 106 on write error.
*/
{
    if(fwrite(buffer, 1, nbytes, diskfile) != nbytes)
        *status = 106;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffread( FILE *diskfile,   /* I - file descriptor                */
            long nbytes,      /* I - number of bytes to read        */
            void *buffer,     /* O - buffer to read into            */
            int *status)      /* O - error status                   */
/*
  lowest level system-dependent routine to read bytes from a file.
  Return status = 108 on error.
*/
{
    if(fread(buffer, 1, nbytes, diskfile) != nbytes)
        *status = 108;

    return(*status);
}

