#include "fitsio.h"
#include <stdlib.h>

main()
{

  int i        = 0;
  int status   = 0;
  int hdutype  = 0;

  long failed  = 0;

  char errstr[FLEN_FILENAME];

  fitsfile *gfptr, *mfptr, *tfptr;

  do
    {

      /*
	test the grouping table creation and modification functions
      */

      status = fits_create_file(&gfptr,"test1.fits",&status);
      if(status != 0){printf("status 0 %d\n",status); continue;};
      
      status = fits_create_group(gfptr,"GROUP1",GT_ID_ALL_URI,&status);
      if(status != 0){printf("status 1 %d\n",status); continue;};
      
      status = fits_create_group(gfptr,"GROUP2",GT_ID_ALL,&status);
      if(status != 0){printf("status 2 %d\n",status); continue;};
      
      status = fits_change_group(gfptr,GT_ID_ALL_URI,&status);
      if(status != 0){printf("status 3 %d\n",status); continue;};
      
      status = fits_create_group(gfptr,"GROUP3",GT_ID_ALL_URI,&status);
      if(status != 0){printf("status 4 %d\n",status); continue;};
      
      status = fits_change_group(gfptr,GT_ID_POS,&status);
      if(status != 0){printf("status 5 %d\n",status); continue;};
      
      status = fits_change_group(gfptr,GT_ID_ALL,&status);
      if(status != 0){printf("status 6 %d\n",status); continue;};
      
      /*
	test the member addtion functions
      */

      fits_movabs_hdu(gfptr,2,&hdutype,&status);
      if(status != 0){printf("status 7 %d\n",status); continue;};
      
      status = fits_add_group_member(gfptr,NULL,3,&status);
      if(status != 0){printf("status 8 %d\n",status); continue;};
      
      status = fits_add_group_member(gfptr,NULL,4,&status);
      if(status != 0){printf("status 9 %d\n",status); continue;};
      
      status = fits_add_group_member(gfptr,NULL,4,&status);
      
      if(status == HDU_ALREADY_MEMBER)
	{
	  printf("Attempted to add HDU #4 twice; error expected, resetting\n");
	  status = 0;
	}
      
      if(status != 0){printf("status 10 %d\n",status); continue;};
      
      status = fits_add_group_member(gfptr,NULL,1,&status);
      if(status != 0){printf("status 11 %d\n",status); continue;};
      
      fits_movabs_hdu(gfptr,3,&hdutype,&status);
      if(status != 0){printf("status 12 %d\n",status); continue;};
      
      status = fits_add_group_member(gfptr,NULL,4,&status);
      if(status != 0){printf("status 13 %d\n",status); continue;};
      
      fits_movabs_hdu(gfptr,4,&hdutype,&status);
      if(status != 0){printf("status 14 %d\n",status); continue;};
      
      status = fits_add_group_member(gfptr,NULL,2,&status);
      if(status != 0){printf("status 15 %d\n",status); continue;};
      
      fits_movabs_hdu(gfptr,2,&hdutype,&status);
      if(status != 0){printf("status 16 %d\n",status); continue;};
      
      /*
	test the group table verify function
      */

      fits_verify_group(gfptr,&failed,&status);
      
      if(failed != 0)printf("verify failed at %d\n",failed);
      if(status != 0){printf("status 17 %d\n",status); continue;};
      
      fits_movabs_hdu(gfptr,3,&hdutype,&status);
      if(status != 0){printf("status 18 %d\n",status); continue;};
      
      fits_verify_group(gfptr,&failed,&status);
      
      if(failed != 0)printf("verify failed at %d\n",failed);
      if(status != 0){printf("status 19 %d\n",status); continue;};
      
      fits_movabs_hdu(gfptr,4,&hdutype,&status);
      if(status != 0){printf("status 19 %d\n",status); continue;};
      
      fits_verify_group(gfptr,&failed,&status);
      
      if(failed != 0)printf("verify failed at %d\n",failed);
      if(status != 0){printf("status 20 %d\n",status); continue;};
      
      fits_movabs_hdu(gfptr,2,&hdutype,&status);
      if(status != 0){printf("status 21 %d\n",status); continue;};
      
      /*
	test the copy group function
      */

      status = fits_create_file(&mfptr,"test2.fits",&status);
      if(status != 0){printf("status 22 %d\n",status); continue;};
      
      fits_copy_group(gfptr,mfptr,OPT_GCP_GPT,&status);
      if(status != 0){printf("status 23 %d\n",status); continue;};
      
      status = fits_create_file(&tfptr,"test3.fits",&status);
      if(status != 0){printf("status 24 %d\n",status); continue;};
      
      fits_copy_group(mfptr,tfptr,OPT_GCP_ALL,&status);
      if(status != 0){printf("status 25 %d\n",status); continue;};
      
      fits_movabs_hdu(gfptr,2,&hdutype,&status);
      if(status != 0){printf("status 26 %d\n",status); continue;};
      
      fits_copy_group(gfptr,mfptr,OPT_GCP_ALL,&status);
      if(status != 0){printf("status 27 %d\n",status); continue;};
      
      fits_movabs_hdu(gfptr,1,&hdutype,&status);
      if(status != 0){printf("status 28 %d\n",status); continue;};

      fits_movabs_hdu(mfptr,2,&hdutype,&status);
      if(status != 0){printf("status 29 %d\n",status); continue;};
      
      /*
	test the remove group function
      */

      fits_remove_group(mfptr,OPT_RM_ALL,&status);
      if(status != 0){printf("status 30 %d\n",status); continue;};
      
      fits_remove_group(tfptr,OPT_RM_GPT,&status);
      if(status != 0){printf("status 31 %d\n",status); continue;};
      
      fits_movabs_hdu(tfptr,2,&hdutype,&status);
      if(status != 0){printf("status 32 %d\n",status); continue;};
      
      /*
	test the compact group, merge group and transfer member functions 
      */

      fits_compact_group(tfptr,OPT_CMT_MBR_DEL,&status);
      if(status != 0){printf("status 33 %d\n",status); continue;}
      status = fits_close_file(tfptr,&status);
      
      fits_movabs_hdu(mfptr,2,&hdutype,&status);
      if(status != 0){printf("status 34 %d\n",status); continue;};
      fits_reopen_file(mfptr,&tfptr,&status);
      fits_movabs_hdu(tfptr,3,&hdutype,&status);
      if(status != 0){printf("status 35 %d\n",status); continue;};
      
      fits_transfer_member(mfptr,tfptr,3,OPT_MCP_MOV,&status);
      if(status != 0){printf("status 36 %d\n",status); continue;};
      
      fits_merge_groups(tfptr,mfptr,OPT_MRG_MOV,&status);
      if(status != 0){printf("status 37 %d\n",status); continue;};
      
      /*
	close all files
      */

      status = fits_close_file(tfptr,&status);
      if(status != 0){printf("status 38 %d\n",status); continue;};
      status = fits_close_file(mfptr,&status);
      if(status != 0){printf("status 39 %d\n",status); continue;};
      status = fits_close_file(gfptr,&status);
      if(status != 0){printf("status 40 %d\n",status); continue;};

    }while(0);


  
  if(status != 0)
    {
      while(fits_read_errmsg(errstr) != 0)
	printf("%s\n",errstr);
    }
}

