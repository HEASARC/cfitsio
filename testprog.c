#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "fitsio.h"

main()
{
/*  
    This is a big and complicated program that tests many of the
    cfitsio routines.  This code does not necessarily represent
    the most efficient method of reading or writing FITS files 
    because this code is primarily designed to stress the cfitsio
    library routines.
*/

    char asciisum[17];
    unsigned long checksum;
    int status, simple, bitpix, naxis, extend, hdutype, hdunum, tfields;
    long ii, jj;
    int nkeys, nfound, colnum, typecode;
    char cval;
    long repeat, offset, width, jnulval;
    int anynull;
    float vers;
    unsigned char xinarray[21], binarray[21], boutarray[21];
    short         iinarray[21], ioutarray[21];
    long          jinarray[21], joutarray[21];
    float         einarray[21], eoutarray[21], cinarray[42];
    double        dinarray[21], doutarray[21], minarray[42];
    double scale, zero;
    long naxes[3], pcount, gcount, npixels, nrows, rowlen;
    int existkeys, morekeys, keynum;

    char larray[42], larray2[42], colname[70], tdisp[40], nulstr[40];
    char oskey[] = "value_string";
    char iskey[21];
    int olkey = 1;
    int ilkey;
    long ojkey = 11, ijkey;
    long otint = 12345678;
    float ofkey = 12.121212;
    float oekey = 13.131313, iekey;
    double ogkey = 14.1414141414141414;
    double odkey = 15.1515151515151515, idkey;
    double otfrac = .1234567890123456;

    char *lsptr;    /* pointer to long string value */
    char  comm[73];
    char *comms[3];
    char *inskey[21];
    char *onskey[3] = {"first string", "second string", "        "};
    int   onlkey[3] = {1, 0, 1}, inlkey[3];
    long  onjkey[3] = {11, 12, 13}, injkey[3];
    float onfkey[3] = {12.121212, 13.131313, 14.141414};
    float onekey[3] = {13.131313, 14.141414, 15.151515}, inekey[3];
    double ongkey[3] = {14.1414141414141414, 15.1515151515151515,
           16.1616161616161616};
    double ondkey[3] = {15.1515151515151515, 16.1616161616161616,
           17.1717171717171717}, indkey[3];

    long tbcol[5] = {1, 17, 28, 43, 56};

    char filename[40], card[FLEN_CARD], keyword[FLEN_KEYWORD];
    char value[FLEN_VALUE], comment[FLEN_COMMENT];
    unsigned char uchars[80];

    fitsfile *fptr, *tmpfile;
    char *ttype[10], *tform[10], *tunit[10];
    char tblname[] = "Test-ASCII";
    char binname[] = "Test-BINTABLE";
    char errmsg[FLEN_ERRMSG];
    short imgarray[30][19], imgarray2[20][10];
    long fpixels[2], lpixels[2], inc[2];

    status = 0;

    for (ii = 0; ii < 21; ii++)  /* allocate space for string column value */
        inskey[ii] = (char *) malloc(21);   

    comms[0] = comm;

    ffvers(&vers);
    printf("CFITSIO TESTPROG, v%.3f\n",vers);

    strcpy(filename, "testprog.fit");

    /* delete previous version of the file, if it exists */

    remove(filename);

    status = 0;

    /*
      #####################
      #  create FITS file #
      #####################
    */
    if (ffinit(&fptr, filename, &status) > 0)
    {
        printf("ffinit create new file status = %d\n", status);
        goto errstatus;
    }
 
    simple = 1;
    bitpix = 32;
    naxis = 2;
    naxes[0] = 10;
    naxes[1] = 2;
    npixels = 20;
    pcount = 0;
    gcount = 1;
    extend = 1;
    /*
      ############################
      #  write single keywords   #
      ############################
    */
    if (ffphps(fptr, bitpix, naxis, naxes, &status) > 0)
        printf("ffphps status = %d\n", status);

    if (ffprec(fptr, 
    "key_prec= 'This keyword was written by fxprec' / comment goes here", 
     &status) > 0 )
        printf("ffprec status = %d\n", status);

    if (ffpkys(fptr, "key_pkys", oskey, "fxpkys comment", &status) > 0)
        printf("ffpkys status = %d\n", status);

    if (ffpkyl(fptr, "key_pkyl", olkey, "fxpkyl comment", &status) > 0)
        printf("ffpkyl status = %d\n", status);

    if (ffpkyj(fptr, "key_pkyj", ojkey, "fxpkyj comment", &status) > 0)
        printf("ffpkyj status = %d\n", status);

    if (ffpkyf(fptr, "key_pkyf", ofkey, 5, "fxpkyf comment", &status) > 0)
        printf("ffpkyf status = %d\n", status);

    if (ffpkye(fptr, "key_pkye", oekey, 6, "fxpkye comment", &status) > 0)
        printf("ffpkye status = %d\n", status);

    if (ffpkyg(fptr, "key_pkyg", ogkey, 14, "fxpkyg comment", &status) > 0)
        printf("ffpkyg status = %d\n", status);

    if (ffpkyd(fptr, "key_pkyd", odkey, 14, "fxpkyd comment", &status) > 0)
        printf("ffpkyd status = %d\n", status);

    if (ffpkls(fptr, "key_pkls", 
"This is a very long string value that is continued over more than one keyword.",
     "fxpkls comment", &status) > 0)
        printf("ffpkls status = %d\n", status);

    if (ffplsw(fptr, &status) > 0 )
        printf("ffplsw status = %d\n", status);

    if (ffpkyt(fptr, "key_pkyt", otint, otfrac, "fxpkyt comment", &status) > 0)
        printf("ffpkyt status = %d\n", status);

    if (ffpcom(fptr, "This keyword was written by fxpcom.", &status) > 0)
        printf("ffpcom status = %d\n", status);

    if (ffphis(fptr, "  This keyword written by fxphis (w/ 2 leading spaces).",
        &status) > 0)
        printf("ffphis status = %d\n", status);

    if (ffpdat(fptr, &status) > 0)
    {
        printf("ffpdat status = %d\n", status);
        goto errstatus;
    }
    /*
      ###############################
      #  write arrays of keywords   #
      ###############################
    */
    nkeys = 3;

    comms[0] = comm;  /* use the inskey array of pointers for the comments */

    strcpy(comm, "fxpkns comment&");
    if (ffpkns(fptr, "ky_pkns", 1, nkeys, onskey, comms, &status) > 0)
        printf("ffpkns status = %d\n", status);

    strcpy(comm, "fxpknl comment&");
    if (ffpknl(fptr, "ky_pknl", 1, nkeys, onlkey, comms, &status) > 0)
        printf("ffpknl status = %d\n", status);

    strcpy(comm, "fxpknj comment&");
    if (ffpknj(fptr, "ky_pknj", 1, nkeys, onjkey, comms, &status) > 0)
        printf("ffpknj status = %d\n", status);

    strcpy(comm, "fxpknf comment&");
    if (ffpknf(fptr, "ky_pknf", 1, nkeys, onfkey, 5, comms, &status) > 0)
        printf("ffpknf status = %d\n", status);

    strcpy(comm, "fxpkne comment&");
    if (ffpkne(fptr, "ky_pkne", 1, nkeys, onekey, 6, comms, &status) > 0)
        printf("ffpkne status = %d\n", status);

    strcpy(comm, "fxpkng comment&");
    if (ffpkng(fptr, "ky_pkng", 1, nkeys, ongkey, 14, comms, &status) > 0)
        printf("ffpkng status = %d\n", status);

    strcpy(comm, "fxpknd comment&");
    if (ffpknd(fptr, "ky_pknd", 1, nkeys, ondkey, 14, comms, &status) > 0)
    {
        printf("ffpknd status = %d\n", status);
        goto errstatus;
    }
    /*
      ############################
      #  write data              #
      ############################
    */
    /* define the null value (must do this before writing any data) */
    if (ffpkyj(fptr, "BLANK", -99, "value to use for undefined pixels",
       &status) > 0)
        printf("BLANK keyword status = %d\n", status);

    /* initialize arrays of values to write to primary array */
    for (ii = 0; ii < npixels; ii++)
    {
        boutarray[ii] = ii + 1;
        ioutarray[ii] = ii + 1;
        joutarray[ii] = ii + 1;
        eoutarray[ii] = ii + 1;
        doutarray[ii] = ii + 1;
    }

    /* write a few pixels with each datatype */
    /* set the last value in each group of 4 as undefined */
    ffpprb(fptr, 1,  1, 2, &boutarray[0],  &status);
    ffppri(fptr, 1,  5, 2, &ioutarray[4],  &status);
    ffpprj(fptr, 1,  9, 2, &joutarray[8],  &status);
    ffppre(fptr, 1, 13, 2, &eoutarray[12], &status);
    ffpprd(fptr, 1, 17, 2, &doutarray[16], &status);
    ffppnb(fptr, 1,  3, 2, &boutarray[2],   4, &status);
    ffppni(fptr, 1,  7, 2, &ioutarray[6],   8, &status);
    ffppnj(fptr, 1, 11, 2, &joutarray[10],  12, &status);
    ffppne(fptr, 1, 15, 2, &eoutarray[14], 16., &status);
    ffppnd(fptr, 1, 19, 2, &doutarray[18], 20., &status);

    ffppru(fptr, 1, 1, 1, &status);
    if (status > 0)
    {
        printf("ffppnx status = %d\n", status);
        goto errstatus;
    }

    ffflus(fptr, &status);   /* flush all data to the disk file */
    printf("ffflus status = %d\n", status);

    /*
      ############################
      #  read data               #
      ############################
    */
    /* read back the data, setting null values = 99 */
    printf("\nValues read back from primary array (99 = null pixel)\n");
    printf("The 1st, and every 4th pixel should be undefined:\n");

    anynull = 0;
    ffgpvb(fptr, 1,  1, 10, 99, binarray, &anynull, &status);

    ffgpvb(fptr, 1, 11, 10, 99, &binarray[10], &anynull, &status);

    for (ii = 0; ii < npixels; ii++)
        printf(" %2d", binarray[ii]);
    printf("  %d (ffgpvb)\n", anynull);  

    ffgpvi(fptr, 1, 1, npixels, 99,   iinarray, &anynull, &status);

    for (ii = 0; ii < npixels; ii++)
        printf(" %2d", iinarray[ii]);
    printf("  %d (ffgpvi)\n", anynull);  

    ffgpvj(fptr, 1, 1, npixels, 99,  jinarray, &anynull, &status);

    for (ii = 0; ii < npixels; ii++)
        printf(" %2d", jinarray[ii]);
    printf("  %d (ffgpvj)\n", anynull);  

    ffgpve(fptr, 1, 1, npixels, 99., einarray, &anynull, &status);

    for (ii = 0; ii < npixels; ii++)
        printf(" %2.0f", einarray[ii]);
    printf("  %d (ffgpve)\n", anynull);  

    ffgpvd(fptr, 1,  1, 10, 99.,  dinarray, &anynull, &status);
    ffgpvd(fptr, 1, 11, 10, 99.,  &dinarray[10], &anynull, &status);

    for (ii = 0; ii < npixels; ii++)
        printf(" %2.0f", dinarray[ii]);
    printf("  %d (ffgpvd)\n", anynull);  

    if (status > 0)
    {
        printf("ERROR: ffgpv_ status = %d\n", status);
        goto errstatus;
    }
    if (anynull == 0)
       printf("ERROR: ffgpv_ did not detect null values\n");

    /* reset the output null value to the expected input value */
    for (ii = 3; ii < npixels; ii += 4)
    {
        boutarray[ii] = 99;
        ioutarray[ii] = 99;
        joutarray[ii] = 99;
        eoutarray[ii] = 99.;
        doutarray[ii] = 99.;
    }
        ii = 0;
        boutarray[ii] = 99;
        ioutarray[ii] = 99;
        joutarray[ii] = 99;
        eoutarray[ii] = 99.;
        doutarray[ii] = 99.;

    /* compare the output with the input; flag any differences */
    for (ii = 0; ii < npixels; ii++)
    {
       if (boutarray[ii] != binarray[ii])
           printf("bout != bin = %u %u \n", boutarray[ii], binarray[ii]);

       if (ioutarray[ii] != iinarray[ii])
           printf("bout != bin = %d %d \n", ioutarray[ii], iinarray[ii]);

       if (joutarray[ii] != jinarray[ii])
           printf("bout != bin = %d %d \n", joutarray[ii], jinarray[ii]);

       if (eoutarray[ii] != einarray[ii])
           printf("bout != bin = %f %f \n", eoutarray[ii], einarray[ii]);

       if (doutarray[ii] != dinarray[ii])
           printf("bout != bin = %f %f \n", doutarray[ii], dinarray[ii]);
    }

    for (ii = 0; ii < npixels; ii++)
    {
      binarray[ii] = 0;
      iinarray[ii] = 0;
      jinarray[ii] = 0;
      einarray[ii] = 0.;
      dinarray[ii] = 0.;
    }

    anynull = 0;
    ffgpfb(fptr, 1,  1, 10, binarray, larray, &anynull, &status);
    ffgpfb(fptr, 1, 11, 10, &binarray[10], &larray[10], &anynull, &status);

    for (ii = 0; ii < npixels; ii++)
      if (larray[ii])
        printf("  *");
      else
        printf(" %2d", binarray[ii]);
    printf("  %d (ffgpfb)\n", anynull);  

    ffgpfi(fptr, 1, 1, npixels, iinarray, larray, &anynull, &status);

    for (ii = 0; ii < npixels; ii++)
      if (larray[ii])
        printf("  *");
      else
        printf(" %2d", iinarray[ii]);
    printf("  %d (ffgpfi)\n", anynull);  

    ffgpfj(fptr, 1, 1, npixels, jinarray, larray, &anynull, &status);

    for (ii = 0; ii < npixels; ii++)
      if (larray[ii])
        printf("  *");
      else
        printf(" %2d", jinarray[ii]);
    printf("  %d (ffgpfj)\n", anynull);  

    ffgpfe(fptr, 1, 1, npixels, einarray, larray, &anynull, &status);

    for (ii = 0; ii < npixels; ii++)
      if (larray[ii])
        printf("  *");
      else
        printf(" %2.0f", einarray[ii]);
    printf("  %d (ffgpfe)\n", anynull);  

    ffgpfd(fptr, 1,  1, 10, dinarray, larray, &anynull, &status);
    ffgpfd(fptr, 1, 11, 10, &dinarray[10], &larray[10], &anynull, &status);

    for (ii = 0; ii < npixels; ii++)
      if (larray[ii])
        printf("  *");
      else
        printf(" %2.0f", dinarray[ii]);
    printf("  %d (ffgpfd)\n", anynull);  

    if (status > 0)
    {
        printf("ERROR: ffgpf_ status = %d\n", status);
        goto errstatus;
    }
    if (anynull == 0)
       printf("ERROR: ffgpf_ did not detect null values\n");


    /*
      ##########################################
      #  close and reopen file multiple times  #
      ##########################################
    */

    for (ii = 0; ii < 10; ii++)
    {
      if (ffclos(fptr, &status) > 0)
      {
        printf("ERROR in ftclos (1) = %d", status);
        goto errstatus;
      }

      if (ffopen(&fptr, filename, READWRITE, &status) > 0)
      {
        printf("ERROR: ffopen open file status = %d\n", status);
        goto errstatus;
      }
    }

    printf("\nClosed then reopened the FITS file 10 times.\n");

    /*
      ############################
      #  read single keywords    #
      ############################
    */

    simple = 0;
    bitpix = 0;
    naxis = 0;
    naxes[0] = 0;
    naxes[1] = 0;
    pcount = -99;
    gcount =  -99;
    extend = -99;
    printf("\nRead back keywords:\n");
    ffghpr(fptr, 99, &simple, &bitpix, &naxis, naxes, &pcount,
           &gcount, &extend, &status);
    printf("simple = %d, bitpix = %d, naxis = %d, naxes = (%d, %d)\n",
           simple, bitpix, naxis, naxes[0], naxes[1]);
    printf("  pcount = %d, gcount = %d, extend = %d\n",
               pcount, gcount, extend);

    ffgrec(fptr, 11, card, &status);
    printf("%s\n", card);
    if (strncmp(card, "KEY_PREC= 'This", 15) )
       printf("ERROR in ffgrec\n");

    ffgkyn(fptr, 11, keyword, value, comment, &status);
    printf("%s : %s : %s :\n",keyword, value, comment);
    if (strncmp(keyword, "KEY_PREC", 8) )
       printf("ERROR in ffgkyn: %s\n", keyword);

    ffgcrd(fptr, keyword, card, &status);
    printf("%s\n", card);

    if (strncmp(keyword, card, 8) )
       printf("ERROR in ffgcrd: %s\n", keyword);

    ffgkey(fptr, "KY_PKNS1", value, comment, &status);
    printf("KY_PKNS1 : %s : %s :\n", value, comment);

    if (strncmp(value, "'first string'", 14) )
       printf("ERROR in ffgkey: %s\n", value);

    ffgkys(fptr, "key_pkys", iskey, comment, &status);
    printf("KEY_PKYS %s %s %d\n", iskey, comment, status);

    ffgkyl(fptr, "key_pkyl", &ilkey, comment, &status);
    printf("KEY_PKYL %d %s %d\n", ilkey, comment, status);

    ffgkyj(fptr, "KEY_PKYJ", &ijkey, comment, &status);
    printf("KEY_PKYJ %d %s %d\n",ijkey, comment, status);

    ffgkye(fptr, "KEY_PKYJ", &iekey, comment, &status);
    printf("KEY_PKYJ %f %s %d\n",iekey, comment, status);

    ffgkyd(fptr, "KEY_PKYJ", &idkey, comment, &status);
    printf("KEY_PKYJ %f %s %d\n",idkey, comment, status);

    if (ijkey != 11 || iekey != 11. || idkey != 11.)
       printf("ERROR in ffgky[jed]: %d, %f, %f\n",ijkey, iekey, idkey);

    ffgkyd(fptr, "KEY_PKYF", &idkey, comment, &status);
    printf("KEY_PKYF %f %s %d\n",idkey, comment, status);

    ffgkyd(fptr, "KEY_PKYE", &idkey, comment, &status);
    printf("KEY_PKYE %f %s %d\n",idkey, comment, status);

    ffgkyd(fptr, "KEY_PKYG", &idkey, comment, &status);
    printf("KEY_PKYG %.14f %s %d\n",idkey, comment, status);

    ffgkyd(fptr, "KEY_PKYD", &idkey, comment, &status);
    printf("KEY_PKYD %.14f %s %d\n",idkey, comment, status);

    ffgkyt(fptr, "KEY_PKYT", &ijkey, &idkey, comment, &status);
    printf("KEY_PKYT %d %.14f %s %d\n",ijkey, idkey, comment, status);

    ffgkls(fptr, "key_pkls", &lsptr, comment, &status);
    printf("KEY_PKLS long string value = \n%s\n", lsptr);

    /* free the memory for the long string value */
    free(lsptr);

    /* get size and position in header */
    ffghps(fptr, &existkeys, &keynum, &status);
    printf("header contains %d keywords; located at keyword %d \n",existkeys,
            keynum);

    /*
      ############################
      #  read array keywords     #
      ############################
    */
    ffgkns(fptr, "ky_pkns", 1, 3, inskey, &nfound, &status);
    printf("ffgkns:  %s, %s, %s\n", inskey[0], inskey[1], inskey[2]);
    if (nfound != 3 || status > 0)
       printf("\nERROR in ffgkns %d, %d\n", nfound, status);

    ffgknl(fptr, "ky_pknl", 1, 3, inlkey, &nfound, &status);
    printf("ffgknl:  %d, %d, %d\n", inlkey[0], inlkey[1], inlkey[2]);
    if (nfound != 3 || status > 0)
       printf("\nERROR in ffgknl %d, %d\n", nfound, status);

    ffgknj(fptr, "ky_pknj", 1, 3, injkey, &nfound, &status);
    printf("ffgknj:  %d, %d, %d\n", injkey[0], injkey[1], injkey[2]);
    if (nfound != 3 || status > 0)
       printf("\nERROR in ffgknj %d, %d\n", nfound, status);

    ffgkne(fptr, "ky_pkne", 1, 3, inekey, &nfound, &status);
    printf("ffgkne:  %f, %f, %f\n", inekey[0], inekey[1], inekey[2]);
    if (nfound != 3 || status > 0)
       printf("\nERROR in ffgkne %d, %d\n", nfound, status);

    ffgknd(fptr, "ky_pknd", 1, 3, indkey, &nfound, &status);
    printf("ffgknd:  %f, %f, %f\n", indkey[0], indkey[1], indkey[2]);
    if (nfound != 3 || status > 0)
       printf("\nERROR in ffgknd %d, %d\n", nfound, status);

    printf("\nBefore deleting the HISTORY and DATE keywords...\n");
    for (ii = 27; ii <= 30; ii++)
    {
        ffgrec(fptr, ii, card, &status);
        printf("%s\n", card);
    }
    /*
      ############################
      #  delete keywords         #
      ############################
    */

    ffdrec(fptr, 28, &status);
    ffdkey(fptr, "DATE", &status);

    printf("\nAfter deleting the keywords...\n");
    for (ii = 27; ii <= 28; ii++)
    {
        ffgrec(fptr, ii, card, &status);
        printf("%s\n", card);
    }

    if (status > 0)
       printf("\nERROR deleting keywords\n");
    /*
      ############################
      #  insert keywords         #
      ############################
    */
    ffirec(fptr, 28, "KY_IREC = 'This keyword inserted by fxirec'",
           &status);
    ffikys(fptr, "KY_IKYS", "insert_value_string", "ikys comment", &status);
    ffikyj(fptr, "KY_IKYJ", 49, "ikyj comment", &status);
    ffikyl(fptr, "KY_IKYL", 1, "ikyl comment", &status);
    ffikye(fptr, "KY_IKYE", 12.3456, 4, "ikye comment", &status);
    ffikyd(fptr, "KY_IKYD", 12.345678901234567, 14, "ikyd comment", &status);
    ffikyf(fptr, "KY_IKYF", 12.3456, 4, "ikyf comment", &status);
    ffikyg(fptr, "KY_IKYG", 12.345678901234567, 14, "ikyg comment", &status);

    printf("\nAfter inserting the keywords...\n");
    for (ii = 27; ii <= 36; ii++)
    {
        ffgrec(fptr, ii, card, &status);
        printf("%s\n", card);
    }

    if (status > 0)
       printf("\nERROR inserting keywords\n");
    /*
      ############################
      #  modify keywords         #
      ############################
    */
    ffmrec(fptr, 27, "COMMENT   This keyword was modified by fxmrec", &status);
    ffmcrd(fptr, "KY_IREC", "KY_MREC = 'This keyword was modified by fxmcrd'",
            &status);
    ffmnam(fptr, "KY_IKYS", "NEWIKYS", &status);

    ffmcom(fptr, "KY_IKYJ","This is a modified comment", &status);
    ffmkyj(fptr, "KY_IKYJ", 50, "&", &status);
    ffmkyl(fptr, "KY_IKYL", 0, "&", &status);
    ffmkys(fptr, "NEWIKYS", "modified_string", "&", &status);
    ffmkye(fptr, "KY_IKYE", -12.3456, 4, "&", &status);
    ffmkyd(fptr, "KY_IKYD", -12.345678901234567, 14, "modified comment",
            &status);
    ffmkyf(fptr, "KY_IKYF", -12.3456, 4, "&", &status);
    ffmkyg(fptr, "KY_IKYG", -12.345678901234567, 14, "&", &status);

    printf("\nAfter modifying the keywords...\n");
    for (ii = 27; ii <= 36; ii++)
    {
        ffgrec(fptr, ii, card, &status);
        printf("%s\n", card);
    }
    if (status > 0)
       printf("\nERROR modifying keywords\n");

    /*
      ############################
      #  update keywords         #
      ############################
    */
    ffucrd(fptr, "KY_MREC", "KY_UCRD = 'This keyword was updated by fxucrd'",
            &status);

    ffukyj(fptr, "KY_IKYJ", 51, "&", &status);
    ffukyl(fptr, "KY_IKYL", 1, "&", &status);
    ffukys(fptr, "NEWIKYS", "updated_string", "&", &status);
    ffukye(fptr, "KY_IKYE", -13.3456, 4, "&", &status);
    ffukyd(fptr, "KY_IKYD", -13.345678901234567, 14, "modified comment",
            &status);
    ffukyf(fptr, "KY_IKYF", -13.3456, 4, "&", &status);
    ffukyg(fptr, "KY_IKYG", -13.345678901234567, 14, "&", &status);

    printf("\nAfter updating the keywords...\n");
    for (ii = 27; ii <= 36; ii++)
    {
        ffgrec(fptr, ii, card, &status);
        printf("%s\n", card);
    }
    if (status > 0)
       printf("\nERROR modifying keywords\n");

    /*
      ############################
      #  create binary table     #
      ############################
    */
    ffcrhd(fptr, &status);
    printf("ffcrhd status = %d\n", status);

    for (ii = 0; ii < 10; ii++)
    {
      ttype[ii] = (char *) malloc(20);
      tform[ii] = (char *) malloc(20);
      tunit[ii] = (char *) malloc(20);
    }

    strcpy(tform[0], "15A");
    strcpy(tform[1], "1L");
    strcpy(tform[2], "16X");
    strcpy(tform[3], "1B");
    strcpy(tform[4], "1I");
    strcpy(tform[5], "1J");
    strcpy(tform[6], "1E");
    strcpy(tform[7], "1D");
    strcpy(tform[8], "1C");
    strcpy(tform[9], "1M");

    strcpy(ttype[0], "Avalue");
    strcpy(ttype[1], "Lvalue");
    strcpy(ttype[2], "Xvalue");
    strcpy(ttype[3], "Bvalue");
    strcpy(ttype[4], "Ivalue");
    strcpy(ttype[5], "Jvalue");
    strcpy(ttype[6], "Evalue");
    strcpy(ttype[7], "Dvalue");
    strcpy(ttype[8], "Cvalue");
    strcpy(ttype[9], "Mvalue");

    strcpy(tunit[0], "");
    strcpy(tunit[1], "m**2");
    strcpy(tunit[2], "cm");
    strcpy(tunit[3], "erg/s");
    strcpy(tunit[4], "km/s");
    strcpy(tunit[5], "");
    strcpy(tunit[6], "");
    strcpy(tunit[7], "");
    strcpy(tunit[8], "");
    strcpy(tunit[9], "");

    nrows = 21;
    tfields = 10;
    pcount = 0;

    ffphbn(fptr, nrows, tfields, ttype, tform, tunit, binname, pcount,
            &status);
    printf("ffphbn status = %d\n", status);

    /* get size and position in header, and reserve space for more keywords */
    ffghps(fptr, &existkeys, &keynum, &status);
    printf("header contains %d keywords; located at keyword %d \n",existkeys,
            keynum);

    morekeys = 40;
    ffhdef(fptr, morekeys, &status);
    ffghsp(fptr, &existkeys, &morekeys, &status);
    printf("header contains %d keywords with room for %d more\n",existkeys,
            morekeys);

    fftnul(fptr, 4, 99, &status);   /* define null value for int cols */
    fftnul(fptr, 5, 99, &status);
    fftnul(fptr, 6, 99, &status);

    ffpkyj(fptr, "TNULL4", 99, "value for undefined pixels", &status);
    ffpkyj(fptr, "TNULL5", 99, "value for undefined pixels", &status);
    ffpkyj(fptr, "TNULL6", 99, "value for undefined pixels", &status);

    naxis = 3;
    naxes[0] = 1;
    naxes[1] = 2;
    naxes[2] = 8;
    ffptdm(fptr, 3, naxis, naxes, &status);

    naxis = 0;
    naxes[0] = 0;
    naxes[1] = 0;
    naxes[2] = 0;
    ffgtdm(fptr, 3, 3, &naxis, naxes, &status);
    ffgkys(fptr, "TDIM3", iskey, comment, &status);
    printf("TDIM3 = %s, %d, %d, %d, %d\n", iskey, naxis, naxes[0],
         naxes[1], naxes[2]);

    ffrdef(fptr, &status);  /* force header to be scanned (not required) */

    /*
      ############################
      #  write data to columns   #
      ############################
    */

    /* initialize arrays of values to write to table */
    for (ii = 0; ii < 21; ii++)
    {
        boutarray[ii] = ii + 1;
        ioutarray[ii] = ii + 1;
        joutarray[ii] = ii + 1;
        eoutarray[ii] = ii + 1;
        doutarray[ii] = ii + 1;
    }

    ffpcls(fptr, 1, 1, 1, 3, onskey, &status);  /* write string values */
    ffpclu(fptr, 1, 4, 1, 1, &status);  /* write null value */

    larray[0] = 0;
    larray[1] = 1;
    larray[2] = 0;
    larray[3] = 0;
    larray[4] = 1;
    larray[5] = 1;
    larray[6] = 0;
    larray[7] = 0;
    larray[8] = 0;
    larray[9] = 1;
    larray[10] = 1;
    larray[11] = 1;
    larray[12] = 0;
    larray[13] = 0;
    larray[14] = 0;
    larray[15] = 0;
    larray[16] = 1;
    larray[17] = 1;
    larray[18] = 1;
    larray[19] = 1;
    larray[20] = 0;
    larray[21] = 0;
    larray[22] = 0;
    larray[23] = 0;
    larray[24] = 0;
    larray[25] = 1;
    larray[26] = 1;
    larray[27] = 1;
    larray[28] = 1;
    larray[29] = 1;
    larray[30] = 0;
    larray[31] = 0;
    larray[32] = 0;
    larray[33] = 0;
    larray[34] = 0;
    larray[35] = 0;


    ffpclx(fptr, 3, 1, 1, 36, larray, &status); /*write bits*/

    for (ii = 4; ii < 9; ii++)   /* loop over cols 4 - 8 */
    {
        ffpclb(fptr, ii, 1, 1, 2, boutarray, &status);
        if (status == NUM_OVERFLOW)
            status = 0;
        ffpcli(fptr, ii, 3, 1, 2, &ioutarray[2], &status); 
        if (status == NUM_OVERFLOW)
            status = 0;
        ffpclj(fptr, ii, 5, 1, 2, &joutarray[4], &status); 
        if (status == NUM_OVERFLOW)
            status = 0;
        ffpcle(fptr, ii, 7, 1, 2, &eoutarray[6], &status);
        if (status == NUM_OVERFLOW)
            status = 0;
        ffpcld(fptr, ii, 9, 1, 2, &doutarray[8], &status);
        if (status == NUM_OVERFLOW)
            status = 0;

        ffpclu(fptr, ii, 11, 1, 1, &status);  /* write null value */
    }

    ffpclc(fptr, 9, 1, 1, 10, eoutarray, &status);
    ffpclm(fptr, 10, 1, 1, 10, doutarray, &status);

    for (ii = 4; ii < 9; ii++)   /* loop over cols 4 - 8 */
    {
        ffpcnb(fptr, ii, 12, 1, 2, &boutarray[11], 13, &status);
        if (status == NUM_OVERFLOW)
            status = 0;
        ffpcni(fptr, ii, 14, 1, 2, &ioutarray[13], 15, &status); 
        if (status == NUM_OVERFLOW)
            status = 0;
        ffpcnj(fptr, ii, 16, 1, 2, &joutarray[15], 17, &status); 
        if (status == NUM_OVERFLOW)
            status = 0;
        ffpcne(fptr, ii, 18, 1, 2, &eoutarray[17], 19., &status);
        if (status == NUM_OVERFLOW)
            status = 0;
        ffpcnd(fptr, ii, 20, 1, 2, &doutarray[19], 21., &status);
        if (status == NUM_OVERFLOW)
            status = 0;

    }
    ffpcll(fptr, 2, 1, 1, 21, larray, &status); /*write logicals*/
    ffpclu(fptr, 2, 11, 1, 1, &status);  /* write null value */
    printf("ffpcl_ status = %d\n", status);

    /*
      #########################################
      #  get information about the columns    #
      #########################################
    */

    ffgcno(fptr, 0, "Xvalue", &colnum, &status);
    printf("\nColumn Xvalue is number %d; status = %d.\n", colnum, status);

    while (status != COL_NOT_FOUND)
    {
      ffgcnn(fptr, 1, "*ue", colname, &colnum, &status);
      printf("Column %s is number %d; status = %d.\n", 
           colname, colnum, status);
    }
    status = 0;

    for (ii = 0; ii < tfields; ii++)
    {
      ffgtcl(fptr, ii + 1, &typecode, &repeat, &width, &status);
      printf("%4s %3d %2d %2d", tform[ii], typecode, repeat, width);
      ffgbcl(fptr, ii + 1, ttype[0], tunit[0], &cval, &repeat, &scale,
           &zero, &jnulval, tdisp, &status);
      printf(" %s, %s, %c, %d, %f, %f, %d, %s.\n",
         ttype[0], tunit[0], cval, repeat, scale, zero, jnulval, tdisp);
    }

    printf("\n");

    /*
      ###############################################
      #  insert ASCII table before the binary table #
      ###############################################
    */

    if (ffmrhd(fptr, -1, &hdutype, &status) > 0)
        goto errstatus;

    for (ii = 0; ii < 5; ii++)
    {
      ttype[ii] = (char *) malloc(20);
      tform[ii] = (char *) malloc(20);
      tunit[ii] = (char *) malloc(20);
    }
    strcpy(tform[0], "A15");
    strcpy(tform[1], "I10");
    strcpy(tform[2], "F14.6");
    strcpy(tform[3], "E12.5");
    strcpy(tform[4], "D21.14");

    strcpy(ttype[0], "Name");
    strcpy(ttype[1], "Ivalue");
    strcpy(ttype[2], "Fvalue");
    strcpy(ttype[3], "Evalue");
    strcpy(ttype[4], "Dvalue");

    strcpy(tunit[0], "");
    strcpy(tunit[1], "m**2");
    strcpy(tunit[2], "cm");
    strcpy(tunit[3], "erg/s");
    strcpy(tunit[4], "km/s");

    rowlen = 76;
    nrows = 11;
    tfields = 5;

    ffitab(fptr, rowlen, nrows, tfields, ttype, tbcol, tform, tunit, tblname,
            &status);
    printf("ffitab status = %d\n", status);

    ffsnul(fptr, 1, "null1", &status);   /* define null value for int cols */
    ffsnul(fptr, 2, "null2", &status);
    ffsnul(fptr, 3, "null3", &status);
    ffsnul(fptr, 4, "null4", &status);
    ffsnul(fptr, 5, "null5", &status);
 
    ffpkys(fptr, "TNULL1", "null1", "value for undefined pixels", &status);
    ffpkys(fptr, "TNULL2", "null2", "value for undefined pixels", &status);
    ffpkys(fptr, "TNULL3", "null3", "value for undefined pixels", &status);
    ffpkys(fptr, "TNULL4", "null4", "value for undefined pixels", &status);
    ffpkys(fptr, "TNULL5", "null5", "value for undefined pixels", &status);

    if (status > 0)
        goto errstatus;

    /*
      ############################
      #  write data to columns   #
      ############################
    */

    /* initialize arrays of values to write to table */
    for (ii = 0; ii < 21; ii++)
    {
        boutarray[ii] = ii + 1;
        ioutarray[ii] = ii + 1;
        joutarray[ii] = ii + 1;
        eoutarray[ii] = ii + 1;
        doutarray[ii] = ii + 1;
    }

    ffpcls(fptr, 1, 1, 1, 3, onskey, &status);  /* write string values */
    ffpclu(fptr, 1, 4, 1, 1, &status);  /* write null value */

    for (ii = 2; ii < 6; ii++)   /* loop over cols 2 - 5 */
    {
        ffpclb(fptr, ii, 1, 1, 2, boutarray, &status);  /* char array */
        if (status == NUM_OVERFLOW)
            status = 0;
        ffpcli(fptr, ii, 3, 1, 2, &ioutarray[2], &status);  /* short array */
        if (status == NUM_OVERFLOW)
            status = 0;
        ffpclj(fptr, ii, 5, 1, 2, &joutarray[4], &status);  /* long array */
        if (status == NUM_OVERFLOW)
            status = 0;
        ffpcle(fptr, ii, 7, 1, 2, &eoutarray[6], &status);  /* float array */
        if (status == NUM_OVERFLOW)
            status = 0;
        ffpcld(fptr, ii, 9, 1, 2, &doutarray[8], &status);  /* double array */
        if (status == NUM_OVERFLOW)
            status = 0;

        ffpclu(fptr, ii, 11, 1, 1, &status);  /* write null value */
    }
    printf("ffpcl_ status = %d\n", status);

    /*
      ################################
      #  read data from ASCII table  #
      ################################
    */
    ffghtb(fptr, 99, &rowlen, &nrows, &tfields, ttype, tbcol, 
           tform, tunit, tblname, &status);

    printf("\nASCII table: rowlen, nrows, tfields, extname: %d %d %d %s\n",
           rowlen, nrows, tfields, tblname);

    for (ii = 0; ii < tfields; ii++)
      printf("%8s %3d %8s %8s \n", ttype[ii], tbcol[ii], 
                                   tform[ii], tunit[ii]);

    nrows = 11;
    ffgcvs(fptr, 1, 1, 1, nrows, "UNDEFINED", inskey,   &anynull, &status);
    ffgcvb(fptr, 2, 1, 1, nrows, 99, binarray, &anynull, &status);
    ffgcvi(fptr, 2, 1, 1, nrows, 99, iinarray, &anynull, &status);
    ffgcvj(fptr, 3, 1, 1, nrows, 99, jinarray, &anynull, &status);
    ffgcve(fptr, 4, 1, 1, nrows, 99., einarray, &anynull, &status);
    ffgcvd(fptr, 5, 1, 1, nrows, 99., dinarray, &anynull, &status);

    printf("\nData values read from ASCII table:\n");
    for (ii = 0; ii < nrows; ii++)
    {
      printf("%15s %2d %2d %2d %4.1f %4.1f\n", inskey[ii], binarray[ii],
           iinarray[ii], jinarray[ii], einarray[ii], dinarray[ii]); 
    }

    ffgtbb(fptr, 1, 20, 78, uchars, &status);
    uchars[78] = '\0';
    printf("%s\n", uchars);
    ffptbb(fptr, 1, 20, 78, uchars, &status);

    /*
      #########################################
      #  get information about the columns    #
      #########################################
    */

    ffgcno(fptr, 0, "name", &colnum, &status);
    printf("\nColumn name is number %d; status = %d.\n", colnum, status);

    while (status != COL_NOT_FOUND)
    {
      ffgcnn(fptr, 1, "*ue", colname, &colnum, &status);
      printf("Column %s is number %d; status = %d.\n", 
           colname, colnum, status);
    }
    status = 0;

    for (ii = 0; ii < tfields; ii++)
    {
      ffgtcl(fptr, ii + 1, &typecode, &repeat, &width, &status);
      printf("%4s %3d %2d %2d", tform[ii], typecode, repeat, width);
      ffgacl(fptr, ii + 1, ttype[0], tbcol, tunit[0], tform[0], &scale,
           &zero, nulstr, tdisp, &status);
      printf(" %s, %d, %s, %s, %f, %f, %s, %s.\n",
         ttype[0], tbcol[0], tunit[0], tform[0], scale, zero,
         nulstr, tdisp);
    }

    printf("\n");

    /*
      ###############################################
      #  test the insert/delete row/column routines #
      ###############################################
    */
    if (ffirow(fptr, 2, 3, &status) > 0)
        goto errstatus;

    nrows = 14;
    ffgcvs(fptr, 1, 1, 1, nrows, "UNDEFINED", inskey,   &anynull, &status);
    ffgcvb(fptr, 2, 1, 1, nrows, 99, binarray, &anynull, &status);
    ffgcvi(fptr, 2, 1, 1, nrows, 99, iinarray, &anynull, &status);
    ffgcvj(fptr, 3, 1, 1, nrows, 99, jinarray, &anynull, &status);
    ffgcve(fptr, 4, 1, 1, nrows, 99., einarray, &anynull, &status);
    ffgcvd(fptr, 5, 1, 1, nrows, 99., dinarray, &anynull, &status);


    printf("\nData values after inserting 3 rows after row 2:\n");
    for (ii = 0; ii < nrows; ii++)
    {
      printf("%15s %2d %2d %2d %4.1f %4.1f\n",  inskey[ii], binarray[ii],
          iinarray[ii], jinarray[ii], einarray[ii], dinarray[ii]);
    }

    if (ffdrow(fptr, 10, 2, &status) > 0)
        goto errstatus;

    nrows = 12;
    ffgcvs(fptr, 1, 1, 1, nrows, "UNDEFINED", inskey,   &anynull, &status);
    ffgcvb(fptr, 2, 1, 1, nrows, 99, binarray, &anynull, &status);
    ffgcvi(fptr, 2, 1, 1, nrows, 99, iinarray, &anynull, &status);
    ffgcvj(fptr, 3, 1, 1, nrows, 99, jinarray, &anynull, &status);
    ffgcve(fptr, 4, 1, 1, nrows, 99., einarray, &anynull, &status);
    ffgcvd(fptr, 5, 1, 1, nrows, 99., dinarray, &anynull, &status);

    printf("\nData values after deleting 2 rows at row 10:\n");
    for (ii = 0; ii < nrows; ii++)
    {
      printf("%15s %2d %2d %2d %4.1f %4.1f\n",  inskey[ii], binarray[ii],
          iinarray[ii], jinarray[ii], einarray[ii], dinarray[ii]);
    }
    if (ffdcol(fptr, 3, &status) > 0)
        goto errstatus;

    ffgcvs(fptr, 1, 1, 1, nrows, "UNDEFINED", inskey,   &anynull, &status);
    ffgcvb(fptr, 2, 1, 1, nrows, 99, binarray, &anynull, &status);
    ffgcvi(fptr, 2, 1, 1, nrows, 99, iinarray, &anynull, &status);
    ffgcve(fptr, 3, 1, 1, nrows, 99., einarray, &anynull, &status);
    ffgcvd(fptr, 4, 1, 1, nrows, 99., dinarray, &anynull, &status);

    printf("\nData values after deleting column 3:\n");
    for (ii = 0; ii < nrows; ii++)
    {
      printf("%15s %2d %2d %4.1f %4.1f\n", inskey[ii], binarray[ii],
          iinarray[ii], einarray[ii], dinarray[ii]);
    }

    if (fficol(fptr, 5, "INSERT_COL", "F14.6", &status) > 0)
        goto errstatus;

    ffgcvs(fptr, 1, 1, 1, nrows, "UNDEFINED", inskey,   &anynull, &status);
    ffgcvb(fptr, 2, 1, 1, nrows, 99, binarray, &anynull, &status);
    ffgcvi(fptr, 2, 1, 1, nrows, 99, iinarray, &anynull, &status);
    ffgcve(fptr, 3, 1, 1, nrows, 99., einarray, &anynull, &status);
    ffgcvd(fptr, 4, 1, 1, nrows, 99., dinarray, &anynull, &status);
    ffgcvj(fptr, 5, 1, 1, nrows, 99, jinarray, &anynull, &status);

    printf("\nData values after inserting column 5:\n");
    for (ii = 0; ii < nrows; ii++)
    {
      printf("%15s %2d %2d %4.1f %4.1f %d\n", inskey[ii], binarray[ii],
          iinarray[ii], einarray[ii], dinarray[ii] , jinarray[ii]);
    }

    /*
      ################################
      #  read data from binary table #
      ################################
    */
    if (ffmrhd(fptr, 1, &hdutype, &status) > 0)
        goto errstatus;

    printf("\nMoved to BINTABLE; headend, datastart = %d %d\n",
        fptr->headend, fptr->datastart);

    ffghsp(fptr, &existkeys, &morekeys, &status);
    printf("header contains %d keywords with room for %d more\n",existkeys,
            morekeys);

    ffghbn(fptr, 99, &nrows, &tfields, ttype, 
           tform, tunit, binname, &pcount, &status);

    printf("\nBinary table: nrows, tfields, extname, pcount: %d %d %s %d\n",
           nrows, tfields, binname, pcount);

    for (ii = 0; ii < tfields; ii++)
      printf("%8s %8s %8s \n", ttype[ii], tform[ii], tunit[ii]);

    for (ii = 0; ii < 40; ii++)
        larray[ii] = 0;

    printf("\nData values read from binary table:\n");
    printf("  Bit column (X) data values: \n\n");

    ffgcx(fptr, 3, 1, 1, 36, larray, &status);
    for (jj = 0; jj < 5; jj++)
    {
      for (ii = 0; ii < 8; ii++)
        printf("%1d",larray[jj * 8 + ii]);
      printf(" ");
    }

    printf("\n\n");

    nrows = 21;
    ffgcvs(fptr, 1, 1, 1, nrows, "NOT DEFINED",  inskey,   &anynull, &status);
    ffgcl( fptr, 2, 1, 1, nrows, larray, &status);
    ffgcvb(fptr, 3, 1, 1, nrows, 98, xinarray, &anynull, &status);
    ffgcvb(fptr, 4, 1, 1, nrows, 98, binarray, &anynull, &status);
    ffgcvi(fptr, 5, 1, 1, nrows, 98, iinarray, &anynull, &status);
    ffgcvj(fptr, 6, 1, 1, nrows, 98, jinarray, &anynull, &status);
    ffgcve(fptr, 7, 1, 1, nrows, 98., einarray, &anynull, &status);
    ffgcvd(fptr, 8, 1, 1, nrows, 98., dinarray, &anynull, &status);
    ffgcvc(fptr, 9, 1, 1, nrows, 98., cinarray, &anynull, &status);
    ffgcvm(fptr, 10, 1, 1, nrows, 98., minarray, &anynull, &status);

    printf("\nRead columns with ffgcv_:\n");
    for (ii = 0; ii < nrows; ii++)
    {
  printf("%15s %d %3d %2d %2d %2d %4.1f %4.1f (%4.1f,%4.1f) (%4.1f,%4.1f) \n",
        inskey[ii], larray[ii], xinarray[ii], binarray[ii], iinarray[ii], 
        jinarray[ii], einarray[ii], dinarray[ii], cinarray[ii * 2], 
        cinarray[ii * 2 + 1], minarray[ii * 2], minarray[ii * 2 + 1]);
    }

    for (ii = 0; ii < nrows; ii++)
    {
      larray[ii] = 0;
      xinarray[ii] = 0;
      binarray[ii] = 0;
      iinarray[ii] = 0; 
      jinarray[ii] = 0;
      einarray[ii] = 0.; 
      dinarray[ii] = 0.;
      cinarray[ii * 2] = 0.; 
      minarray[ii * 2] = 0.;
      cinarray[ii * 2 + 1] = 0.; 
      minarray[ii * 2 + 1] = 0.;
    }

    ffgcfs(fptr, 1, 1, 1, nrows, inskey,   larray2, &anynull, &status);
    ffgcfl(fptr, 2, 1, 1, nrows, larray,   larray2, &anynull, &status);
    ffgcfb(fptr, 3, 1, 1, nrows, xinarray, larray2, &anynull, &status);
    ffgcfb(fptr, 4, 1, 1, nrows, binarray, larray2, &anynull, &status);
    ffgcfi(fptr, 5, 1, 1, nrows, iinarray, larray2, &anynull, &status);
    ffgcfj(fptr, 6, 1, 1, nrows, jinarray, larray2, &anynull, &status);
    ffgcfe(fptr, 7, 1, 1, nrows, einarray, larray2, &anynull, &status);
    ffgcfd(fptr, 8, 1, 1, nrows, dinarray, larray2, &anynull, &status);
    ffgcfc(fptr, 9, 1, 1, nrows, cinarray, larray2, &anynull, &status);
    ffgcfm(fptr, 10, 1, 1, nrows, minarray, larray2, &anynull, &status);

    printf("\nRead columns with ffgcf_:\n");
    for (ii = 0; ii < 10; ii++)
    {
    
    printf("%15s %d %3d %2d %2d %2d %4.1f %4.1f (%4.1f,%4.1f) (%4.1f,%4.1f)\n",
        inskey[ii], larray[ii], xinarray[ii], binarray[ii], iinarray[ii], 
        jinarray[ii], einarray[ii], dinarray[ii], cinarray[ii * 2], 
        cinarray[ii * 2 + 1], minarray[ii * 2], minarray[ii * 2 + 1]);
    }
    for (ii = 10; ii < nrows; ii++)
    {
      /* don't try to print the NaN values */
      printf("%15s %d %3d %2d %2d %2d \n",
        inskey[ii], larray[ii], xinarray[ii], binarray[ii], iinarray[ii], 
        jinarray[ii]);
    }
    ffprec(fptr, 
    "key_prec= 'This keyword was written by ffprec' / comment here", &status);

    /*
      ###############################################
      #  test the insert/delete row/column routines #
      ###############################################
    */
    if (ffirow(fptr, 2, 3, &status) > 0)
        goto errstatus;

    nrows = 14;
    ffgcvs(fptr, 1, 1, 1, nrows, "NOT DEFINED",  inskey,   &anynull, &status);
    ffgcvb(fptr, 4, 1, 1, nrows, 98, binarray, &anynull, &status);
    ffgcvi(fptr, 5, 1, 1, nrows, 98, iinarray, &anynull, &status);
    ffgcvj(fptr, 6, 1, 1, nrows, 98, jinarray, &anynull, &status);
    ffgcve(fptr, 7, 1, 1, nrows, 98., einarray, &anynull, &status);
    ffgcvd(fptr, 8, 1, 1, nrows, 98., dinarray, &anynull, &status);

    printf("\nData values after inserting 3 rows after row 2:\n");
    for (ii = 0; ii < nrows; ii++)
    {
      printf("%15s %2d %2d %2d %4.1f %4.1f\n",  inskey[ii], binarray[ii],
          iinarray[ii], jinarray[ii], einarray[ii], dinarray[ii]);
    }

    if (ffdrow(fptr, 10, 2, &status) > 0)
        goto errstatus;

    nrows = 12;
    ffgcvs(fptr, 1, 1, 1, nrows, "NOT DEFINED",  inskey,   &anynull, &status);
    ffgcvb(fptr, 4, 1, 1, nrows, 98, binarray, &anynull, &status);
    ffgcvi(fptr, 5, 1, 1, nrows, 98, iinarray, &anynull, &status);
    ffgcvj(fptr, 6, 1, 1, nrows, 98, jinarray, &anynull, &status);
    ffgcve(fptr, 7, 1, 1, nrows, 98., einarray, &anynull, &status);
    ffgcvd(fptr, 8, 1, 1, nrows, 98., dinarray, &anynull, &status);

    printf("\nData values after deleting 2 rows at row 10:\n");
    for (ii = 0; ii < nrows; ii++)
    {
      printf("%15s %2d %2d %2d %4.1f %4.1f\n",  inskey[ii], binarray[ii],
          iinarray[ii], jinarray[ii], einarray[ii], dinarray[ii]);
    }

    if (ffdcol(fptr, 6, &status) > 0)
        goto errstatus;

    ffgcvs(fptr, 1, 1, 1, nrows, "NOT DEFINED",  inskey,   &anynull, &status);
    ffgcvb(fptr, 4, 1, 1, nrows, 98, binarray, &anynull, &status);
    ffgcvi(fptr, 5, 1, 1, nrows, 98, iinarray, &anynull, &status);
    ffgcve(fptr, 6, 1, 1, nrows, 98., einarray, &anynull, &status);
    ffgcvd(fptr, 7, 1, 1, nrows, 98., dinarray, &anynull, &status);

    printf("\nData values after deleting column 6:\n");
    for (ii = 0; ii < nrows; ii++)
    {
      printf("%15s %2d %2d %4.1f %4.1f\n", inskey[ii], binarray[ii],
          iinarray[ii], einarray[ii], dinarray[ii]);
    }

    if (fficol(fptr, 8, "INSERT_COL", "1E", &status) > 0)
        goto errstatus;

    ffgcvs(fptr, 1, 1, 1, nrows, "NOT DEFINED",  inskey,   &anynull, &status);
    ffgcvb(fptr, 4, 1, 1, nrows, 98, binarray, &anynull, &status);
    ffgcvi(fptr, 5, 1, 1, nrows, 98, iinarray, &anynull, &status);
    ffgcve(fptr, 6, 1, 1, nrows, 98., einarray, &anynull, &status);
    ffgcvd(fptr, 7, 1, 1, nrows, 98., dinarray, &anynull, &status);
    ffgcvj(fptr, 8, 1, 1, nrows, 98, jinarray, &anynull, &status);

    printf("\nData values after inserting column 8:\n");
    for (ii = 0; ii < nrows; ii++)
    {
      printf("%15s %2d %2d %4.1f %4.1f %d\n", inskey[ii], binarray[ii],
          iinarray[ii], einarray[ii], dinarray[ii] , jinarray[ii]);
    }

    /*
      ####################################################
      #  insert binary table following the primary array #
      ####################################################
    */

    ffmahd(fptr,  1, &hdutype, &status);

    for (ii = 0; ii < 10; ii++)
    {
      ttype[ii] = (char *) malloc(20);
      tform[ii] = (char *) malloc(20);
      tunit[ii] = (char *) malloc(20);
    }

    strcpy(tform[0], "15A");
    strcpy(tform[1], "1L");
    strcpy(tform[2], "16X");
    strcpy(tform[3], "1B");
    strcpy(tform[4], "1I");
    strcpy(tform[5], "1J");
    strcpy(tform[6], "1E");
    strcpy(tform[7], "1D");
    strcpy(tform[8], "1C");
    strcpy(tform[9], "1M");

    strcpy(ttype[0], "Avalue");
    strcpy(ttype[1], "Lvalue");
    strcpy(ttype[2], "Xvalue");
    strcpy(ttype[3], "Bvalue");
    strcpy(ttype[4], "Ivalue");
    strcpy(ttype[5], "Jvalue");
    strcpy(ttype[6], "Evalue");
    strcpy(ttype[7], "Dvalue");
    strcpy(ttype[8], "Cvalue");
    strcpy(ttype[9], "Mvalue");

    strcpy(tunit[0], "");
    strcpy(tunit[1], "m**2");
    strcpy(tunit[2], "cm");
    strcpy(tunit[3], "erg/s");
    strcpy(tunit[4], "km/s");
    strcpy(tunit[5], "");
    strcpy(tunit[6], "");
    strcpy(tunit[7], "");
    strcpy(tunit[8], "");
    strcpy(tunit[9], "");

    nrows = 20;
    tfields = 10;
    pcount = 0;

    ffibin(fptr, nrows, tfields, ttype, tform, tunit, binname, pcount,
            &status);
    printf("ffibin status = %d\n", status);


    ffpkyj(fptr, "TNULL4", 77, "value for undefined pixels", &status);
    ffpkyj(fptr, "TNULL5", 77, "value for undefined pixels", &status);
    ffpkyj(fptr, "TNULL6", 77, "value for undefined pixels", &status);

    ffpkyj(fptr, "TSCAL4", 1000, "scaling factor", &status);
    ffpkyj(fptr, "TSCAL5", 1, "scaling factor", &status);
    ffpkyj(fptr, "TSCAL6", 100, "scaling factor", &status);

    ffpkyj(fptr, "TZERO4", 0, "scaling offset", &status);
    ffpkyj(fptr, "TZERO5", 32768, "scaling offset", &status);
    ffpkyj(fptr, "TZERO6", 100, "scaling offset", &status);

    fftnul(fptr, 4, 77, &status);   /* define null value for int cols */
    fftnul(fptr, 5, 77, &status);
    fftnul(fptr, 6, 77, &status);
    /* set scaling */
    fftscl(fptr, 4, 1000., 0., &status);   
    fftscl(fptr, 5, 1., 32768., &status);
    fftscl(fptr, 6, 100., 100., &status);

    /*
      ############################
      #  write data to columns   #
      ############################
    */

    /* initialize arrays of values to write to table */
 
    joutarray[0] = 0;
    joutarray[1] = 1000;
    joutarray[2] = 10000;
    joutarray[3] = 32768;
    joutarray[4] = 65535;


    for (ii = 4; ii < 7; ii++)
    {
        ffpclj(fptr, ii, 1, 1, 5, joutarray, &status); 
        if (status == NUM_OVERFLOW)
        {
            printf("Overflow writing to column %d\n", ii);
        }

        ffpclu(fptr, ii, 6, 1, 1, &status);  /* write null value */
    }

    for (jj = 4; jj < 7; jj++)
    {
      ffgcvj(fptr, jj, 1, 1, 6, -999, jinarray, &anynull, &status);
      for (ii = 0; ii < 6; ii++)
      {
        printf(" %6d", jinarray[ii]);
      }
      printf(" status = %d\n", status);
    }

    printf("\n");
    /* turn off scaling, and read the unscaled values */
    fftscl(fptr, 4, 1., 0., &status);   
    fftscl(fptr, 5, 1., 0., &status);
    fftscl(fptr, 6, 1., 0., &status);

    for (jj = 4; jj < 7; jj++)
    {
      ffgcvj(fptr, jj, 1, 1, 6, -999, jinarray, &anynull, &status);
      for (ii = 0; ii < 6; ii++)
      {
        printf(" %6d", jinarray[ii]);
      }
      printf(" status = %d\n", status);
      status = 0;
    }

    /*
      ######################################################
      #  insert image extension following the binary table #
      ######################################################
    */

    bitpix = -32;
    naxis = 2;
    naxes[0] = 15;
    naxes[1] = 25;
    ffiimg(fptr, bitpix, naxis, naxes, &status);
    printf("\nCreate image extension: ffiimg status = %d\n", status);

    for (jj = 0; jj < 30; jj++)
    {
      for (ii = 0; ii < 19; ii++)
      {
        imgarray[jj][ii] = (jj * 10) + ii;
      }
    }

    ffp2di(fptr, 1, 19, naxes[0], naxes[1], imgarray[0], &status);
    printf("\nWrote whole 2D array: ffp2di status = %d\n", status);

    for (jj = 0; jj < 30; jj++)
    {
      for (ii = 0; ii < 19; ii++)
      {
        imgarray[jj][ii] = 0;
      }
    }
    
    ffg2di(fptr, 1, 0, 19, naxes[0], naxes[1], imgarray[0], &anynull,
           &status);
    printf("\nRead whole 2D array: ffg2di status = %d\n", status);

    for (jj = 0; jj < 30; jj++)
    {
      for (ii = 0; ii < 19; ii++)
      {
        printf(" %3d", imgarray[jj][ii]);
      }
      printf("\n");
    }

    for (jj = 0; jj < 30; jj++)
    {
      for (ii = 0; ii < 19; ii++)
      {
        imgarray[jj][ii] = 0;
      }
    }
    
    for (jj = 0; jj < 20; jj++)
    {
      for (ii = 0; ii < 10; ii++)
      {
        imgarray2[jj][ii] = (jj * -10) - ii;
      }
    }

    fpixels[0] = 5;
    fpixels[1] = 5;
    lpixels[0] = 14;
    lpixels[1] = 14;
    ffpssi(fptr, 1, naxis, naxes, fpixels, lpixels, 
         imgarray2[0], &status);
    printf("\nWrote subset 2D array: ffpssi status = %d\n", status);

    ffg2di(fptr, 1, 0, 19, naxes[0], naxes[1], imgarray[0], &anynull,
           &status);
    printf("\nRead whole 2D array: ffg2di status = %d\n", status);

    for (jj = 0; jj < 30; jj++)
    {
      for (ii = 0; ii < 19; ii++)
      {
        printf(" %3d", imgarray[jj][ii]);
      }
      printf("\n");
    }

    fpixels[0] = 2;
    fpixels[1] = 5;
    lpixels[0] = 10;
    lpixels[1] = 8;
    inc[0] = 2;
    inc[1] = 3;

    for (jj = 0; jj < 30; jj++)
    {
      for (ii = 0; ii < 19; ii++)
      {
        imgarray[jj][ii] = 0;
      }
    }
    
    ffgsvi(fptr, 1, naxis, naxes, fpixels, lpixels, inc, 0,
          imgarray[0], &anynull, &status);
    printf("\nRead subset of 2D array: ffgsvi status = %d\n", status);

    for (ii = 0; ii < 10; ii++)
    {
        printf(" %3d", imgarray[0][ii]);
    }
    printf("\n");

    /*
      ###########################################################
      #  insert another image extension                         #
      #  copy the image extension to primary array of tmp file. #
      #  then delete the tmp file, and the image extension      #
      ###########################################################
    */
    bitpix = 16;
    naxis = 2;
    naxes[0] = 15;
    naxes[1] = 25;
    ffiimg(fptr, bitpix, naxis, naxes, &status);
    printf("\nCreate image extension: ffiimg status = %d\n", status);

    strcpy(filename, "t1q2s3v4.tmp");
    ffinit(&tmpfile, filename, &status);
    printf("Create temporary file: ffinit status = %d\n", status);

    ffcopy(fptr, tmpfile, 0, &status);
    printf("Copy image extension to primary array of tmp file.");
    printf(" ffcopy status = %d\n", status);

    ffgrec(tmpfile, 1, card, &status);
    printf("%s\n", card);
    ffgrec(tmpfile, 2, card, &status);
    printf("%s\n", card);
    ffgrec(tmpfile, 3, card, &status);
    printf("%s\n", card);
    ffgrec(tmpfile, 4, card, &status);
    printf("%s\n", card);
    ffgrec(tmpfile, 5, card, &status);
    printf("%s\n", card);
    ffgrec(tmpfile, 6, card, &status);
    printf("%s\n", card);

    ffdelt(tmpfile, &status);
    printf("Delete the tmp file: ffdelt status = %d\n", status);

    ffdhdu(fptr, &hdutype, &status);
    printf("Delete the image extension; hdutype, status = %d %d\n",
             hdutype, status);

    /*
      ###########################################################
      #  append bintable extension with variable length columns #
      ###########################################################
    */

    ffcrhd(fptr, &status);
    printf("ffcrhd status = %d\n", status);

    for (ii = 0; ii < 10; ii++)
    {
      ttype[ii] = (char *) malloc(20);
      tform[ii] = (char *) malloc(20);
      tunit[ii] = (char *) malloc(20);
    }

    strcpy(tform[0], "1PA");
    strcpy(tform[1], "1PL");
    strcpy(tform[2], "1PX");
    strcpy(tform[3], "1PB");
    strcpy(tform[4], "1PI");
    strcpy(tform[5], "1PJ");
    strcpy(tform[6], "1PE");
    strcpy(tform[7], "1PD");
    strcpy(tform[8], "1PC");
    strcpy(tform[9], "1PM");

    strcpy(ttype[0], "Avalue");
    strcpy(ttype[1], "Lvalue");
    strcpy(ttype[2], "Xvalue");
    strcpy(ttype[3], "Bvalue");
    strcpy(ttype[4], "Ivalue");
    strcpy(ttype[5], "Jvalue");
    strcpy(ttype[6], "Evalue");
    strcpy(ttype[7], "Dvalue");
    strcpy(ttype[8], "Cvalue");
    strcpy(ttype[9], "Mvalue");

    strcpy(tunit[0], "");
    strcpy(tunit[1], "m**2");
    strcpy(tunit[2], "cm");
    strcpy(tunit[3], "erg/s");
    strcpy(tunit[4], "km/s");
    strcpy(tunit[5], "");
    strcpy(tunit[6], "");
    strcpy(tunit[7], "");
    strcpy(tunit[8], "");
    strcpy(tunit[9], "");

    nrows = 20;
    tfields = 10;
    pcount = 0;

    ffphbn(fptr, nrows, tfields, ttype, tform, tunit, binname, pcount,
            &status);
    printf("Variable length arrays: ffphbn status = %d\n", status);
    ffpkyj(fptr, "TNULL4", 88, "value for undefined pixels", &status);
    ffpkyj(fptr, "TNULL5", 88, "value for undefined pixels", &status);
    ffpkyj(fptr, "TNULL6", 88, "value for undefined pixels", &status);

    /*
      ############################
      #  write data to columns   #
      ############################
    */

    /* initialize arrays of values to write to table */
    strcpy(iskey,"abcdefghijklmnopqrst");

    for (ii = 0; ii < 20; ii++)
    {
        boutarray[ii] = ii + 1;
        ioutarray[ii] = ii + 1;
        joutarray[ii] = ii + 1;
        eoutarray[ii] = ii + 1;
        doutarray[ii] = ii + 1;
    }

    larray[0] = 0;
    larray[1] = 1;
    larray[2] = 0;
    larray[3] = 0;
    larray[4] = 1;
    larray[5] = 1;
    larray[6] = 0;
    larray[7] = 0;
    larray[8] = 0;
    larray[9] = 1;
    larray[10] = 1;
    larray[11] = 1;
    larray[12] = 0;
    larray[13] = 0;
    larray[14] = 0;
    larray[15] = 0;
    larray[16] = 1;
    larray[17] = 1;
    larray[18] = 1;
    larray[19] = 1;

      strncpy(inskey[0], iskey, 1);
      inskey[0][1] = '\0';
      ffpcls(fptr, 1, 1, 1, 1, inskey, &status);  /* write string values */
      ffpcll(fptr, 2, 1, 1, 1, larray, &status);  /* write logicals */
      ffpclx(fptr, 3, 1, 1, 1, larray, &status);  /* write bits */
      ffpclb(fptr, 4, 1, 1, 1, boutarray, &status);
      ffpcli(fptr, 5, 1, 1, 1, ioutarray, &status); 
      ffpclj(fptr, 6, 1, 1, 1, joutarray, &status); 
      ffpcle(fptr, 7, 1, 1, 1, eoutarray, &status);
      ffpcld(fptr, 8, 1, 1, 1, doutarray, &status);
    for (ii = 2; ii <= 20; ii++)   /* loop over rows 1 - 20 */
    {
      strncpy(inskey[0], iskey, ii);
      inskey[0][ii] = '\0';
      ffpcls(fptr, 1, ii, 1, 1, inskey, &status);  /* write string values */

      ffpcll(fptr, 2, ii, 1, ii, larray, &status);  /* write logicals */
      ffpclu(fptr, 2, ii, ii-1, 1, &status);

      ffpclx(fptr, 3, ii, 1, ii, larray, &status);  /* write bits */

      ffpclb(fptr, 4, ii, 1, ii, boutarray, &status);
      ffpclu(fptr, 4, ii, ii-1, 1, &status);

      ffpcli(fptr, 5, ii, 1, ii, ioutarray, &status); 
      ffpclu(fptr, 5, ii, ii-1, 1, &status);

      ffpclj(fptr, 6, ii, 1, ii, joutarray, &status); 
      ffpclu(fptr, 6, ii, ii-1, 1, &status);

      ffpcle(fptr, 7, ii, 1, ii, eoutarray, &status);
      ffpclu(fptr, 7, ii, ii-1, 1, &status);

      ffpcld(fptr, 8, ii, 1, ii, doutarray, &status);
      ffpclu(fptr, 8, ii, ii-1, 1, &status);
    }
    printf("ffpcl_ status = %d\n", status);
    /*
      #################################
      #  close then reopen this HDU   #
      #################################
    */

     ffmrhd(fptr, -1, &hdutype, &status);
     ffmrhd(fptr,  1, &hdutype, &status);

    /*
      #############################
      #  read data from columns   #
      #############################
    */

    ffgkyj(fptr, "PCOUNT", &pcount, comm, &status);
    printf("PCOUNT = %d\n", pcount);

    /* initialize the variables to be read */
    strcpy(inskey[0]," ");
    strcpy(iskey," ");

    for (ii = 0; ii < 20; ii++)
    {
        larray[ii] = 0;
        boutarray[ii] = 0;
        ioutarray[ii] = 0;
        joutarray[ii] = 0;
        eoutarray[ii] = 0;
        doutarray[ii] = 0;
    }

    printf("HDU number = %d\n", ffghdn(fptr, &hdunum));
    for (ii = 1; ii <= 20; ii++)   /* loop over rows 1 - 20 */
    {
      ffgcvs(fptr, 1, ii, 1, 1, iskey, inskey, &anynull, &status);  
      printf("A %s %d\nL", inskey[0], status);

      ffgcl( fptr, 2, ii, 1, ii, larray, &status); 
      for (jj = 0; jj < ii; jj++)
        printf(" %2d", larray[jj]);
      printf(" %d\nX", status);

      ffgcx(fptr, 3, ii, 1, ii, larray, &status);
      for (jj = 0; jj < ii; jj++)
        printf(" %2d", larray[jj]);
      printf(" %d\nB", status);

      ffgcvb(fptr, 4, ii, 1, ii, 99, boutarray, &anynull, &status);
      for (jj = 0; jj < ii; jj++)
        printf(" %2d", boutarray[jj]);
      printf(" %d\nI", status);

      ffgcvi(fptr, 5, ii, 1, ii, 99, ioutarray, &anynull, &status); 
      for (jj = 0; jj < ii; jj++)
        printf(" %2d", ioutarray[jj]);
      printf(" %d\nJ", status);

      ffgcvj(fptr, 6, ii, 1, ii, 99, joutarray, &anynull, &status); 
      for (jj = 0; jj < ii; jj++)
        printf(" %2d", joutarray[jj]);
      printf(" %d\nE", status);

      ffgcve(fptr, 7, ii, 1, ii, 99., eoutarray, &anynull, &status);
      for (jj = 0; jj < ii; jj++)
        printf(" %2.0f", eoutarray[jj]);
      printf(" %d\nD", status);

      ffgcvd(fptr, 8, ii, 1, ii, 99., doutarray, &anynull, &status);
      for (jj = 0; jj < ii; jj++)
        printf(" %2.0f", doutarray[jj]);
      printf(" %d\n", status);

      ffgdes(fptr, 8, ii, &repeat, &offset, &status);
      printf("Column 8 repeat and offset = %d %d\n", repeat, offset);
    }

    /*
      ###########################################################
      #  perform stress test by cycling thru all the extensions #
      ###########################################################
    */
    printf("\nRepeatedly move to the 1st 4 HDUs of the file:\n");
    for (ii = 0; ii < 10; ii++)
    {
      ffmahd(fptr,  1, &hdutype, &status);
      printf("%d", ffghdn(fptr, &hdunum));
      ffmrhd(fptr,  1, &hdutype, &status);
      printf("%d", ffghdn(fptr, &hdunum));
      ffmrhd(fptr,  1, &hdutype, &status);
      printf("%d", ffghdn(fptr, &hdunum));
      ffmrhd(fptr,  1, &hdutype, &status);
      printf("%d", ffghdn(fptr, &hdunum));
      ffmrhd(fptr, -1, &hdutype, &status);
      printf("%d", ffghdn(fptr, &hdunum));
      if (status > 0)
         break;
    }
    printf("\n");

    checksum = 1426738146;
    ffesum(checksum, 0, asciisum);
    printf("\nEncode checksum: %u -> %s\n", checksum, asciisum);
    checksum = 0;
    ffdsum(asciisum, 0, &checksum);
    printf("Decode checksum: %s -> %u\n", asciisum, checksum);

    /*
      ############################
      #  close file and quit     #
      ############################
    */

 errstatus:  /* jump here on error */

    ffclos(fptr, &status);
    printf("ffclos status = %d\n", status);

    ffgerr(status, errmsg);
    printf("\nStatus = %d: %s\n", status, errmsg);

    ffgmsg(errmsg);

    if (errmsg[0])
        printf("\n***** Error message stack: *****\n");

    while (errmsg[0])
    {
        printf(" %s\n", errmsg);
        ffgmsg(errmsg);
    }
    return(0);
}
