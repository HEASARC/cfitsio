typedef union {
    int    Node;        /* Index of Node */
    double dbl;         /* real value    */
    long   lng;         /* integer value */
    char   log;         /* logical value */
    char   str[256];    /* string value  */
} FFSTYPE;
#define	BOOLEAN	258
#define	LONG	259
#define	DOUBLE	260
#define	STRING	261
#define	BITSTR	262
#define	FUNCTION	263
#define	BFUNCTION	264
#define	COLUMN	265
#define	BCOLUMN	266
#define	SCOLUMN	267
#define	BITCOL	268
#define	ROWREF	269
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
