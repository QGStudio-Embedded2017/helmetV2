/***************************************************************************************
*        File Name              :	spi.h
*        CopyRight              :	Zoisite
*        ModuleName             :	sccb
*
*        CPU                    :	Intel i5-4210u
*        RTOS                   :	Windows 10 pro
*
*        Create Data            :	
*        Author/Corportation    : Shengdui/Zoisite
*
*        Abstract Description   :	提供SPI接口
*
*--------------------------------Revision History--------------------------------------
*       No      version     Data        Revised By      Item        Description
*       1       v1.0        2016/10/18   Shengdui                   Create this file
*
***************************************************************************************/
/**************************************************************
*        Multi-Include-Prevent Section
**************************************************************/
#ifndef _SPI_H
#define _SPI_H

/**************************************************************
*        Include File Section
**************************************************************/
#include"stm32f10x.h"

/**************************************************************
*        Macro Define Section
**************************************************************/

/**************************************************************
*        Prototype Declare Section
**************************************************************/

/**
 * @brief  初始化SPI0
 * @param  
 * @retval None
 */
void SPI1_Init(void);

/**
 * @brief  通过SPI发送，读取一个字节
 * @param  txData 要发送的字节
 * @retval 读取的字节
 */
u8 SPI1_ReadWriteByte(u8 txData);

/**
 * @brief  设置SPI1的时钟分频
 * @param  SpeedSet 分频值
				SPI_BaudRatePrescaler——2   2   	(SPI 36M@sys 72M)
				SPI_BaudRatePrescaler_8   8  	(SPI 9M@sys 72M)
				SPI_BaudRatePrescaler_16  16 	(SPI 4.5M@sys 72M)
				SPI_BaudRatePrescaler_256 256 	(SPI 281.25K@sys 72M)
 * @retval None
 */
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler);

#endif
