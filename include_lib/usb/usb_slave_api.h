#ifndef _USB_SLAVE_API_H_
#define _USB_SLAVE_API_H_

#include "typedef.h"
#include "uart.h"

/*USB模块打印*/

//#define USB_SLAVE_PUTS
#ifdef USB_SLAVE_PUTS
#define susb_puts           puts
#define susb_printf         printf
#define susb_printf_buf     printf_buf
#define susb_put_u32hex0    put_u32hex
#else
#define susb_puts(...)
#define susb_printf(...)
#define susb_printf_buf(...)
#define susb_put_u32hex0(...)
#endif


/**************************************************************************/
/*
               CLASS  BITMAP
    7   |   6   |   5   |   4   |   3   |   2   |   1   |   0
                                            HID    Audio   Mass Storage
*/
/**************************************************************************/

#define USB_MALLOC_EN       0

#define MASSSTORAGE_CLASS   BIT(0)
#define SPEAKER_CLASS       BIT(1)
#define MIC_CLASS           BIT(2)
#define AUDIO_CLASS         (SPEAKER_CLASS|MIC_CLASS)
#define HID_CLASS           BIT(3)


//----------------------------------
// HID key for audio
//----------------------------------
#define USB_AUDIO_STOP          0x8000
#define USB_AUDIO_FASTFORWARD   0x4000
#define USB_AUDIO_PREFILE       0x2000
#define USB_AUDIO_NEXTFILE      0x1000
#define USB_AUDIO_PP            0x0800
#define USB_AUDIO_MUTE          0x0400
#define USB_AUDIO_VOLDOWN       0x0200
#define USB_AUDIO_VOLUP         0x0100
#define USB_AUDIO_NONE          0x0000

#define USB_AUDIO_PAUSE         0x0080
#define USB_AUDIO_PLAY          0x0040
#define USB_AUDIO_BALANCE_LEFT  0x0020
#define USB_AUDIO_BALANCE_RIGHT 0x0010
#define USB_AUDIO_VOLUME        0x0008
#define USB_AUDIO_STOP_EJECT    0x0004
#define USB_AUDIO_TRACKING_DEC  0x0002
#define USB_AUDIO_TRACKING_INC  0x0001
#define USB_AUDIO_NONE          0x0000

/*for usb slave mass*/
typedef union _U32_U8 {
    u32 dWord;
    u8  aByte[4];
} U32_U8;

typedef struct _USB_SCSI_CSW {
    u32 dCSWSignature;          //[3:0]
    u32 dCSWTag;                //[7:4]
    U32_U8  uCSWDataResidue;        //[11:8]
    u8  bCSWStatus;             //[12]
} USB_SCSI_CSW;

typedef union _USB_BULK_ONLY_TX {
    USB_SCSI_CSW CSW;
    u8 	Ep1_TX_FIFO[13];
} sUSB_BULK_ONLY_TX;

typedef struct _USB_BULK_ONLY_S {
    u8 	Ep1_Data[64];
} sUSB_BULK_ONLY_S;

typedef enum {
    USB_SCSI_STATE_CBW = 0,
    USB_SCSI_STATE_DATA_TX,
    USB_SCSI_STATE_DATA_RX,
    USB_SCSI_STATE_CSW,
} USB_SCSI_STATE;

typedef struct _USB_BULK_INFO {
    u32 dwDatalen;
    void *pBulkBuf;
    volatile USB_SCSI_STATE bState;
    u8	bError;			//<Bulk 传输期间错误，进一步询问错误细节
    u8  bSenseKey;
    u8  bAdditionalSenseCode;
    u8  bAddiSenseCodeQualifier;
} sUSB_BULK_INFO;

typedef struct _USB_SCSI_CBW {
    u32 dCBWSignature;          //[3:0]
    u32 dCBWTag;                //[7:4]
    U32_U8  uCBWDataTransferLength; //[11:8]
    u8  bmCBWFlags;             //[12]
    u8  bCBWLUN;                //[13]
    u8  bCBWLength;             //[14]
    //COMMAND_BLOCK_FORMAT  CBWCB;              //[30:15]CBW
    u8  operationCode;
    u8  LUN;            //<Logical Unit Number
    u8  LBA[4];           //<Logical Block Address[7:31]
    u8  Reserved;
    u8  LengthH;         //<Transfer or Parameter List or Allocation Length
    u8  LengthL;
    u8  XLength;        //<
    u8  Null[6];
} USB_SCSI_CBW;

typedef union _USB_BULK_ONLY_RX { //Bulk-Only Transport protocol
    USB_SCSI_CBW CBW;           //<CBW use SCSI Command Set
    u8  Ep1_RX_FIFO[31];          //<Endpoint1 缓冲 Buffer
} sUSB_BULK_ONLY_RX;

typedef enum {
    SCSI_INTERCEPT_SUCC = 0,        //SCSI拦截成功
    SCSI_INTERCEPT_FAIL,            //SCSI拦截失败
    SCSI_INTERCEPT_UNKNOW_WRITE,    //SCSI拦截写功能
    SCSI_INTERCEPT_UNKNOW_READ,     //SCSI拦截读功能
} SUSB_SCSI_INTERCEPT_RESULT;

typedef struct _sUSB_SLAVE_SCSI_PRIVATE {
    SUSB_SCSI_INTERCEPT_RESULT(*scsi_intercept)(void *, u8);
    void (*cardreader_addrs_protect)(u8);
} sUSB_SLAVE_SCSI_PRIVATE;


/*****************************
        PC_CARD
*****************************/
typedef struct _card_reader_parm {
//    u32(* usb_get_dev_capacity)(void);
    u8  *sDevName;
    u8  bWriteProtectFlag;
    u8  align[3];
} card_reader_parm;

typedef struct _card_reader_status {
    u8 working;
    u8 busy;
    u8 align[2];
} card_reader_status;

typedef struct _usb_slave_init_parm {
    void (*os_msg_post_init)(void);
    u8 vol_left;
    u8 vol_right;
} usb_slave_init_parm;


/*****************************
        PC_CARD_PROTECT
*****************************/
typedef struct _usb_pc_protect {
    u32  gc_ProtectFATNum;
    u32  ProtectedFATSectorIndex[0];
} usb_pc_protect;

typedef struct _sUSB_DEV_IO {
    card_reader_parm card_r_parm;
    void *next_dev;
    u32(* usb_write_dev)(void *buf, u32 lba);
    u32(* usb_read_dev)(void *buf, u32 lba);
    u8(* usb_dev_init)(void);
    u8(* usb_wait_sd_end)(u8 op_sd, u8 retry);
    u32(* usb_get_dev_capacity)(void);
    void (*set_cardreader_popup)(void);
    void (*recover_cardreader_popup)(void);
    u8(*get_sdmmc_popup_status)(void);
    u8(*get_sdmmc_dev_status)(void);
    u32 bAttr;
} sUSB_DEV_IO;

/*****************************
        Typedef Mix
*****************************/
typedef struct _USB_SLAVE_MASS {
    sUSB_BULK_ONLY_S    USB_bulk_only_S;
    sUSB_BULK_ONLY_TX   USB_bulk_only_Tx;
    sUSB_BULK_INFO      USB_bulk_info;
    sUSB_BULK_ONLY_RX   USB_bulk_only_Rx;
    sUSB_SLAVE_SCSI_PRIVATE private_scsi;
    u8 usb_ep_flip;
} sUSB_SLAVE_MASS;


/*for usb slave ep0*/
typedef struct _USB_CLASS_INFO {
    u8  bAudioVolSetFlag;
    u8  bAudioCurL[2];
    u8  bAudioCurR[2];
    u8  bAudioMute;
    u8  bMicSetCur[2];
    u8  bMicMute;
    u8  bHidkey;
    u8  bLun;               //<描述符配置长度
    u8  bClass;             //USB Class
    u8  bmEpBusy;           //<各个端点繁忙情况 [0]Ep0 [1]Ep1 Tx [2]Ep1 Rx [3]Ep2 Rx
} sUSB_CLASS_INFO;

typedef struct _USB_SLAVE_CTL {
    u8  bAddr;              //<Host 分配的地址
    u8  bState;             //<Ep0 控制传输状态机
    u16 wDatalen;           //<Data 阶段数据长度
    u8  *Tx_ptr;		    //<Ep0 控制传输发送buffer
    volatile u8 bEpInStatus;
    volatile u8 bEpOutStatus;
    volatile u8  bDevConfig;
} sUSB_SLAVE_CTL;

typedef struct _USB_DEVICE_STRING {
    u8 *pString;
    u8  bLen;
} USB_DEVICE_STRING;

typedef enum {
    DEVICE_DESCRIPTOR_STR = 0x0,
    CONFIG_DESCRIPTOR_STR,
    MSD_CONFIG_DESCRIPTOR,
    AUDIO_CONFIG_DESCRIPTOR,
    SPEAKER_CONFIG_DESCRIPTOR,
    MIC_CONFIG_DESCRIPTOR,
    HID_CONFIG_DESCRIPTOR,
    HID_REPORT_DESCRIPTOR,
    LANGUAGE_STRING,
    IMANUFACTURE_STRING,
    IPRODUCT_STRING,
    ISERIAL_NUMBER_STRING,
    AUDIO_VOL,
    MIC_VOL,
    MAX_STRING = 15,
} USB_DES;


/*for usb slave audio*/
typedef struct __SPEAKER_FUNAPI {
    void (*output)(u8 *obuf, u32 Len);
    u32(*getlen)(void);
} SPEAKER_FUNAPI;


typedef struct __ADC_USB_SYN_OPS {
    void (*open)(u8 *ptr, u16 frame_len, u16 nch); //frame_len是输出1帧多少byte，nch是输入数据是单声道还是双声道(1/2)
    u32(*need_buf)(void);
    void (*adc_isr_run)(u8 *ptr, u8 *inbuf, u32 inLen); //ADC地址，长度每次 32*2*2个byte
    u32(*usb_isr_run)(u8 *ptr);   //返回值是输出buf的地址，长度是192个byte
    void (*set_frameSize)(u8 *ptr, u16 frame_len);
} ADC_USB_SYN_OPS;


typedef struct __DAC_USB_SYN_OPS {
    void (*open)(u8 *ptr, u32 target_val, SPEAKER_FUNAPI *sf);
    u32(*need_buf)(void);
    u32(*run)(u8 *ptr, u8 *inbuf, u32 inlen);
} DAC_USB_SYN_OPS;

typedef struct __AUDIO_OUTPUT_IO {
    u32(*get_output_cbuf_len)();
    u32(*get_output_cbuf_size)();
    u32(*output_func)(s16 *buffer, u32 buffer_len);
} AUDIO_OUTPUT_IO;

typedef struct _USB_SLAVE_AUDIO {
    u8 *dacsyn_ptr;
    DAC_USB_SYN_OPS *dacUSBsyn_ops;
    u8 *adcsyn_ptr;
    ADC_USB_SYN_OPS *adcUSBsyn_ops;
    AUDIO_OUTPUT_IO output_io;
} USB_SLAVE_AUDIO;


/*****************************
        Typedef Mix
*****************************/
typedef struct _USB_SLAVE_EP0 {
    sUSB_CLASS_INFO     usb_class_info;
    sUSB_SLAVE_CTL      USB_Slave_ctl;
    USB_DEVICE_STRING   usb_device_str[MAX_STRING];
    u8  descriptor[256];
    sUSB_DEV_IO usb_device_reg_null;   //<设备注册表默认为空
    sUSB_DEV_IO sdmmc0_dev;
    sUSB_DEV_IO sdmmc1_dev;
    void (*susb_post_msg)(u32);
    USB_SLAVE_AUDIO audio_var;
} sUSB_SLAVE_EP0;


/*****************************
        Typedef Mix
*****************************/
typedef struct _USB_SLAVE_VAR {
    sUSB_SLAVE_EP0  ep0_var;
    sUSB_SLAVE_MASS card_reader;
} USB_SLAVE_VAR;

// extern func
u8 USB_MassStorage(sUSB_SLAVE_MASS *mass_var, u32 cmd);
u32 user_set_descriptor(USB_DES index, void *pstring, u32 len);


//对应msg.h
enum {
    USB_SLAVE_CMD_MUTE		= 0x502,
    USB_SLAVE_CMD_UNMUTE	= 0x503,
    USB_SLAVE_CMD_VOL		= 0x504,
    USB_SLAVE_CMD_UPDATA	= 0x50b,
};

//----------------------------------
// IO CONTROL CMD
//----------------------------------
enum {
    USB_SLAVE_CLASS_CONFIG_RESET = 1,
    USB_SLAVE_CARD_READER0_REG,
    USB_SLAVE_CARD_READER1_REG,
    USB_SLAVE_GET_SPEAKER_VOL,
    USB_SLAVE_ONLINE_DETECT,
    USB_SLAVE_GET_ONLINE_STATUS,
    USB_SLAVE_RUN_CARD_READER,
    USB_SLAVE_HID,
    USB_SLAVE_MD_PROTECT,
    USB_SLAVE_SPK_OUTPUT_IO_REG,
};

s32 usb_slave_ioctrl_api(void *parm, u32 cmd);
s32 usb_slave_close_api(void *pram);
s32 usb_slave_open_api(void *pram);
s32 usb_slave_init_api(void *parm);

void usb_mic_ladc_input(s16 *buf, u32 inlen);
void usb_device_default_register(void);
void recover_cardreader_popup_api(void);

#if USB_MALLOC_EN == 0
void  usb_slave_alloc_init(void *alloc_buffer, u32 buffer_len);
void *usb_slave_alloc(u32 size);
#endif
#endif/*_USB_SLAVE_API_H_*/
