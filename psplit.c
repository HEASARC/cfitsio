/*    /redshift/sgi6/lheavc/ftools/cfitsio/psplit.c,v 1.3 1996/05/29 18:19:48 dunfee Exp

     jdd May 1996

  psplit (Pence split) -- splits a file up into smaller parcels according
  a keyword: split_key.
   
  Usage: "psplit bigfile1.c bigfile2.c bigfile3.c ..."

  where bigfile*.c is in the form:

          #include | define | ...
          # ...
   
          split_key
  
            routine1

          split_key

            routine2

                 etc...


   Everything before the first split_key goes into a file "<bigfilename>.h".
   Each routine is written to a file "bigfilenameXX.c" where XX is 01, 02,
   03, etc. At the beginning of each bigfilenameXX.c is a line to include the
   stripped preprocs': "#include <bigfilename>.h"


  Important Points:

  1) Every time the split_key is encountered a new c file is created.
  
  2) The split_key must begin the line.

  
*/


#include <stdio.h>
#include <string.h>
#define ENOFILE 12 /* error number if fopen doesn't work */
#define line_size 256 /* max chars line buffer can hold */
#define name_size 80 /* longest filename for an argument */
#define header_size 100
#define split_key "/*-------------" /* this is what to split on */

main(int argc, char **argv)
{

	FILE * bigfile, *c_temp, *h_temp;

	char	*argv0 = *argv; /* Get program name */
	char	*current_line = (char *)malloc(line_size);
	char	*c_name = (char *)malloc(name_size);
	char    *h_name = (char *)malloc(name_size);
	char    *common_header = (char *)malloc(header_size);
	char    *base_name = (char *)malloc(name_size);

	size_t     n;
	int	c_num;
	int     key_size = strlen(split_key);


	/*  Cycle through passed arguments until done. */

	while( *(++argv) != NULL) {

		c_num =1; /* reset temp file number for each group */
		/* clear out strings */
		memset(c_name, 0, name_size);
		memset(h_name, 0, name_size);
		memset(common_header, 0, header_size);
		memset(base_name, 0, name_size);

		if ( (bigfile = fopen(*argv, "r")) == NULL) {
			fprintf(stderr, " %s : couldn't open %s \n", argv0, *argv);
			exit(ENOFILE);
		}

		/* Strip argument of the .c if it has it. base_name holds good name.  */
		if ( (n = strcspn(*argv, ".")) != (strlen(*argv)+1 )) {
			strncpy(base_name, *argv, n);
		} else {
			base_name = *argv; /* no .c so pass along unchanged. */
		}
		printf(" %s: Working on: %s\n", argv0, base_name);
		sprintf(h_name, "%s.h", base_name);
		sprintf(common_header, "#include \"%s\" \n ", h_name);

		if ((h_temp = fopen(h_name, "w")) == NULL) {
			fprintf(stderr, " %s : couldn't open %s \n", argv0, h_name);
			exit(ENOFILE);
		}


		/* dump everything up to the first split_key into .h */
		while ( (fgets(current_line, line_size, bigfile) != NULL) && 
                        (memcmp(current_line, split_key, key_size) != 0)     ) {
				fputs(current_line, h_temp);
		}
		fclose(h_temp); /* done with header file */

		sprintf(c_name, "%s%02d.c", base_name, c_num++);

		/* start dumping to c files  */
		if ((c_temp = fopen(c_name, "w")) == NULL) {
                        fprintf(stderr, " %s : couldn't open %s \n", argv0, c_name);
			exit(ENOFILE);
		}

		/* start 001 off with header include and last split_key line */
		fputs(common_header, c_temp);
		fputs(current_line, c_temp);

		while ( fgets(current_line, line_size, bigfile) != NULL) {
			if (memcmp(current_line, split_key, key_size) == 0) {
				fclose(c_temp);
				sprintf(c_name, "%s%02d.c", base_name, c_num++);
				if ((c_temp = fopen(c_name, "w")) == NULL) {
					fprintf(stderr, " %s : couldn't open %s \n", argv0, h_name);
					exit(ENOFILE);
				}

				fputs(common_header, c_temp);
				fputs(current_line, c_temp);

			} else {
				fputs(current_line, c_temp);
			}
		}

		fclose(c_temp);
		fclose(bigfile);

	}

	free(current_line);
	free(c_name);
	free(h_name);
	free(common_header);
	free(base_name);
	exit(0);

}



