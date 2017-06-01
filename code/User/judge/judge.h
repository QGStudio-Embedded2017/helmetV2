#include "stm32f10x.h"

#ifndef	_JUDGEH_
#define _JUDGEH_
#define KEY GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_4)

void KEY_Init(void);

u8 KEY_Scan(void);

void TIM4_Init(u16 arr,u16 psc);



#endif