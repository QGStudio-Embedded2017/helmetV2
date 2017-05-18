#include "bsp_usart3.h"
#include "sim900a.h"

void gprs_init(void)
{
		if(sdfs_app_mnt() == FR_OK) 
			printf("ok\r\n");//SDFS初始化
		else printf("ERROR\r\n");
		
		while(sim900a_cmd("AT\r","OK",1000) != 1)//检测模块响应是否正常
		{
			printf("模块响应测试不正常！重新连接中\r\n");
		}
		printf("GPRS模块响应正常\r\n");

		if (gprs_reg_status() == 0)
			sim900a_gprs_init();//初始化GPRS
		
		if (sim900a_gprs_status() == 0)//检测网络状态
		{
			if (1 == sim900a_gprs_tcp_link("123.207.124.49", "6666"))
			{
				printf("TCP建立连接成功！\r\n\r\n");
				if(sim900a_gprs_send("测试一下！123456") == 1)//发送数据
				{
					printf("发送成功！\r\n\r\n");
					sim900a_gprs_link_close();//关闭连接
					delay_ms(1000);
				}
				else 
					printf("发送失败！\r\n\r\n");

				SIM900A_CLEAN_RX();//清除串口3返回的数据
			}			
		}
}