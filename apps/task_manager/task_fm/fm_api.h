/*--------------------------------------------------------------------------*/
/**@file    fm_api.h
   @brief   FM 模式功能接口
   @details
   @author  guowei
   @date    2014-9-15
   @note    DV10
*/
/*----------------------------------------------------------------------------*/
#ifndef	_FM_API_H_
#define _FM_API_H_
#include "typedef.h"
#include "common/app_cfg.h"

#define MIN_FRE	   875
#define MAX_FRE    1080
#define MAX_CHANNL (MAX_FRE - MIN_FRE + 1)
#define FM_STEP    100
#define MEM_FM_LEN  26

enum {
    FM_FRE = 0,
    FM_CHAN = 1,
    FM_CHANNL = 2,
};

enum {
    FM_CUR_FRE = 0,
    FM_FRE_DEC = 1,
    FM_FRE_INC	= 2,
};

/*****************************
        Function Declare
*****************************/

bool fm_mode_read_id(void);
bool fm_mode_init(void);
void fm_mode_powerdown(void);
bool fm_module_set_fre(u8 mode);
void fm_module_mute(u8 flag);
bool fm_radio_scan(u8 mode);
tu8 my_get_one_count(u8 byte);
tu8 get_total_mem_channel(void);
tu8 get_channel_via_fre(u8 fre);
tu8 get_fre_via_channle(u8 channel);
void clear_all_fm_point(void);
void save_fm_point(u8 fre);
void delete_fm_point(u8 fre);
void ch_save(void);
void fm_save_info(void);
void fm_read_info(void);
void fm_mutex_init(void *priv);
void fm_mutex_stop(void *priv);
#endif
