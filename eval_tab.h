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
#define	FILTER	265
#define	COLUMN	266
#define	BCOLUMN	267
#define	SCOLUMN	268
#define	BITCOL	269
#define	ROWREF	270
#define	OR	271
#define	AND	272
#define	EQ	273
#define	NE	274
#define	GT	275
#define	LT	276
#define	LTE	277
#define	GTE	278
#define	POWER	279
#define	NOT	280
#define	INTCAST	281
#define	FLTCAST	282
#define	UMINUS	283


extern FFSTYPE fflval;
