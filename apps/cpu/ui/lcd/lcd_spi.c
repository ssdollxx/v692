#include "sdk_cfg.h"
#include "ui/lcd/lcd_spi.h"
#include "ui/ui_api.h"
#include "ui/lcd/lcd_drv_api.h"

#if LCD_128X64_EN

#define WAIT_SPI1_OK()

void SPI1_init(bool is3Wrie, u8 speed)
{

}


void SPI1_WriteByte(u8 dat)
{

}


void SPI1_DmaWrite(void *buf, u16 len)
{

}

#if 0
u8 SPI1_ReadByte(void)
{
    JL_SPI1->CON |= BIT(12);
    JL_SPI1_ > BUF = 0xff;
    WAIT_SPI1_OK();

    return SPI1_BUF;
}

void SPI1_DmaRead(void *buf, tu16 len)
{

}
#endif


#endif/*LCD_128x64_EN*/
