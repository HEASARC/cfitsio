/*  This file, utilproc.c, contains general utility routines that are      */
/*  used by other FITSIO routines.                                         */

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
#include <math.h>
#include <ctype.h>
#include "fitsio2.h"

/*--------------------------------------------------------------------------*/
float ffvers(float *version)  /* IO - version number */
/*
  return the current version number of the FITSIO software
*/
{
    *version = 0.99;  /* beta development version */

    return(*version);
}
/*--------------------------------------------------------------------------*/
void ffgerr(int status,     /* I - error status value */
            char *errtext)  /* O - error message (max 30 char long + null) */
/*
  Return a short descriptive error message that corresponds to the input
  error status value.  The message may be up to 30 characters long, plus
  the terminating null character.
*/
{
  if (status < 300)
  {
    switch (status) {

    case 0:
       strcpy(errtext, "OK - no error");
       break;
    case 101:
       strcpy(errtext, "same input and output files");
       break;
    case 104:
       strcpy(errtext, "could not open the named file");
       break;
    case 105:
       strcpy(errtext, "couldn't create the named file");
       break;
    case 106:
       strcpy(errtext, "error writing to FITS file");
       break;
    case 107:
       strcpy(errtext, "tried to move past end of file");
       break;
    case 108:
       strcpy(errtext, "error reading from FITS file");
       break;
    case 110:
       strcpy(errtext, "could not close the file");
       break;
    case 111:
       strcpy(errtext, "array dimensions too big");
       break;
    case 112:
       strcpy(errtext, "cannot write to readonly file");
       break;
    case 201:
       strcpy(errtext, "header already has keywords");
       break;
    case 202:
       strcpy(errtext, "keyword not found in header");
       break;
    case 203:
       strcpy(errtext, "keyword number out of bounds");
       break;
    case 204:
       strcpy(errtext, "keyword value field is blank");
       break;
    case 205:
       strcpy(errtext, "string missing closing quote");
       break;
    case 207:
       strcpy(errtext, "illegal character in keyword");
       break;
    case 208:
       strcpy(errtext, "required keywords out of order");
       break;
    case 209:
       strcpy(errtext, "keyword value not positive int");
       break;
    case 210:
       strcpy(errtext, "END keyword not found");
       break;
    case 211:
       strcpy(errtext, "illegal BITPIX keyword value");
       break;
    case 212:
       strcpy(errtext, "illegal NAXIS keyword value");
       break;
    case 213:
       strcpy(errtext, "illegal NAXISn keyword value");
       break;
    case 214:
       strcpy(errtext, "illegal PCOUNT keyword value");
       break;
    case 215:
       strcpy(errtext, "illegal GCOUNT keyword value");
       break;
    case 216:
       strcpy(errtext, "illegal TFIELDS keyword value");
       break;
    case 217:
       strcpy(errtext, "negative table row size");
       break;
    case 218:
       strcpy(errtext, "negative number of rows");
       break;
    case 219:
       strcpy(errtext, "named column not found");
       break;
    case 220:
       strcpy(errtext, "illegal SIMPLE keyword value");
       break;
    case 221:
       strcpy(errtext, "first keyword not SIMPLE");
       break;
    case 222:
       strcpy(errtext, "second keyword not BITPIX");
       break;
    case 223:
       strcpy(errtext, "third keyword not NAXIS");
       break;
    case 224:
       strcpy(errtext, "missing NAXISn keywords");
       break;
    case 225:
       strcpy(errtext, "first keyword not XTENSION");
       break;
    case 226:
       strcpy(errtext, "CHDU not an ASCII table");
       break;
    case 227:
       strcpy(errtext, "CHDU not a binary table");
       break;
    case 228:
       strcpy(errtext, "PCOUNT keyword not found");
       break;
    case 229:
       strcpy(errtext, "GCOUNT keyword not found");
       break;
    case 230:
       strcpy(errtext, "TFIELDS keyword not found");
       break;
    case 231:
       strcpy(errtext, "missing TBCOLn keyword");
       break;
    case 232:
       strcpy(errtext, "missing TFORMn keyword");
       break;
    case 233:
       strcpy(errtext, "CHDU not an IMAGE extension");
       break;
    case 234:
       strcpy(errtext, "illegal TBCOLn keyword value");
       break;
    case 235:
       strcpy(errtext, "CHDU not a table extension");
       break;
    case 236:
       strcpy(errtext, "column exceeds width of table");
       break;
    case 237:
       strcpy(errtext, "more than 1 matching col. name");
       break;
    case 241:
       strcpy(errtext, "row width not = field widths");
       break;
    case 251:
       strcpy(errtext, "unknown FITS extension type");
       break;
    case 252:
       strcpy(errtext, "unknown FITS record type");
       break;
    case 253:
       strcpy(errtext, "END keyword is not blank");
       break;
    case 261:
       strcpy(errtext, "illegal TFORM format code");
       break;
    case 262:
       strcpy(errtext, "unknown TFORM datatype code");
       break;
    case 263:
       strcpy(errtext, "illegal TDIMn keyword value");
       break;
    }
  }
  else
  {
    switch(status) {

    case 301:
       strcpy(errtext, "illegal HDU number");
       break;
    case 302:
       strcpy(errtext, "column number < 1 or > tfields");
       break;
    case 304:
       strcpy(errtext, "negative byte address");
       break;
    case 306:
       strcpy(errtext, "negative number of elements");
       break;
    case 307:
       strcpy(errtext, "bad first row number");
       break;
    case 308:
       strcpy(errtext, "bad first element number");
       break;
    case 309:
       strcpy(errtext, "not an ASCII (A) column");
       break;
    case 310:
       strcpy(errtext, "not a logical (L) column");
       break;
    case 311:
       strcpy(errtext, "bad ASCII table datatype");
       break;
    case 312:
       strcpy(errtext, "bad binary table datatype");
       break;
    case 314:
       strcpy(errtext, "null value not defined");
       break;
    case 317:
       strcpy(errtext, "not a variable length column");
       break;
    case 320:
       strcpy(errtext, "illegal number of dimensions");
       break;
    case 322:
       strcpy(errtext, "BSCALE or TSCALn = 0.");
       break;
    case 323:
       strcpy(errtext, "illegal axis length < 1");
       break;
    case 401:
       strcpy(errtext, "bad int to string conversion");
       break;
    case 402:
       strcpy(errtext, "bad float to string conversion");
       break;
    case 403:
       strcpy(errtext, "keyword value not integer");
       break;
    case 404:
       strcpy(errtext, "keyword value not logical");
       break;
    case 405:
       strcpy(errtext, "keyword value not floating pt");
       break;
    case 406:
       strcpy(errtext, "keyword value not double");
       break;
    case 407:
       strcpy(errtext, "bad string to int conversion");
       break;
    case 408:
       strcpy(errtext, "bad string to float conversion");
       break;
    case 409:
       strcpy(errtext, "bad string to double convert");
       break;
    case 410:
       strcpy(errtext, "illegal datatype code value");
       break;
    case 411:
       strcpy(errtext, "illegal no. of decimals");
       break;
    case 412:
       strcpy(errtext, "datatype conversion overflow");
       break;
    }
  }
  return;
}
/*--------------------------------------------------------------------------*/
int fftkey(char *keyword,    /* I -  keyword name */
           int *status)      /* IO - error status */
/*
  Test that the keyword name conforms to the FITS standard.  Must contain
  only capital letters, digits, minus or underscore chars.  Trailing spaces
  are allowed.
*/
{
    size_t maxchr, ii;
    int spaces=0;
    char msg[81];

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    maxchr=strlen(keyword);
    if (maxchr > 8)
        maxchr = 8;

    for (ii = 0; ii < maxchr; ii++)
    {
        if ( (keyword[ii] >= 'A' && keyword[ii] <= 'Z') ||
             (keyword[ii] >= '0' && keyword[ii] <= '9') ||
              keyword[ii] == '-' || keyword[ii] == '_'   )
              {
                if (spaces)
                {
                 sprintf(msg, "Keyword name contains embedded space(s): %.8s",
                        keyword);
                    ffpmsg(msg);
                    return(*status = BAD_KEYCHAR);        
                }
              }
        else if (keyword[ii] == ' ')
            spaces = 1;

        else     
        {
            sprintf(msg, "Character %d in this keyword is illegal: %.8s",
                    ii+1, keyword);
            ffpmsg(msg);

            /* explicitly flag the 2 most common cases */
            if (keyword[ii] == 0) 
                ffpmsg(" (This a NULL (0) character).");                
            else if (keyword[ii] == 9)
                ffpmsg(" (This an ASCII TAB (9) character).");                

            return(*status = BAD_KEYCHAR);        

        }                
    }
    return(*status);        
}
/*--------------------------------------------------------------------------*/
int fftrec(char *card,       /* I -  keyword card to test */
           int *status)      /* IO - error status */
/*
  Test that the keyword card conforms to the FITS standard.  Must contain
  only printable ASCII characters;
*/
{
    size_t ii, maxchr;
    char msg[81];

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    maxchr = strlen(card);

    for (ii = 8; ii < maxchr; ii++)
    {
        if (!isprint(card[ii]))
        {
            sprintf(msg, "Character %d in this keyword record is illegal:",
              (int) (ii+1) );
            ffpmsg(msg);

            strncpy(msg, card, 80);
            msg[80] = '\0';
            ffpmsg(msg);
            return(*status = BAD_KEYCHAR);        
        }
    }
    return(*status);        
}
/*--------------------------------------------------------------------------*/
void ffupch(char *string)
/*
  convert string to upper case, in place.
*/
{
    size_t len, ii;

    len = strlen(string);
    for (ii = 0; ii < len; ii++)
        string[ii] = toupper(string[ii]);
    return;
}
/*--------------------------------------------------------------------------*/
void ffmkky(char *keyname,   /* I - keyword name    */
            char *value,     /* I - keyword value   */
            char *comm,      /* I - keyword comment */
            char *card)      /* O - constructed keyword card */
/*
  Make a complete FITS 80-byte keyword card from the input name, value and
  comment strings. Output card is null terminated without any trailing blanks.
*/
{
    size_t len, ii;

    strncpy(card, keyname, 8);   /* copy keyword name to buffer */
    card[8] = '\0'; 
   
    len = strlen(card);
    for (ii = len; ii < 8; ii++)
        card[ii] = ' ';          /* pad keyword name with spaces */

    card[8] = '=';              /* append '= ' in columns 9-10 */
    card[9] = ' ';
    card[10] = '\0';                 /* terminate the partial string */

    len = strlen(value);        
    if (len > 0)
    {
        if (value[0] == '\'')  /* is this a quoted string value? */
        {
            strncat(card, value, 70);       /* append the value string */
            len = strlen(card);

            if (comm[0] != 0)
            {
                for (ii = len; ii < 30; ii++)
                  strcat(card, " "); /* add spaces so field ends in col 30 */
            }
        }
        else
        {
            for (ii = len; ii < 20; ii++)
                strcat(card, " ");  /* add spaces so field ends in col 30 */

            strncat(card, value, 70);       /* append the value string */
        }

        len = strlen(card);
        if ((len < 77) && ( strlen(comm) > 0) )  /* room for a comment? */
        {
            strcat(card, " / ");   /* append comment separator */
            strncat(card, comm, 77 - len); /* append comment (what fits) */
        } 
    }
    else
    {
        card[8] = ' ';   /* keywords with no value have no equal sign */ 
        strncat(card, comm, 70);   /* append comment (whatever fits) */
    }
}
/*--------------------------------------------------------------------------*/
int ffkeyn(char *keyroot,   /* I - root string for keyword name */
           int value,       /* I - index number to be appended to root name */
           char *keyname,   /* O - output root + index keyword name */
           int *status)     /* IO - error status  */
/*
  Construct a keyword name string by appending the index number to the root.
  e.g., if root = "TTYPE" and value = 12 then keyname = "TTYPE12".
*/
{
    char suffix[4];
    size_t rootlen;

    keyname[0] = '\0';            /* initialize output name to null */
    rootlen = strlen(keyroot);

    if (rootlen == 0 || rootlen > 7 || value < 1 || value > 999)
       return(*status = 206);

    sprintf(suffix, "%d", value); /* construct keyword suffix */

    if ( strlen(suffix) + rootlen > 8)
       return(*status = 206);

    strcpy(keyname, keyroot);   /* copy root string to name string */
    strcat(keyname, suffix);    /* append suffix to the root */
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffnkey(int value,       /* I - index number to be appended to root name */
           char *keyroot,   /* I - root string for keyword name */
           char *keyname,   /* O - output root + index keyword name */
           int *status)     /* IO - error status  */
/*
  Construct a keyword name string by appending the root string to the index
  number. e.g., if root = "TTYPE" and value = 12 then keyname = "12TTYPE".
*/
{
    size_t rootlen;

    keyname[0] = '\0';            /* initialize output name to null */
    rootlen = strlen(keyroot);

    if (rootlen == 0 || rootlen > 7 || value < 1 || value > 999)
       return(*status = 206);

    sprintf(keyname, "%d", value); /* construct keyword prefix */

    if (rootlen +  strlen(keyname) > 8)
       return(*status = 206);

    strcat(keyname, keyroot);  /* append root to the prefix */
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpsvc(char *card,    /* I - FITS header card (nominally 80 bytes long) */
           char *value,   /* O - value string parsed from the card */
           char *comm,    /* O - comment string parsed from the card */
           int *status)   /* IO - error status   */
/*
  ParSe the Value and Comment strings from the input header card string.
  If the card contains a quoted string value, the returned value string
  includes the enclosing quote characters.  
*/
{
    int jj;
    size_t ii, cardlen, nblank;
    char errmsg[FLEN_ERRMSG];

    if (*status > 0)
        return(*status);

    value[0] = '\0';
    comm[0] = '\0';

    cardlen = strlen(card);
    
    if (cardlen < 9  ||
        strncmp(card, "COMMENT ", 8) == 0 ||  /* keywords with no value */
        strncmp(card, "HISTORY ", 8) == 0 ||
        strncmp(card, "END     ", 8) == 0 ||
        strncmp(card, "        ", 8) == 0 ||
        strncmp(&card[8],      "= ", 2) != 0  ) /* no '= ' in cols 9-10 */
    {
        /*  no value and the comment extends from cols 9 - 80  */
        if (cardlen > 8)
            strcpy(comm, &card[8]);

        jj=strlen(comm);
        for (jj--; jj >= 0; jj--)  /* replace trailing blanks with nulls */
        {
            if (comm[jj] == ' ')
                comm[jj] = '\0';
            else
                break;
        }
        return(*status);
    }

    nblank = strspn(&card[10], " ");  /*  find number of leading blanks */

    if (nblank + 10 == cardlen)
    {
        strcpy(errmsg,"The keyword ");
        strncat(errmsg, card, 8);
        strcat(errmsg, " has no value string after the equal sign:");
        ffpmsg(errmsg);
        ffpmsg(card);
        return(*status = NO_VALUE);
    }

    ii = nblank + 10;
    if (card[ii] == '\'' )  /* is this a quoted string value? */
    {
        value[0] = card[ii];
        for (jj=1, ii++; ii < cardlen; ii++, jj++)
        {
            if (card[ii] == '\'')  /*  is this the closing quote?  */
            {
                if (card[ii+1] == '\'')  /* 2 successive quotes? */ 
                {
                   value[jj] = card[ii];
                   ii++;  
                   jj++;
                }
                else
                {
                    value[jj] = card[ii];
                    break;   /* found the closing quote, so exit this loop  */
                }
            }
            value[jj] = card[ii];  /* copy the next character to the output */
        }

        if (ii == cardlen)
        {
            value[jj] = '\0';  /*  terminate the bad value string  */
            ffpmsg("This keyword string value has no closing quote:");
            ffpmsg(card);
            return(*status = 205);
        }
        else
        {
            value[jj+1] = '\0';  /*  terminate the good value string  */
            ii++;   /*  point to the character following the value  */
        }
    }

    else   /*  an integer, floating point, or logical FITS value string  */
    {
        nblank = strcspn(&card[ii], " /");  /* find the end of the token */
        strncpy(value, &card[ii], nblank);
        value[nblank] = '\0';
        ii = ii + nblank;
    }

    /*  now find the comment string, if any  */
    comm[0] = '\0';

    nblank = strspn(&card[ii], " ");  /*  find next non-space character  */
    ii = ii + nblank;

    if (ii < 80)
    {
        if (card[ii] == '/')   /*  ignore the slash separator  */
        {
            ii++;
            if (card[ii] == ' ')  /*  also ignore the following space  */
                ii++;
        }
        strcat(comm, &card[ii]);  /*  copy the remaining characters  */

        jj=strlen(comm);
        for (jj--; jj >= 0; jj--)  /* replace trailing blanks with nulls */
        {
            if (comm[jj] == ' ')
                comm[jj] = '\0';
            else
                break;
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgthd(char *tmplt, /* I - input header template string */
           char *card,  /* O - returned FITS header record */
           int *hdtype, /* O - how to interpreter the returned card string */ 
            /*
              -2 = modify the name of a keyword; the old keyword name
                   is returned starting at address chars[0]; the new name
                   is returned starting at address char[40] (to be consistent
                   with the Fortran version).  Both names are null terminated. 
              -1 = card contains the name of a keyword that is to be deleted
               0 = append this keyword if it doesn't already exist, or 
                   modify the value if the keyword already exists.
               1 = append this comment keyword ('HISTORY', 
                   'COMMENT', or blank keyword name) 
               2  =  this is the END keyword; do not write it to the header
            */
           int *status)   /* IO - error status   */
/*
  'Get Template HeaDer'
  parse a template header line and create a formated
  character string which is suitable for appending to a FITS header 
*/
{
    char keyname[FLEN_KEYWORD], value[FLEN_VALUE], comment[FLEN_COMMENT];
    char *tok, *suffix;
    int len, vlen, more;
    double dval;

    if (*status > 0)
        return(*status);

    card[0]   = '\0';
    *hdtype   = 0;

    if (!strncmp(tmplt, "        ", 8) )
    {
        /* if first 8 chars of template are blank, then this is a comment */
        strncat(card, tmplt, 80);
        *hdtype = 1;
        return(*status);
    }

    tok = tmplt;   /* point to start of template string */
 
    keyname[0] = '\0';
    value[0]   = '\0';
    comment[0] = '\0';

    len = strspn(tok, " ");  /* no. of spaces before keyword */
    tok += len;

    if (tok[0] == '-')  /* is there a leading minus sign? */
    {
        /* first token is name of keyword to be deleted or renamed */
        *hdtype = -1;
        tok++;
        len = strspn(tok, " ");  /* no. of spaces before keyword */
        tok += len;
        if (len < 8)  /* not a blank name? */
        {
          len = strcspn(tok, " =");  /* length of name */
          if (len > 8)
            return(*status = BAD_KEYCHAR);

          strncat(card, tok, len);
          ffupch(card);
          if (fftkey(card, status) > 0)
              return(*status);      /* illegal chars in name */

          tok += len;
        }

        /* second token, if present, is the new name for the keyword */

        len = strspn(tok, " ");  /* no. of spaces before next token */
        tok += len;

        if (tok[0] == '\0' || tok[0] == '=')
            return(*status);  /* no second token */

        *hdtype = -2;
        len = strcspn(tok, " ");  /* length of new name */
        if (len > 8)
          return(*status = BAD_KEYCHAR);

        /* copy the new name to card + 40;  This is awkward, */
        /* but is consistent with the way the Fortran FITSIO works */
        strncpy(&card[40], tok, len);
        ffupch(&card[40]);
        fftkey(&card[40], status);
    }
    else  /* no negative sign at beginning of template */
    {
      /* get the keyword name token */

      len = strcspn(tok, " =");  /* length of keyword name */
      if (len > 8)
        return(*status = BAD_KEYCHAR);

      strncat(keyname, tok, len);
      ffupch(keyname);
      if (fftkey(keyname, status) > 0)
          return(*status);

      if (!strcmp(keyname, "END") )
      {
         strcpy(card, "END");
         *hdtype = 2;
         return(*status);
      }

      tok += len;
      len = strspn(tok, " =");  /* space between name and value */
      
      if (strcmp(keyname, "COMMENT") && strcmp(keyname, "HISTORY") )
      {
        /* Get the Value token */
        tok += len;

        if (*tok == '\'') /* is value enclosed in quotes? */
        {
          more = TRUE;
          while (more)
          {
            tok++;  /* temporarily move past the quote char */
            len = strcspn(tok, "'");  /* length of quoted string */
            tok--;
            strncat(value, tok, len + 2); 
 
            tok += len + 1;
            if (tok[0] != '\'')   /* check there is a closing quote */
              return(*status = NO_QUOTE);

            tok++;
            if (tok[0] != '\'')  /* 2 quote chars = literal quote */
              more = FALSE;
          }
        }
        else   /* not a quoted string value */
        {
          len = strcspn(tok, " /"); /* length of value string */

          strncat(value, tok, len);
          if (tok[0] != 'T' && tok[0] != 'F') /* not a logical value */
          {
            dval = strtod(value, &suffix); /* try to read value as number */

            if (*suffix != '\0' && *suffix != ' ' && *suffix != '/')
            { 
              /* value is not a number; must enclose it in quotes */
              strcpy(value, "'");
              strncat(value, tok, len);
              strcat(value, "'");

              /* the following useless statement stops the compiler warning */
              /* that dval is not used anywhere */
              if (dval == 0.)
                 len += (int) dval; 
            }
          }
          tok += len;
        }

        len = strspn(tok, " /"); /* no. of spaces between value and comment */
        tok += len;
      }
      else
      {
        *hdtype = 1;   /* simply append COMMENT and HISTORY keywords */
        strcpy(card, keyname);
        strncat(card, tok, 73);
        return(*status);
      }

      vlen = strlen(value);
      if (vlen > 0 && vlen < 10 && value[0] == '\'')
      {
          /* pad quoted string with blanks so it is at least 8 chars long */
          value[vlen-1] = '\0';
          strncat(value, "        ", 10 - vlen);
          strcat(&value[9], "'");
      }

      /* get the comment string */
      strncpy(comment, tok, 70);

      /* construct the complete FITS header card */
      ffmkky(keyname, value, comment, card);
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffasfm(char *tform,    /* I - format code from the TFORMn keyword */
           int *datacode,  /* O - numerical datatype code */
           long *width,    /* O - width of the field, in chars */
           int *decimals,  /* O - number of decimal places (F, E, D format) */
           int *status)    /* IO - error status      */
{
/*
  parse the ASCII table TFORM column format to determine the data
  type, the field width, and number of decimal places (if relevant)
*/
    int ii;
    long longval;
    float fwidth;
    char *form, temp[FLEN_VALUE], message[FLEN_ERRMSG];

    if (*status > 0)
        return(*status);

    *datacode = 0;
    *width = 0;
    *decimals = 0;

    ii = 0;
    while (tform[ii] != 0 && tform[ii] == ' ') /* find first non-blank char */
         ii++;

    strcpy(temp, &tform[ii]); /* copy format string */
    ffupch(temp);     /* make sure it is in upper case */
    form = temp;      /* point to start of format string */


    if (form[0] == 0)
    {
        ffpmsg("Error: ASCII table TFORM code is blank");
        return(*status = BAD_TFORM);
    }

    /*-----------------------------------------------*/
    /*       determine default datatype code         */
    /*-----------------------------------------------*/
    if (form[0] == 'A')
        *datacode = TSTRING;
    else if (form[0] == 'I')
        *datacode = TLONG;
    else if (form[0] == 'F')
        *datacode = TFLOAT;
    else if (form[0] == 'E')
        *datacode = TFLOAT;
    else if (form[0] == 'D')
        *datacode = TDOUBLE;
    else
    {
        sprintf(message,
        "Illegal ASCII table TFORMn datatype: \'%s\'", tform);
        ffpmsg(message);
        return(*status = BAD_TFORM_DTYPE);
    }

    form++;  /* point to the start of field width */

    if (*datacode == TSTRING || *datacode == TLONG)
    { 
        /*-----------------------------------------------*/
        /*              A or I data formats:             */
        /*-----------------------------------------------*/

        if (ffc2ii(form, width, status) <= 0)  /* read the width field */
        {
            if (*width <= 0)
            {
                *width = 0;
                *status = BAD_TFORM;
            }
            else
            {                
                /* set to shorter precision if I4 or less */
                if (*width <= 4 && *datacode == TLONG)
                    *datacode = TSHORT;
            }
        }
    }
    else
    {  
        /*-----------------------------------------------*/
        /*              F, E or D data formats:          */
        /*-----------------------------------------------*/

        if (ffc2rr(form, &fwidth, status) <= 0) /* read ww.dd width field */
        {
          if (fwidth <= 0.)
            *status = BAD_TFORM;
          else
          {
            *width = (long) fwidth;  /* convert from float to long */

            if (*width > 7 && *(form-1) == 'F')
                *datacode = TDOUBLE;  /* type double if >7 digits */

            if (*width < 10)
                form = form + 1; /* skip 1 digit  */
            else
                form = form + 2; /* skip 2 digits */

            if (form[0] == '.') /* should be a decimal point here */
            {
                form++;  /*  point to start of decimals field */

                if (ffc2ii(form, &longval, status) <= 0) /* read decimals */
                {
                    *decimals = longval;  /* long to short convertion */

                    if (*decimals >= *width)  /* width < no. of decimals */
                        *status = BAD_TFORM; 
                }
            }
          }
        }
    }

    if (*status > 0)
    {
        *status = BAD_TFORM;
        sprintf(message,"Illegal ASCII table TFORMn code: \'%s\'", tform);
        ffpmsg(message);
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffbnfm(char *tform,     /* I - format code from the TFORMn keyword */
           int *datacode,   /* O - numerical datatype code */
           long *repeat,    /* O - repeat count of the field  */
           long *width,     /* O - width of the field, in chars */
           int *status)     /* IO - error status      */
{
/*
  parse the binary table TFORM column format to determine the data
  type, repeat count, and the field width (if it is an ASCII (A) field)
*/
    size_t ii, nchar;
    int variable, iread;
    char *form, temp[FLEN_VALUE], message[FLEN_ERRMSG];

    if (*status > 0)
        return(*status);

    *datacode = 0;
    *repeat = 0;
    *width = 0;

    nchar = strlen(tform);

    for (ii = 0; ii < nchar; ii++)
    {
        if (tform[ii] != ' ')     /* find first non-space char */
            break;
    }

    if (ii == nchar)
    {
        ffpmsg("Error: binary table TFORM code is blank (ffbnfm).");
        return(*status = BAD_TFORM);
    }

    strcpy(temp, &tform[ii]); /* copy format string */
    ffupch(temp);     /* make sure it is in upper case */
    form = temp;      /* point to start of format string */

    /*-----------------------------------------------*/
    /*       get the repeat count                    */
    /*-----------------------------------------------*/

    ii = 0;
    while(isdigit(form[ii]))
        ii++;   /* look for leading digits in the field */

    if (ii == 0)
        *repeat = 1;  /* no explicit repeat count */
    else
        sscanf(form,"%ld", repeat);  /* read repeat count */

    /*-----------------------------------------------*/
    /*             determine datatype code           */
    /*-----------------------------------------------*/

    form = form + ii;  /* skip over the repeat field */

    if (form[0] == 'P')
    {
        variable = 1;  /* this is a variable length column */
        *repeat = 1;   /* disregard any other repeat value */
        form++;        /* move to the next data type code char */
    }
    else
        variable = 0;

    if      (form[0] == 'I')
    {
        *datacode = TSHORT;
        *width = 2;
    }
    else if (form[0] == 'J')
    {
        *datacode = TLONG;
        *width = 4;
    }
    else if (form[0] == 'E')
    {
        *datacode = TFLOAT;
        *width = 4;
    }
    else if (form[0] == 'D')
    {
        *datacode = TDOUBLE;
        *width = 8;
    }
    else if (form[0] == 'A')
    {
        *datacode = TSTRING;

        /*
          the following code is used to support the non-standard
          datatype of the form rAw where r = total width of the field
          and w = width of fixed-length substrings within the field.
        */
        iread = 0;
        if (form[1] != 0)
            iread = sscanf(&form[1],"%ld", width);

        if (iread != 1)
            *width = *repeat;  
    }
    else if (form[0] == 'L')
    {
        *datacode = TLOGICAL;
        *width = 1;
    }
    else if (form[0] == 'X')
    {
        *datacode = TBIT;
        *width = 1;
    }
    else if (form[0] == 'B')
    {
        *datacode = TBYTE;
        *width = 1;
    }
    else if (form[0] == 'C')
    {
        *datacode = TCOMPLEX;
        *width = 8;
    }
    else if (form[0] == 'M')
    {
        *datacode = TDBLCOMPLEX;
        *width = 16;
    }
    else
    {
        sprintf(message,
        "Illegal binary table TFORMn datatype: \'%s\' ", tform);
        ffpmsg(message);
        return(*status = BAD_TFORM_DTYPE);
    }

    if (variable)
    {
        *datacode = *datacode * (-1); /* flag variable cols w/ neg type code */
        *width = 8;
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
void ffcfmt(char *tform,    /* value of an ASCII table TFORMn keyword */
            char *cform)    /* equivalent format code in C language syntax */
/*
  convert the FITS format string for an ASCII Table extension column into the
  equivalent C format string that can be used in a printf statement.
*/
{
    int ii;

    cform[0] = '\0';
    ii = 0;
    while (tform[ii] != 0 && tform[ii] == ' ') /* find first non-blank char */
         ii++;

    if (tform[ii] == 0)
        return;    /* input format string was blank */

    cform[0] = '%';  /* start the format string */

    strcpy(&cform[1], &tform[ii + 1]); /* append the width and decimal code */


    if (tform[ii] == 'A')
        strcat(cform, "s");
    else if (tform[ii] == 'I')
        strcat(cform, ".0lf");  /*  0 precision to suppress decimal point */
    if (tform[ii] == 'F')
        strcat(cform, "lf");
    if (tform[ii] == 'E')
        strcat(cform, "lE");
    if (tform[ii] == 'D')
        strcat(cform, "lE");

    return;
}
/*--------------------------------------------------------------------------*/
void ffswap2(short *svalues,  /* IO - pointer to shorts to be swapped       */
             long nvals)      /* I  - number of shorts to be swapped        */
/*
  swap the bytes in the input short integers: ( 0 1 -> 1 0 )
*/
{
    register char *cvalues, *cval0, *cval1;
    short sval;
    register long ii;

    cval0 = (char *) &sval;  /* pointer to 1st byte of 2-byte short */
    cval1 = cval0 + 1;       /* pointer to 2nd byte of 2-byte short */

    cvalues = (char *) svalues;      /* copy the initial pointer value */

    for (ii = 0; ii < nvals;)
    {
        sval = svalues[ii++];  /* copy next short to temporary buffer */

        *cvalues++ = *cval1; /* copy the 2 bytes to output in reverse order */
        *cvalues++ = *cval0;
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
    register char *cvalues, *cval0, *cval1, *cval2, *cval3;
    register long ii;
    long lval;

    cval0 = (char *) &lval;  /* pointer to 1st byte of 4-byte long */
    cval1 = cval0 + 1;       /* pointer to 2nd byte of 4-byte long */
    cval2 = cval1 + 1;       /* pointer to 3rd byte of 4-byte long */
    cval3 = cval2 + 1;       /* pointer to 4th byte of 4-byte long */

    cvalues = (char *) lvalues;    /* copy the initial pointer value */

    for (ii = 0; ii < nvals;)
    {
        lval = lvalues[ii++];  /* copy next long to temporary buffer */

        *cvalues++ = *cval3; /* copy the 4 bytes to output in reverse order */
        *cvalues++ = *cval2;
        *cvalues++ = *cval1;
        *cvalues++ = *cval0; 
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
    register unsigned char *cvalues, *cval0, *cval1, *cval2, *cval3;
    register long ii;
    long lval;

    cval0 = (unsigned char *) &lval; /* pointer to 1st byte of 4-byte long */
    cval1 = cval0 + 1;               /* pointer to 2nd byte of 4-byte long */
    cval2 = cval1 + 1;               /* pointer to 3rd byte of 4-byte long */
    cval3 = cval2 + 1;               /* pointer to 4th byte of 4-byte long */

    /* set pointer to last byte of last long to be swapped */
    cvalues = (unsigned char *) lvalues + (nvals * 4) - 1;  

    for (ii = nvals - 1; ii >= 0;)
    {

#if MACHINE == ALPHA_OSF

        /* if the 4-byte long is stored in an 8 byte word, we must */
        /* initialize the word with the sign bit which is in the   */
        /* most sig. byte, i.e. at location *(cvalues-3)           */
        if ( *(cvalues-3) >  127)
            lval = -1; 
        else
            lval = 0;

#endif

        *cval0 =  *cvalues--;  /* copy the 4 bytes in turn into buffer */
        *cval1 =  *cvalues--;
        *cval2 =  *cvalues--;
        *cval3 =  *cvalues--;

        lvalues[ii--] = lval; /* copy swapped value back to the long array */
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
  will require separate ffswapfloat and ffunswapfloat routines, similar
  to the ffswaplong and ffunswaplong routines.
*/
{
    register char *cvalues, *cval0, *cval1, *cval2, *cval3;
    register long ii;
    float fval;

    cval0 = (char *) &fval;  /* pointer to 1st byte of 4-byte float */
    cval1 = cval0 + 1;       /* pointer to 2nd byte of 4-byte float */
    cval2 = cval1 + 1;       /* pointer to 3rd byte of 4-byte float */
    cval3 = cval2 + 1;       /* pointer to 4th byte of 4-byte float */

    cvalues = (char *) fvalues;   /* copy the initial pointer value */

    for (ii = 0; ii < nvals;)
    {
        fval = fvalues[ii++];  /* copy next float to temporary buffer */

        *cvalues++ = *cval3; /* copy the 4 bytes to output in reverse order */
        *cvalues++ = *cval2;
        *cvalues++ = *cval1;
        *cvalues++ = *cval0; 
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
    char *cvalues;
    long j1, ii;

    union u_tag {
        char cvals[8];   /* equivalence an array of 4 bytes with */
        double dval;      /* a float */
    } u;

    cvalues = (char *) dvalues;      /* copy the pointer value */

    for (j1 = 0, ii = 0; ii < nvals; ii++)
    {
        u.dval = dvalues[ii];  /* copy next long to equivalenced buffer */

        cvalues[j1++] = u.cvals[7];   /* copy the 8 bytes in turn */
        cvalues[j1++] = u.cvals[6];
        cvalues[j1++] = u.cvals[5];
        cvalues[j1++] = u.cvals[4];
        cvalues[j1++] = u.cvals[3];
        cvalues[j1++] = u.cvals[2];
        cvalues[j1++] = u.cvals[1];
        cvalues[j1++] = u.cvals[0];
    }
    return;
}
/*--------------------------------------------------------------------------*/
int ffi2c(long ival,   /* I - value to be converted to a string */
          char *cval,  /* O - character string representation of the value */
          int *status) /* IO - error status */
/*
  convert  value to a null-terminated formatted string.
*/
{
    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    cval[0] = '\0';

    if (sprintf(cval, "%ld", ival) < 0)
    {
        ffpmsg("Error in ffi2c converting integer to string");
        *status = BAD_I2C;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffl2c(int lval,    /* I - value to be converted to a string */
          char *cval,  /* O - character string representation of the value */
          int *status) /* IO - error status ) */
/*
  convert logical value to a null-terminated formatted string.  If the
  input value == 0, then the output character is the letter F, else
  the output character is the letter T.  The output string is null terminated.
*/
{
    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    if (lval)
        strcpy(cval,"T");
    else
        strcpy(cval,"F");

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffs2c(char *instr,   /* I - null terminated input string  */
          char *outstr,  /* O - null terminated quoted output string */
          int *status)   /* IO - error status */
/*
  convert an input string to a quoted string. Leading spaces 
  are significant.  FITS string keyword values must be at least 
  8 chars long so pad out string with spaces if necessary.
      Example:   km/s ==> 'km/s    '
  Single quote characters in the input string will be replace by
  two single quote characters. e.g., o'brian ==> 'o''brian'
*/
{
    size_t len, ii, jj;

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    outstr[0] = '\'';      /* start output string with a quote */

    len = strlen(instr);
    if (len > 68)
        len = 68;    /* limit input string to 68 chars */

    for (ii=0, jj=1; ii < len && jj < 69; ii++, jj++)
    {
        outstr[jj] = instr[ii];  /* copy each char from input to output */
        if (instr[ii] == '\'')
        {
            jj++;
            outstr[jj]='\'';   /* duplicate any apostrophies in the input */
        }
    }

    for (; jj < 9; jj++)       /* pad string so it is at least 8 chars long */
        outstr[jj] = ' ';

    if (jj == 70)   /* only occurs if the last char of string was a quote */
        outstr[69] = '\0';
    else
    {
        outstr[jj] = '\'';         /* append closing quote character */
        outstr[jj+1] = '\0';          /* terminate the string */
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffr2f(float fval,   /* I - value to be converted to a string */
          int  decim,   /* I - number of decimal places to display */
          char *cval,   /* O - character string representation of the value */
          int  *status) /* IO - error status */
/*
  convert float value to a null-terminated F format string
*/
{
    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    cval[0] = '\0';

    if (decim < 0)
    {
        ffpmsg("Error in ffr2f:  no. of decimal places < 0");
        return(*status = BAD_DECIM);
    }

    if (sprintf(cval, "%.*f", decim, fval) < 0)
    {
        ffpmsg("Error in ffr2f converting float to string");
        *status = BAD_F2C;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffr2e(float fval,  /* I - value to be converted to a string */
         int decim,    /* I - number of decimal places to display */
         char *cval,   /* O - character string representation of the value */
         int *status)  /* IO - error status */
/*
  convert float value to a null-terminated exponential format string
*/
{
    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    cval[0] = '\0';

    if (decim < 0)
    {
        ffpmsg("Error in ffr2e:  no. of decimal places < 0");
        return(*status = BAD_DECIM);
    }

    if (sprintf(cval, "%.*E", decim, fval) < 0)
    {
        ffpmsg("Error in ffr2e converting float to string");
        *status = BAD_F2C;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffd2f(double dval,  /* I - value to be converted to a string */
          int decim,    /* I - number of decimal places to display */
          char *cval,   /* O - character string representation of the value */
          int *status)  /* IO - error status */
/*
  convert double value to a null-terminated F format string
*/
{
    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    cval[0] = '\0';

    if (decim < 0)
    {
        ffpmsg("Error in ffd2f:  no. of decimal places < 0");
        return(*status = BAD_DECIM);
    }

    if (sprintf(cval, "%.*f", decim, dval) < 0)
    {
        ffpmsg("Error in ffd2f converting double to string");
        *status = BAD_F2C;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffd2e(double dval,  /* I - value to be converted to a string */
          int decim,    /* I - number of decimal places to display */
          char *cval,   /* O - character string representation of the value */
          int *status)  /* IO - error status */
/*
  convert double value to a null-terminated exponential format string.
*/
{
    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    cval[0] = '\0';

    if (decim < 0)
    {
        ffpmsg("Error in ffd2e:  no. of decimal places < 0");
        return(*status = BAD_DECIM);
    }

    if (sprintf(cval, "%.*E", decim, dval) < 0)
    {
        ffpmsg("Error in ffd2e converting double to string");
        *status = BAD_F2C;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffc2x(char *cval,   /* I - formatted string representation of the value */
          char *dtype,  /* O - datatype code: C, L, F or I  */

    /* Only one of the following will be defined, depending on datatype */
          long *ival,    /* O - integer value       */
          int *lval,     /* O - logical value       */
          char *sval,    /* O - string value        */
          double *dval,  /* O - double value        */

          int *status)   /* IO - error status */
/*
  high level routine to convert formatted character string to its
  intrinsic data type
*/
{
    ffdtyp(cval, dtype, status);     /* determine the datatype */

    if (*dtype == 'I')
        ffc2ii(cval, ival, status);
    else if (*dtype == 'F')
        ffc2dd(cval, dval, status);
    else if (*dtype == 'L')
        ffc2ll(cval, lval, status);
    else 
        ffc2s(cval, sval, status);
        
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffdtyp(char *cval,  /* I - formatted string representation of the value */
           char *dtype, /* O - datatype code: C, L, F or I */
          int *status)  /* IO - error status */
/*
  determine implicit datatype of input string.
  This assumes that the string conforms to the FITS standard
  for keyword values, so may not detect all invalid formats.
*/
{

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    if (cval[0] == '\'')
        *dtype = 'C';          /* character string starts with a quote */
    else if (cval[0] == 'T' || cval[0] == 'F')
        *dtype = 'L';          /* logical = T or F character */
    else if (strchr(cval,'.'))
        *dtype = 'F';          /* float contains a decimal point */
    else
        *dtype = 'I';          /* if none of the above must be an integer */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffc2i(char *cval,   /* I - string representation of the value */
          long *ival,   /* O - numerical value of the input string */
          int *status)  /* IO - error status */
/*
  convert formatted string to an integer value, doing implicit
  datatype conversion if necessary.
*/
{
    char dtype, sval[81], msg[81];
    int lval;
    double dval;
    
    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    /* convert the keyword to its native datatype */
    ffc2x(cval, &dtype, ival, &lval, sval, &dval, status);

    if (dtype == 'C')
        *status = BAD_INTKEY;

    if (*status > 0)
    {
            *ival = 0;
            strcpy(msg,"Error in ffc2i evaluating string as an integer: ");
            strncat(msg,cval,30);
            ffpmsg(msg);
            return(*status);
    }

    if (dtype == 'F')
            *ival = (long) dval;
    else if (dtype == 'L')
            *ival = (long) lval;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffc2l(char *cval,  /* I - string representation of the value */
         int *lval,    /* O - numerical value of the input string */
         int *status)  /* IO - error status */
/*
  convert formatted string to a logical value, doing implicit
  datatype conversion if necessary
*/
{
    char dtype, sval[81], msg[81];
    long ival;
    double dval;
    
    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    /* convert the keyword to its native datatype */
    ffc2x(cval, &dtype, &ival, lval, sval, &dval, status);

    if (dtype == 'C')
        *status = BAD_LOGICALKEY;

    if (*status > 0)
    {
            *lval = 0;
            strcpy(msg,"Error in ffc2l evaluating string as a logical: ");
            strncat(msg,cval,30);
            ffpmsg(msg);
            return(*status);
    }

    if (dtype == 'I')
    {
        if (ival)
            *lval = 1;
        else
            *lval = 0;
    }
    else if (dtype == 'F')
    {
        if (dval)
            *lval = 1;
        else
            *lval = 0;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffc2r(char *cval,   /* I - string representation of the value */
          float *fval,  /* O - numerical value of the input string */
          int *status)  /* IO - error status */
/*
  convert formatted string to a real float value, doing implicit
  datatype conversion if necessary
*/
{
    char dtype, sval[81], msg[81];
    long ival;
    int lval;
    double dval;
    
    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    /* convert the keyword to its native datatype */
    ffc2x(cval, &dtype, &ival, &lval, sval, &dval, status);

    if (dtype == 'C')
        *status = BAD_FLOATKEY;

    if (*status > 0)
    {
            *fval = 0.;
            strcpy(msg,"Error in ffc2r evaluating string as a float: ");
            strncat(msg,cval,30);
            ffpmsg(msg);
            return(*status);
    }

    if (dtype == 'F')
        *fval = (float) dval;
    else if (dtype == 'I')
        *fval = (float) ival;
    else if (dtype == 'L')
        *fval = (float) lval;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffc2d(char *cval,   /* I - string representation of the value */
          double *dval, /* O - numerical value of the input string */
          int *status)  /* IO - error status */
/*
  convert formatted string to a double value, doing implicit
  datatype conversion if necessary
*/
{
    char dtype, sval[81], msg[81];
    long ival;
    int lval;
    
    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    /* convert the keyword to its native datatype */
    ffc2x(cval, &dtype, &ival, &lval, sval, dval, status);

    if (dtype == 'C')
        *status = BAD_DOUBLEKEY;

    if (*status > 0)
    {
            *dval = 0.;
            strcpy(msg,"Error in ffc2d evaluating string as a double: ");
            strncat(msg,cval,30);
            ffpmsg(msg);
            return(*status);
    }

    if (dtype == 'I')
        *dval = (double) ival;
    else if (dtype == 'L')
        *dval = (double) lval;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffc2ii(char *cval,  /* I - string representation of the value */
          long *ival,   /* O - numerical value of the input string */
          int *status)  /* IO - error status */
/*
  convert null-terminated formatted string to an integer value
*/
{
    char *loc;

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    *ival = 0;
    *ival = strtol(cval, &loc, 10);  /* read the string as an integer */

    /* check for read error, or junk following the integer */
    if (*loc != '\0' && *loc != ' ' ) 
        *status = BAD_C2I;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffc2ll(char *cval,  /* I - string representation of the value: T or F */
           int *lval,   /* O - numerical value of the input string: 1 or 0 */
           int *status) /* IO - error status */
/*
  convert null-terminated formatted string to a logical value
*/
{
    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    if (cval[0] == 'T')
        *lval = 1;
    else                
        *lval = 0;        /* any character besides T is considered false */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffc2s(char *instr,  /* I - null terminated quoted input string */
          char *outstr, /* O - null terminated output string without quotes */
          int *status)  /* IO - error status */
/*
    convert an input quoted string to an unquoted string by removing
    the leading and trailing quote character.  Also, replace any
    pairs of single quote characters with just a single quote 
    character (FITS used a pair of single quotes to represent
    a literal quote character within the string).
*/
{
    int jj;
    size_t len, ii;

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    if (instr[0] != '\'')
    {
        strcpy(outstr, instr);  /* no leading quote, so return input string */
        return(*status);
    }

    len = strlen(instr);

    for (ii=1, jj=0; ii < len; ii++, jj++)
    {
        if (instr[ii] == '\'')  /*  is this the closing quote?  */
        {
            if (instr[ii+1] == '\'')  /* 2 successive quotes? */
                ii++;  /* copy only one of the quotes */
            else
                break;   /*  found the closing quote, so exit this loop  */
        }
        outstr[jj] = instr[ii];   /* copy the next character to the output */
    }

    outstr[jj] = '\0';             /*  terminate the output string  */

    if (ii == len)
    {
        ffpmsg("This string value has no closing quote (ffc2s):");
        ffpmsg(instr);
        return(*status = 205);
    }

    for (jj--; jj >= 0; jj--)  /* replace trailing blanks with nulls */
    {
        if (outstr[jj] == ' ')
            outstr[jj] = 0;
        else
            break;
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffc2rr(char *cval,   /* I - string representation of the value */
           float *fval,  /* O - numerical value of the input string */
           int *status)  /* IO - error status */
/*
  convert null-terminated formatted string to a float value
*/
{
    char *loc, msg[81];

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    *fval = 0.;
    *fval = (float) strtod(cval, &loc);  /* read the string as an float */

    /* check for read error, or junk following the value */
    if (*loc != '\0' && *loc != ' ' )
    {
        strcpy(msg,"Error in ffc2rr converting string to float: ");
        strncat(msg,cval,30);
        ffpmsg(msg);

        *status = BAD_C2F;   
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffc2dd(char *cval,   /* I - string representation of the value */
           double *dval, /* O - numerical value of the input string */
           int *status)  /* IO - error status */
/*
  convert null-terminated formatted string to a double value
*/
{
    char msg[81], tval[73], *loc;

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);


    strcpy(tval, cval);
    loc = strchr(tval, 'D');

    if (loc)            /*  The C language does not support a 'D' */
       *loc = 'E';      /*  exponent so replace any D's with E's. */               
    *dval = 0.;
    *dval = strtod(tval, &loc);  /* read the string as an double */

    /* check for read error, or junk following the value */
    if (*loc != '\0' && *loc != ' ' )
    {
        strcpy(msg,"Error in ffc2dd converting string to double: ");
        strncat(msg,cval,30);
        ffpmsg(msg);

        *status = BAD_C2D;   
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
void ffpmsg(char *err_message)
/*
  put message on to error stack
*/
{
    ffxmsg(1, err_message);
    return;
}
/*--------------------------------------------------------------------------*/
int ffgmsg(char *err_message)
/*
  get oldest message from error stack
*/
{
    ffxmsg(-1, err_message);
    return(*err_message);
}
/*--------------------------------------------------------------------------*/
void ffcmsg(void)
/*
  erase all messages in the error stack
*/
{
    char *dummy;

    ffxmsg(0, dummy);
    return;
}
/*--------------------------------------------------------------------------*/
void ffxmsg( int action,
            char *errmsg)
/*
  general routine to get, put, or clear the error message stack 
*/
{
    int ii;
    size_t len;
#define errmsgsiz 50
    static char *txtbuff[errmsgsiz];
    static nummsg = 0;

    if (action == -1)  /* return and remove oldest message from stack */ 
    {
      if (nummsg > 0)
      {
        strcpy(errmsg, txtbuff[0]);   /* copy oldest message to output */

        free(txtbuff[0]);  /* free the memory for this msg */
           
        nummsg--;  
        for (ii = 0; ii < nummsg; ii++)
             txtbuff[ii] = txtbuff[ii + 1]; /* shift remaining pointers */
      }
      else
          errmsg[0] = 0;  /*  no messages in the stack */
    }
    else if (action == 1)  /* add new message to stack */
    {
      if (nummsg == errmsgsiz)
      {
        free(txtbuff[0]);  /* buffer full; delete oldest msg */
        nummsg--;
        for (ii = 0; ii < nummsg; ii++)
             txtbuff[ii] = txtbuff[ii + 1];   /* shift remaining pointers */
      }

      len = minvalue(strlen(errmsg), 80);

      txtbuff[nummsg] = (char *) malloc(len + 1);

      if (!txtbuff[nummsg])
      {
        printf("\nmalloc failed in the ffpmsg routine of cfitsio.\n");
        printf("%s\n", errmsg);
      }
      else
      {
        *txtbuff[nummsg] = '\0';  /* initialize a null string */
        strncat(txtbuff[nummsg], errmsg, len);
        nummsg++;
      }
    }
    else if (action == 0)
    {
      for (ii = 0; ii < nummsg; ii ++)
        free(txtbuff[ii]);

      nummsg = 0;
    }
    return;
}

