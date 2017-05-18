/***************************************************************************************
*        File Name              :	sd.h
*        CopyRight              :	Zoisite
*        ModuleName             :	sd
*
*        CPU                    :	Intel i5-4210u
*        RTOS                   :	Windows 10 pro
*
*        Create Data            :	
*        Author/Corportation    :   Shengdui/Zoisite
*
*        Abstract Description   :	offer API of SD card
*
*--------------------------------Revision History--------------------------------------
*      	No      version     Date        	Revised By      Item        Description
*       1       v1.0        2016/10/18      Shengdui                    Create this file
***************************************************************************************/
/**************************************************************
*        Multi-Include-Prevent Section
**************************************************************/
#ifndef _SD_H
#define _SD_H

/**************************************************************
*        Debug switch Section
**************************************************************/

/**
  * @brief 	SD调试开关
  */
#define SD_DEBUG_ON 0

/**************************************************************
*        Include File Section
**************************************************************/
#include "spi.h"
#include "delay.h"
#include "stm32f10x.h"


/**************************************************************
*        Macro Define Section
**************************************************************/

/**
  * @brief 	默认扇区大小512
  */
#define SECTOR_SIZE 512



/**
  * @brief 	SD卡类型定义  
  */
#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06


/**
  * @brief 	SD卡指令表 
  */
#define CMD0    0       //卡复位
#define CMD1		1
#define CMD8    8       //命令8 ，SEND_IF_COND
#define CMD9    9       //命令9 ，读CSD数据
#define CMD10   10      //命令10，读CID数据
#define CMD12   12      //命令12，停止数据传输
#define CMD16   16      //命令16，设置SectorSize 应返回0x00
#define CMD17   17      //命令17，读sector
#define CMD18   18      //命令18，读Multi sector
#define CMD23   23      //命令23，设置多sector写入前预先擦除N个block
#define CMD24   24      //命令24，写sector
#define CMD25   25      //命令25，写Multi sector
#define CMD41   41      //命令41，应返回0x00
#define CMD55   55      //命令55，应返回0x01
#define CMD58   58      //命令58，读OCR信息
#define CMD59   59      //命令59，使能/禁止CRC，应返回0x00


/**
  * @brief	数据写入回应字意义
  */
#define SD_DATA_OK                0x05
#define SD_DATA_CRC_ERROR         0x0B
#define SD_DATA_WRITE_ERROR       0x0D
#define SD_DATA_OTHER_ERROR       0xFF


/**
  * @brief	SD卡回应标记字
  */
#define SD_RESPONSE_NO_ERROR      0x00
#define SD_IN_IDLE_STATE          0x01
#define SD_ERASE_RESET            0x02
#define SD_ILLEGAL_COMMAND        0x04
#define SD_COM_CRC_ERROR          0x08
#define SD_ERASE_SEQUENCE_ERROR   0x10
#define SD_ADDRESS_ERROR          0x20
#define SD_PARAMETER_ERROR        0x40
#define SD_RESPONSE_FAILURE       0xFF


/**
  * @brief  Dummy byte
  */
#define SD_DUMMY_BYTE 0xFF


/**
  * @brief	数据起始令牌定义
  */
#define SD_BEGIN_END_FLAG 0xFE


/**
  * @brief	通过SPI读写一个字节
  */
#define SD_SPI_ReadWriteByte(dat) SPI1_ReadWriteByte(dat)


/**
 * @brief	调试信息输出宏定义
 */
#if SD_DEBUG_ON
	#define SD_DEBUG(fmt,args...) printf (fmt ,##args)
#else
	#define SD_DEBUG(fmt,args...)
#endif
   
/**************************************************************
*        Struct Define Section
**************************************************************/


/**************************************************************
*        Prototype Declare Section
**************************************************************/
/**
 * @brief  	初始化SD，使其进入工作模式
 * @param  
 * @retval 	0 卡错误
 */
u8 SD_Init(void);


/**
 *  @name SD_ReadDisk
 *	@description 读SD卡扇区
 *	@param 	buf 数据缓冲区
			sector 扇区号
			cnt 要读取的扇区数目
 *	@return 操作结果，1 成功；其他，失败
 *  @notice
 */
u8 SD_ReadDisk(u8 *buf, u32 sector, u8 cnt);


/**
 *  @name SD_WriteDisk
 *	@description 写SD卡扇区
 *	@param 	buf 数据缓冲区
			sector 扇区号
			cnt 要写入的扇区数目
 *	@return 操作结果，1 成功；其他，失败
 *  @notice
 */
u8 SD_WriteDisk(const u8 *buf, u32 sector, u8 cnt);


/**
 *  @name SD_RecvData
 *	@description 从sd卡读取一定长度的内容，保存到缓冲区buf中
 *	@param 	buf 数据缓存区
			len 要读取的数据长度
 *	@return 1 读取成功
			0 读取失败	
 *  @notice
 */
u8 SD_RecvData(u8 *buf,u16 len);


/**
 * @brief 	带超时退出的SD卡初始化函数
 * @param 	
 * @return 	1 初始化成功
			0 初始化失败
 */
u8 SD_TryInit(void);


/**
 * @brief 	查看某个扇区的内容
 * @param 	sector 扇区号
 * @return 	None
 */
void SD_ViewSector(u8 sector);


/**************************************************************
*        End-Multi-Include-Prevent Section
**************************************************************/
#endif
