/**
  ******************************************************************************
  * @file    gpio.c
  * @author  Shengqiang.Zhang
  * @version V1.1
  * @date    2016-10-25
  * @brief   智能唤醒引脚初始化
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
#include "gpio.h"

/**
  * @brief  GPIO_Configuration()
  * @param  无
  * @retval 无
  */
 void GPIO_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;/*红外对管接收信号*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;//上拉输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;/*继电器*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;//开漏输出,这里用的继电器是5V触发的，所以确保I/O口能够容忍5V，否则会损坏硬件
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_SetBits(GPIOD,GPIO_Pin_12);//高电平，关闭继电器
}
