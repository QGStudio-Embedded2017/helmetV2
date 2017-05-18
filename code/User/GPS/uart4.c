#include "delay.h"
#include "uart4.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	   
 	
u8 UART4_RX_BUF[UART4_MAX_RECV_LEN]; 				//接收缓冲,最大UART4_MAX_RECV_LEN个字节.
u16 UART4_RX_STA=0;  

/*
@name: UART4_IRQHandler
@param: none
@return: none
@description: UART4中断处理函数（接收数据）
*/
void UART4_IRQHandler(void)
{
	u8 res;	    

	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//接收到数据
	{	 
    res = USART_ReceiveData(UART4);		  	
		if(UART4_RX_STA < UART4_MAX_RECV_LEN)		//还可以接收数据
		{ 

			TIM_SetCounter(TIM3,0);//计数器清空        				 
			if(UART4_RX_STA==0)   TIM3_Set(1);	 	//使能定时器3的中断 
			UART4_RX_BUF[UART4_RX_STA++]=res;		//记录接收到的值	 
			UART4_RX_BUF[UART4_RX_STA] = '\0';
		}
		else  UART4_RX_STA |= 1<<15;					//强制标记接收完成
	}  											 
}   

/*
@name:UART4_Init();
@param: bound--串口4的波特率
@return: none
@description: 初始化串口4
*/
void UART4_Init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);	// GPIOC时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);   //USART2时钟

  /*配置PC10，即UART4 Tx*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PC.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化PA2
  /*配置PC11，即UART4 Rx*/    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOC, &GPIO_InitStructure);  //初始化PC11
	
	/*配置UART4串口*/
	USART_InitStructure.USART_BaudRate = bound;//波特率设置为bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(UART4, &USART_InitStructure); //初始化串口4
  
	USART_Cmd(UART4, ENABLE);                    //使能串口 
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启中断   
	/*UART4中断*/
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	TIM3_Init(99,7199);		//10ms中断
	UART4_RX_STA=0;		//清零
	TIM3_Set(0);			//关闭定时器3	 	
}

/*
@name:TIM3_IRQHandler();
@param:none
@return:none
@description:定时器3中断函数，设置串口2的接受完成标记
*/
void TIM3_IRQHandler(void)
{ 	
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == RESET) return ;//是更新中断
 	UART4_RX_STA |= 0x8000;	//标记接收完成
//	printf("UART4%d",UART4_RX_STA);
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx更新中断标志    
	TIM3_Set(0);			//关闭TIM3   
}

/*
@name： TIM3_Set();
@param:  sta--定时器状态，1为打开计时器，0为关闭计时器
@return： none
@description:定时器3的使能与失能
*/
void TIM3_Set(u8 sta)
{
	if(sta == 0)  
	{
		TIM_Cmd(TIM3, DISABLE);//关闭定时器3
		return ;
	}
    TIM_SetCounter(TIM3,0);//计数器清空
	TIM_Cmd(TIM3, ENABLE);  //使能TIMx	 	   
}


/*
@name:TIM3_Init(u16 arr,u16 psc);
@param:arr--自动重装值,psc--时钟预分频数	
@return: none
@desription:通用定时器中断初始化
*/
void TIM3_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能//TIM3时钟使能    
	/*定时器TIM3初始化*/
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断
	/*配置TIM3中断*/ 	  
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
}

