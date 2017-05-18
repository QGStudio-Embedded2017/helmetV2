#include "stm32f10x.h"

#ifndef	_JUDGEH_
#define _JUDGEH_
#define KEY GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2)

void KEY_Init();

u8 KEY_Scan();

void TIM4_Init(u16 arr,u16 psc);



#endif