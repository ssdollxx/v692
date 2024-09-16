#include "sdk_cfg.h"
#include "common/common.h"
#include "ui/lcd/lcd_drv_api.h"
#include "ui/lcd/lcd_spi.h"
#include "ui/lcd/lcd_disp.h"
#include "ui/lcd/lcd_drv_interface.h"
#include "uicon/ui_con.h"
#include "uicon/ui.h"
#include "ui/ui_api.h"
#include "music_ui.h"
#include "bt_ui.h"
#include "ui/ui_common.h"
#include "fm_ui.h"
#include "timer.h"
#include "key_drv/key.h"
#include "file_operate/file_op.h"
#include "dac.h"
#include "rtc_setting.h"
#include "fmtx_api.h"
#include "eq.h"
#include "task_manager.h"
#include "power.h"
#include "task_fm.h"
#include "lyrics_api.h"
#include "encode/encode.h"
#include "rec_api.h"

#define MUSIC_UI_FILENAME_TEXTSIZE  14
#define MUSIC_UI_FOLDER_TEXTSIZE    14


//#if  LCD_128X64_EN

u16 music_ui_filename_pt;
u16 music_ui_foldername_pt;

static const _lcd_area_size_t time_set_backgroup[] = {
    //left,top,right,bottom

    //DVcTime1_5
    {19, 3, 55, 4},
    {64, 3, 82, 4},
    {91, 3, 109, 4},

    //DVcTime2_5
    {41, 5, 59, 6},
    {68, 5, 86, 6},

    //DVcRzImg1_5
    {90, 5, 106, 6},
};

static const GUI_POINT  del_pic_point = {0, 32};
static const GUI_POINT  pause_pic_point = {16, 32};
static const GUI_POINT  vol_pic_point = {0, 0};
static const GUI_POINT  mute_pic_point = {112, 0};
//static const GUI_POINT  echo_pic_point = {80,0};
static const GUI_POINT  eq_pic_point = {110, 0};
static const GUI_POINT  playmode_pic_point = {96, 0};
static const GUI_POINT  rec_opt_pic_point = {32, 48};

static const u8 play_mode_pic[] = {DVcRzImg1_6, DVcRzImg15_6, DVcRzImg3_6, DVcRzImg4_6, DVcRzImg2_6};
static const u8 eq_mode_pic[] = {DVcRzImg5_6, DVcRzImg6_6, DVcRzImg7_6, DVcRzImg8_6, DVcRzImg9_6, DVcRzImg10_6};
static const u8 rec_opt_pic[] = {DVcRzImg4_8, DVcRzImg3_8, DVcRzImg2_8, DVcRzImg4_8};
static const u8 mode_pic[] = {DVcRzImg4_3, DVcRzImg5_3, DVcRzImg6_3}; //SD/U/bt

///------------common-api--------------
void lcd_setX(u8 X)
{

}

int unicode_len(u8 *str)
{
    u8 *src = str;
    while ((*str != 0) || (*(str + 1) != 0)) {
        str += 2;
    }
    return (int)(str - src);
}

void lcd_set_cur_menu(u8 menu)
{
    UI_var.bMenuReturnCnt = 0;
    UI_var.bCurMenu = menu;
}

u16 lcd_disp_text2(const char *str, u8 dvtxt_num, u8 flags)
{


    return 0;
}


void lcd_disp_text(const char *str, u8 dvtxt_num)
{

   // lcd_disp_text2(str, dvtxt_num, ANSI);

    /*
        DTEXT disp_txt;

        disp_txt.buf = (u8 *)str;

        disp_txt.flags = ANSI;
        disp_txt.len = strlen((const char *)disp_txt.buf);
        ui_text(dvtxt_num, &disp_txt);
    */
}

void lcd_disp_string_menu(u8 menu)
{

}

void lcd_null_dis(void)
{

}

int lcd_check_sys_vol(void)//静音显示
{
#if 1
    if ((*(UI_var.sys_vol) == 0) || (is_dac_mute())) {
      //  ui_pic(DVcRzImg1_3, (void *)&mute_pic_point);
        return 1;
    } else {
        return 0;
    }
#endif
}

void  lcd_check_echo_sw(void)//混响显示
{

}


void lcd_disp_rec_error(void)
{

}

void  lcd_disp_rec_opt_state(RECORD_OP_API *rec_api)
{

}

///------------main-display--------------
void lcd_disp_welcome(void)
{
    const char lcd_string[] = {"     HELLO      "};
    lcd_disp_text(lcd_string, DVcTxt1_2);
}

void lcd_disp_IR_number(s32 input_number)
{
    u8 i, input_cnt;

    input_number &= 0xffff;
    //input_cnt = (input_number>>24);
    //printf("ir-input: %d,%d\n",input_number,input_cnt);

    ui_number(DVcNumber1_2, input_number, -1, -1, 0);
}

void lcd_disp_vol(s32 vol)
{
    ui_pic(DVcRzImg5_8, 0);
    ui_number(DVcNumber1_8, vol, -1, -1, 0);
}

void lcd_disp_power(void)
{
    u32 voltage;
    ui_pic(DVcRzImg1_5, 0);
    voltage = get_power_external_value() / 10;
    ui_number(DVcNumber1_5, (u32)voltage, -1, -1, 1);
    voltage = get_power_external_value() % 10;
    ui_number(DVcNumber2_5, (u32)voltage, -1, -1, 1);
}


void lcd_disp_dev(void *dev, u8 dvtxt_num)
{
    char lcd_dev_str[][4] = {
        "SD0",
        "SD1",
        "USB",
        "NON",
    };
    if (dev == sd0) {
        lcd_disp_text(lcd_dev_str[0], dvtxt_num);
    } else if (dev == sd1) {
        lcd_disp_text(lcd_dev_str[1], dvtxt_num);
    } else if (dev == usb) {
        lcd_disp_text(lcd_dev_str[2], dvtxt_num);
    } else {
        lcd_disp_text(lcd_dev_str[3], dvtxt_num);
    }


}

void lcd_disp_eq(s32 eq_mode)
{
    char eq_str[][17] = {
        "     NORMAL",
        "      ROCK",
        "       POP",
        "     CLASSIC",
        "      JAZZ",
        "     COUNTRY",
    };

    lcd_disp_text((char *)eq_str[eq_mode], DVcTxt3_1);
}

void lcd_disp_playmode(s32 play_mode)
{
    char playmode_str[][17] = {
        "       ALL",
        "    ONE LGDEV",
        "       ONE",
        "      RANDOM",
        "      FOLDER",
    };

    lcd_disp_text((char *)playmode_str[play_mode - REPEAT_ALL], DVcTxt3_1);
}

void lcd_disp_filenumber(void)
{

}

///-------------app-display---------------

//返回显示字符偏移位置
int text_scroll_deal(u16 disp_len, u16 text_size, int pt)
{
    if ((disp_len <= text_size) || (disp_len < pt)) {
        return 0;
    }

    disp_len = disp_len - pt;

    if (text_size >= disp_len) {
        return 0;
    }

    return pt + 1;

}

int disp_unicode_len(u8 *str)
{
    int char_cnt = 0;
    u8 *src = str;
    while ((*str != 0) || (*(str + 1) != 0)) {
        if (*(str + 1) == 0) {
            char_cnt++;
        }
        str += 2;
    }
    return (int)(str - src) - char_cnt;
}







///------------music-display--------------


#define BUF_SIZE (160)

uint8_t *bt_data;
int rxBytes = 40;
char event_bt[BUF_SIZE] = {""};




void win1250(void)     // unicode_to_win1250
{

        uint8_t event_index = 0;
        uint8_t count = 0;


        for (int i = event_index; i < rxBytes; i = i + 1)
        {
            if (bt_data[i + 1] == 1)  // polskie znaki mają 01 w pierwszem bajcie
            {

///////////////////////////////////////////
                if (bt_data[i] == 0x04) // Ą
                {
                    event_bt[count] = (char)0xa5;
                    count++;
                }
               // else
               // {
                if (bt_data[i] == 0x06)    // Ć
                    {
                        event_bt[count] = (char)0xc6;
                        count++;
                    }


            if (bt_data[i] == 0x18)    // Ę
                    {
                        event_bt[count] = (char)0xca;
                        count++;
                       // event_bt[count] = (char)bt_data[i] + (char)0x40;
                    }

            if (bt_data[i] == 0x41)    // L
                    {
                        event_bt[count] = (char)0xa3;
                        count++;
                       // event_bt[count] = (char)bt_data[i] + (char)0x40;
                    }

            if (bt_data[i] == 0x43)    // N
                    {
                        event_bt[count] = (char)0xd1;
                        count++;
                    }


            if (bt_data[i] == 0x5a)    // S
                    {
                        event_bt[count] = (char)0x8c;
                        count++;

                    }

            if (bt_data[i] == 0x79)    // Z
                    {
                        event_bt[count] = (char)0x8f;
                        count++;
                       // event_bt[count] = (char)bt_data[i] + (char)0x40;
                    }



            if (bt_data[i] == 0x7b)    // Z
                    {
                        event_bt[count] = (char)0xaf;
                        count++;
                       // event_bt[count] = (char)bt_data[i] + (char)0x40;
                    }


            if (bt_data[i] == 0x05)    // a
                    {
                        event_bt[count] = (char)0xb9;
                        count++;
                       // event_bt[count] = (char)bt_data[i] + (char)0x40;
                    }

            if (bt_data[i] == 0x07)    // c
                    {
                        event_bt[count] = (char)0xe6;
                        count++;
                       // event_bt[count] = (char)bt_data[i] + (char)0x40;
                    }

            if (bt_data[i] == 0x19)    // e
                    {
                        event_bt[count] = (char)0xea;
                        count++;
                       // event_bt[count] = (char)bt_data[i] + (char)0x40;
                    }


            if (bt_data[i] == 0x42)    // l
                    {
                        event_bt[count] = (char)0xb3;
                        count++;
                       // event_bt[count] = (char)bt_data[i] + (char)0x40;
                    }

            if (bt_data[i] == 0x44)    // n
                    {
                        event_bt[count] = (char)0xf1;
                        count++;
                       // event_bt[count] = (char)bt_data[i] + (char)0x40;
                    }


                    if (bt_data[i] == 0x5b)    // s
                    {
                        event_bt[count] = (char)0x9c;
                        count++;
                       // event_bt[count] = (char)bt_data[i] + (char)0x40;
                    }


            if (bt_data[i] == 0x7a)    // z
                    {
                        event_bt[count] = (char)0x9f;
                        count++;
                       // event_bt[count] = (char)bt_data[i] + (char)0x40;
                    }

            if (bt_data[i] == 0x7c)    // z
                    {
                        event_bt[count] = (char)0xbf;
                        count++;
                       // event_bt[count] = (char)bt_data[i] + (char)0x40;
                    }

               } // polskie znaki w 1 bajcie

///////////////////////////////////////////////////////////




            else if (bt_data[i + 1] == 0)
            {
                if (bt_data[i] > 0x1F && bt_data[i] < 0xff)
                {
                    event_bt[count] = (char)bt_data[i];
                    count++;
                }
            }
            else
            {
                if (bt_data[i] > 0x1F && bt_data[i] < 0xFf)
                {
                    event_bt[count] = (char)bt_data[i];
                    count++;
                }
            }
        }
        event_bt[count] = '\0';
   // }


}  // func











extern void lcd_music_main(void)
{

  //  char lcd_file_num[] = {" 0000/0000"};
    char lcd_playtime_str[] = {"00:00"};
    char lcd_total_str[] = {"00:00"};

    MUSIC_DIS_VAR *music_var;

    music_var = (MUSIC_DIS_VAR *)UI_var.ui_buf_adr;

    if (music_var) {


       if (music_var->ui_total_file == 0) {
          //  lcd_disp_text(lcd_string_Chinese, DVcTxt1_2);
            return;
        }



       // music_var->ui_file_info->update_flag = 1;


        if (music_var->ui_file_info->update_flag != 0) {
            //start play music,init parm
            music_ui_filename_pt = 0;
            music_ui_foldername_pt = 0;
            music_var->ui_file_info->update_flag = 0;
            //puts("file_ui_update\n");
        }

      //  point2 = (u8 *)music_var->ui_file_info->file_name.lfn;
     //   point = (u8 *)music_var->ui_file_info->file_name.lfn;
       // tmp = music_var->ui_file_info->file_name.lfn_cnt;



        rxBytes = music_var->ui_file_info->file_name.lfn_cnt;
        bt_data = (u8 *)music_var->ui_file_info->file_name.lfn;


        win1250();

                //   printf("\n");

                   //puts(event_bt) ;

                   printf("#FILE:%s \n",event_bt);


                   printf("#CURR %d \n",music_var->ui_curr_file);
                   printf("#TOTAL %d \n",music_var->ui_total_file);
                 //  printf("\n");


      //  itoa4_api(music_var->ui_curr_file, (u8 *)&lcd_file_num[1]);
     //  itoa4_api(music_var->ui_total_file, (u8 *)&lcd_file_num[6]);

        itoa2_api(music_var->total_time / 60, (u8 *)&lcd_total_str[0]); //tot
        itoa2_api(music_var->total_time % 60, (u8 *)&lcd_total_str[3]); //tot

        itoa2_api(music_var->play_time / 60, (u8 *)&lcd_playtime_str[0]); //play_time
        itoa2_api(music_var->play_time % 60, (u8 *)&lcd_playtime_str[3]); //play_time


       // printf("ggg %s \n",lcd_file_num);

      //  printf("#playtime: %s \n",lcd_playtime_str);
      //  printf("#totaltime: %s \n",lcd_total_str);

        printf("#TIME: %s / %s \n",lcd_playtime_str,lcd_total_str);

        //printf("#tot:%d \n",music_var->total_time);

       // music_var->total_time


    }
} //music main














///------------bt-display--------------
void lcd_bt_main(u8 menu)
{
    BT_DIS_VAR *bt_var;
    GUI_POINT main_pic_point;
    char *lcd_string;

    bt_var = (BT_DIS_VAR *)UI_var.ui_buf_adr;

    const char lcd_string_Chinese[][20] = {
        "   ",
        "   ",
        "    ",
        "    ",
    };

    if (bt_var) {

      //  ui_pic(mode_pic[2], NULL);
      //  ui_pic(eq_mode_pic[bt_var->bt_eq_mode], (GUI_POINT *)&eq_pic_point);




        if ((BT_STATUS_CONNECTING == bt_var->ui_bt_connect_sta) ||
            (BT_STATUS_TAKEING_PHONE == bt_var->ui_bt_connect_sta) ||
            (BT_STATUS_PLAYING_MUSIC == bt_var->ui_bt_connect_sta)) {

            main_pic_point.x = (128 - 60) / 2;
            main_pic_point.y = 32;
          //  ui_pic(DVcRzImg2_2, &main_pic_point);
//#endif
        } else {

            main_pic_point.x = (128 - 42) / 2;
            main_pic_point.y = 32;
          //  ui_pic(DVcRzImg1_2, &main_pic_point);
//#endif
        }

        if (BT_STATUS_CONNECTING == bt_var->ui_bt_connect_sta) {
         //   lcd_string = (char *)lcd_string_Chinese[1];

//            printf("##STATUS_CONNECTING \n");

        } else if (BT_STATUS_PLAYING_MUSIC == bt_var->ui_bt_connect_sta) {
           // lcd_string = (char *)lcd_string_Chinese[2];
     //       printf("##STATUS_PLAYING_MUSIC \n");


        } else if (BT_STATUS_TAKEING_PHONE == bt_var->ui_bt_connect_sta) {
          //  lcd_string = (char *)lcd_string_Chinese[3];
        //    printf("##STATUS_TAKEING_PHONE \n");

        } else {
          //  lcd_string = (char *)lcd_string_Chinese[0];
            //printf("## \n");
        }

      //  lcd_disp_text(lcd_string, DVcTxt2_1);


    }
}

///------------pc-display--------------
void lcd_pc_main(u8 menu)
{

}

void lcd_echo_main(u8 menu)
{}

///------------fm-display--------------
//extern u8 get_sys_halfsec(void);
void lcd_fmtx_main(void)
{}

void lcd_fm_main(void)
{}

void lcd_fm_channel(void)
{}

///------------rtc-display--------------
void lcd_rtc_main(void)
{}

///------------rec-display--------------
void lcd_rec_main(void)
{}

///------------aux-display--------------
void lcd_aux_main(u8 menu)
{}


///------------rtc,alarm set-display--------------
void lcd_rtc_set(void)
{}

///------------rtc,alarm set-display--------------
void lcd_alarm_set(void)
{}


///------------rtc,alarm set-display--------------
void lcd_alarm_ring(void)
{}


//#endif

