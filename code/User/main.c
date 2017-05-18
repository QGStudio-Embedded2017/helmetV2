/**
  ******************************************************************************
  * @file    main.c
  * @author  Jinji.Fang
  * @version V1.0
  * @date    2016-10-16
  * @brief   用3.5.0版本库建的智能急救头盔程序 (可以发送数据到服务器上)
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "delay.h"
#include "exti.h"
#include "gps.h"
#include "uart4.h"
#include "gpio.h"
#include "usart.h"
#include "beep.h"
#include "judge.h"
#include "MPU-6050.h"
#include "answer.h"
#include "spi.h"
#include "malloc.h"
#include "ov2640.h"
#include "sdfs_app.h"
#include "sd.h"
#include "debug.h"
#include "ff.h"
#include "Bluetooth.h"
#include "bsp_usart3.h"
#include "sim900a.h"

#define PHOTO_NUM 2



u8 isAccident = 0;
extern u8* ov2640_framebuf;//照相机帧缓存
extern u8 timecount;
void camera_init();

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */

int main(void)
{
	bool sysWorking = TRUE;
	int count = 0;
	int count2 = 0;
	int i = 0;

	char MPU_data[21];		
  nmea_msg mse;     //GPS信息结构体
	nmea_msg *gps = &mse;              
	u8 *pname;					//带路径的文件名 
	RCC_Configuration();//RCC时钟初始化              
	delay_init();//嘀嗒定时器初始化
	NVIC_Configuration();//NVIC优先级初始化（RTC 和 外部中断）
	GPIO_Configuration();//继电器引脚初始化PC13,PD12
	EXTI_Configuration();//外部中断初始化,中断引脚PC14
	USART_Config(115200);//串口1初始化，用于蓝牙传输数据
	USART3_Config(115200);//串口3初始化，用于GPRS
	UART4_Init(115200);
	delay_ms(1800);

	while(1)
	{	
		delay_ms(100);
		GoToSleep();//进入睡眠模式，等待中断唤醒
		SystemInit();//设置系统时钟
		delay_init();//嘀嗒定时器初始化

		GPIO_Configuration();//继电器引脚初始化PC14,PA11
		GPIO_ResetBits(GPIOD,GPIO_Pin_12);//打开继电器
	
		NVIC_Configuration();//NVIC优先级初始化                                                                                                                                                                                                                                                                                 
		EXTI_Configuration();//外部中断初始化   	
		BEEP_Init();
		KEY_Init(); 
//串口1初始化，用于蓝牙传输数据	
		USART_Config(115200);
//串口3初始化，用于GPRS				
		USART3_Config(115200);
//串口4初始化，用于gps传输数据		
		UART4_Init(9600);
////SD卡初始化
		if(sdfs_app_mnt() != FR_OK)	printf("\r\nSD卡ERROR\r\n");
		else printf("sd ok\r\n");
////gprs初始化		
		count2 = 0;
		//sim900a_tx_printf("ATE1\r");
		while((!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13)) && gprs_init("123.207.124.49","6666") == 0)//GPRS初始化
		{
			count2++;
			if(count2 >= 5)
			{
				printf("GPRS网络状态错误\r\n");//尝试重新初始化
				break ;
			}
		}
		my_mem_init(SRAMIN);//初始化内部内存池
		ov2640_framebuf = mymalloc(SRAMIN,52*1024);//申请帧缓存
		pname=mymalloc(SRAMIN,30);		//为带路径的文件名分配30个字节的内存		    
		if(!pname||!ov2640_framebuf)	//内存分配出错
		{		    
				printf("\r\n内存分配失败!\r\n");
				delay_ms(200);				   
		} 
    i = 0;
		while(OV2640_Init())		//初始化OV2640
		{
				i++;
				delay_ms(200);
				if(i == 5)	break;
		}
		
		while(1)
		{
			SystemInit();
			USART2_Config();//串口2初始化
			USART2_DMA_Config();//DMA初始化
			
			DMA_ClearFlag(DMA1_FLAG_TC6);
			USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);		
			
			sysWorking = TRUE; //判断系统是否处于工作状态
			while(sysWorking)	/* 系统开始工作 */
			{					
				if(DMA_GetFlagStatus(DMA1_FLAG_TC6) == SET)
				{	
					if(!IS_MPU_RUNNING)					break;
					else
					{ 	
						printf("b");			

						if(TRUE == IsAccident())
						{ 

							SendFromMPUtoBluetooth(MPU_data);//打包加速度数据给蓝牙
//						printf("Accident!%f %f %f %f\n", Axis[0], Axis[1], Axis[2], Square(Axis[0]) + Square(Axis[1]) + Square(Axis[2]));						
//							printf("%s\n", MPU_data);				
              			        BEEP_Open();
							
							timecount = 0;
							isAccident = 0;
							TIM4_Init(4999,7199);
							while(isAccident == 0)
							{
								if(KEY_Scan() == 1) isAccident = 1;      							
							}
							BEEP_Close();
							TIM_Cmd(TIM4, DISABLE);
							if(isAccident == 1) 
							{ 	
								printf("not accidet!\r\n");
								break;
							}
							while((UART4_RX_STA & 0x8000 ) == 0);       //等待GPS信息发送完毕
							GPSMSG_Analysis(gps,UART4_RX_BUF);           //分析GPS信息
							Send_NMEA_MSG(gps);                           //打印相关的GPS信息
							UART4_RX_STA = 0;	
//							/*拍摄照片*/	            
							for(count = 0; count < PHOTO_NUM; )
							{
												sprintf((char *)pname, "%d.jpg", count++);
												ov2640_jpg_photo(pname);  
												delay_ms(1000);  
							}			
							
						}
					}
				}
				DMA_ClearFlag(DMA1_FLAG_TC6);
	
				delay_ms(100);
				
				
				if( GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 1)//高点平表示要进入睡眠
				{
					sysWorking = FALSE;
					delay_ms(100);
				}
			}
		  if( GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 1)//高点平表示要进入睡眠
			{
				EXTI->IMR&=~(1<<0);//屏蔽line0上的中断，用于摄像头外部中断通道0		
				break;
			}
		}
	}		
}


/*********************************************END OF FILE**********************/
