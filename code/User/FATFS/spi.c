#include"spi.h"
#include"usart.h"
#include"delay.h"
void SPI1_Init(void)
{
	SPI_InitTypeDef	SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;                   
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //PA4 推挽 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	GPIO_SetBits(GPIOA,GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//设置双向全双工SPI数据模式
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//设置为主SPI
	SPI_InitStructure.SPI_CPHA  = SPI_CPHA_2Edge;//数据捕获于第二个时钟沿
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//串行时钟的稳态，时钟悬空高
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//发送接收8位帧数据
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//内部NSS信号由SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;//CRC值计算的多项式
	SPI_Init(SPI1,&SPI_InitStructure);
	
	SPI_Cmd(SPI1,ENABLE);//使能SPI1外设
}

u8 SPI1_ReadWriteByte(u8 txData)
{
		while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET) 
			;//发送缓存空标志位
		SPI_I2S_SendData(SPI1,txData);//通过SPI发送一个数据
		while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) == RESET)
			  ;//接收缓存非空标志位
	  return SPI_I2S_ReceiveData(SPI1);
}

//SPI 速度设置函数
//SpeedSet:
//SPI_BaudRatePrescaler_2   2分频   
//SPI_BaudRatePrescaler_8   8分频   
//SPI_BaudRatePrescaler_16  16分频  
//SPI_BaudRatePrescaler_256 256分频 
  
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI1->CR1&=0XFFC7;
	SPI1->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度 
	SPI_Cmd(SPI1,ENABLE); 
} 
