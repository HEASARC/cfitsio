typedef union {
    double real;           /* real value */
    long   integer;        /* integer value */
    char   boolean;        /* logical value */
    bvec   boolvec;        /* boolean array */
    ivec   intvec;         /* integer array */
    rvec   realvec;        /* real array */
    iseq   intseq;         /* integer sequence */
    rseq   realseq;        /* real sequence */
    char   string[256];    /* string value */
} FFSTYPE;
#define	REAL	258
#define	REALV	259
#define	INTEGER	260
#define	INTEGERV	261
#define	BOOLEAN	262
#define	BOOLEANV	263
#define	STRING	264
#define	BITSTR	265
#define	FUNCTION	266
#define	IFUNCTION	267
#define	RFUNCTION	268
#define	BFUNCTION	269
#define	OR	270
#define	AND	271
#define	EQ	272
#define	NE	273
#define	GT	274
#define	LT	275
#define	LTE	276
#define	GTE	277
#define	POWER	278
#define	NOT	279
#define	INTCAST	280
#define	FLTCAST	281
#define	UMINUS	282


extern FFSTYPE fflval;
