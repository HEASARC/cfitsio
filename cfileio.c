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
#include <stdlib.h>
/* stddef.h is apparently needed to define size_t */
#include <stddef.h>
#include "fitsio2.h"
/*--------------------------------------------------------------------------*/
int ffsbuf(fitsfile **fptr,    /* I - FITS file pointer                   */
             void **buffptr,   /* I - address of memory pointer           */
             size_t *buffsize, /* I - size of buffer, in bytes            */
             size_t deltasize, /* I - increment for future realloc's      */
             void *(*mem_realloc)(void *p, size_t newsize), /* function   */
             int *status)    /* O - error status                          */
/*
  Low-level routine to create and initialize the fitsfile stucture for
  reading or writing a FITS file in memory rather than in a disk file.
*/
{
    if (*status > 0)
        return(*status);

    /* allocate mem and init = 0 */
    *fptr = (fitsfile *) calloc(1, sizeof(fitsfile)); 

    if (*fptr)
    {
        (*fptr)->bufftype = MEMBUFF;
        (*fptr)->memptr = buffptr;
        (*fptr)->memsize = buffsize;
        (*fptr)->mem_realloc = mem_realloc;
        (*fptr)->deltasize = deltasize;
        return(*status = USE_MEM_BUFF);
    }
    else
        return(*status = FILE_NOT_CREATED);
}
/*--------------------------------------------------------------------------*/
int ffwbuf(void *buffptr,          /* I - memory pointer                    */
             size_t buffsize,      /* I - size of buffer, in bytes          */
             const char *filename, /* I - name of file to create            */
             int *status)          /* IO - error status                     */
/*
  Copy the FITS file in the memory buffer to a disk file.  If filename = '-'
  then the memory buffer will be copied to the stdout stream.
*/
{
    fitsfile *fptr;       /* pointer to the FITS file, defined in fitsio.h */
    FILE *diskfile;
    void *tmpptr;
    size_t tmpsize, deltasize;
    int ii, jj, hdutype, wrote_err;
    long chdu, filesize;
    char errmsg[8];

    if (*status > 0)
        return(*status);

    ii = 0;
    while (filename[ii] == ' ')  /* ignore leading spaces in the filename */
        ii++;

    if (!filename[ii])
    {
        ffpmsg("Name of file to create is blank. (ffwbuf)");
        return(*status = FILE_NOT_CREATED);
    }

    /* set the parameters for the memory buffer */
    tmpptr = buffptr;
    tmpsize = buffsize;
    deltasize = 2880;
    fits_set_mem_buff(&fptr, &tmpptr, &tmpsize, deltasize, 0, status);

    /* open the memory FITS file using a blank name */
    if (ffopen(&fptr, " ", READONLY, status) > 0)
    {
        ffpmsg("ffwbuf failed to open memory FITS file:");
        return(*status);
    }

    /* find the end of file */
    wrote_err = TRUE;
    filesize = 0;
    for (jj = 1; ffmahd(fptr, jj, &hdutype, status) <= 0; jj++)
    {
        ffghad(fptr, &chdu, &filesize);    /* get the size of the file */
        if (filesize >= buffsize)  /* don't go beyond allocated memory */
        {
             *status = END_OF_FILE;
             wrote_err = FALSE;
             break;
        }
    }

    if (*status != END_OF_FILE)
    {
        ffpmsg("ffwbuf failed to find end of memory FITS file:");
        return(*status);
    }
    else
    {
        if (wrote_err)
            ffxmsg( -2, errmsg);  /* clear the EOF error message */
    }


    *status = 0;              /* got the expected EOF error; reset = 0  */

    ffclos(fptr, status);              /* close the memory file */

    if (strcmp(&filename[ii],"-") )
    {
        /* copy memory file to a real disk file */

        diskfile = fopen(&filename[ii], "r"); /* does file already exist? */
        if (diskfile)
        {
            fclose(diskfile);         /* close file and exit with error */
            ffpmsg("ffwbuf failed; the output file already exists:");
            ffpmsg(&filename[ii]);
            return(*status = FILE_NOT_CREATED);
        }

        diskfile = fopen(&filename[ii],"w+b");  /* create output diskfile */
        if (!diskfile)
        {
            ffpmsg("ffwbuf failed to create the following new file:");
            ffpmsg(&filename[ii]);
            return(*status = FILE_NOT_CREATED);
        }

        /* now copy from memory to the file (or standard out) */
        if(fwrite(buffptr, 1, filesize, diskfile) != filesize)
        {
            ffpmsg("ffwbuf failed to copy memory file to disk file");
            *status = WRITE_ERROR;
        }

        fclose(diskfile);  /* close the diskfile */
    }
    else
    {
        /* copy from memory to standard out */
        if(fwrite(buffptr, 1, filesize, stdout) != filesize)
        {
            ffpmsg("ffwbuf failed to copy memory file to stdout");
            *status = WRITE_ERROR;
        }
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffopen(fitsfile **fptr,      /* O - FITS file pointer                   */ 
           const char *filename, /* I - name of file to open                */
           int readwrite,        /* I - 0 = open readonly; 1 = read/write   */
           int *status)          /* IO - error status                       */
/*
  Open an existing FITS file with either readonly or read/write access.
*/
{
    void **buffptr;
    size_t *buffsize;
    int ii, hdutype, slen, tstatus;
    long filesize;

    if (*status > 0)
        return(*status);

    ii = 0;
    while (filename[ii] == ' ')  /* ignore leading spaces in the filename */
        ii++;

    if (*status != USE_MEM_BUFF)  /* not using memory? */
    {
      if (!filename[ii])
      {
        ffpmsg("Name of file to open is blank. (ffopen)");
        return(*status = FILE_NOT_OPENED);
      }

      /* allocate mem and init = 0 */
      *fptr = (fitsfile *) calloc(1, sizeof(fitsfile)); 

      if (!strcmp(&filename[ii], "-") )
      {
        /* 
        if reading file from stdin, must allocate temporary memory
        buffer since the user has not already done so.  Must remember
        to free the memory when the file is closed.
        */
        if (*fptr)
        {
            buffsize = malloc(sizeof(size_t));
            if (!buffsize)
            {
                ffpmsg("malloc of buffsize failed (ffopen)");
                return(*status = FILE_NOT_OPENED);
             }

            *buffsize = 2880;            /* initial size of the buffer */

            buffptr  = malloc(sizeof(buffptr));
            if (!buffptr)
            {
                ffpmsg("malloc of buffptr failed (ffopen)");
                free(buffsize);
                return(*status = FILE_NOT_OPENED);
             }

            *buffptr  = malloc(2880); 
            if (!(*buffptr))
            {
                ffpmsg("malloc of *buffptr failed (ffopen)");
                free(buffptr);
                free(buffsize);
                return(*status = FILE_NOT_OPENED);
             }

            (*fptr)->bufftype = TMPMEMBUFF;  /* using temporary buffer */
            (*fptr)->memptr = buffptr;
            (*fptr)->memsize = buffsize;
            (*fptr)->mem_realloc = realloc;  /* use the standard function */
            (*fptr)->deltasize = 2880;       /* increment by 1 FITS block */
        }
      }
    }
    else
    {
      *status = 0;  /* clear the USE_MEM_BUFF status flag */
    }

    if (*fptr)
    {
      slen = strlen(&filename[ii]) + 1;
      slen = maxvalue(slen, 20); /* malloc may crash on small lengths */ 
      (*fptr)->filename = (char *) malloc(slen); /* mem for file name */

      if ( (*fptr)->filename )
      {
        if (!strcmp(&filename[ii], "-") )
            strcpy((*fptr)->filename, "_stdin"); /* reading from stdin */
        else
            strcpy((*fptr)->filename, &filename[ii]); /* store the filename */

        ffopenx(*fptr, 0, readwrite, &filesize, &tstatus); /* open file */

        if (!tstatus)
        {
            (*fptr)->filesize = filesize;          /* physical file size */
            (*fptr)->logfilesize = filesize;       /* logical file size */
            (*fptr)->writemode = readwrite;        /* read-write mode    */
            (*fptr)->datastart = DATA_UNDEFINED;   /* unknown start of data */
            (*fptr)->curbuf = -1;   /* undefined current IO buffer */

            ffldrc(*fptr, 0, REPORT_EOF, status);     /* load first record */
            if (ffrhdu(*fptr, &hdutype, status) > 0)  /* set HDU structure */
            {
              ffpmsg(
                "ffopen could not interpret primary array header of file:");
              ffpmsg(&filename[ii]);

              if (*status == UNKNOWN_REC)
                 ffpmsg("This does not look like a FITS file.");
            }
            return(*status);   
        }
        /* error opening the file; free previously allocated memeory */
        if ((*fptr)->bufftype == TMPMEMBUFF)
        {
            free(*buffptr);
            free(buffptr);
            free(buffsize);
        }

        free( (*fptr)->filename);       /* free memory for the filename */
      }
      free(*fptr);            /* free memory for the FITS structure */
      *fptr = 0;              /* return null file pointer */
    }

    ffpmsg("ffopen failed to find and/or open the following file:");
    ffpmsg(filename);

    return(*status = FILE_NOT_OPENED);
}
/*--------------------------------------------------------------------------*/
int ffinit(fitsfile **fptr,      /* O - FITS file pointer                   */
           const char *filename, /* I - name of file to create              */
           int *status)          /* IO - error status                       */
/*
  Create and initialize a new FITS file.
*/
{
    void **buffptr;
    size_t *buffsize;
    int ii, slen, tstatus;
    long filesize;

    if (*status > 0)
        return(*status);

    ii = 0;
    while (filename[ii] == ' ')  /* ignore leading spaces in the filename */
        ii++;

    if (*status != USE_MEM_BUFF)  /* not using memory? */
    {
      if (!filename[ii])
      {
        ffpmsg("Name of file to create is blank. (ffinit)");
        return(*status = FILE_NOT_CREATED);
      }

      /* allocate mem and init = 0 */
      *fptr = (fitsfile *) calloc(1, sizeof(fitsfile)); 

      if (!strcmp(&filename[ii], "-") )
      {
        /* 
        if writing file to stdout, must allocate temporary memory
        buffer since the user has not already done so.  Must remember
        to free the memory when the file is closed.
        */
        if (*fptr)
        {
            buffsize = malloc(sizeof(size_t));
            if (!buffsize)
            {
                ffpmsg("malloc of buffsize failed (ffinit)");
                return(*status = FILE_NOT_OPENED);
             }

            *buffsize = 2880;            /* initial size of the buffer */

            buffptr  = malloc(sizeof(buffptr));
            if (!buffptr)
            {
                ffpmsg("malloc of buffptr failed (ffinit)");
                free(buffsize);
                return(*status = FILE_NOT_OPENED);
             }

            *buffptr  = malloc(2880); 
            if (!(*buffptr))
            {
                ffpmsg("malloc of *buffptr failed (ffinit)");
                free(buffptr);
                free(buffsize);
                return(*status = FILE_NOT_OPENED);
             }

            (*fptr)->bufftype = TMPMEMBUFF;  /* using temporary buffer */
            (*fptr)->memptr = buffptr;
            (*fptr)->memsize = buffsize;
            (*fptr)->mem_realloc = realloc;  /* use the standard function */
            (*fptr)->deltasize = 2880;       /* increment by 1 FITS block */
        }
      }
    }
    else
    {
      *status = 0;  /* clear the USE_MEM_BUFF status flag */
    }

    if (*fptr)
    {
      slen = strlen(&filename[ii]) + 1;
      slen = maxvalue(slen, 20); /* malloc may crash on small lengths */ 
      (*fptr)->filename = (char *) malloc(slen); /* mem for file name */

      if ( (*fptr)->filename )
      {
        if (!strcmp(&filename[ii], "-") )
            strcpy((*fptr)->filename, "_stdout"); /* writing to stdout */
        else
            strcpy((*fptr)->filename, &filename[ii]); /* store the filename */

        ffopenx(*fptr, 1, 1, &filesize, &tstatus);    /* create file */

        if (!tstatus)
        {
          (*fptr)->writemode = 1;                /* read-write mode    */
          (*fptr)->datastart = DATA_UNDEFINED;   /* unknown start of data */
          (*fptr)->curbuf = -1;   /* undefined current IO buffer */

          ffldrc(*fptr, 0, IGNORE_EOF, status);  /* initialize first record */
          return(*status);                       /* successful return */
        }
        /* error opening the file; free previously allocated memeory */
        if ((*fptr)->bufftype == TMPMEMBUFF)
        {
            free(*buffptr);
            free(buffptr);
            free(buffsize);
        }

        free( (*fptr)->filename);       /* free memory for the filename */
      } 
      free(*fptr);            /* free memory for the FITS structure */
      *fptr = 0;              /* return null file pointer */
    }

    ffpmsg("ffinit failed to create the following new file:");
    ffpmsg(filename);

    return(*status = FILE_NOT_CREATED);
}
/*--------------------------------------------------------------------------*/
int ffopenx(fitsfile *fptr,  /* I - FITS file pointer                       */
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
    FILE *diskfile;
    char mode[4];
    void *ptr;
    long tmpsize, nread;

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
   printf
("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
   printf(" Byteswapping is not being done correctly on this system.\n");
   printf(" Check the MACHINE and BYTESWAPPED definitions in fitsio2.h\n");
   printf(" Please report this problem to the author at\n");
   printf("     pence@tetra.gsfc.nasa.gov\n");
   printf(  "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
   }

/* end of temporary byteswap checking code */

    *filesize = 0;

    if (fptr->bufftype >= MEMBUFF) /* using memory buffer instead of file? */
    {
      if (newfile)
      {
          /* creating new file in memory; don't have to do anything */
      }
      else if (!(fptr->filename[0])) 
      {
          /* blank filename, so file must already exist in memory */
          *filesize = *(fptr->memsize);
      }
      else if (!strcmp(fptr->filename, "_stdin") )
      {
          /* Copy the FITS file from STDIN into memeory.  This   */
          /* simple algorithm allocates memory 1 block at a time */

          tmpsize = 0;
          nread = 2880;
          ptr = *(fptr->memptr);
          while(nread == 2880)
          {
              tmpsize += 2880;
              if (tmpsize > *(fptr->memsize))
              {
                  /* allocate memory for another FITS block */
                  ptr = (fptr->mem_realloc)(*(fptr->memptr), tmpsize);

                  if (!ptr)
                  {
                      ffpmsg("Failed to allocate memory in ffopenx!");
                      return(*status = 1);
                  }

                  *(fptr->memptr) = ptr;
                  *(fptr->memsize) = tmpsize;
              }

              /* try to read another FITS block into memory */
              nread = fread( (char *) ptr + tmpsize - 2880, 1, 2880, stdin);
          }

          *filesize = tmpsize - 2880;  /* final file size */
      }
      else
      {
          /* open the disk file and copy into memory */

#if MACHINE == ALPHAVMS || MACHINE == VAXVMS
          diskfile = fopen(fptr->filename,"rb","rfm=fix","mrs=2880","ctx=stm"); 
#else
          diskfile = fopen(fptr->filename,"rb"); 
#endif

          if (!diskfile)  /* couldn't open the disk file */
              return(*status = 1); 

          /* determine the size of the file */
          fseek(diskfile, 0, 2);   /* move to end of the existing file */
          *filesize = ftell(diskfile);  /* position = size of file */
          fseek(diskfile, 0, 0);   /* move back to beginning of file */

          if (*filesize > *(fptr->memsize))
          {
              /* have to allocate more memory to hold the file */
              if (!(fptr->mem_realloc))
              {
                  ffpmsg("need a realloc function in ffopenx");
                  return(*status = 1);
              }
              ptr = (fptr->mem_realloc)(*(fptr->memptr), *filesize);
              if (!ptr)
              {
                  ffpmsg("Failed to allocate memory in ffopenx!");
                  return(*status = 1);
              }

              *(fptr->memptr) = ptr;
              *(fptr->memsize) = *filesize;
          }

          /* now copy the file into memory */
          if(fread(ptr, 1, *filesize, diskfile) != *filesize)
          {
              fclose(diskfile);  /* close the diskfile */
              return(*status = 1);
          }

          fclose(diskfile);  /* close the diskfile */
      }
    }             /* end of memory buffer case */
    else
    {             /* file will exist on magnetic disk */
        if (newfile)
        {
          strcpy(mode, "w+b");  /* new file must have read and write access */

          diskfile = fopen(fptr->filename, "r"); /* does file already exist? */

          if (diskfile)
          {
              fclose(diskfile);         /* close file and exit with error */
              return(*status = 1); 
          }
        }

        else if (readwrite)
          strcpy(mode, "r+b");    /* open existing file with read-write */
        else
          strcpy(mode, "rb");     /* open existing file readonly */

        /* now open the file */

#if MACHINE == ALPHAVMS || MACHINE == VAXVMS

        /* specify VMS record structure: fixed format, 2880 byte records */
        /* but force stream mode access to enable random I/O access      */
        diskfile = fopen(fptr->filename,mode,"rfm=fix","mrs=2880","ctx=stm"); 
#else
        diskfile = fopen(fptr->filename,mode); 
#endif

        if (!diskfile)           /* couldn't open file */
            return(*status = 1); 

        fptr->fileptr = diskfile; /* copy file pointer to the structure */
        if (!newfile)
        {
            fseek(diskfile, 0, 2);   /* move to end of the existing file */
            *filesize = ftell(diskfile);  /* position = size of file */
            fseek(diskfile, 0, 0);   /* move back to beginning of file */
        }
    }
    return(*status = 0);
}
/*--------------------------------------------------------------------------*/
int ffclosex(fitsfile *fptr, /* I - FITS file pointer                      */
             int keep,       /* I - 0=discard the file, else keep the file */
             int *status)    /* O - error status                           */

/*
  Low-level, system dependent routine to close the disk file.
  If keep = 0, then the file will also be deleted from disk.
*/
{
    *status = 0;

    if (fptr->bufftype < MEMBUFF) /* not using memory buffer? */
    {
      if ( fclose(fptr->fileptr) )   /* close the disk file */
        *status = 1;

      if (!keep)
        remove(fptr->filename);    /* delete the file */
    }
    else    /* using memory buffer */
    {
        if (!strcmp(fptr->filename,"_stdout"))
        {
            /* copy from memory to standard out */
            if(fwrite(*(fptr->memptr), 1, fptr->filesize, stdout) !=
                  fptr->filesize)
            {
                ffpmsg("ffclosex failed to copy memory file to stdout");
                *status = WRITE_ERROR;
            }
        }

        if (fptr->bufftype == TMPMEMBUFF)
        {
            /* CFITSIO created the temporary buffers, so now free them */
            free(*(fptr->memptr));
            free(fptr->memptr);
            free(fptr->memsize);
        }

        fptr->bufftype = 0; /* unset the buffer type flag */  
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffflushx( fitsfile *fptr)     /* I - FITS file pointer                  */
/*
  lowest level system-dependent routine to flush file buffers to disk.
*/
{
    if (fptr->bufftype < MEMBUFF) /* not using memory buffer? */
        return(fflush(fptr->fileptr));
    else
        return(0);
}
/*--------------------------------------------------------------------------*/
int ffseek( fitsfile *fptr,   /* I - FITS file pointer              */
            long position)    /* I - byte position to seek to       */
/*
  lowest level system-dependent routine to seek to a position in a file.
  Return non-zero on error.
*/
{
    if (fptr->bufftype < MEMBUFF) /* not using memory buffer? */
        return(fseek(fptr->fileptr, position, 0));
    else
    {
        fptr->io_pos = position;
        return(0);
    }
}
/*--------------------------------------------------------------------------*/
int ffwrite( fitsfile *fptr,   /* I - FITS file pointer              */
             long nbytes,      /* I - number of bytes to write       */
             void *buffer,     /* I - buffer to write                */
             int *status)      /* O - error status                   */
/*
  lowest level system-dependent routine to write bytes to a file.
  Return status = 106 on write error.
*/
{
    size_t newsize;
    void *ptr;

    if (fptr->bufftype < MEMBUFF) /* not using memory buffer? */
    {
        if(fwrite(buffer, 1, nbytes, fptr->fileptr) != nbytes)
            *status = 106;
    }
    else
    {

        if (fptr->io_pos + nbytes > *(fptr->memsize))
        {
          /*
             Attempt to reallocate additional memory:
             the memory buffer size is incremented by the larger of:
                  1 FITS block (2880 bytes),
                  the user-specified 'deltasize' parameter, or
                  amount needed to hold the current size of the 'file'.
          */
               
            newsize = maxvalue(
                       (((fptr->io_pos + nbytes - 1) / 2880) + 1) * 2880,
                       *(fptr->memsize) + fptr->deltasize);

            newsize = maxvalue(newsize, fptr->headstart[fptr->maxhdu +1]);

            if (fptr->mem_realloc)
               ptr = (fptr->mem_realloc)(*(fptr->memptr), newsize);
            else
            {
               ffpmsg("need a realloc function in ffwrite");
               return(*status = 106);
            }

            if (ptr)
            {
               *(fptr->memptr) = ptr;
               *(fptr->memsize) = newsize;
            }
            else
            {
               ffpmsg("Failed to reallocate memory in ffwrite");
               return(*status = 106);
            }
        }

        /* copy bytes into memory buffer */
        memcpy(((char *) *(fptr->memptr)) + fptr->io_pos , buffer, nbytes);
        fptr->io_pos += nbytes; 
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffread( fitsfile *fptr,   /* I - FITS file pointer              */
            long nbytes,      /* I - number of bytes to read        */
            void *buffer,     /* O - buffer to read into            */
            int *status)      /* O - error status                   */
/*
  lowest level system-dependent routine to read bytes from a file.
  Return status = 108 on error.
*/
{
    if (fptr->bufftype < MEMBUFF) /* not using memory buffer? */
    {
        if(fread(buffer, 1, nbytes, fptr->fileptr) != nbytes)
            *status = 108;
    }
    else
    {
        if (fptr->io_pos + nbytes <= *(fptr->memsize))
        {
            memcpy(buffer, ((char *) *(fptr->memptr)) + fptr->io_pos , nbytes);
            fptr->io_pos += nbytes; 
        }
        else
            *status = 108;
    }
    return(*status);
}

