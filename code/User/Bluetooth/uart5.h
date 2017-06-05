#ifndef __UART5_H
#define __UART5_H	 
#include "sys.h"  

#define UART5_MAX_RECV_LEN		100			//最大接收缓存字节数
extern u8  UART5_RX_BUF[UART5_MAX_RECV_LEN]; 		//接收缓冲,最大UART5_MAX_RECV_LEN字节
extern u16 UART5_RX_STA;   						//接收数据状态

void UART5_Init(u32 bound);				//串口5初始化 
void TIM2_Set(u8 sta);
void TIM2_Init(u16 arr,u16 psc);

#endif
