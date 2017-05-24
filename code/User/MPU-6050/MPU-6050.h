/***************************************************************************************
*        File Name              :	MPU6050.h
*        CopyRight              :	Jinji.Fang
*        ModuleName             :	MPU6050
*
*        CPU                    :	Intel i5-4210u
*        RTOS                   :	Windows 7 pro
*
*        Create Data            :	
*        Author/Corportation    :   Gaunthan/Zoisite/Jinji.Fang
*
*        Abstract Description   :	offer API of MPU6050 device
*
*--------------------------------Revision History--------------------------------------
*       No      version     Data        Revised By      Item        Description
*       1       v1.0        2016/       Subai                    Create this file
*				2				v1.1				2016-10-16	Jinji.Fang							 rewrite the file
***************************************************************************************/
/**************************************************************
*        Multi-Include-Prevent Section
**************************************************************/
#ifndef _MPU6050_H
#define _MPU6050_H

/**************************************************************
*        Debug switch Section
**************************************************************/


/**************************************************************
*        Include File Section
**************************************************************/
#include "stm32f10x.h"
#include "math.h"
#include <stdio.h>
/**************************************************************
*        Macro Define Section
**************************************************************/
#define PI 3.1415926
#define Square(x) ((x) * (x))
#define IS_MPU_RUNNING (MPU_Data[0] == 0x55)

/**************************************************************
*        Struct Define Section
**************************************************************/
extern uint8_t MPU_Data [33];

extern float Axis[3];
extern float Angle[3];
extern float Angular[3];
extern float FinalAxis[3];

/**************************************************************
*        Prototype Declare Section
**************************************************************/

/**
 * @brief  获得三个方向轴的角度
 * @param  
 * @retval None
 */
void Angle_DataTransfrom(void);

/**
 * @brief  得出三个方向轴的角速度
 * @param  
 * @retval None
 */
void Angular_DataTransFrom(void);

/**
 * @brief  得出三个方向轴的加速度
 * @param  
 * @retval None
 */
void Axis_DataTransfrom(void);

/**
 * @brief 处理获得的加速度，去除基准值 
 * @param  
 * @retval None
 */
void Axis_GetFinalData(void);

/**
 * @brief  USART2的初始化
 * @param  
 * @retval None
 */
void USART2_Config(void);

/**
 * @brief  DMA的初始化
 * @param  
 * @retval None
 */
void USART2_DMA_Config(void);

//mpu6050的初始化
void mpu6050_init(void);
/**
 * @brief  		处理旋转后加速度的消除平衡问题
 * @param  		void
 */
void DealWithRotation(void);

/**
 * @brief  		通过检测加速度的剧烈变化来判断是否发生车祸
 * @param  		void
 * @retval 		TRUE:表示发生车祸;FALSE:表示没有发生车祸
 */
bool IsAccident(void);



extern int  fputc(int ch, FILE *f);
extern int  fgetc(FILE *f);
extern void bsp_USART1_IRQHandler(void);
extern char *get_usart1_Rxbuf(uint8_t *len);
extern void clean_usart1_Rxbuf(void);         
/**************************************************************
*        End-Multi-Include-Prevent Section
**************************************************************/
#endif


