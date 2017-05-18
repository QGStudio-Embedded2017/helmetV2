/**
  ******************************************************************************
  * @file    exit.c
  * @author  Shengqiang.Zhang
  * @version V1.1
  * @date    2016-10-25
  * @brief   智能唤醒的中断部分
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
#include "exti.h"


/**
  * @brief  Sys_Sleepy()
  * @param  无
  * @retval 无
  */
void GoToSleep(void)
{
	GPIO_SetBits(GPIOD,GPIO_Pin_12);//关闭继电器
/*	
	SCB->SCR |= 0X00;
	

	#if defined ( __CC_ARM   )                          
	__force_stores();
	#endif
	__WFI();//等待中断
*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能PWR时钟
	PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);//进入停止模式
}

/**
  * @brief  RCC_Configuration()
  * @param  无
  * @retval 无
  */
void RCC_Configuration(void)
{
	//将RCC寄存器重新设置为默认值
	RCC_DeInit(); 
	//打开外部高速时钟晶振HSE  
	RCC_HSEConfig(RCC_HSE_ON);
	//等待外部高速时钟晶振工作 
	while( ERROR == RCC_WaitForHSEStartUp());
	//设置AHB时钟AHB clock = SYSCLK 
	RCC_HCLKConfig(RCC_SYSCLK_Div1); 
	//设置高速AHB时钟 APB2 clock = HCLK 
	RCC_PCLK2Config(RCC_HCLK_Div1);
	//设置低速AHB时钟 APB1 clock = HCLK/2 
	RCC_PCLK1Config(RCC_HCLK_Div2);
	// 设置PLL 时钟源为HSE 倍频系数为9  
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9); 
	// 打开PLL 
	RCC_PLLCmd(ENABLE);
	// 等待PLL工作
	while(RESET ==  RCC_GetFlagStatus(RCC_FLAG_PLLRDY) );
	// 设置系统时钟   RCC_SYSCLKSource_XX    可选( PLLCLK  HSI  HSE )  
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); 
	// 判断是否PLL是系统时钟 
	while(RCC_GetSYSCLKSource() != 0x08);

}

/**
  * @brief  to recovery the symtem clock
  * @param  无
  * @retval 无
  */
void SYSCLKConfig(void)
{
	ErrorStatus HSEStartUpStatus;
  /* 使能 HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* 等待 HSE 准备就绪 */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {

    /* 使能 PLL */ 
    RCC_PLLCmd(ENABLE);

    /* 等待 PLL 准备就绪 */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* 选择PLL作为系统时钟源 */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* 等待PLL被选择为系统时钟源 */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
}

/**
  * @brief  RTC_Configuration()
  * @param  无
  * @retval 无
  */
void RTC_Configuration (void)
{
	//使能PWR电源管理时钟和BKP后备寄存器
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	//配置RTC时钟源,允许访问BKP
	PWR_BackupAccessCmd(ENABLE); 
	//复位备份域
	BKP_DeInit();
	//允许LSE
	RCC_LSEConfig(RCC_LSE_ON); 
	//等待LSE有效
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET) ;
	//选择LSE做为RTC时钟
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); 
	//允许RTC时钟
	RCC_RTCCLKCmd(ENABLE);
	//配置RTC,等待RTC APB同步
	RTC_WaitForSynchro();
	//预分频值为1s
	RTC_SetPrescaler(32767);
	//等待最后一条写指令完成
	RTC_WaitForLastTask();
	//允许RTC报警中断
	RTC_ITConfig(RTC_IT_ALR, ENABLE);
	//等待最后一条写指令完成
	RTC_WaitForLastTask(); 
}

/**
  * @brief  EXTI_Configuration()
  * @param  无
  * @retval 无
  */
void EXTI_Configuration(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	//使用外部中断方式
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);
	EXTI_InitStructure.EXTI_Line = EXTI_Line13;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
	EXTI_InitStructure.EXTI_LineCmd =ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}


/**
  * @brief  This function handles EXTI3 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void) /*中断唤醒*/
{
	if(EXTI_GetITStatus(EXTI_Line13) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line13);
	}
}
