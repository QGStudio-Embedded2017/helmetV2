/***************************************************************************************
*        File Name              :	sdfs_app.h
*        CopyRight              :	Zoisite
*        ModuleName             :	sdfs
*
*        CPU                    :	Intel i5-4210u
*        RTOS                   :	Windows 7 pro
*
*        Create Data            :	
*        Author/Corportation    :   Shengdui
*
*        Abstract Description   :	基于SD底层IO驱动构建的文件系统FATFS的应用。如打开文件、读写文件等。
*
*--------------------------------Revision History--------------------------------------
*       No      version     Data        	Revised By      Item        Description
*       1       v1.0       	2016/10/19     Shengdui                    Create this file
***************************************************************************************/


/**************************************************************
*        Multi-Include-Prevent Section
**************************************************************/
#ifndef _SDFS_APP_H
#define _SDFS_APP_H

/**************************************************************
*        Debug switch Section
**************************************************************/

#define SDFS_APP_DEBUG_ON 1

/**************************************************************
*        Include File Section
**************************************************************/



/**************************************************************
*        Macro Define Section
**************************************************************/

/**
 * @brief	调试信息输出宏
 */
#if SDFS_APP_DEBUG_ON
	#define SDFS_APP_DEBUG(fmt,args...) printf (fmt ,##args)
#else
	#define SDFS_APP_DEBUG(fmt,args...)
#endif


#define GETR_FROM_RGB16(RGB565)  ((unsigned char)(( ((unsigned short int )RGB565) >>11)<<3))		  	//返回8位 R
#define GETG_FROM_RGB16(RGB565)  ((unsigned char)(( ((unsigned short int )(RGB565 & 0x7ff)) >>5)<<2)) 	//返回8位 G
#define GETB_FROM_RGB16(RGB565)  ((unsigned char)(( ((unsigned short int )(RGB565 & 0x1f))<<3)))       	//返回8位 B


/**************************************************************
*        Struct Define Section
**************************************************************/


/**************************************************************
*        Prototype Declare Section
**************************************************************/

/**
 * @brief 	测试文件系统是否正常工作
 * @param  
 * @retval 	None
 */
void sdfs_app_test(void);


/**
 * @brief 	从OV7725获取一帧图像并以bmp格式写入SD卡
 * @param	filename 文件名
			width 图像帧的宽度
			height 图形帧的高度
 * @retval 	1 操作成功
			0 操作失败
 */
int sdfs_app_writeBmp(char *filename, int width, int height);


/**
 * @brief 	挂载SD卡
 * @param	
 * @retval 	1 操作成功
			0 操作失败
 */
int sdfs_app_mnt(void);


/**
 *  @name	sdfs_app_savePhoto
 *  @brief	带超时计数地初始化OV7725
 *  @param  None        
 *  @return None
 *  @notice
 */
void sdfs_app_savePhoto(void);

/**************************************************************
*        End-Multi-Include-Prevent Section
**************************************************************/
#endif
