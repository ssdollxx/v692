#ifndef _FF_API_
#define _FF_API_

#include "typedef.h"
#include "tff.h"



enum {
    SEARCH_FILE_BY_NUMBER,  //按文件序号搜索文件
    SEARCH_SPEC_FILE,       //搜索指定的文件
    SEARCH_FILE_BY_PATH,   //按路径查找文件
    SEARCH_FILE_IN_PATH,   //给定路径查找文件
};

enum {
    SEARCH_DISK,                //搜索总有效文件数
    SEARCH_FOLDER,           //搜索当前目录根下的有效文件数
};


/* function list */
void fs_ext_setting(const char *str);
void ui_ext_setting(const char *str);
bool check_ext(char _xdata *fname, char const *ext);
FRESULT compare_path(u8 *spath, u8 *dpath);
bool fast_scan_files(FATFS _xdata *fs, DIR _xdata *dj, u32 spec_sclust, u32 *file_total, u8 search_mode);
bool scan_files(FIL _xdata *fp, DIR _xdata *dj, FS_NAME _xdata *fs_n, u32 spec_sclust, u8 search_mode);
bool fs_load_file(FIL *fp);
bool fs_scan_disk(FATFS _xdata *fs, u32 sp_sclust);
bool fs_getfile_bysclust(FATFS *fs, FIL *fp, u32 sclust);
bool fs_getfile_bynumber(FATFS *fs, FIL *fp, u16 file_number);
bool fs_getfile_bypath(FATFS *fs, FIL *fp, u8 *path);
u16 fs_getfile_totalindir(FATFS *fs, DIR _xdata *f_dj);
u32 fs_getrec_fileinfo(FATFS _xdata *fs, char *path, char *ext, u32 *first_fn);
u32 fs_open_dir_info(FATFS _xdata *f_s, FIL *f_p, void *dir_dj);
u32 fs_exit_dir_info(FATFS _xdata *f_s, FIL *f_p);
u32 fs_get_dir_info(FATFS _xdata *f_s, FIL *f_p, u32 start_num, u32 total_num, void *buf_info);

u32 fs_getfile_byname_indir(FATFS _xdata *f_s, FIL *tag_file, FIL *get_file, char *ext_name);

void fat_para_init(FATFS **fat_fs, FIL **fhandle);
void fat_para_free(FATFS **fat_fs, FIL **fhandle);
void fs_lfn_deal(FATFS _xdata *f_s, FIL *f_p);
bool create_rec_file(FATFS *fat_fs, FIL *fhandle);

void reg_ff_scan_ret_fun(u32(*fun)(void)); //注册回调函数，fast_scan时候调用，注册函数返回1即退出scan

void set_swin_buf(void *buf);
void set_lfn_buf(void *buf);

#endif
