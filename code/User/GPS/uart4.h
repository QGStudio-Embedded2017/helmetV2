#ifndef __UART4_H
#define __UART4_H	 
#include "sys.h"  

#define UART4_MAX_RECV_LEN		300			//最大接收缓存字节数
extern u8  UART4_RX_BUF[UART4_MAX_RECV_LEN]; 		//接收缓冲,最大UART4_MAX_RECV_LEN字节
extern u16 UART4_RX_STA;   						//接收数据状态

void UART4_Init(u32 bound);				//串口4初始化 
void TIM3_Set(u8 sta);
void TIM3_Init(u16 arr,u16 psc);

#endif
