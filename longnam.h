#ifndef _LONGNAME_H
#define _LONGNAME_H

#define fits_open_file      ffopen
#define fits_create_file    ffinit
#define fits_flush_file     ffflus
#define fits_close_file     ffclos
#define fits_delete_file    ffdelt

#define fits_get_version    ffvers
#define fits_uppercase      ffupch
#define fits_get_errstatus  ffgerr
#define fits_write_errmsg   ffpmsg
#define fits_read_errmsg    ffgmsg
#define fits_clear_errmsg   ffcmsg
#define fits_compare_str    ffcmps
#define fits_test_keyword   fftkey
#define fits_test_record    fftrec
#define fits_make_keyn      ffkeyn
#define fits_make_nkey      ffnkey
#define fits_get_keytype    ffdtyp
#define fits_parse_value    ffpsvc
#define fits_parse_template ffgthd
#define fits_ascii_tform    ffasfm
#define fits_binary_tform   ffbnfm
#define fits_get_tbcol      ffgabc
#define fits_get_rowsize    ffgrsz

#define fits_write_record       ffprec
#define fits_write_key          ffpky
#define fits_write_comment      ffpcom
#define fits_write_history      ffphis 
#define fits_write_date         ffpdat
#define fits_write_key_longstr  ffpkls
#define fits_write_key_longwarn ffplwn
#define fits_write_key_str      ffpkys
#define fits_write_key_log      ffpkyl
#define fits_write_key_lng      ffpkyj
#define fits_write_key_fixflt   ffpkyf
#define fits_write_key_flt      ffpkye
#define fits_write_key_fixdbl   ffpkyg
#define fits_write_key_dbl      ffpkyd
#define fits_write_key_triple   ffpkyt
#define fits_write_tdim         ffptdm
#define fits_write_keys_str     ffpkns
#define fits_write_keys_log     ffpknl
#define fits_write_keys_lng     ffpknj
#define fits_write_keys_fixflt  ffpknf
#define fits_write_keys_flt     ffpkne
#define fits_write_keys_fixdbl  ffpkng
#define fits_write_keys_dbl     ffpknd
#define fits_write_imghdr       ffphps
#define fits_write_grphdr       ffphpr
#define fits_write_atblhdr      ffphtb
#define fits_write_btblhdr      ffphbn

#define fits_get_hdrspace      ffghsp
#define fits_get_hdrpos        ffghps
#define fits_movabs_key        ffmaky
#define fits_movrel_key        ffmrky
#define fits_find_nextkey      ffgnxk

#define fits_read_record       ffgrec
#define fits_read_card         ffgcrd
#define fits_read_keyn         ffgkyn
#define fits_read_key          ffgky
#define fits_read_keyword      ffgkey
#define fits_read_key_str      ffgkys
#define fits_read_key_log      ffgkyl
#define fits_read_key_lng      ffgkyj
#define fits_read_key_flt      ffgkye
#define fits_read_key_dbl      ffgkyd
#define fits_read_key_triple   ffgkyt
#define fits_read_key_longstr  ffgkls
#define fits_read_tdim         ffgtdm
#define fits_read_keys_str     ffgkns
#define fits_read_keys_log     ffgknl
#define fits_read_keys_lng     ffgknj
#define fits_read_keys_flt     ffgkne
#define fits_read_keys_dbl     ffgknd
#define fits_read_imghdr       ffghpr
#define fits_read_atblhdr      ffghtb
#define fits_read_btblhdr      ffghbn

#define fits_update_card       ffucrd
#define fits_update_key_str    ffukys
#define fits_update_key_log    ffukyl
#define fits_update_key_lng    ffukyj
#define fits_update_key_fixflt ffukyf
#define fits_update_key_flt    ffukye
#define fits_update_key_fixdbl ffukyg
#define fits_update_key_dbl    ffukyd

#define fits_modify_record     ffmrec
#define fits_modify_card       ffmcrd
#define fits_modify_name       ffmnam
#define fits_modify_comment    ffmcom
#define fits_modify_key_str    ffmkys
#define fits_modify_key_log    ffmkyl
#define fits_modify_key_lng    ffmkyj
#define fits_modify_key_fixflt ffmkyf
#define fits_modify_key_flt    ffmkye
#define fits_modify_key_fixdbl ffmkyg
#define fits_modify_key_dbl    ffmkyd

#define fits_insert_record     ffirec
#define fits_insert_key_str    ffikys
#define fits_insert_key_log    ffikyl
#define fits_insert_key_lng    ffikyj
#define fits_insert_key_fixflt ffikyf
#define fits_insert_key_flt    ffikye
#define fits_insert_key_fixdbl ffikyg
#define fits_insert_key_dbl    ffikyd

#define fits_delete_key     ffdkey
#define fits_delete_record  ffdrec

#define fits_get_hdunum     ffghdn
#define fits_get_hduaddr    ffghad

#define fits_movabs_hdu     ffmahd
#define fits_movrel_hdu     ffmrhd
#define fits_create_img     ffcrim
#define fits_create_tbl     ffcrtb
#define fits_create_hdu     ffcrhd
#define fits_insert_img     ffiimg
#define fits_insert_atbl    ffitab
#define fits_insert_btbl    ffibin
#define fits_delete_hdu     ffdhdu
#define fits_copy_hdu       ffcopy
#define fits_copy_data      ffcpdt

#define fits_set_hdustruc   ffrdef
#define fits_set_hdrsize    ffhdef
#define fits_write_theap    ffpthp

#define fits_encode_chksum  ffesum
#define fits_decode_chksum  ffdsum
#define fits_write_chksum   ffpcks
#define fits_update_chksum  ffupck
#define fits_verify_chksum  ffvcks
#define fits_get_chksum     ffgcks

#define fits_set_bscale     ffpscl
#define fits_set_tscale     fftscl
#define fits_set_imgnull    ffpnul
#define fits_set_btblnull   fftnul
#define fits_set_atblnull   ffsnul

#define fits_get_colnum     ffgcno
#define fits_get_colname    ffgcnn
#define fits_get_coltype    ffgtcl
#define fits_get_acolparms  ffgacl
#define fits_get_bcolparms  ffgbcl
 
#define fits_read_grppar_byt  ffggpb
#define fits_read_grppar_sht  ffggpi
#define fits_read_grppar_lng  ffggpj
#define fits_read_grppar_int  ffggpk
#define fits_read_grppar_flt  ffggpe
#define fits_read_grppar_dbl  ffggpd

#define fits_read_img         ffgpv
#define fits_read_img_byt     ffgpvb
#define fits_read_img_sht     ffgpvi
#define fits_read_img_lng     ffgpvj
#define fits_read_img_int     ffgpvk
#define fits_read_img_flt     ffgpve
#define fits_read_img_dbl     ffgpvd

#define fits_read_imgnull_byt ffgpfb
#define fits_read_imgnull_sht ffgpfi
#define fits_read_imgnull_lng ffgpfj
#define fits_read_imgnull_int ffgpfk
#define fits_read_imgnull_flt ffgpfe
#define fits_read_imgnull_dbl ffgpfd

#define fits_read_2d_byt      ffg2db
#define fits_read_2d_sht      ffg2di
#define fits_read_2d_lng      ffg2dj
#define fits_read_2d_int      ffg2dk
#define fits_read_2d_flt      ffg2de
#define fits_read_2d_dbl      ffg2dd

#define fits_read_3d_byt      ffg3db
#define fits_read_3d_sht      ffg3di
#define fits_read_3d_lng      ffg3dj
#define fits_read_3d_int      ffg3dk
#define fits_read_3d_flt      ffg3de
#define fits_read_3d_dbl      ffg3dd

#define fits_read_subset_byt  ffgsvb
#define fits_read_subset_sht  ffgsvi
#define fits_read_subset_lng  ffgsvj
#define fits_read_subset_int  ffgsvk
#define fits_read_subset_flt  ffgsve
#define fits_read_subset_dbl  ffgsvd

#define fits_read_subsetnull_byt ffgsfb
#define fits_read_subsetnull_sht ffgsfi
#define fits_read_subsetnull_lng ffgsfj
#define fits_read_subsetnull_int ffgsfk
#define fits_read_subsetnull_flt ffgsfe
#define fits_read_subsetnull_dbl ffgsfd

#define fits_read_col        ffgcv
#define fits_read_col_str    ffgcvs
#define fits_read_col_log    ffgcl
#define fits_read_col_byt    ffgcvb
#define fits_read_col_sht    ffgcvi
#define fits_read_col_lng    ffgcvj
#define fits_read_col_int    ffgcvk
#define fits_read_col_flt    ffgcve
#define fits_read_col_dbl    ffgcvd
#define fits_read_col_cmp    ffgcvc
#define fits_read_col_dblcmp ffgcvm
#define fits_read_col_bit    ffgcx

#define fits_read_colnull_str    ffgcfs
#define fits_read_colnull_log    ffgcfl
#define fits_read_colnull_byt    ffgcfb
#define fits_read_colnull_sht    ffgcfi
#define fits_read_colnull_lng    ffgcfj
#define fits_read_colnull_int    ffgcfk
#define fits_read_colnull_flt    ffgcfe
#define fits_read_colnull_dbl    ffgcfd
#define fits_read_colnull_cmp    ffgcfc
#define fits_read_colnull_dblcmp ffgcfm

#define fits_read_descript ffgdes
#define fits_read_tblbytes    ffgtbb

#define fits_write_grppar_byt ffpgpb
#define fits_write_grppar_sht ffpgpi
#define fits_write_grppar_lng ffpgpj
#define fits_write_grppar_int ffpgpk
#define fits_write_grppar_flt ffpgpe
#define fits_write_grppar_dbl ffpgpd

#define fits_write_img        ffppr
#define fits_write_img_byt    ffpprb
#define fits_write_img_sht    ffppri
#define fits_write_img_lng    ffpprj
#define fits_write_img_int    ffpprk
#define fits_write_img_flt    ffppre
#define fits_write_img_dbl    ffpprd

#define fits_write_imgnull_byt ffppnb
#define fits_write_imgnull_sht ffppni
#define fits_write_imgnull_lng ffppnj
#define fits_write_imgnull_int ffppnk
#define fits_write_imgnull_flt ffppne
#define fits_write_imgnull_dbl ffppnd

#define fits_write_img_null ffppru

#define fits_write_2d_byt   ffp2db
#define fits_write_2d_sht   ffp2di
#define fits_write_2d_lng   ffp2dj
#define fits_write_2d_int   ffp2dk
#define fits_write_2d_flt   ffp2de
#define fits_write_2d_dbl   ffp2dd

#define fits_write_3d_byt   ffp3db
#define fits_write_3d_sht   ffp3di
#define fits_write_3d_lng   ffp3dj
#define fits_write_3d_int   ffp3dk
#define fits_write_3d_flt   ffp3de
#define fits_write_3d_dbl   ffp3dd

#define fits_write_subset_byt  ffpssb
#define fits_write_subset_sht  ffpssi
#define fits_write_subset_lng  ffpssj
#define fits_write_subset_int  ffpssk
#define fits_write_subset_flt  ffpsse
#define fits_write_subset_dbl  ffpssd

#define fits_write_col         ffpcl
#define fits_write_col_str     ffpcls
#define fits_write_col_log     ffpcll
#define fits_write_col_byt     ffpclb
#define fits_write_col_sht     ffpcli
#define fits_write_col_lng     ffpclj
#define fits_write_col_int     ffpclk
#define fits_write_col_flt     ffpcle
#define fits_write_col_dbl     ffpcld
#define fits_write_col_cmp     ffpclc
#define fits_write_col_dblcmp  ffpclm
#define fits_write_col_null    ffpclu
#define fits_write_col_bit     ffpclx

#define fits_write_colnull_byt ffpcnb
#define fits_write_colnull_sht ffpcni
#define fits_write_colnull_lng ffpcnj
#define fits_write_colnull_int ffpcnk
#define fits_write_colnull_flt ffpcne
#define fits_write_colnull_dbl ffpcnd

#define fits_write_descript  ffpdes

#define fits_write_tblbytes  ffptbb
#define fits_insert_rows  ffirow
#define fits_delete_rows  ffdrow
#define fits_insert_col   fficol
#define fits_insert_cols  fficls
#define fits_delete_col   ffdcol

#define fits_read_img_coord ffgics
#define fits_read_tbl_coord ffgtcs
#define fits_pix_to_world ffwldp
#define fits_world_to_pix ffxypx
#endif
