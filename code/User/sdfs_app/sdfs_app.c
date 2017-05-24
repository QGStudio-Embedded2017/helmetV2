/**
  ******************************************************************************
  * @file    sdfs_app.c
  * @author  Shengdui.Liang
  * @version V1.1
  * @date    2016-10-25
  * @brief   文件系统
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#include "sdfs_app.h"
#include "stm32f10x.h"
#include "sd.h"
#include "ff.h"

/* 用于文件系统。由于HT板堆栈空间较小，因此需要将myfat定义为全局的 */
FATFS myfat;

/* 默认的目录 */
const char *root = "";


#if SDFS_APP_DEBUG_ON
/**
 * @brief 	测试文件系统是否正常工作
 * @param  
 * @retval 	None
 */
void sdfs_app_test(void)
{
	/* 文件对象 */
    FIL fil;
 
	/* 挂载SD卡 */
	if(f_mount(&myfat, root, 0) == FR_OK)
	{
		printf("Mount SD successfully\r\n");
		if (f_open(&fil, "testfile.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE) == FR_OK)
		{
			printf("Open file successfully.\r\n");
			if (f_printf(&fil, "Hello,World!\n") > 0)
				printf("Puts file successfully.\r\n");
			else
				printf("Puts file failed.\r\n");
		}
		else
			printf("Open file failed.\r\n");
		f_close(&fil);
	}
	else
		SDFS_APP_DEBUG("Mount failed\r\n");
}
#endif


/**
 * @brief 	挂载SD卡
 * @param	
 * @retval 	1 操作成功
			0 操作失败
 */
int sdfs_app_mnt(void)
{
	/* 注册一个工作区，逻辑号为1 */
	printf("sd start!!!!\n");
	if(f_mount(&myfat, root, 1) != FR_OK)
	{
		SDFS_APP_DEBUG("Mount SD failed.\r\n");
		return 1;
	}
	return 0;
}

