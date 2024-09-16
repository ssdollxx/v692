#ifndef _SYS_DETECT_H_
#define _SYS_DETECT_H_

//#define SYS_DETECT_DBG
#ifdef SYS_DETECT_DBG
#define sys_det_puts           puts
#define sys_det_printf         printf
#define sys_det_buf            put_buf
#else
#define sys_det_puts(...)
#define sys_det_printf(...)
#define sys_det_buf(...)
#endif/*SYS_DETECT_DBG*/

#define SD_DADA_MULT	0

#define BD_CONN_MODE    0x01
#define BD_PAGE_MODE    0x02

#define PWR_ON          0x01
#define PWR_OFF         0x00

//USB IO define
#define USB_DP_OUT(x)		JL_USB->IO_CON0 = ((JL_USB->IO_CON0 & ~BIT(0)) | ((x & 0x1)<<0))
#define USB_DM_OUT(x)		JL_USB->IO_CON0 = ((JL_USB->IO_CON0 & ~BIT(1)) | ((x & 0x1)<<1))
#define USB_DP_DIR(x)		JL_USB->IO_CON0 = ((JL_USB->IO_CON0 & ~BIT(2)) | ((x & 0x1)<<2))
#define USB_DM_DIR(x)		JL_USB->IO_CON0 = ((JL_USB->IO_CON0 & ~BIT(3)) | ((x & 0x1)<<3))
#define USB_DP_PD(x)		JL_USB->IO_CON0 = ((JL_USB->IO_CON0 & ~BIT(4)) | ((x & 0x1)<<4))
#define USB_DM_PD(x)		JL_USB->IO_CON0 = ((JL_USB->IO_CON0 & ~BIT(5)) | ((x & 0x1)<<5))
#define USB_DP_PU(x)		JL_USB->IO_CON0 = ((JL_USB->IO_CON0 & ~BIT(6)) | ((x & 0x1)<<6))
#define USB_DM_PU(x)		JL_USB->IO_CON0 = ((JL_USB->IO_CON0 & ~BIT(7)) | ((x & 0x1)<<7))
#define USB_DP_DIE(x)		JL_USB->IO_CON0 = ((JL_USB->IO_CON0 & ~BIT(9)) | ((x & 0x1)<<9))
#define USB_DM_DIE(x)		JL_USB->IO_CON0 = ((JL_USB->IO_CON0 & ~BIT(10)) | ((x & 0x1)<<10))

#define USB_DP_IN()     	(JL_USB->CON0&BIT(17))
#define USB_DM_IN()     	(JL_USB->CON0&BIT(16))

void sys_init(void);
void check_power_off(void);
void clr_PINR_ctl(void);
extern void set_keep_osci_flag(u8 flag);
extern void soft_poweroff();
extern u8 soft_poweroff_cnt;
extern u8 bd_work_mode;

void open_wdt(u8 cfg);//开看门狗
void close_wdt(void);//关看门狗
void clear_wdt(void);//清看门狗
void sd_data_multiplex(void);
void pwm4_cfg(u8 toggle, u8 pre_div, u8 duty);
void usb_sd_detect_mult_api(void);
void pwr_level_config(u8 default_level);
u8 get_pwr_config_flag();
void pa_umute(void);
void pa_mute(void);
#endif/*_SYS_DETECT_H_*/
