#ifndef _LCD_SPI_H_
#define _LCD_SPI_H_

#include "typedef.h"

//SPI1接口选择
//#if 1//
//SPI1A
#define LCD_SPI_SET_GROUP()
#define LCD_SPI_SET_DATAIN()
#define LCD_SPI_SET_CLK()
#define LCD_SPI_SET_DATAOUT()



#define LCD_DATA_OUT()
#define	LCD_CLK_OUT()
#define LCD_SPI_SET_MODE_OUT()
#define LCD_SPI_SET_MODE_INOUT()

extern void SPI1_init(bool is3Wrie, u8 speed);
extern void SPI1_WriteByte(u8 dat);
extern u8 SPI1_ReadByte(void);
extern void SPI1_DmaWrite(void *buf, u16 len);
extern void SPI1_DmaRead(void *buf, u16 len);

#endif/*_LCD_SPI_H_*/
