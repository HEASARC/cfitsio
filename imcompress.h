#define MAX_COMPRESS_DIM     6
#define COMPRESS_NULL_VALUE -2147483647

typedef struct {
        char zcmptype[12];      /* compression type string */
        int compress_type;      /* type of compression algorithm */
	int bitpix;		/* FITS data type of image (BITPIX) */
	int ndim;		/* dimension of image */
	long naxis[MAX_COMPRESS_DIM];  /* length of each axis */
	int cn_compressed;	/* column number for COMPRESSED_DATA column */
	int cn_uncompressed;	/* column number for UNCOMPRESSED_DATA column */
	int cn_zscale;		/* column number for CSCALE column */
	int cn_zzero;		/* column number for CZERO column */
        int cn_zblank;          /* column number for the CBLANK column */
        double zscale;          /* scaling value, if keyword and not column */
        double zzero;           /* zero pt, if keyword and not column */
        int zblank;             /* value for null pixels, if not a column */
	long nrows;		/* number of rows in table */
	int ncols;		/* number of columns in table */
        int rice_blocksize;     /* first compression parameter */
        int rice_nbits;         /* second compression parameter */
        long tilesize[MAX_COMPRESS_DIM]; /* size of compression tiles */
        long maxtilelen;        /* max number of pixels in each image tile */
	long maxelem;		/* maximum length of variable length arrays */
} CompressImageInfo;

/*  image compression routines */
static int imcomp_get_image_params(fitsfile *infptr, CompressImageInfo *otb,
                 int *status);
static int imcomp_init_table(fitsfile *outfptr,
                CompressImageInfo *otb, int *status);
static int imcomp_calc_max_elem (int nx, int blocksize);
static int imcomp_copy_imheader(fitsfile *infptr, fitsfile *outfptr,
                int *status);
static int imcomp_img_to_tbl_special (char *card);
static int imcomp_compress_image (fitsfile *infptr, fitsfile *outfptr,
                 CompressImageInfo *otb, int *status);

/*  image decompression routines */
int fits_read_compressed_img_plane(fitsfile *fptr, int  datatype, 
      int  bytesperpixel,  long   nplane, long *firstcoord, long *lastcoord, 
      long *inc,  long *naxes,  int  nullcheck,  void *nullval, 
      void *array, char *nullarray, int  *anynul, int  *status);
static int imcomp_get_table_params(fitsfile *infptr, CompressImageInfo *itb,
            int *status);
static int imcomp_copy_tblheader(fitsfile *infptr, fitsfile *outfptr,   
           int *status);
static int imcomp_tbl_to_img_special (char *keyname);
static int imcomp_decompress_tile (fitsfile *infptr, CompressImageInfo *itb,
          int nrow, int tilesize, int datatype, int nullcheck,
          void *nulval, void *buffer, char *bnullarray, int *anynul,
          int *status);
static int imcomp_copy_overlap (char *tile, int datatype, int ndim,
         long *tfpixel, long *tlpixel, char *bnullarray, char *image,
         long *fpixel, long *lpixel, int nullcheck, char *nullarray,
         int *status);
