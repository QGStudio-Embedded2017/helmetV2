#include "stm32f10x.h"
#include "debug.h"

void key_bee_init()
{
	//key
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU; //…œ¿≠ ‰»Î
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//bee
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
