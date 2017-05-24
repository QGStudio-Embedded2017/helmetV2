#include "MPU-6050.h"

/**************************************************************
*        Global Value Define Section
**************************************************************/
uint8_t MPU_Data [33] = {0};

float Angle[3];
float Angular[3];
float FinalBaseAxis[3];

/**
  * @brief  USART2 GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
void USART2_Config(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		
		/* config USART2 clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
		
		/* USART2 GPIO config */
		/* Configure USART2 Tx (PA.02) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);    
		/* Configure USART2 Rx (PA.3) as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
			
		/* USART2 mode config */
		USART_InitStructure.USART_BaudRate = 115200 ;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No ;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART2, &USART_InitStructure);
		USART_Cmd(USART2, ENABLE);
}

/**
  * @brief  USART2 RX DMA 配置，外设到内存(USART2->DR)
  * @param  无
  * @retval 无
  */
void USART2_DMA_Config(void)
{
		DMA_InitTypeDef DMA_InitStructure;
	
		/*开启DMA时钟*/
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	
	
		/*设置DMA源：串口数据寄存器地址*/
		DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR;	   

		/*内存地址(要接受数据的变量的指针)*/
		DMA_InitStructure.DMA_MemoryBaseAddr =  (u32)&MPU_Data;

		/*方向：从外设读*/		
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	

		/*传输大小*/	
		DMA_InitStructure.DMA_BufferSize = 33;

		/*外设地址不增*/	    
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 

		/*内存地址自增*/
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	

		/*外设数据单位 8bit*/	
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

		/*内存数据单位 8bit*/
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 

		/*DMA模式：循环模式*/
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	 

		/*优先级：高*/	
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;  

		/*禁止内存到内存的传输	*/
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

		/*配置DMA1的6通道*/		   
		DMA_Init(DMA1_Channel6, &DMA_InitStructure); 	   
		
		/*使能DMA*/
		DMA_Cmd (DMA1_Channel6,ENABLE);					
}

//初始化mpu6050
void mpu6050_init(void)
{
	USART2_Config();  //配置串口2
	USART2_DMA_Config();   //配置串口2的DMA传输
	DMA_ClearFlag(DMA1_FLAG_TC6);
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);  //使能串口2的DMA传输
}
	

/**
 * @brief  得出三个方向轴的加速度
 * @param  
 * @retval None
 */
void Axis_DataTransfrom(void)
{
	int i, j;
	for(i = 0 ; i < 3; i++)
	{			
		if(MPU_Data[i * 11 + 1] == 0x51)
		{
			for(j = 0; j < 3; ++j)
				Axis[j] = ((short)(MPU_Data[3 + 2 * j + i * 11] << 8 | MPU_Data[2 + j * 2 + i * 11])) / 32768.0 * 16;
				
			return;
				
//			Axis[0] = ((short)(MPU_Data[3 + i * 11] << 8 | MPU_Data[2 + i * 11])) / 32768.0 * 16;		//X轴加速度	 
//			Axis[1] = ((short)(MPU_Data[5 + i * 11] << 8 | MPU_Data[4 + i * 11])) / 32768.0 * 16;    	//Y轴加速度
//			Axis[2] = ((short)(MPU_Data[7 + i * 11] << 8 | MPU_Data[6 + i * 11])) / 32768.0 * 16;    	//Z轴加速度		
		}
	}
}

/**
 * @brief  得出三个方向轴的角速度
 * @param  
 * @retval None
 */
void Angular_DataTransFrom(void)
{
	int i, j;
	for(i = 0 ; i < 3; i++)
	{			
		if(MPU_Data[i * 11 + 1] == 0x52)
		{
			for(j = 0; j < 3; ++j)
				Angular[j] = ((short)(MPU_Data[3 + 2 * j + i * 11] << 8 | MPU_Data[2 + j * 2 + i * 11])) / 32768.0 * 2000;
					
		return;
					
//			Angular[0] = ((short)(MPU_Data[3 + i * 11] << 8 | MPU_Data[2 + i * 11])) / 32768.0 * 2000;		//X轴角速度	 
//			Angular[1] = ((short)(MPU_Data[5 + i * 11] << 8 | MPU_Data[4 + i * 11])) / 32768.0 * 2000;		//Y轴角速度
//			Angular[2] = ((short)(MPU_Data[7 + i * 11] << 8 | MPU_Data[6 + i * 11])) / 32768.0 * 2000;		//Z轴角速度		
		}
	}
}

/**
 * @brief  获得三个方向轴的角度
 * @param  
 * @retval None
 */
void Angle_DataTransfrom(void)
{
	int i, j;
	for(i = 0; i < 3; i++)
	{
		if(MPU_Data[i * 11 + 1] == 0x53)
		{
			for(j = 0; j < 3; ++j)
				Angle[j] = ((short)(MPU_Data[3 + 2 * j + i * 11] << 8 | MPU_Data[2 + j * 2 + i * 11])) / 32768.0 * 180;
					
			return;
//			Angle[0] = ((short)(MPU_Data[3 + i * 11] << 8) | MPU_Data[2 + i * 11]) / 32768.0 * 180;	//X轴角度
//			Angle[1] = ((short)(MPU_Data[5 + i * 11] << 8) | MPU_Data[4 + i * 11]) / 32768.0 * 180;	//Y轴角度
//			Angle[2] = ((short)(MPU_Data[7 + i * 11] << 8) | MPU_Data[6 + i * 11]) / 32768.0 * 180;	//Z轴角度
		}
	}
}

/**
 * @brief  处理获得的加速度，去除基准值
 * @param  
 * @retval None
 */
void Axis_GetFinalData(void)
{
	/*分别获得角速度和加速度基准值*/
	Axis_DataTransfrom();
	Angle_DataTransfrom();
	Angular_DataTransFrom();
	
	/*处理加速度的基准值*/
	DealWithRotation();
}

/**
 * @brief  		通过检测加速度的剧烈变化来判断是否发生车祸
 * @param  		void
 * @retval 		TRUE：表示发生车祸了；FALSE：表示并没有发生车祸
 */
bool IsAccident(void)
{
	double a;
	Axis_GetFinalData();	//收集数据
	if(fabs(Axis[0]) > 1.0 || fabs(Axis[1]) > 1.0 || fabs(Axis[2]) > 1.0)
	{
		if((a = (Square(Axis[0]) + Square(Axis[1]) + Square(Axis[2]))) > 2)
		{
				printf("a = %f", a);	
				return TRUE;
		}
	}
	else if(fabs(Angular[2]) > 800.0)
	{
		return TRUE;
	}
	
	return FALSE;
}

/**
 * @brief  		按照X、Y或Z轴旋转后得到旋转矩阵
 * @param  		matrix：旋转矩阵，存储旋转后的数据
 * 				angleIndegree：角度，以熟知的度数方式表示，需要转换为π / n
 *				axis：	表示绕着哪个轴转
 */
#define MATRIX( row, col ) matrix[ row * 3 + col ]
static void Rotate(float matrix[], float angleInDegree, float x, float y, float z)
{
	float alpha = angleInDegree / 180.0 * PI;	//将角度转换为π / n的格式
	float Sin = sin(alpha);						
	float Cos = cos(alpha);						
	float translateValue = 1.0 - Cos;			//矩阵系数值

	MATRIX(0, 0) = translateValue * x * x + Cos;
	MATRIX(0, 1) = translateValue * x * y + Sin * z;
	MATRIX(0, 2) = translateValue * x * z - Sin * y;
	MATRIX(1, 0) = translateValue * x * y - Sin * z;
	MATRIX(1, 1) = translateValue * y * y + Cos;
	MATRIX(1, 2) = translateValue * y * z + Sin * x;
	MATRIX(2, 0) = translateValue * x * z + Sin * y;
	MATRIX(2, 1) = translateValue * y * z - Sin * x;
	MATRIX(2, 2) = translateValue * z * z + Cos;
}

/**
 * @brief  		按照矩阵乘法进行矩阵右乘获得各个轴旋转后的位置
 * @param  		matrix：旋转矩阵，存储旋转后的数据
 *				coord：坐标矩阵，初始时的位置坐标
 */
static void GetAcceleration(float matrix[9], float originCoord[3], float finalCoord[3])
{
	int i, j;
	for(i = 0; i < 3; ++i)
	{
		finalCoord[i] = 0.0;
		for(j = 0; j < 3; ++j)//矩阵右乘
		{
			finalCoord[i] += originCoord[j] * MATRIX(j, i);
		}
	}
}

/**
 * @brief  		处理旋转后加速度的消除平衡问题
 * @param  		void
 */
void DealWithRotation(void)
{
	float matrix[9] = {0.0};
	float originCoord[3];
	int i, j;
	
	for(i = 0; i < 3; ++i)
	{
		for(j = 0; j < 3; ++j)
			originCoord[j] = 0.0;
		
		originCoord[i] = 1.0;
		
		Rotate(matrix, Angle[0], 1.0, 0.0, 0.0);
		GetAcceleration(matrix, originCoord, FinalBaseAxis);	//依次将获得的矩阵中的值进行右乘
		Rotate(matrix, Angle[1], 0.0, 1.0, 0.0);
		GetAcceleration(matrix, FinalBaseAxis, originCoord);	//交换的原因是方便存储和修改，因为每一次可以将获得的作为下一次的参数
		Rotate(matrix, Angle[2], 0.0, 0.0, 1.0);
		GetAcceleration(matrix, originCoord, FinalBaseAxis);
		
		Axis[i] -= FinalBaseAxis[2];	//减去Z轴的(其为其重力方向的加速度)
	}
}
