/*#include "answer.h"
char usart2_buff[20]={0};
int count=0;
u8 velocity=0;




void answer(void)
{
	if(usart2_buff[0]=='h'&&usart2_buff[1]=='p')//有电话接入指令
	{
		if(velocity>=15.6)//判断加速度是否超速
		{
			printf("cnr\n");//返回禁止接听指令
		}
		else
		{
			printf("cr\n");//返回允许接听指令
		}
		count=0;
	}

}
*/

