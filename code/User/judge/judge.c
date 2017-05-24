
#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "judge.h"

extern u8 isAccident;
u8 timecount = 0;
	
//按键函数初始化，用于按键的检测
void KEY_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
}

//按键扫描函数
u8 KEY_Scan(void)
{
	static u8 key_up = 1;
	if(key_up && KEY == 0)
	{
		delay_ms(10);
		if(KEY == 0) return 1;
	}
	else if(KEY == 1) key_up = 1;
	return 0;//无按键按下
		
}

//定时器4中断，用于判断误判时间
void TIM4_Init(u16 arr,u16 psc)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //TIM4时钟使能    
	/*定时器TIM3初始化*/
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM4中断,允许更新中断
	/*配置TIM3中断*/ 	  
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器		
	
	TIM_Cmd(TIM4, ENABLE);  //使能TIMx
}


	
//定时器4中断函数
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update) == RESET) return ;
	printf("count == %d\r\n",timecount);
	if(++timecount == 10)
	{
		isAccident = 2;
		TIM_Cmd(TIM4, DISABLE);  //失能TIMx
		timecount = 0;
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
}