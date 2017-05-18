/**
  ******************************************************************************
  * @file    Bluetooth.c
  * @author  Xiaorong.Zhang  Jinji.Fang
  * @version V1.1
  * @date    2016-10-25
  * @brief   蓝牙通信协议
  ******************************************************************************
  * @attention  
  *
  ******************************************************************************
  */
	
#include "Bluetooth.h"
#include "ov7725.h"

float Axis[3];
static char num[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};//数字以字符显示

/**
 * @brief  车祸信号处理函数
 * @param  
 * @retval 1表示车祸发生，0表示误触发
 */
int CrashFunction(void)
{
	u8 recSignal[3] = {0};
	u8 ch;
	u8 flag;	//判断车祸发生时，移动端是否完成任务
	while(1)
	{
		printf("aaa\n");//发送车祸发生信号

		/*接受移动端发来的‘ok’信号*/
		while('o' != (ch = getchar()));
		
		printf("%c",ch);
		recSignal[0] = ch;
		recSignal[1] = getchar();
		
		printf("%c",recSignal[1]);
	//	BLUETOOTH_DEBUG("%c,%c\n", recSignal[0], recSignal[1]);   
		
		if(recSignal[1] == 'k')
		{
			//GPIO_WriteOutBits(HT_GPIOC, GPIO_PIN_9, RESET);//关闭蜂鸣器
			//如果接收到ac，则代表车祸真的发生；接收到wt则代表是错误触发车祸
			recSignal[0] = getchar();
			printf("%c",recSignal[0]);
			recSignal[1] = getchar();
			printf("%c",recSignal[1]);
			recSignal[2] = '\0';
			
			if(recSignal[0] == 'a' && recSignal[1] == 'c')
			{
				//移动端完成任务
				flag = getchar();
				printf("%c",flag);
				if(flag != '0')//确认车祸时，app发送字符'0'
					continue;
				printf("success!\r\n");
			//	BLUETOOTH_DEBUG("ac success!\n");
				//GPIO_WriteOutBits(HT_GPIOC, GPIO_PIN_9, RESET);//关闭蜂鸣器
				return 1;
			}
			else if(recSignal[0] == 'w' && recSignal[1] == 't')
			{
				//GPIO_WriteOutBits(HT_GPIOC, GPIO_PIN_9, RESET);//关闭蜂鸣器
				BLUETOOTH_DEBUG("wt success! %s\n", recSignal);
				return 0;
			}
			else{
				BLUETOOTH_DEBUG("ac wt error! %s\n", recSignal);
				continue;//移动端出故障
			}
		}
		else{
			BLUETOOTH_DEBUG("ok error %s\n", recSignal);
		}
	}
}
/**
 * @brief  		打包加速度数据以发送给手机端显示
 * @param  		char *data:存储待发送的数据
 * @retval 		void
 */
void SendFromMPUtoBluetooth(char *data)
{
	int i, j;
	
	for(i = 0; i < 3; ++i)
		*data++ = 'b';//前三个字符为'b',且data依次向前移动一位
	
	for(i = 0; i < 3; ++i)//xyz三次循环
	{
		if(Axis[i] < 0)
		{
			Axis[i] = -Axis[i];
			*(data + i * 6) = num[1];//负数符号位为1
		}
		else
			*(data + i * 6) = num[0];
		
		Axis[i] *= 1000;
		for(j = 5; j >= 1; --j)
		{
			*(data + i * 6 + j) = num[(int)Axis[i] % 10];
			Axis[i] /= 10.0;
		}
	}
}


static u8 BT_SendByte(u8 b) 		
{						
	USART_SendData(USART2, b);	
	while (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 
	{ }
	return b;
}	



