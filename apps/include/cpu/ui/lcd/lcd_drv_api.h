#ifndef __LCD_DRV_API_H__
#define __LCD_DRV_API_H__

#include "sdk_cfg.h"
#include "rtc_api.h"

///************************************************************/
///****************配置屏幕的大小
///****************说明：点阵屏是一个点对应一个bit
///************************************************************/
#define  LCDPAGE            8
#define  LCDCOLUMN          128

#define SCR_WIDTH           LCDCOLUMN
#define SCR_HEIGHT          (LCDPAGE*8)



#if 0
#define LCD_A0_PORT         JL_PORTC
#define LCD_A0_BIT          (BIT(3))
#else
#define LCD_A0_PORT         JL_PORTA
#define LCD_A0_BIT          (BIT(11))
#endif
#define	LCD_A0_OUT()
#define	LCD_A0_L()
#define	LCD_A0_H()

#if 0
#define LCD_RES_PORT        JL_PORTC
#define LCD_RES_BIT         (BIT(2))
#else
#define LCD_RES_PORT        JL_PORTA
#define LCD_RES_BIT         (BIT(13))
#endif
#define LCD_RES_OUT()
#define LCD_RES_L()
#define LCD_RES_H()
#if 0
#define LCD_CS_PORT         JL_PORTC
#define LCD_CS_BIT          (BIT(2))
#else
#define LCD_CS_PORT         JL_PORTA
#define LCD_CS_BIT          (BIT(9))
#endif
#define LCD_CS_OUT()
#define LCD_CS_L()
#define LCD_CS_H()


#define LCD_BL_ON()
#define LCD_PORT_OUT()
#define LCD_PORT_OUT_H()

//全局变量声明
extern u8 disp_buf[];

//函数声明
void draw_lcd_buf(void);
void lcd_clear(void);
void lcd_init(void);

#endif/*__LCD_DRV_API_H__*/
