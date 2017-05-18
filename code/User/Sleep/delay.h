#include "stm32f10x.h"
#define Delay_ms(x) Delay_us(100*x);
void Delay(vu16 cnt);
void SysTick_Init(void);
void Delay_us(__IO u32 nTime);
