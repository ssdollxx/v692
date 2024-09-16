#include "sdk_cfg.h"
#include "uart_user.h"
#include "uart_param.h"
#include "common/msg.h"
#include "audio/eq.h"
#include "clock.h"
#include "fcc_test.h"
/* #include "uart_driver.h" */
#include "irq_api.h"
#include "updata.h"
#include "music_player.h"
#include "msg.h"

#include "task_manager.h"
#include "task_common.h"

#include "common/flash_api.h"
#include "common/common.h"

#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".system_bss")
#pragma data_seg(	".system_data")
#pragma const_seg(	".system_const")
#pragma code_seg(	".system_code")
#endif


extern void register_handle_printf_putchar(void (*handle)(char a));

void parse_uart_data( char * pBuf );



//#define UART_DEBUG_RAE	 UART_BAUD_RAE
//#define UART_FCC_RAE     9600
//#define UART_BQB_RAE	 57600
//#define UART_EQ_BDG_RAE  9600

#define __DEBUG

u8 *rx_uart0_buf = NULL;
u8 *rx_uart1_buf = NULL;
u8 *rx_uart2_buf = NULL;
u32 ut_dma_wr_cnt[MAX_UART_NUM];

static __uart_info uart_info[MAX_UART_NUM];

void putbyte(char a)
{
#ifdef __DEBUG
#if (DEBUG_UART == UART0)
    JL_UART0->BUF = a;
    __asm__ volatile("csync");

    while ((JL_UART0->CON0 & BIT(15)) == 0);    //TX IDLE
#elif (DEBUG_UART == UART1)
    JL_UART1->BUF = a;
    __asm__ volatile("csync");

    while ((JL_UART1->CON0 & BIT(15)) == 0);    //TX IDLE
#elif (DEBUG_UART == UART2)
    JL_UART2->BUF = a;
    __asm__ volatile("csync");

    while ((JL_UART2->CON & BIT(15)) == 0);    //TX IDLE
#endif
#endif
}

char getbyte()
{
#ifdef __DEBUG
    s32 i;
    char c = 0;
#if (DEBUG_UART == UART0)
    if ((JL_UART0->CON0 & BIT(14))) {
        c = JL_UART0->BUF;
        JL_UART0->CON0 |= BIT(12);
    }

#elif (DEBUG_UART == UART1)
    if ((JL_UART1->CON0 & BIT(14))) {
        c = JL_UART1->BUF;
        JL_UART1->CON0 |= BIT(12);
        JL_UART1->CON1 |= BIT(13);
    }

#elif (DEBUG_UART == UART2)
    if ((JL_UART2->CON & BIT(14))) {
        c = JL_UART2->BUF;
        JL_UART2->CON |= BIT(12);
    }
#endif

    return c;
#endif
    return 0;
}



void putchar(char a)
{
#ifdef __DEBUG
    if (a == '\n') {
        putbyte(0x0d);
        putbyte(0x0a);
    } else {
        putbyte(a);
    }
#endif
}

void put_u4hex(u8 dat)
{
#ifdef __DEBUG
    dat = 0xf & dat;

    if (dat > 9) {
        putbyte(dat - 10 + 'A');
    } else {
        putbyte(dat + '0');
    }
#endif
}
void put_u32hex(u32 dat)
{
#ifdef __DEBUG
    putbyte('0');
    putbyte('x');
    put_u4hex(dat >> 28);
    put_u4hex(dat >> 24);

    put_u4hex(dat >> 20);
    put_u4hex(dat >> 16);

    put_u4hex(dat >> 12);
    put_u4hex(dat >> 8);

    put_u4hex(dat >> 4);
    put_u4hex(dat);
    putchar('\n');
#endif
}

void put_u32hex0(u32 dat)
{
#ifdef __DEBUG
    put_u4hex(dat >> 28);
    put_u4hex(dat >> 24);

    put_u4hex(dat >> 20);
    put_u4hex(dat >> 16);

    put_u4hex(dat >> 12);
    put_u4hex(dat >> 8);

    put_u4hex(dat >> 4);
    put_u4hex(dat);
#endif
}

void put_u64hex(u64 dat)
{
#ifdef __DEBUG
    putbyte('0');
    putbyte('x');
    put_u32hex0(dat >> 32);
    put_u32hex0(dat);
    putchar('\n');
#endif
}
void put_u16hex(u16 dat)
{
#ifdef __DEBUG
    putbyte('0');
    putbyte('x');


    put_u4hex(dat >> 12);
    put_u4hex(dat >> 8);

    put_u4hex(dat >> 4);
    put_u4hex(dat);
    putchar(' ');
#endif
}

void put_u8hex(u8 dat)
{
#ifdef __DEBUG
    put_u4hex(dat >> 4);
    put_u4hex(dat);
    putchar(' ');
#endif
}

void printf_buf(u8 *buf, u32 len)
{
#ifdef __DEBUG
    put_u32hex((u32)buf);
    u32 i ;
    for (i = 0 ; i < len ; i++) {
        if (i && (i % 16) == 0) {
            putbyte('\r') ;
            putbyte('\n') ;
        }
        put_u8hex(buf[i]) ;
    }
    putbyte('\n') ;
#endif

}

int puts(const char *out)
{
#ifdef __DEBUG
    while (*out != '\0') {
        putchar(*out);
        out++;
    }
#endif
    return 0;
}

void put_buf(u8 *buf, u32 len)
{
#ifdef __DEBUG
    u32 i ;
    for (i = 0 ; i < len ; i++) {
        if ((i % 16) == 0) {
            putchar('\n') ;
        }
        put_u8hex(buf[i]) ;
    }
    putchar('\n') ;
#endif
}



void uart1_isr_fun()
{

   // puts("%%%%%%%%%%%%%%%%%%%%%%%%5555555_booti");

    u8 uto_buf;
    if ((JL_UART1->CON0 & BIT(3)) && (JL_UART1->CON0 & BIT(14))) {
        uto_buf = JL_UART1->BUF;
        JL_UART1->CON0 |= BIT(12);
        if (uart_info[1].callback_fun) {
            uart_info[1].callback_fun(uto_buf, rx_uart1_buf, UART_ISR_TYPE_DATA_COME);
        }
    }

    if ((JL_UART1->CON0 & BIT(2)) \
        && (JL_UART1->CON0 & BIT(15))) {     //TX PND
        JL_UART1->CON0 |= BIT(13);     //清TX PND
        if (uart_info[1].callback_fun) {
            uart_info[1].callback_fun(0, NULL, UART_ISR_TYPE_WRITE_OVER);
        }
    }

    if (JL_UART1->CON0 & BIT(11)) {    //OTCNT PND
        JL_UART1->CON0 |= BIT(10);    //清OTCNT PND
        JL_UART1->CON0 |= BIT(7);     //RDC
        asm volatile("nop");   //写RDC立刻读JL_UART1->HRCNT会有问题
        JL_UART1->CON0 |= BIT(12);    //清RX PND(这里的顺序不能改变，这里要清一次)
        if (uart_info[1].callback_fun) {
            uart_info[1].callback_fun(0, rx_uart1_buf, UART_ISR_TYPE_TIMEOUT);
        }
        if (rx_uart1_buf != NULL) {
            JL_UART1->RXSADR = (u32)rx_uart1_buf;
            JL_UART1->RXCNT = (u32)ut_dma_wr_cnt[1];;
        }
    }
}
IRQ_REGISTER(IRQ_UART1_IDX, uart1_isr_fun);

/////////////////////////// *****************************
#define UART_RX_BUF_SIZE 64
#define UART_RX_BUF_MASK ( UART_RX_BUF_SIZE - 1)

volatile uint8_t ascii_line;
// definiujemy w końcu nasz bufor UART_RxBuf
volatile char UART_RxBuf[UART_RX_BUF_SIZE];
// definiujemy indeksy określające ilość danych w buforze
volatile uint8_t UART_RxHead; // indeks oznaczający „głowę węża”
volatile uint8_t UART_RxTail; // indeks oznaczający „ogon węża”



// definiujemy funkcję pobierającą jeden bajt z bufora cyklicznego
int uart_getc(void) {
	int data = -1;
    // sprawdzamy czy indeksy są równe
    if ( UART_RxHead == UART_RxTail ) return data;
   // ATOMIC_BLOCK( ATOMIC_RESTORESTATE ) {
        // obliczamy i zapamiętujemy nowy indeks „ogona węża” (może się zrównać z głową)
        UART_RxTail = (UART_RxTail + 1) & UART_RX_BUF_MASK;
        // zwracamy bajt pobrany z bufora  jako rezultat funkcji
        data = UART_RxBuf[UART_RxTail];
   // }
    return data;
}

char * uart_get_str(char * buf) {
	int c;
	char * wsk = buf;
	if( ascii_line ) {
		while( (c = uart_getc()) ) {
			if( 13 == c || c < 0) break;
			*buf++ = c;
		}
		*buf=0;
		ascii_line--;
	}
	return wsk;
}




void UART_RX_STR_EVENT() {

char zbuf[64];

	if( ascii_line ) {

		uart_get_str( zbuf );
        parse_uart_data( zbuf );

		} else {
		//	UART_RxHead = UART_RxTail;
		}
	} //end






void user_common_uart_isr(u8 uto_buf, void *p, u8 isr_flag)
{

uint8_t tmp_head;


    if (UART_ISR_TYPE_DATA_COME == isr_flag) {

        tmp_head = ( UART_RxHead + 1) & UART_RX_BUF_MASK;

    if ( tmp_head == UART_RxTail ) {

    	//UART_RxHead = UART_RxTail;
    } else {
    	switch( uto_buf ) {
    		case 0:					// ignorujemy bajt = 0
    		case 10: break;			// ignorujemy znak LF
    		case 13: ascii_line++;	// sygnalizujemy obecność kolejnej linii w buforze
    		default : UART_RxHead = tmp_head; UART_RxBuf[tmp_head] = uto_buf;
    	                  }
            }
                                            }
}



static void uart_module_on()
{

    SFR(JL_CLOCK->CLK_CON1, 10, 2, 0); //use osc
    uart_info[1].callback_fun = user_common_uart_isr;

    JL_UART1->CON0 = 0;
   // JL_UART2->CON = 0;
}



s32 uart_debug_int(u32 baud_rate)    //UART1_TXPB0_RXPB1
{
    s32 status = 0;


//UART1 CONFIG
#if (DEBUG_UART==UART1)
#if (DEBUG_UART_SEL ==  UART1_TXPB0_RXPB1)
    JL_IOMAP->CON1 &= ~(BIT(3) | BIT(2));
    JL_PORTB->OUT |= BIT(0) ;
    JL_PORTB->DIR |= BIT(1) ;
    JL_PORTB->DIR &= ~BIT(0) ;


#endif  //end of UART1 IO SEL
    JL_UART1->BAUD = (UART_CLK / baud_rate) / 4 - 1;

#endif  //end of UART1 CONFIG

    register_handle_printf_putchar(putchar);

       // uart_info[0].callback_fun = user_common_uart_isr;
    uart_info[1].callback_fun = user_common_uart_isr;
  //  uart_info[2].callback_fun = user_common_uart_isr;


    JL_UART1->CON0 = BIT(13) | BIT(12) | BIT(10) | BIT(3)| BIT(0);  //bit3 przetrwanie rx

    IRQ_REQUEST(IRQ_UART1_IDX, uart1_isr_fun);
    irq_set_prio(IRQ_UART1_IDX, 2); // The priority is set to 2, up to 3


    return status;
}


void uart_module_init()
{
    uart_module_on();
    uart_debug_int(DEBUG_UART_RATE);
}


//  AT+CA Specified volume There are parameters behind.See 4.2.1 for details.
//  AT+CB Play pause See 4.2.1 for details.
//  AT+CC Next song See 4.2.1 for details.
//  AT+CD Last song See 4.2.1 for details.
//  AT+CE Volume + See 4.2.1 for details.
//  AT+CF Volume - See 4.2.1 for details.

//////// ############################## /////////


//MUSIC_PLAYER *obj;

void sw_to_bt(void){

    int i;
    for(i = 0; i <= 20; i++) {

    if (task_get_cur() != TASK_ID_BT) { //TASK_ID_BT TASK_ID_IDLE

           task_post_msg(NULL, 1, MSG_CHANGE_WORKMODE);
                                        }//if
}

}


void parse_uart_data( char * xbuf ) {
     //  	char * reszta;
	//uint8_t  len;

        if (strncmp (xbuf,"AT+CC",5) == 0)    /// AT+CC -> NEXT_FILE  1
    {
    printf("NEXT_FILE\n");
    task_post_msg(NULL, 1, MSG_MUSIC_NEXT_FILE);
    task_post_msg(NULL, 1, MSG_BT_NEXT_FILE);

    //MSG_BT_NEXT_FILE
    }



        if (strncmp (xbuf,"AT+CD",5) == 0) /// AT+CD -> PREV_FILE 2
    {
    printf("PREV_FILE\n");
    task_post_msg(NULL, 1, MSG_MUSIC_PREV_FILE);
    task_post_msg(NULL, 1, MSG_BT_PREV_FILE);


    if (task_get_cur() == TASK_ID_MUSIC) { //TASK_ID_BT
                puts("MUSIC 1410 \n");
                                         }

        if (task_get_cur() == TASK_ID_BT) { //TASK_ID_BT
             //   puts("blutaczek 1410 \n");
                                         }




    }


            if (strncmp (xbuf,"AT+XX",5) == 0) /// AT+CD -> PREV_FILE 2
    {
   // printf("PREV_FILE\n");
    //task_post_msg(NULL, 1, MSG_MUSIC_PREV_FILE);
    //task_post_msg(NULL, 1, MSG_BT_PREV_FILE);


    if (task_get_cur() == TASK_ID_MUSIC) { //TASK_ID_BT
              //  puts("xxx 1410 \n");
                task_post_msg(NULL, 1, MSG_BT_PP);
                                         }






    }


        if (strncmp (xbuf,"AT+CB",5) == 0) /// AT+CB -> MSG_MUSIC_PP 3
    {
    printf("MSG_MUSIC_PP \n");
    task_post_msg(NULL, 1, MSG_MUSIC_PP);
    }


            if (strncmp (xbuf,"AT+ST",5) == 0) /// AT+CB -> MSG_MUSIC_PP 3
    {
    printf("MSG_MUSIC_stop \n");
    task_post_msg(NULL, 1, MSG_MUSIC_STOP);
    }



       if (strncmp (xbuf,"AT+CU",5) == 0) /// AT+CU -> MSG_MUTE 4
    {
    printf("MSG_MUTE \n");
    task_post_msg(NULL, 1, MSG_MUTE);
    }


        if (strncmp (xbuf,"AT+CE",5) == 0) /// AT+CE -> MSG_VOL_UP 5
    {
    printf("MSG_VOL_UP \n");
    task_post_msg(NULL, 1, MSG_VOL_UP);
    }



        if (strncmp (xbuf,"AT+CF",5) == 0) /// AT+CF -> MSG_VOL_DOWN 6
    {
    printf("MSG_VOL_DOWN \n");
    task_post_msg(NULL, 1, MSG_VOL_DOWN);
    }


    //if (dev_online_mount(usb)) {
   // }


        if (strncmp (xbuf,"AT+CM00",7) == 0) /// AT+CB -> MSG_CHANGE_WORKMODE
    {
    printf("MSG_CHANGE_WORKMODE \n");
    task_post_msg(NULL, 1, MSG_CHANGE_WORKMODE);
  //  task_switch(TASK_ID_BT, NULL);

    }


        if (strncmp (xbuf,"AT+CM01",7) == 0) /// AT+CM01 -> switch to blue
    {
    printf("switch to blue \n");
    sw_to_bt();

    } //end

        if (strncmp (xbuf,"AT+CM02",7) == 0) /// AT+CM02 -> switch to music
    {
    printf("switch to music \n");


   // int i;
   // for(i = 0; i <= 100; i++) {

    if ((task_get_cur() == TASK_ID_BT) |  (task_get_cur() == TASK_ID_IDLE)) { //TASK_ID_BT TASK_ID_IDLE


           while(task_post_msg(NULL, 1, MSG_CHANGE_WORKMODE));
           printf("sprawdzanie czy bt lub idle %d \n",task_get_cur()); }


        if ((task_get_cur() == TASK_ID_BT) |  (task_get_cur() == TASK_ID_IDLE)) { //TASK_ID_BT TASK_ID_IDLE


           while(task_post_msg(NULL, 1, MSG_CHANGE_WORKMODE));
           printf("sprawdzanie czy bt lub idle %d \n",task_get_cur()); }

               if ((task_get_cur() == TASK_ID_BT) |  (task_get_cur() == TASK_ID_IDLE)) { //TASK_ID_BT TASK_ID_IDLE


           while(task_post_msg(NULL, 1, MSG_CHANGE_WORKMODE));
           printf("sprawdzanie czy bt lub idle %d \n",task_get_cur()); }


    } //end

        if (strncmp (xbuf,"AT+CM08",7) == 0) /// AT+CM02 -> switch to music
    {
    printf("switch tomusic \n");
    //task_post_msg(NULL, 1, MSG_SD1_ONLINE);
    task_switch(TASK_ID_MUSIC, NULL);

    }


    // TASK_ID_MUSIC


       if (strncmp (xbuf,"AT+88",5) == 0) /// AT+CU -> MSG_
    {
    printf("MSG_MUTE \n");
    task_post_msg(NULL, 1, MSG_MUTE);
    }


        if (strncmp (xbuf,"AT+88",5) == 0) /// AT+CB -> MSG_
    {
    printf("MSG_VOL_UP \n");
    task_post_msg(NULL, 1, MSG_VOL_UP);
    }



        if (strncmp (xbuf,"AT+88",5) == 0) /// AT+CB -> MSG_
    {
    printf("MSG_VOL_DOWN \n");
    task_post_msg(NULL, 1, MSG_VOL_DOWN);
    }




//    MSG_CHANGE_WORKMODE


    if (strncmp (xbuf,"M1+",3) == 0) /// tylko jak zaczyna sie na m1+
    {

    //	send_string("AT+AA01\r\n") ; //Play command
    	//send_string(pBuf+8); send_string("\r") ;
    	//num = (int)strtol(pBuf+8, NULL, 16); // hex to int

    }

    #include "charge.h"

    if (strncmp (xbuf,"S+",2) == 0) /// tylko jak zaczyna sie na mf+
    {


    char tyty[8] = "poko";

   // vm_write(VM_FM_TRANSMIT_FREQ, &tyty, 6);

     vm_write(55, &tyty, 4);

     printf("write vm \n");

     printf("reboot !!! \n");

     delay_2ms(500);

     JL_POWER->CON |= BIT(4);  //reboot

    //	strtok(xbuf, "+");





}






}
