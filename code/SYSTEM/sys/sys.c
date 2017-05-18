#include "sys.h"

/**
  * @brief  NVIC_Configuration()
  * @param  нч
  * @retval нч
  */
void NVIC_Configuration(void)
{   
	NVIC_InitTypeDef NVIC_InitStructure;

	#ifdef  VECT_TAB_RAM  
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);//Set the Vector Table base location at 0x20000000  
	#else  /* VECT_TAB_FLASH  */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);//Set the Vector Table base location at 0x08000000 
	#endif

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	NVIC_InitStructure.NVIC_IRQChannel= RTCAlarm_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority= 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd= ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

/*	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =2; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);*/
}
