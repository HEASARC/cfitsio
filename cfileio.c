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
#include <stddef.h>  /* apparently needed to define size_t */
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
        (*fptr)->filename = (char *) malloc(160); /* mem for file name */
        if (!((*fptr)->filename))
        {
            free(*fptr);
        }
        else
        {
            (*fptr)->bufftype = MEMBUFF;
            (*fptr)->memptr = buffptr;
            (*fptr)->memsize = buffsize;
            (*fptr)->mem_realloc = mem_realloc;
            (*fptr)->deltasize = deltasize;
            return(*status = USE_MEM_BUFF);
        }
    }

    ffpmsg("ffsbuf failed to allocate memory for FITS file structure");
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
    int ii, hdutype, slen, tstatus;
    size_t filesize, finalsize;
    FILE *diskfile;

    if (*status > 0)
        return(*status);

    ii = 0;
    while (filename[ii] == ' ')  /* ignore leading spaces in the filename */
        ii++;

    if (*status == USE_MEM_BUFF)  /*  using preallocated memory? */
    {
        /* the FITS file structure has already been created by ffsbuff */

        *status = 0;  /* simply clear the status flag */
    }
    else   /* must allocate the FITS file structure */
    {
        if (!filename[ii])
        {
            ffpmsg("Name of file to open is blank. (ffopen)");
            return(*status = FILE_NOT_OPENED);
        }

        /* allocate FITSfile structure and init = 0 */
        *fptr = (fitsfile *) calloc(1, sizeof(fitsfile));

        if (!(*fptr))
        {
            ffpmsg("ffopen failed to allocate memory for following file:");
            ffpmsg(filename);
            return(*status = FILE_NOT_OPENED);
        }

        slen = strlen(&filename[ii]) + 1;
        slen = maxvalue(slen, 32); /* reserve at least 32 chars */ 
        (*fptr)->filename = (char *) malloc(slen); /* mem for file name */

        if ( !((*fptr)->filename) )
        {
            ffpmsg("ffopen failed to allocate memory for filename:");
            ffpmsg(filename);
            free(*fptr);
            *fptr = 0;              /* return null file pointer */
            return(*status = FILE_NOT_OPENED);
        }
    }

    strcpy((*fptr)->filename, &filename[ii]); /* store the input filename */

    /*
    Now open the file.  There are 5 different cases to handle:
      1.  FITS file already exists in preallocated memory (blank filename)
      2.  FITS file piped in on stdin; 
      3.  Open a compressed FITS file on disk; uncompress into memory
      4.  Copy a disk file into preallocated memory
      5.  Open an ordinary FITS file on disk
    */

    if (!filename[ii])   /*  Case 1: file pre-exists in memory (no name) */
    {
        filesize = *((*fptr)->memsize);  /* upper limit to the file size */
    }
    else if ( !strcmp(&filename[ii], "-") )  /* Case 2: reading from stdin */
    {
        strcpy((*fptr)->filename, "_stdin"); /* store special filename */

        if ((*fptr)->bufftype != MEMBUFF) /* must allocate temporary memory */
        {
            if (ffcreate_mem(*fptr, 2880L, status) > 0)
            {
                ffpmsg("failed to create temporary memory buffer");
                free((*fptr)->filename);
                free(*fptr);
                *fptr = 0;              /* return null file pointer */
                return(*status = FILE_NOT_OPENED);
            }
        }

        /* call routine that copies stdin into memory */

        if ( ffstdin2mem((*fptr)->memptr, (*fptr)->memsize, 
           (*fptr)->mem_realloc, &filesize, status) > 0 )
        {
            /* error copying stdin; free previously allocated memeory */
            if ((*fptr)->bufftype == TMPMEMBUFF)
            {
                /* CFITSIO created the temporary buffers, so now free them */
                free(*((*fptr)->memptr));
                free((*fptr)->memptr);
                free((*fptr)->memsize);
            }

            free((*fptr)->filename);
            free(*fptr);
            *fptr = 0;              /* return null file pointer */
            return(*status = FILE_NOT_OPENED);
        }
    }
    else if (ffisfilecompressed((*fptr)->filename, &diskfile, 
            &filesize, &finalsize, status)) /* Case 3: compressed file? */
    {
        if (readwrite)
        {
            ffpmsg("Compressed files may only be opened 'READONLY'");
            ffclosefile((*fptr)->filename, diskfile, 1, status);
            free((*fptr)->filename);
            free(*fptr);
            *fptr = 0;              /* return null file pointer */
            return(*status = FILE_NOT_OPENED);
        }

        /* if memory has not been preallocated, them must allocate  */
        /* temporary memory buffer to uncompress the file into.     */

        if ((*fptr)->bufftype != MEMBUFF) 
        {
            if (finalsize == 0)  /* unknow uncompressed file size? */
            {
                finalsize = filesize * 3;  /* estimate final size */
            }

            if (ffcreate_mem(*fptr, finalsize, status) > 0)
            {
                ffpmsg
              ("failed to create temporary memory buffer for decompression");
                ffclosefile((*fptr)->filename, diskfile, 1, status);
                free((*fptr)->filename);
                free(*fptr);
                *fptr = 0;              /* return null file pointer */
                return(*status = FILE_NOT_OPENED);
            }
        }

        /* call routine to uncompress the file into memory */

        if ( ffuncompress2mem((*fptr)->filename, diskfile, (*fptr)->memptr,
           (*fptr)->memsize, (*fptr)->mem_realloc, &filesize, status) > 0 )
        {
            ffclosefile((*fptr)->filename, diskfile, 1, status);

            /* error uncompressing file; free previously allocated memeory */
            if ((*fptr)->bufftype == TMPMEMBUFF)
            {
                /* CFITSIO created the temporary buffers, so now free them */
                free(*((*fptr)->memptr));
                free((*fptr)->memptr);
                free((*fptr)->memsize);
            }

            free((*fptr)->filename);
            free(*fptr);
            *fptr = 0;              /* return null file pointer */

            return(*status = FILE_NOT_OPENED);
        }

        ffclosefile((*fptr)->filename, diskfile, 1, status);

        if ((*fptr)->bufftype == TMPMEMBUFF)
        {
           if (*((*fptr)->memsize) > (filesize + 256L) )
           {
              /* if we allocated too much memory initially, then free it */
              (*fptr)->memptr = realloc((*fptr)->memptr, filesize);
              *((*fptr)->memsize) = filesize;
           }
        }
    }
    else if ((*fptr)->bufftype == MEMBUFF) /* Case 4: copy file into memory */
    {
        if ( fffile2mem((char *)&filename[ii], (*fptr)->memptr, 
           (*fptr)->memsize, (*fptr)->mem_realloc, &filesize, status) > 0 )
        {
            /* error copying file; free previously allocated memeory */
            free((*fptr)->filename);
            free(*fptr);
            *fptr = 0;              /* return null file pointer */
            return(*status = FILE_NOT_OPENED);
        }
    }
    else  /* case 5: open ordinary FITS file on disk */
    {
        if (ffopenfile((*fptr)->filename, 0, readwrite, &diskfile, 
            &filesize, status) > 0)
        {
            free((*fptr)->filename);
            free(*fptr);
            *fptr = 0;              /* return null file pointer */
            return(*status = FILE_NOT_OPENED);
        }

        (*fptr)->fileptr = diskfile;   /* store the file pointer */
    }

    /* store the file size and other parameters */

    (*fptr)->filesize = filesize;          /* physical file size */
    (*fptr)->logfilesize = filesize;       /* logical file size */
    (*fptr)->writemode = readwrite;        /* read-write mode    */
    (*fptr)->datastart = DATA_UNDEFINED;   /* unknown start of data */
    (*fptr)->curbuf = -1;   /* undefined current IO buffer */

    ffldrc(*fptr, 0, REPORT_EOF, status);     /* load first record */

    if (ffrhdu(*fptr, &hdutype, status) > 0)  /* determine HDU structure */
    {
        ffpmsg(
          "ffopen could not interpret primary array header of file:");
        ffpmsg(&filename[ii]);

        if (*status == UNKNOWN_REC)
           ffpmsg("This does not look like a FITS file.");

        /* free previously allocated memeory */
        if ((*fptr)->bufftype == TMPMEMBUFF)
        {
            /* CFITSIO created the temporary buffers, so now free them */
            free(*((*fptr)->memptr));
            free((*fptr)->memptr);
            free((*fptr)->memsize);
        }

        free((*fptr)->filename);
        free(*fptr);
        *fptr = 0;              /* return null file pointer */
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffinit(fitsfile **fptr,      /* O - FITS file pointer                   */
           const char *filename, /* I - name of file to create              */
           int *status)          /* IO - error status                       */
/*
  Create and initialize a new FITS file.
*/
{
    int ii, slen, tstatus;
    size_t filesize;
    FILE *diskfile;

    if (*status > 0)
        return(*status);

    ii = 0;
    while (filename[ii] == ' ')  /* ignore leading spaces in the filename */
        ii++;

    if (*status == USE_MEM_BUFF)  /*  using preallocated memory? */
    {
        /* the FITS file structure has already been created by ffsbuff */

        *status = 0;  /* simply clear the status flag */
    }
    else   /* must allocate the FITS file structure */
    {
        if (!filename[ii])
        {
            ffpmsg("Name of file to create is blank. (ffinit)");
            return(*status = FILE_NOT_CREATED);
        }

        /* allocate FITSfile structure and init = 0 */
        *fptr = (fitsfile *) calloc(1, sizeof(fitsfile));

        if (!(*fptr))
        {
            ffpmsg("ffinit failed to allocate memory for new file:");
            ffpmsg(filename);
            return(*status = FILE_NOT_CREATED);
        }

        slen = strlen(&filename[ii]) + 1;
        slen = maxvalue(slen, 32); /* reserve at least 32 chars */ 
        (*fptr)->filename = (char *) malloc(slen); /* mem for file name */

        if ( !((*fptr)->filename) )
        {
            ffpmsg("ffinit failed to allocate memory for filename:");
            ffpmsg(filename);
            free(*fptr);
            *fptr = 0;              /* return null file pointer */
            return(*status = FILE_NOT_CREATED);
        }
    }

    if ( !strcmp(&filename[ii], "-") )  /* Case 1: writing to stdout */
    {
        strcpy((*fptr)->filename, "_stdout"); /* store special filename */

        if ((*fptr)->bufftype != MEMBUFF) /* must allocate temporary memory */
        {
            if (ffcreate_mem(*fptr, 2880, status) > 0)
            {
                ffpmsg("failed to create temporary memory buffer for stdout");
                free((*fptr)->filename);
                free(*fptr);
                *fptr = 0;              /* return null file pointer */
                return(*status = FILE_NOT_CREATED);
            }
        }
    }
    else if ((*fptr)->bufftype == MEMBUFF) /* Case 2: create file in memory */
    {
       /* don't have to do anything */
    }
    else   /* Case 3: create ordinary disk file */
    {
        strcpy((*fptr)->filename, &filename[ii]); /* store the filename */

        if (ffopenfile((*fptr)->filename, 1, 1, &diskfile, 
            &filesize, status) > 0)
        {
            free((*fptr)->filename);
            free(*fptr);
            *fptr = 0;              /* return null file pointer */
            return(*status = FILE_NOT_CREATED);
        }

        (*fptr)->fileptr = diskfile;   /* store the file pointer */
    }

    /* store the file  parameters */

    (*fptr)->writemode = 1;           /* read-write mode    */
    (*fptr)->datastart = DATA_UNDEFINED;   /* unknown start of data */
    (*fptr)->curbuf = -1;   /* undefined current IO buffer */

    ffldrc(*fptr, 0, IGNORE_EOF, status);     /* initialize first record */

    return(*status);                       /* successful return */
}
/*--------------------------------------------------------------------------*/
int ffcreate_mem(fitsfile *fptr,  /* I - FITS file pointer                  */
             size_t init_size,    /* I - initial size of the buffer         */
             int *status)         /* IO - error status                      */

/* 
    Create temporary memory buffer to hold the input FITS file,
*/
{
    void **buffptr;
    size_t *buffsize;

    buffsize = malloc(sizeof(size_t));
    if (!buffsize)
    {
        ffpmsg("malloc of buffsize failed (ffcreate_mem)");
        return(*status = 1);
    }

    *buffsize = init_size;            /* initial size of the buffer */

    buffptr  = malloc(sizeof(buffptr));
    if (!buffptr)
    {
        ffpmsg("malloc of buffptr failed (ffcreate_mem)");
        free(buffsize);
        return(*status = 1);
    }

    *buffptr  = malloc(init_size); 
    if (!(*buffptr))
    {
        ffpmsg("malloc of *buffptr failed (ffcreate_mem)");
        free(buffptr);
        free(buffsize);
        return(*status = FILE_NOT_OPENED);
    }

    fptr->bufftype = TMPMEMBUFF;  /* using temporary buffer */
    fptr->memptr = buffptr;
    fptr->memsize = buffsize;
    fptr->mem_realloc = realloc;  /* use the standard function */
    fptr->deltasize = 2880;       /* increment by 1 FITS block */
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffstdin2mem(void **buffptr,  /* IO - memory pointer                     */
             size_t *buffsize,   /* IO - size of buffer, in bytes           */
             void *(*mem_realloc)(void *p, size_t newsize), /* function     */
             size_t *filesize,   /* O - size of file, in bytes              */
             int *status)        /* IO - error status                       */

/*
  Copy the stdin stream into memory.  Fill whatever amount of memory
  has already been allocated, then realloc more memory, using the
  input function, if necessary.
*/
{
    size_t nread;

    if (*status > 0)
       return(*status);

    *filesize = 0;

    /* fill up the initial buffer allocation */
    nread = fread( (char *) *buffptr, 1, *buffsize, stdin);

    *filesize += nread;

    if (nread < *buffsize)    /* reached the end? */
       return(*status);

    while (1)
    {
        /* allocate memory for another FITS block */
        *buffptr = mem_realloc(*buffptr, *buffsize + 2880);
        if (!(*buffptr))
        {
            ffpmsg("malloc failed while copying stdin (ffstdin2mem)");
            return(*status = FILE_NOT_OPENED);
        }
        *buffsize += 2880;

        /* read another FITS block */
        nread = fread( (char *) (*buffptr) + *filesize, 1, 2880, stdin);
        *filesize += nread;

        if (nread < 2880)    /* reached the end? */
           break;
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fffile2mem(char *filename,   /* I - file to copy into memory      */
             void **buffptr,     /* IO - memory pointer                     */
             size_t *buffsize,   /* IO - size of buffer, in bytes           */
             void *(*mem_realloc)(void *p, size_t newsize), /* function     */
             size_t *filesize,   /* O - size of file, in bytes              */
             int *status)        /* IO - error status                       */

/*
  Copy the file into memory.  
*/
{
    FILE *diskfile;
    size_t nread;
    void *ptr;

    if (*status > 0)
       return(*status);

    *filesize = 0;

    if (ffopenfile(filename, 0, 0, &diskfile, filesize, status) > 0)
        return(*status);

     if (*filesize > *buffsize)
    {
        /* have to allocate more memory to hold the file */
        if (!mem_realloc)
        {
            ffpmsg("need a realloc function in fffile2mem");
            return(*status = 1);
        }
        ptr = mem_realloc(*buffptr, *filesize);
        if (!ptr)
        {
            ffpmsg("Failed to allocate memory in fffile2mem!");
            return(*status = 1);
        }

        *buffptr = ptr;
        *buffsize = *filesize;
    }

    /* now read the file into memory */
    if(fread(ptr, 1, *filesize, diskfile) != *filesize)
       *status = 1;

    /* close the diskfile */
    ffclosefile(filename, diskfile, 1, status);

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffopenfile(char *filename, /* I - FITS file pointer                    */
            int newfile,     /* I - 0=open existing file; 1=create new file */
            int readwrite,   /* I - 0 = open readonly; 1 = open read/write  */
            FILE **diskfile, /* O - file pointer                            */
            size_t *filesize, /* O - size of file (bytes)                   */
            int *status)     /* O - error status: 0 = success, 1 = error    */
/*
  lowest-level routine to open or create a file;  If creating a new file,
  make sure a file with the same name doesn't already exist.  Return the 
  current size of the file.
*/
{
    char mode[4];
    void *ptr;

/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* temporarily add test for correct byteswapping.  Remove this after
   cfitsio is more fully tested on all machines.
*/
   union u_tag {
     short ival;
     char cval[2];
   } u;

   u.ival = 1;
   if  ((BYTESWAPPED && u.cval[0] != 1) ||
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

/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    *filesize = 0;

    if (newfile)
    {
          strcpy(mode, "w+b");  /* new file must have read and write access */

          *diskfile = fopen(filename, "r"); /* does file already exist? */

          if (*diskfile)
          {
              fclose(*diskfile);         /* close file and exit with error */
              return(*status = 1); 
          }
    }
    else if (readwrite)
    {
          strcpy(mode, "r+b");    /* open existing file with read-write */
    }
    else
    {
          strcpy(mode, "rb");     /* open existing file readonly */
    }
    /* now open the file */

#if MACHINE == ALPHAVMS || MACHINE == VAXVMS
        /* specify VMS record structure: fixed format, 2880 byte records */
        /* but force stream mode access to enable random I/O access      */
    *diskfile = fopen(filename,mode,"rfm=fix","mrs=2880","ctx=stm"); 
#else
    *diskfile = fopen(filename,mode); 
#endif

    if (!(*diskfile))           /* couldn't open file */
    {
            return(*status = 1); 
    }

    if (!newfile)
    {
            fseek(*diskfile, 0, 2);   /* move to end of the existing file */
            *filesize = ftell(*diskfile);  /* position = size of file */
            fseek(*diskfile, 0, 0);   /* move back to beginning of file */
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffisfilecompressed(char *filename, /* I - FITS file name          */
            FILE **diskfile, /* O - file pointer                            */
            size_t *filesize, /* O - size of file (bytes)                   */
            size_t *finalsize, /* O - size of uncompressed file             */
            int *status)     /* O - error status: 0 = success, 1 = error    */
/*
  Test if the file is compressed, and if so, return the file pointer
  and size of the compressed file.
*/
{
    void *ptr;
    unsigned char buffer[4];
    char tmpfilename[160];

    if (*status > 0)
       return(0);

    strcpy(tmpfilename,filename);

    /* Open file.  Try various suffix combinations */  
    if (ffopenfile(filename, 0, 0, diskfile, filesize, status) > 0)
    {
      *status = 0;
      strcat(filename,".gz");
      if (ffopenfile(filename, 0, 0, diskfile, filesize, status) > 0)
      {
        *status = 0;
        strcpy(filename, tmpfilename);
        strcat(filename,".Z");
        if (ffopenfile(filename, 0, 0, diskfile, filesize, status) > 0)
        {
          *status = 0;
          strcpy(filename, tmpfilename);
          strcat(filename,".z");   /* it's often lower case on CDROMs */
          if (ffopenfile(filename, 0, 0, diskfile, filesize, status) > 0)
          {
            *status = 0;
            strcpy(filename, tmpfilename);
            strcat(filename,".zip");
            if (ffopenfile(filename, 0, 0, diskfile, filesize, status) > 0)
            {
             *status = 0;
             strcpy(filename, tmpfilename);
             strcat(filename,"-z");      /* VMS suffix */
             if (ffopenfile(filename, 0, 0, diskfile, filesize, status) > 0)
             {
              *status = 0;
              strcpy(filename, tmpfilename);
              strcat(filename,"-gz");    /* VMS suffix */
              if (ffopenfile(filename, 0, 0, diskfile, filesize, status) > 0)
              {
                *status = 0;
                strcpy(filename,tmpfilename);  /* restore original name */
                ffclosefile(filename, *diskfile, 1, status);
                return(0);    /* file not found */
              }
             }
            }
          }
        }
      }
    }

    if(fread(buffer, 1, 2, *diskfile) != 2)  /* read 2 bytes */
    {
        ffclosefile(filename, *diskfile, 1, status);
        return(0);
    }

    if (memcmp(buffer, "\037\213", 2) == 0)  /* GZIP */
    {
        /* the uncompressed file size is give at the end of the file */

        fseek(*diskfile, 0, 2);            /* move to end of file */
        fseek(*diskfile, -4L, 1);          /* move back 4 bytes */
        fread(buffer, 1, 4L, *diskfile);   /* read 4 bytes

        /* have to worry about integer byte order */
	*finalsize  = buffer[0];
	*finalsize |= buffer[1] << 8;
	*finalsize |= buffer[2] << 16;
	*finalsize |= buffer[3] << 24;
    }
    else if (memcmp(buffer, "\120\113", 2) == 0)   /* PKZIP */
    {
        /* the uncompressed file size is give at byte 22 the file */

        fseek(*diskfile, 22L, 0);            /* move to byte 22 */
        fread(buffer, 1, 4L, *diskfile);   /* read 4 bytes

        /* have to worry about integer byte order */
	*finalsize  = buffer[0];
	*finalsize |= buffer[1] << 8;
	*finalsize |= buffer[2] << 16;
	*finalsize |= buffer[3] << 24;
    }
    else if (memcmp(buffer, "\037\036", 2) == 0)  /* PACK */
        *finalsize = 0;  /* for most methods we can't determine final size */
    else if (memcmp(buffer, "\037\235", 2) == 0)  /* LZW */
        *finalsize = 0;  /* for most methods we can't determine final size */
    else if (memcmp(buffer, "\037\240", 2) == 0)  /* LZH */
        *finalsize = 0;  /* for most methods we can't determine final size */
    else
    {
        /* not a compressed file */
        ffclosefile(filename, *diskfile, 1, status);
        return(0);
    }

    fseek(*diskfile, 0, 0);   /* move back to beginning of file */
    return(1);                /* this may be a compressed FITS file */
}
/*--------------------------------------------------------------------------*/
int ffclosex(fitsfile *fptr, /* I - FITS file pointer                      */
             int keep,       /* I - 0=discard the file, else keep the file */
             int *status)    /* O - error status                           */

/*
  Low-level routine to close the disk file.
  If keep = 0, then the file will also be deleted from disk.
*/
{
    *status = 0;

    if (fptr->bufftype < MEMBUFF) /* not using memory buffer? */
    {
      ffclosefile(fptr->filename, fptr->fileptr, keep, status);
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
int ffclosefile(char *filename, /* I - FITS file pointer                   */
             FILE *diskfile, /* file pointer                                */
             int keep,       /* I - 0=discard the file, else keep the file  */
             int *status)    /* O - error status                            */

/*
  Lowest-level routine to close a disk file.
  If keep = 0, then the file will also be deleted from disk.
*/
{
      if ( fclose(diskfile) )   /* close the disk file */
        *status = 1;

      if (!keep)
        remove(filename);       /* delete the file */

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
/*--------------------------------------------------------------------------*/
void ffrprt( FILE *stream, int status)
/* 
   Print out report of cfitsio error status and messages on the error stack.
*/
{
    char status_str[FLEN_STATUS], errmsg[FLEN_ERRMSG];
  
    if (status)
    {
      fprintf(stream, "\n*** Error occurred during program execution ***\n");

      fits_get_errstatus(status, status_str);  /* get the error description */
      fprintf(stream, "\nstatus = %d: %s\n", status, status_str);

      /* get first message; null if stack is empty */
      if ( fits_read_errmsg(errmsg) ) 
      {
         fprintf(stream, "\nError message stack:\n");
         fprintf(stream, " %s\n", errmsg);

         while ( fits_read_errmsg(errmsg) )  /* get remaining messages */
             fprintf(stream, " %s\n", errmsg);
      }
    }
    return; 
}
