#ifndef _SIM900A_H_
#define _SIM900A_H_

#include "stm32f10x.h"

typedef enum
{
	SIM900A_FALSE = 0,
    SIM900A_TRUE = 1,
}sim900a_res_e;

typedef enum
{
    SIM900A_NULL                = 0,
    SIM900A_CMD_SEND            = '\r',         
    SIM900A_DATA_SEND           = 0x1A,         //发送数据(ctrl + z)
    SIM900A_DATA_CANCLE         = 0x1B,         //发送数据(Esc)
}sim900a_cmd_end_e;

/*模块控制配置，移植到其它芯片时请修改这些宏*/
#define     SIM900A_TX(cmd)                 sim900a_tx_printf("%s",cmd)   //发送字符串
#define     SIM900A_DELAY(time)             delay_ms(time)                 //延时
#define     SIM900A_SWAP16(data)            __REVSH(data)                  //交换16位高低字节

//#define USE_USART2

#ifdef	USE_USART2
	//串口相关宏定义   当前使用串口2
	#define     SIM900A_IS_RX()                 (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET) //获取串口数据寄存器状态
	#define     SIM900A_RX_LEN(len)             ((char*)get_usart2_Rxbuf_len(&(len)))  	//获取串口接收到的数据长度
	#define     SIM900A_RX()                 	((char*)get_usart2_Rxbuf())  		//获取串口接收到的数据
	#define     sim900a_tx_printf(fmt, ...)     USART2_printf(USART2,fmt,##__VA_ARGS__)     //printf格式发送命令（纯发送，不进行任何接收数据处理）
	#define     SIM900A_CLEAN_RX()              clean_usart2_Rxbuf()
	#define 	SIM900A_SEND_ENDCHAR()			USART_SendData(USART2, 0x1A)
#else
	//串口相关宏定义   当前使用串口3
	#define     SIM900A_IS_RX()                 (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) != RESET) //获取串口数据寄存器状态
	#define     SIM900A_RX_LEN(len)             ((char*)get_usart3_Rxbuf_len(&(len)))  	//获取串口接收到的数据长度
	#define     SIM900A_RX()                 	((char*)get_usart3_Rxbuf())  			//获取串口接收到的数据
	#define     sim900a_tx_printf(fmt, ...)     USART3_printf(USART3,fmt,##__VA_ARGS__)     //printf格式发送命令（纯发送，不进行任何接收数据处理）
	#define     SIM900A_CLEAN_RX()              clean_usart3_Rxbuf()
	#define 	SIM900A_SEND_ENDCHAR()			USART_SendData(USART3, 0x1A)
#endif

/*************************** 基本 功能 ***************************/
extern  uint8_t     	sim900a_cmd         (char *cmd, char *reply,uint32_t waittime );
extern  sim900a_res_e   sim900a_cmd_check   (char *reply);
extern  void 			get_CCID					(char num[20]);
extern  void 			get_CSQ						(char *num);
extern  void 			get_IMEI					(char num[15]);

/*************************** 电话 功能 ***************************/
extern  uint8_t     	sim900a_cnum                (char *num);                        //获取本机号码
extern  char *      	sim900a_waitask             (uint8_t waitask_hook(void));       //等待有数据应答，返回接收缓冲区地址
        
/*************************** GPRS 功能 ***************************/
extern  sim900a_res_e   gprs_init(char* ip,char* port);
extern  void        	  sim900a_gprs_init           (void);                                                 //GPRS初始化环境
extern  sim900a_res_e   sim900a_gprs_tcp_link       (char * serverip,char * serverport);    //TCP连接
extern	sim900a_res_e sim900a_gprs_send(char * str);
extern  sim900a_res_e   sim900a_gprs_send_photo           (char *str,u32 len);                                            //发送数据
extern  void        	  sim900a_gprs_link_close     (void);                                                 //IP链接断开
extern  sim900a_res_e  	sim900a_gprs_status			(void);	//判断网络连接状态 
extern  sim900a_res_e 	gprs_reg_status				(void);
extern  char* 			    sim900a_gprs_checkIP		(void);
extern  sim900a_res_e   gps_to_place(float longitude,float latitude,char* result);


extern  char ccid[20];
extern  char csq;
extern  char ime[16];

#endif

