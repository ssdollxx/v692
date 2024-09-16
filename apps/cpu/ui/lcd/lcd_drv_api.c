#include "sdk_cfg.h"
#include "ui/lcd/lcd_drv_api.h"
#include "ui/lcd/lcd_spi.h"
#include "ui/lcd/lcd_drv_interface.h"
#include "ui/lcd/lcd_disp.h"
#include "ui/lcd/lcd128X64.h"
#include "uicon/ui_con.h"
#include "uicon/menu.h"
#include "ui/ui_api.h"
#include "uicon_api.h"
#include "font_api.h"

#if LCD_128X64_EN

#define LCD_MALLOC_EN	0

#define LCD_SIZE	(0)

#if LCD_MALLOC_EN
extern void *zebra_malloc(u32 size);
u8 *disp_buf;
#else
/* u8 disp_buf[LCD_SIZE] AT(.ram1_bss); */
u8 disp_buf[LCD_SIZE] ;
#endif


/*----------------------------------------------------------------------------*/
/**@brief   lcd update buf
   @param   void
   @return  void
   @note    void draw_lcd_buf(void)
*/
/*----------------------------------------------------------------------------*/
void draw_lcd_buf(void)
{

}

/*----------------------------------------------------------------------------*/
/**@brief   lcd spi初始化
   @param   void
   @return  void
   @note    void lcd_spi_init(void)
*/
/*----------------------------------------------------------------------------*/
void lcd_clear(void)
{

}


/*----------------------------------------------------------------------------*/
/**@brief   LCD显示初始化,包括：硬件初始化，字体初始化，控件初始化
   @param   void
   @return  void
   @note    void lcd_init(void)
*/
/*----------------------------------------------------------------------------*/
void lcd_init(void)
{

}

#endif/*LCD_128x64_EN*/
