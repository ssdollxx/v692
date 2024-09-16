#include "sdk_cfg.h"
#include "task_music.h"
#include "task_music_key.h"
#include "msg.h"
#include "string.h"
#include "clock.h"
#include "uart.h"
#include "audio/dac_api.h"
#include "dac.h"
#include "audio/audio.h"
#include "dev_manage.h"
#include "dev_mg_api.h"
#include "music_player.h"
#include "pitchshifter.h"
#include "task_common.h"
#include "flash_api.h"
#include "warning_tone.h"
#include "music_ui.h"
#include "ui/ui_api.h"
#include "updata.h"
#include "music_decrypt.h"
#include "fat_io.h"
#include "led.h"
#include "task_echo_key.h"
#include "fmtx_api.h"
#include "lyrics_api.h"

#include "echo_api.h"

#define MUSIC_TASK_DEBUG_ENABLE
#ifdef MUSIC_TASK_DEBUG_ENABLE
#define music_task_printf log_printf
#else
#define music_task_printf(...)
#endif

#include "fs_io.h"
MUSIC_DIS_VAR music_ui;

#if SPEED_PITCH_EN
static u8 speed_mode = 0;
static u8 pitch_mode = 0;
const u16 speed_tab[] = {
    PS_SPEED_DEFAULT_VAL,
    PS_SPEED_DEFAULT_VAL + 10,
    PS_SPEED_DEFAULT_VAL + 20,
    PS_SPEED_DEFAULT_VAL - 10,
    PS_SPEED_DEFAULT_VAL - 20,
};
const u16 pitch_tab[] = {
    PS_PITCHT_DEFAULT_VAL,
    PS_PITCHT_DEFAULT_VAL + 4000,
    PS_PITCHT_DEFAULT_VAL + 6000,
    PS_PITCHT_DEFAULT_VAL - 3000,
    PS_PITCHT_DEFAULT_VAL - 6000,
};
#endif	//SPEED_PITCH_EN

static u8 music_rpt_mode = REPEAT_ALL;
static DEV_HANDLE cur_use_dev = NULL;
static MUSIC_DECODER_ST music_status;
CIPHER  dec_cipher = {0};

const u8 music_rpt_str[][10] = {
    {"ALL"},
    {"ONE_DEV"},
    {"ONE_FILE"},
    {"RANDOM"},
    {"FOLDER"},
};

u8 music_play_get_rpt_mode(void)
{
    return music_rpt_mode;
}

static tbool task_music_skip_check(void **priv)
{
    music_task_printf("task_music_skip_check !!\n");

    //DEV_HANDLE *dev = *priv;
    u32 parm;
    u32 dev_status;

    tbool dev_cnt = 0;

    printf("1music cur_use_dev:0x%x,0x%x\n", cur_use_dev, *priv);
    //check some device online
    if (*priv == NULL) {
        if (cur_use_dev != NULL) {
            if (!dev_get_online_status(cur_use_dev, &dev_status)) {
                if (dev_status != DEV_ONLINE) {                               //上一次退出时的设备不在线，重新获取一个
                    *priv = dev_get_fisrt(MUSIC_DEV_TYPE, DEV_ONLINE);
                } else {                                                      //在线，使用上次保存的设备播放
                    *priv = cur_use_dev;
                }
            }
        } else {
            *priv = dev_get_fisrt(MUSIC_DEV_TYPE, DEV_ONLINE);
        }
        printf("2music cur_use_dev:0x%x,0x%x\n", cur_use_dev, *priv);
        if (*priv == (void *)sd0) {
            puts("[PRIV SD0]\n");
        } else if (*priv == (void *)sd1) {
            puts("[PRIV SD1]\n");
        } else if (*priv == (void *)usb) {
            puts("[PRIV USB]\n");
        } else {
            puts("[PRIV CACHE]\n");
        }
        if (*priv == NULL) {
            return false;
        } else {
            return true;
        }
    }

    //check specific device online
    return true;
}


static void music_play_mutex_init(void *priv)
{
    music_task_printf("#MODE:MP3 \n");
    tbool ret;
    MUSIC_PLAYER *obj = priv;

    if (cur_use_dev == (void *)sd0) {
        puts("[PRIV SD0]\n");
    } else if (cur_use_dev == (void *)sd1) {
        puts("[PRIV SD1]\n");
    } else if (cur_use_dev == (void *)usb) {
        puts("[PRIV USB]\n");
    } else {
        puts("[PRIV CACHE]\n");
    }

    dac_channel_on(MUSIC_CHANNEL, 0);
    set_sys_vol(sound.vol.sys_vol_l, sound.vol.sys_vol_r, FADE_ON);
    music_player_creat();

    music_task_printf("music obj:0x%x,dev:0x%x\n", obj, cur_use_dev);
    if ((obj == NULL) || (cur_use_dev == NULL)) {
        music_task_printf("music player play err!!, fun = %s, line = %d\n", __func__, __LINE__);
        task_post_msg(NULL, 1, MSG_CHANGE_WORKMODE);
    }

    music_ui.mapi = obj;
    if (music_status == MUSIC_PLAYRR_ST_PAUSE) {
        music_player_set_decoder_init_sta(obj, MUSIC_PLAYRR_ST_PAUSE);
    }

    ret = music_player_play_spec_dev(obj, (u32)cur_use_dev);
    if (ret == false) {
        music_task_printf("music player play err!!, fun = %s, line = %d\n", __func__, __LINE__);
        task_post_msg(NULL, 1, MSG_CHANGE_WORKMODE);
    }
}

static void music_play_mutex_stop(void *priv)
{
    music_task_printf("music_play_mutex_stop:0x%x\n", priv);
    MUSIC_PLAYER *obj = priv;
    music_status = music_player_get_status(obj);
    cur_use_dev = music_player_get_cur_dev(obj);

    if (obj && cur_use_dev) {
        music_player_destroy(&obj);
        music_ui.mapi = NULL;
    }
}

static MUSIC_PLAYER *music_play_start(void)
{
    MUSIC_PLAYER *obj = NULL;
    obj = music_player_creat();
    music_status = MUSIC_DECODER_ST_PLAY;
    ui_open_music(&music_ui, sizeof(MUSIC_DIS_VAR));
    music_task_printf("music_play_creat:0x%x\n", obj);
    if (obj) {
        mutex_resource_apply("music", 3, music_play_mutex_init, music_play_mutex_stop, obj);
    }
    return obj;
}

static void music_play_stop(void *priv)
{
    music_tone_stop();
    mutex_resource_release("music");
    task_common_msg_deal(priv, NO_MSG);
}

static void *task_music_init(void *priv)
{
    MUSIC_PLAYER *obj = NULL;

#if SPEED_PITCH_EN
    speed_mode = 0;
    pitch_mode = 0;
#endif

    fat_init();
    fat_mem_init(NULL, 0);  //使用默认的RAM， overlay task

    obj = music_player_creat();
    cur_use_dev = (DEV_HANDLE)priv;
    music_task_printf("task_music_init:0x%x,0x%x\n", obj, priv);
#if MUSIC_DECRYPT_EN
    cipher_obj_creat(0x12345678, &dec_cipher, "SMP");
#endif
#if LRC_LYRICS_EN
    lrc_init();
#endif
    audio_set_output_buf_limit(OUTPUT_BUF_LIMIT_MUSIC_TASK);
    tone_play(TONE_MUSIC_MODE, 0);
    led_fre_set(C_BLED_SLOW_MODE);

    return obj;
}

static void task_music_exit(void **hdl)
{
    music_task_printf("EXIT_MODE:MP3 !!\n");
    /* dac_channel_off(MUSIC_CHANNEL, 0); */
    //cur_use_dev = music_player_get_cur_dev(*hdl);
    music_player_destroy((MUSIC_PLAYER **)hdl);
#if LRC_LYRICS_EN
    lrc_exit();
#endif
    ui_close_music(&music_ui);
    audio_set_output_buf_limit(OUTPUT_BUF_LIMIT);
    set_sys_freq(SYS_Hz);
    task_clear_all_message();
    fat_del();
    puts("music_exit ok!\n");
}

void music_msg_filter(int *msg)
{
    switch (*msg) {
    case MSG_MUSIC_PP:
    case MSG_MUSIC_NEXT_FILE:
    case MSG_MUSIC_PREV_FILE:
    case MSG_MUSIC_FF:
    case MSG_MUSIC_FR:
    case MSG_MUSIC_AB_RPT:
    case MSG_HALF_SECOND:
        *msg = NO_MSG;
        break;
    }
}
#include "echo_api.h"
static void task_music_deal(void *hdl)
{
    music_task_printf("MODE:MP3\n");
    int msg = NO_MSG;
    int msg_error = MSG_NO_ERROR;
    u32 music_input_num;
    u32 err;
    tbool ret = true;
    MUSIC_PLAYER *obj = (MUSIC_PLAYER *)hdl;
    u8 music_start = 0;

    music_task_printf("fun = %s, line = %d\n", __func__, __LINE__);
    if (obj == NULL) {
        music_task_printf("fun = %s, line = %d\n", __func__, __LINE__);
        ret = false;
    }

    music_task_printf("fun = %s, line = %d\n", __func__, __LINE__);
    while (1) {
        msg_error = task_get_msg(0, 1, &msg);
        if (get_tone_status()) {    //提示音还未播完前过滤涉及解码器操作的消息
            music_msg_filter(&msg);
        }
        if (task_common_msg_deal(obj, msg) == false) {
            if (music_player_get_cur_dev(obj) != (void *)cache) {
                cur_use_dev = music_player_get_cur_dev(obj);
            }
            puts("exit music_task\n");
            music_play_stop(obj);
            return;
        }
        if (NO_MSG == msg) {
            continue;
        }
        if (msg != MSG_HALF_SECOND) {
//            printf("msg = %d\n", msg);
        }
        switch (msg) {
        case SYS_EVENT_DEC_FR_END:
            break;
        case SYS_EVENT_DEC_FF_END:
        case SYS_EVENT_DEC_END:
#if FILTER_NULL_TIME_MUSIC
            if (music_player_get_total_time(obj) == 0) { //对解码时间为0的文件特殊处理
                if (music_player_get_auto_next_flag(obj) == 0) { //错误自动上一曲
                    ret = music_player_operation(obj, PLAY_PREV_FILE);
                } else {
                    ret = music_player_operation(obj, PLAY_NEXT_FILE);
                }
                break;
            }
#endif
            music_task_printf("SYS_EVENT_DEC_END, fun = %s, line = %d\n", __func__, __LINE__);
            ret = music_player_operation(obj, PLAY_AUTO_NEXT);
            break;

        case SYS_EVENT_DEC_DEVICE_ERR:
            puts("SYS_EVENT_DEC_DEVICE_ERR\n");
            ret = music_player_play_next_dev(obj);
            break;

        case SYS_EVENT_PLAY_SEL_END:
            music_task_printf("SYS_EVENT_PLAY_TONE_END\n");
            if (music_start == 0) {
                music_start = 1 ;
                obj = music_play_start();
                music_ui.mapi = obj;
            }
            break;

        case MSG_MUSIC_AB_RPT:
            music_task_printf("MSG_MUSIC_AB_RPT\n");
            music_player_ab_repeat_switch(obj);
            break;

        case MSG_MUSIC_U_SD:
            music_task_printf("MSG_MUSIC_U_SD\n");
            if (dev_get_phydev_total(MUSIC_DEV_TYPE, DEV_ONLINE) > 1) {
                ret = music_player_play_next_dev(obj);
            }
            break;

        case MSG_SD0_MOUNT_SUCC:
            music_play_stop(NULL);
            cur_use_dev = sd0;
            obj = music_play_start();
            music_start = 1 ;
            break;

        case MSG_SD1_MOUNT_SUCC:
            music_play_stop(NULL);
            cur_use_dev = sd1;
            obj = music_play_start();
            music_start = 1 ;
            break;

        case MSG_USB_MOUNT_SUCC:
            music_play_stop(NULL);
            cur_use_dev = usb;
            obj = music_play_start();
            music_start = 1 ;
            break;

        case MSG_SD0_OFFLINE:
            if (sd0 == music_player_get_cur_dev(obj) && MUSIC_PLAYRR_ST_PAUSE == music_player_get_status(obj)) {  //防止拔出设备时重复推消息
                task_post_msg(NULL, 1, SYS_EVENT_DEC_DEVICE_ERR);
            }
            break;

        case MSG_SD1_OFFLINE:
            if (sd1 == music_player_get_cur_dev(obj) && MUSIC_PLAYRR_ST_PAUSE == music_player_get_status(obj)) {
                task_post_msg(NULL, 1, SYS_EVENT_DEC_DEVICE_ERR);
            }
            break;

        case MSG_HUSB_OFFLINE:
            music_task_printf("music MSG_SUSB_OFFLINE\n");
            if (usb == music_player_get_cur_dev(obj) && MUSIC_PLAYRR_ST_PAUSE == music_player_get_status(obj)) {
                task_post_msg(NULL, 1, SYS_EVENT_DEC_DEVICE_ERR);
            }
            break;

        case MSG_MUSIC_PP:
            music_task_printf("MSG_MUSIC_PP\n");
            music_status = music_player_pp(obj);
            if (music_status == MUSIC_DECODER_ST_PLAY) {
                music_task_printf("fun = %s, line = %d\n", __func__, __LINE__);
            } else if (music_status == MUSIC_PLAYRR_ST_PAUSE) {
                music_task_printf("fun = %s, line = %d\n", __func__, __LINE__);
            } else {
                music_task_printf("fun = %s, line = %d\n", __func__, __LINE__);
            }
            //UI_menu_mux(MENU_MUSIC_MAIN, MENU_LIST_DISPLAY);
            break;

        case MSG_INPUT_NUMBER_END:
        case MSG_INPUT_TIMEOUT:
            if (get_input_number(&music_input_num) && (music_input_num != 0) && (music_input_num <= music_player_get_total_file(obj))) {
                ret = music_player_play_spec_num_file(obj, music_input_num);
            }
            UI_DIS_MAIN();
            break;

        case MSG_MUSIC_SPC_FILE:
            ret = music_player_play_spec_num_file(obj, 1);
            break;

        case MSG_MUSIC_NEXT_FILE:
#if FMTX_EN
            if (fmtx_get_state() == FREQ_SETTING) {
                fmtx_setfre(FREQ_NEXT, 0);
                UI_menu(MENU_FM_DISP_FRE, 0, UI_FREQ_RETURN);
                break;
            }
#endif
            ret = music_player_operation(obj, PLAY_NEXT_FILE);
            UI_menu(MENU_FILENUM, 0, 3);
            break;
        case MSG_MUSIC_PREV_FILE:
#if FMTX_EN
            if (fmtx_get_state() == FREQ_SETTING) {
                fmtx_setfre(FREQ_PREV, 0);
                UI_menu(MENU_FM_DISP_FRE, 0, UI_FREQ_RETURN);
                break;
            }
#endif
            ret = music_player_operation(obj, PLAY_PREV_FILE);
            UI_menu(MENU_FILENUM, 0, 3);
            break;
        case MSG_MUSIC_PREV_FOLDER:
            ret = music_player_operation(obj, PLAY_PRE_FOLDER);
            break;
        case MSG_MUSIC_NEXT_FOLDER:
            ret = music_player_operation(obj, PLAY_NEXT_FOLDER);
            break;

        case MSG_MUSIC_RPT:
            /* music_task_printf("MSG_MUSIC_RPT\n"); */
            music_rpt_mode++;
            if (music_rpt_mode >= MAX_PLAY_MODE) {
                music_rpt_mode = REPEAT_ALL;
            }
            music_task_printf("cur_play_mode : %s\r", &music_rpt_str[music_rpt_mode - REPEAT_ALL][0]);
            music_player_set_repeat_mode(obj, music_rpt_mode);
            UI_menu(MENU_SET_PLAY_MODE, music_rpt_mode, 3);
            break;

#if (SPEED_PITCH_EN)
        case MSG_MUSIC_SET_SPEED:
            speed_mode ++;
            if (speed_mode >= (sizeof(speed_tab) / sizeof(speed_tab[0]))) {
                speed_mode = 0;
            }
            music_task_printf("speed_mode = %d\n", speed_mode);
            pitchshifter_set_speed_val(speed_tab[speed_mode]);
            break;
        case MSG_MUSIC_SET_PITCH:
            pitch_mode ++;
            if (pitch_mode >= (sizeof(pitch_tab) / sizeof(pitch_tab[0]))) {
                pitch_mode = 0;
            }
            music_task_printf("pitch_mode = %d\n", pitch_mode);
            pitchshifter_set_pitch_val(pitch_tab[pitch_mode]);
            break;
#endif//SPEED_PITCH_EN

        case MSG_MUSIC_FF:
            music_player_ff(obj, 2);
            music_ui.opt_state |= MUSIC_OPT_BIT_FF;
            UI_DIS_MAIN();
            break;
        case MSG_MUSIC_FR:
            music_player_fr(obj, 2);
            music_ui.opt_state |= MUSIC_OPT_BIT_FR;
            UI_DIS_MAIN();
            break;

        case MSG_MUSIC_FF_KEY_UP:
            music_ui.opt_state &= ~MUSIC_OPT_BIT_FF;
            break;
        case MSG_MUSIC_FR_KEY_UP:
            music_ui.opt_state &= ~MUSIC_OPT_BIT_FR;
            break;

        case MSG_MUSIC_FFR_DONE:
            break;

        case MSG_MUSIC_DEL_FILE:
            log_printf("MSG_MUSIC_DEL_FILE\n");
            {
                u32 filenum;
                filenum = music_player_get_file_number(obj);
                music_player_delete_playing_file(obj);
                ret = music_player_play_spec_num_file(obj, filenum);
                music_ui.opt_state |= MUSIC_OPT_BIT_DEL;
            }
            break;

        case MSG_UPDATA:
            log_printf("MSG_UPDATA\n");
            device_updata(music_player_get_cur_dev(obj));
            break;


        case MSG_HALF_SECOND:
            ui_music_update_var(&music_ui);
            if (music_player_get_status(obj) == MUSIC_DECODER_ST_PLAY) {
                led_fre_set(C_BLED_SLOW_MODE);
            } else {
                led_fre_set(C_BLED_ON_MODE);
            }
            break;

        default:
            break;
        }

        if (ret == false && obj != NULL) {
            music_task_printf("music player play err!!, fun = %s, line = %d\n", __func__, __LINE__);
            music_play_stop(obj);
            task_switch(TASK_ID_TYPE_NEXT, NULL);
            return ;
        }
    }
}

const TASK_APP task_music_info = {
    .skip_check = task_music_skip_check,//
    .init 		= task_music_init,
    .exit 		= task_music_exit,
    .task 		= task_music_deal,
    .key 		= &task_music_key,
};

