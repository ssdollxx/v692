#include "sdk_cfg.h"
#include "ui/lcd/lcd128X64.h"
#include "ui/lcd/lcd_drv_interface.h"
#include "ui/lcd/lcd_drv_api.h"
#include "ui/lcd/lcd_spi.h"
#include "ui/ui_api.h"
#include "common/common.h"
#include "common/flash_api.h"

#if LCD_128X64_EN

u8 lcd_contrast = 0;

///LCD 初始化命令表
const u8 lcd_init_table[] = {

};

/*----------------------------------------------------------------------------*/
/**@brief    设置显示的起始坐标
   @param     x:列坐标
   @param     y:行坐标
   @return
   @note      void lcd_set(u8 y,u8 x)
*/
/*----------------------------------------------------------------------------*/
void lcd_128x64_set(u8 page, u8 column)
{
    lcd_sendcmd((u8)(LCD_COLUMN_HIGH | column >> 4));
    lcd_sendcmd((u8)(LCD_COLUMN_LOW | (0x0f & column)));
    lcd_sendcmd((u8)(LCD_PAGE_ADDR | page));
}

/*----------------------------------------------------------------------------*/
/**@brief    设置对比度,lcd对比度范围可以是0-63
   @param    c:对比度的值，lcd对比度范围可以是0~60
   @return   void
   @note     void lcd_set_contrast(u8 c)
*/
/*----------------------------------------------------------------------------*/
void lcd_set_contrast(u8 c)
{

}

/*----------------------------------------------------------------------------*/
/**@brief    LCD 硬件初始化
   @param    void
   @return   void
   @note     void lcd_hardware_init(void)
*/
/*----------------------------------------------------------------------------*/
void lcd_hardware_init(void)
{

}

#endif/*LCD_128X64_EN*/

