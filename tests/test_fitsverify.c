/*
 * Tests for the fitsverify utility (utilities/fvrf_*.c).
 *
 * Two kinds of test live here.  Routines that can be driven on their own -
 * the keyword value parsers, the error reporting - are called in process:
 * their source file is #included, and the handful of symbols the rest of
 * fitsverify would have provided are stubbed below.  Defects that only show
 * up on a whole file are exercised by writing that file and running the
 * fitsverify binary over it.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "fitsio.h"
#include "test_macros.h"

/* Globals that utilities/ftverify.c owns in the real program. */
int prhead = 0;
int testdata = 1;
int testfill = 1;
int testcsum = 1;
int totalhdu = 0;
int err_report = 0;
int heasarc_conv = 1;
int testhierarch = 0;
int prstat = 0;

/* Only reached after MAXERRORS reports, which these tests stay well under. */
void update_parfile(int nerr, int nwrn) { (void)nerr; (void)nwrn; }
void close_report(FILE *out) { (void)out; }

#include "../utilities/fvrf_key.c"
#include "../utilities/fvrf_misc.c"

/*--------------------------------------------------------------------------
 * get_cmp: complex keyword values (utilities/fvrf_key.c)
 *------------------------------------------------------------------------*/

/* Parse a complex keyword value and return the accumulated error mask. */
static unsigned long
parse_cmp(const char *value, char *kvalue, kwdtyp *ktype)
{
	char card[FLEN_CARD];
	char *p = card;
	unsigned long stat = 0;

	memset(card, 0, sizeof(card));
	strcpy(card, value);
	kvalue[0] = '\0';
	*ktype = UNKNOWN;
	get_cmp(&p, kvalue, ktype, &stat);
	return stat;
}

static void
test_complex_keyword_values(void)
{
	char kvalue[FLEN_CARD];
	kwdtyp ktype;
	unsigned long stat;

	/* A well formed integer complex value. */
	stat = parse_cmp("(1,2)", kvalue, &ktype);
	fail_if(stat != 0);
	fail_if(ktype != CMI_KEY);

	/* A well formed floating point complex value. */
	stat = parse_cmp("(1.5,-2.5)", kvalue, &ktype);
	fail_if(stat != 0);
	fail_if(ktype != CMF_KEY);

	/*
	 * No comma: there is no imaginary part to analyse.  This used to
	 * write through a NULL pr_end and read through an uninitialised
	 * pi_beg, so the process died here.
	 */
	stat = parse_cmp("(1 2)", kvalue, &ktype);
	fail_if(!(stat & NO_COMMA));

	/* No comma and no closing paren either. */
	stat = parse_cmp("(1 2", kvalue, &ktype);
	fail_if(!(stat & NO_COMMA));
	fail_if(!(stat & NO_TRAIL_PAREN));

	/* Nothing but the opening paren. */
	stat = parse_cmp("(", kvalue, &ktype);
	fail_if(!(stat & NO_COMMA));

	/* A comma but no closing paren still gets both parts analysed. */
	stat = parse_cmp("(1,2", kvalue, &ktype);
	fail_if(stat & NO_COMMA);
	fail_if(!(stat & NO_TRAIL_PAREN));

	/* Too many commas is reported, and must not crash. */
	stat = parse_cmp("(1,2,3)", kvalue, &ktype);
	fail_if(!(stat & TOO_MANY_COMMA));
}

/*--------------------------------------------------------------------------
 * Driving the fitsverify binary over a file built for the purpose
 *------------------------------------------------------------------------*/

#define FITSVERIFY	"./fitsverify"
#define TESTFILE	"test_fitsverify.fits"
#define REPORT		"test_fitsverify_report.txt"

/* Write one 80 column card, blank padded. */
static void
put_card(FILE *fp, long *nbytes, const char *card)
{
	char buf[81];

	snprintf(buf, sizeof(buf), "%-80.80s", card);
	fwrite(buf, 1, 80, fp);
	*nbytes += 80;
}

/* Write count copies of one byte. */
static void
put_bytes(FILE *fp, long *nbytes, int byte, long count)
{
	long i;

	for (i = 0; i < count; i++) {
		fputc(byte, fp);
		(*nbytes)++;
	}
}

/* Pad with blanks out to the next 2880 byte block. */
static void
pad_block(FILE *fp, long *nbytes)
{
	while (*nbytes % 2880) {
		fputc(' ', fp);
		(*nbytes)++;
	}
}

/* Start TESTFILE with an empty primary array that allows extensions. */
static FILE *
open_testfile(long *nbytes)
{
	FILE *fp = fopen(TESTFILE, "wb");

	fail_if(fp == NULL);
	*nbytes = 0;
	put_card(fp, nbytes, "SIMPLE  =                    T");
	put_card(fp, nbytes, "BITPIX  =                    8");
	put_card(fp, nbytes, "NAXIS   =                    0");
	put_card(fp, nbytes, "EXTEND  =                    T");
	put_card(fp, nbytes, "END");
	pad_block(fp, nbytes);
	return fp;
}

static void
close_testfile(FILE *fp, long *nbytes)
{
	pad_block(fp, nbytes);
	fail_if(fclose(fp) != 0);
}

/*
 * Run fitsverify on TESTFILE and return the raw wait status.  fork/exec
 * rather than system(), so that a fatal signal shows up as WIFSIGNALED
 * instead of being flattened into the shell's 128+n exit status.
 */
static int
run_fitsverify(void)
{
	pid_t pid;
	int st = 0;

	pid = fork();
	fail_if(pid < 0);
	if (pid == 0) {
		if (freopen(REPORT, "w", stdout) == NULL)
			_exit(126);
		if (dup2(fileno(stdout), fileno(stderr)) < 0)
			_exit(126);
		execl(FITSVERIFY, FITSVERIFY, TESTFILE, (char *)NULL);
		_exit(127);
	}
	fail_if(waitpid(pid, &st, 0) != pid);
	return st;
}

/* Verify the run finished on its own terms, and return its exit status. */
static int
check_no_crash(const char *what)
{
	int st = run_fitsverify();

	if (WIFSIGNALED(st))
		fprintf(stderr, "fitsverify died on %s with signal %d\n",
			what, WTERMSIG(st));
	fail_if(WIFSIGNALED(st));
	/* 126/127: the binary could not be run at all. */
	fail_if(WEXITSTATUS(st) == 126 || WEXITSTATUS(st) == 127);
	return WEXITSTATUS(st);
}

/*--------------------------------------------------------------------------
 * test_bin_ext: the "rAw" TFORM check (utilities/fvrf_head.c)
 *------------------------------------------------------------------------*/

/* A one column binary table with the given TFORM. */
static void
write_bintable_column(const char *tform)
{
	long n;
	FILE *fp = open_testfile(&n);
	char card[81];

	put_card(fp, &n, "XTENSION= 'BINTABLE'");
	put_card(fp, &n, "BITPIX  =                    8");
	put_card(fp, &n, "NAXIS   =                    2");
	put_card(fp, &n, "NAXIS1  =                   10");
	put_card(fp, &n, "NAXIS2  =                    1");
	put_card(fp, &n, "PCOUNT  =                    0");
	put_card(fp, &n, "GCOUNT  =                    1");
	put_card(fp, &n, "TFIELDS =                    1");
	snprintf(card, sizeof(card), "TFORM1  = '%-8s'", tform);
	put_card(fp, &n, card);
	put_card(fp, &n, "TTYPE1  = 'COL1    '");
	put_card(fp, &n, "END");
	pad_block(fp, &n);

	put_bytes(fp, &n, 'x', 10);
	close_testfile(fp, &n);
}

static void
test_tform_substring_width(void)
{
	/*
	 * The check divides the repeat count by the substring width taken
	 * straight from the TFORM value, and only guards it with isdigit(),
	 * which admits '0'.  A width of zero used to raise SIGFPE.
	 */
	static const char *tforms[] = { "10A0", "0A0", "3A0", "10A", "10A5" };
	size_t i;

	for (i = 0; i < sizeof(tforms) / sizeof(tforms[0]); i++) {
		char what[64];

		snprintf(what, sizeof(what), "TFORM '%s'", tforms[i]);
		write_bintable_column(tforms[i]);
		check_no_crash(what);
	}
}

/*--------------------------------------------------------------------------
 * iterdata: the bit column justification report (utilities/fvrf_data.c)
 *------------------------------------------------------------------------*/

/* Count occurrences of a string in the report. */
static int
count_in_report(const char *needle)
{
	FILE *fp = fopen(REPORT, "r");
	char line[4096];
	int nfound = 0;

	fail_if(fp == NULL);
	while (fgets(line, sizeof(line), fp) != NULL) {
		char *p = line;

		while ((p = strstr(p, needle)) != NULL) {
			nfound++;
			p += strlen(needle);
		}
	}
	fclose(fp);
	return nfound;
}

/*
 * A binary table holding a single X column of nbits bits.  nbits is chosen
 * by the caller so that it is not a multiple of 8, which leaves fill bits in
 * the last byte; the data is all ones, so those fill bits are set and the
 * "not left justified" report is produced.
 */
static void
write_bit_column_table(int nbits)
{
	long n;
	FILE *fp = open_testfile(&n);
	long nbytes = (nbits + 7) / 8;
	char card[81];

	put_card(fp, &n, "XTENSION= 'BINTABLE'");
	put_card(fp, &n, "BITPIX  =                    8");
	put_card(fp, &n, "NAXIS   =                    2");
	snprintf(card, sizeof(card), "NAXIS1  = %20ld", nbytes);
	put_card(fp, &n, card);
	put_card(fp, &n, "NAXIS2  =                    1");
	put_card(fp, &n, "PCOUNT  =                    0");
	put_card(fp, &n, "GCOUNT  =                    1");
	put_card(fp, &n, "TFIELDS =                    1");
	snprintf(card, sizeof(card), "TFORM1  = '%dX'", nbits);
	put_card(fp, &n, card);
	put_card(fp, &n, "TTYPE1  = 'BITS    '");
	put_card(fp, &n, "END");
	pad_block(fp, &n);

	put_bytes(fp, &n, 0xff, nbytes);
	close_testfile(fp, &n);
}

static void
check_bit_column(int nbits)
{
	char what[64];

	snprintf(what, sizeof(what), "a %dX column", nbits);
	write_bit_column_table(nbits);
	check_no_crash(what);

	fail_if(count_in_report("is not left justified.") != 1);
	/*
	 * errmes is 256 bytes and each element costs five of them, so a
	 * bounded report can never hold much more than fifty.  An unbounded
	 * one prints every byte of the column.
	 */
	fail_if(count_in_report("0x") > 64);
}

static void
test_bit_column_report(void)
{
	/* 100 bits in a 13 byte row: the report fits in errmes. */
	check_bit_column(100);

	/* 3996 bits in a 500 byte row: it does not. */
	check_bit_column(3996);
}

/*--------------------------------------------------------------------------
 * init_hdu: the card number printed for the first cards of an HDU
 * (utilities/fvrf_head.c)
 *------------------------------------------------------------------------*/

/* An extension whose XTENSION value is an integer rather than a string. */
static void
write_bad_xtension_table(void)
{
	long n;
	FILE *fp = open_testfile(&n);

	put_card(fp, &n, "XTENSION=                 1234");
	put_card(fp, &n, "BITPIX  =                    8");
	put_card(fp, &n, "NAXIS   =                    2");
	put_card(fp, &n, "NAXIS1  =                   10");
	put_card(fp, &n, "NAXIS2  =                    1");
	put_card(fp, &n, "PCOUNT  =                    0");
	put_card(fp, &n, "GCOUNT  =                    1");
	put_card(fp, &n, "TFIELDS =                    1");
	put_card(fp, &n, "TFORM1  = '10A     '");
	put_card(fp, &n, "TTYPE1  = 'COL1    '");
	put_card(fp, &n, "END");
	pad_block(fp, &n);

	put_bytes(fp, &n, 'x', 10);
	close_testfile(fp, &n);
}

static void
test_first_card_keyword_index(void)
{
	/*
	 * init_hdu parses the first cards of an HDU into an automatic FitsKey
	 * whose kindex was never assigned, so this diagnostic used to carry
	 * whatever happened to be on the stack.  XTENSION is card 1.
	 */
	write_bad_xtension_table();
	check_no_crash("an XTENSION value that is not a string");

	fail_if(count_in_report(
	    "Keyword #1, XTENSION: \"1234\" is not a string.") != 1);
}

/*--------------------------------------------------------------------------
 * test_agap: the column template (utilities/fvrf_data.c)
 *------------------------------------------------------------------------*/

/* A single column ASCII table with the given row length, start and format. */
static void
write_ascii_table(long naxis1, long tbcol, const char *tform)
{
	long n;
	FILE *fp = open_testfile(&n);
	char card[81];

	put_card(fp, &n, "XTENSION= 'TABLE   '");
	put_card(fp, &n, "BITPIX  =                    8");
	put_card(fp, &n, "NAXIS   =                    2");
	snprintf(card, sizeof(card), "NAXIS1  = %20ld", naxis1);
	put_card(fp, &n, card);
	put_card(fp, &n, "NAXIS2  =                    1");
	put_card(fp, &n, "PCOUNT  =                    0");
	put_card(fp, &n, "GCOUNT  =                    1");
	put_card(fp, &n, "TFIELDS =                    1");
	put_card(fp, &n, "TTYPE1  = 'COL1    '");
	snprintf(card, sizeof(card), "TBCOL1  = %20ld", tbcol);
	put_card(fp, &n, card);
	snprintf(card, sizeof(card), "TFORM1  = '%-8s'", tform);
	put_card(fp, &n, card);
	put_card(fp, &n, "END");
	pad_block(fp, &n);

	put_bytes(fp, &n, '1', naxis1);
	close_testfile(fp, &n);
}

static void
check_ascii_table(long naxis1, long tbcol, const char *tform)
{
	char what[96];

	snprintf(what, sizeof(what), "NAXIS1=%ld TBCOL1=%ld TFORM1=%s",
		 naxis1, tbcol, tform);
	write_ascii_table(naxis1, tbcol, tform);
	check_no_crash(what);
}

static void
test_ascii_column_template(void)
{
	/*
	 * test_agap marks which bytes of a row belong to a column in a NAXIS1
	 * long template, from TBCOLn and TFORMn.  Nothing bounded those
	 * writes, and CFITSIO skips its own TBCOLn range checks when the row
	 * length is zero.
	 */

	/* A well formed table still verifies. */
	check_ascii_table(10, 1, "A10");

	/* Zero row length: the template has no room for any column at all. */
	check_ascii_table(0, 1, "A20");

	/* ... and the column start is unchecked in that case. */
	check_ascii_table(0, 100000000, "A20");

	/* A zero column start would write below the template. */
	check_ascii_table(0, 0, "A20");
}

/*--------------------------------------------------------------------------
 * test_agap: reading a truncated table (utilities/fvrf_data.c)
 *------------------------------------------------------------------------*/

static int
report_has(const char *needle)
{
	return count_in_report(needle) > 0;
}

/*
 * A 500 row ASCII table.  The first row holds bytes that are not ASCII text,
 * so the data scan has something to find; every other row is ordinary text.
 * If truncate_it is set the last 2880 byte block is dropped, which cuts the
 * table data short while the header still claims all 500 rows.
 */
static void
write_ascii_data_table(int truncate_it)
{
	long n;
	FILE *fp = open_testfile(&n);
	long datastart;
	long i;

	put_card(fp, &n, "XTENSION= 'TABLE   '");
	put_card(fp, &n, "BITPIX  =                    8");
	put_card(fp, &n, "NAXIS   =                    2");
	put_card(fp, &n, "NAXIS1  =                   10");
	put_card(fp, &n, "NAXIS2  =                  500");
	put_card(fp, &n, "PCOUNT  =                    0");
	put_card(fp, &n, "GCOUNT  =                    1");
	put_card(fp, &n, "TFIELDS =                    1");
	put_card(fp, &n, "TTYPE1  = 'COL1    '");
	put_card(fp, &n, "TBCOL1  =                    1");
	put_card(fp, &n, "TFORM1  = 'A10     '");
	put_card(fp, &n, "END");
	pad_block(fp, &n);

	datastart = n;
	put_bytes(fp, &n, 0xff, 10);
	for (i = 10; i < 500 * 10; i++) {
		fputc('0' + (int)(i % 10), fp);
		n++;
	}
	close_testfile(fp, &n);

	if (truncate_it) {
		fail_if(n - 2880 <= datastart);
		fail_if(truncate(TESTFILE, n - 2880) != 0);
	}
}

static void
test_truncated_ascii_table(void)
{
	/*
	 * Control: the whole file is there, so the bad bytes in row 1 are
	 * read and have to be reported, all ten of them.
	 */
	write_ascii_data_table(0);
	check_no_crash("an ASCII table holding non-text bytes");
	fail_if(!report_has("contains non-ASCII characters."));
	fail_if(!report_has(
	    "This ASCII table contains 10 non-ASCII-text characters"));

	/*
	 * Truncated: the rows cannot be read at all, so the data scan has
	 * nothing to say about them.  test_agap used to report the failed
	 * read and then scan the buffer anyway, describing memory rather
	 * than the file.
	 */
	write_ascii_data_table(1);
	fail_if(check_no_crash("a truncated ASCII table") == 0);
	if (report_has("non-ASCII"))
		fprintf(stderr, "data scanned past the end of the file\n");
	fail_if(report_has("non-ASCII"));
}

int
main(void)
{
	test_complex_keyword_values();

	if (access(FITSVERIFY, X_OK) != 0) {
		fprintf(stderr,
			"%s has not been built; skipping the tests "
			"that need it\n", FITSVERIFY);
		return 0;
	}

	test_tform_substring_width();
	test_bit_column_report();
	test_first_card_keyword_index();
	test_ascii_column_template();
	test_truncated_ascii_table();

	remove(TESTFILE);
	remove(REPORT);
	return 0;
}
