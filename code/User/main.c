/**
  ******************************************************************************
  * @file    main.c
  * @author  Shengdui.Liang
  * @version V2.0
  * @date    2017-5-30
  * @brief   用3.5.0版本库建的智能急救头盔程序 (可以发送数据到服务器上)
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "delay.h"
#include "exti.h"
#include "usart.h"
#include "judge.h"
#include "MPU-6050.h"
#include "gpio.h"
#include "answer.h"
#include "beep.h"
#include "malloc.h"
#include "ov2640.h"
#include "sdfs_app.h"
#include "sd.h"
#include "stdlib.h"
#include "debug.h"
#include "ff.h" 
#include "gps.h"
#include "Bluetooth.h"
#include "bsp_usart3.h"
#include "sim900a.h"
#include "uart4.h"


#define PHOTO_NUM 3




extern u8* ov2640_framebuf;//照相机帧缓存
extern u8 timecount;
extern u8 isAccident;
extern float Angular[3];
nmea_msg mse;
nmea_msg *gps = &mse;


void gpsData_send(void);
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */

int main(void)
{
	u8 initCount,photoNum;
	char MPU_data[21];		             
	u8 *pname;					//带路径的文件名 
	RCC_Configuration();//RCC时钟初始化              
	delay_init();//嘀嗒定时器初始化
	NVIC_Configuration();//NVIC优先级初始化（RTC 和 外部中断）
	GPIO_Configuration();	
	EXTI_Configuration();//外部中断初始化   
loop:	
	GPIO_SetBits(GPIOD,GPIO_Pin_12);//关闭继电器
	GoToSleep();
	SystemInit();//系统时钟初始化
	
	RCC_Configuration();//RCC时钟初始化              
	delay_init();//嘀嗒定时器初始化
	GPIO_Configuration();//继电器引脚初始化PC14,PA11
	GPIO_ResetBits(GPIOD,GPIO_Pin_12);//打开继电器
	NVIC_Configuration();//NVIC优先级初始化（RTC 和 外部中断）                                                                                                                                                                                                                                                                                
	EXTI_Configuration();//外部中断初始化  
	USART_Config(9600);  //串口1初始化，用于调试使用
	USART3_Config(115200); //串口3初始化，用于gprs
	UART4_Init(9600);// 串口4初始化，用于GPS传输数据

	if(sdfs_app_mnt() != FR_OK)	printf("\r\nSD卡ERROR\r\n");    //SD卡初始化
	my_mem_init(SRAMIN);//初始化内部内存池
	ov2640_framebuf = mymalloc(SRAMIN,52*1024);//申请帧缓存
	pname=mymalloc(SRAMIN,30);		//为带路径的文件名分配30个字节的内存		    
	if(!pname||!ov2640_framebuf)	printf("\r\n摄像头内存分配失败!\r\n");

	for(initCount = 0;initCount < 5 && OV2640_Init();initCount++)  //摄像头初始化
	{
		delay_ms(200);
	}	
	SystemInit();
	KEY_Init();
	BEEP_Init();
	mpu6050_init();
	while(1)
	{
		if( GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 1)  goto loop;
		DMA_ClearFlag(DMA1_FLAG_TC6);
		delay_ms(100);
		if(DMA_GetFlagStatus(DMA1_FLAG_TC6) != SET) continue;
		if(!IS_MPU_RUNNING)
		{
			SystemInit();
			mpu6050_init();
			continue;
		}
		printf("b");
		if(IsAccident() == FALSE) continue;
		SendFromMPUtoBluetooth(MPU_data);
		printf("MPU_data:%s\r\n",MPU_data);
		BEEP_Open();
    isAccident = timecount = 0;  //调整定时器和车祸判断标记
		TIM4_Init(4999,7199);//500ms定时
		while(isAccident == 0) if(KEY_Scan() == 1) isAccident = 1;//等待回应     		
		BEEP_Close();
		TIM_Cmd(TIM4,DISABLE);//关闭定时器4
		if(isAccident == 1) continue;
		srand((unsigned)Angular[2]);
//		gpsData_send();   	
		for(initCount = 0;initCount < 5 && !gprs_init("123.207.124.49","6666");initCount++)//GPRS初始化
		{
				printf("GPRS网络状态错误\r\n");//尝试重新初始化
		}
		for(photoNum = 0; photoNum < PHOTO_NUM;photoNum++ )
		{
			sprintf((char *)pname, "%d.jpg",rand() % 10000);
			ov2640_jpg_savephoto(pname);  
			delay_ms(100);  
		}	  

		sim900a_gprs_send("HM+SMS\r\n3115005537.txt\r\n");
		sim900a_gprs_send("我的名字是梁盛兑，我在XX地点遭遇到了车祸，请及时救援.\r\n");
		for(photoNum = 0; photoNum < PHOTO_NUM;photoNum++ )
		{
			ov2640_jpg_sendphoto();  
			delay_ms(100);  
		}	
		sim900a_gprs_link_close();//关闭连接  
	}
}


void gpsData_send(void)
{
			while((UART4_RX_STA & 0x8000 ) == 0);       //等待GPS信息发送完毕
			GPSMSG_Analysis(gps,UART4_RX_BUF);           //分析GPS信息
			Send_NMEA_MSG(gps);                           //打印相关的GPS信息
			UART4_RX_STA = 0;	   										
}

/*********************************************END OF FILE**********************/
