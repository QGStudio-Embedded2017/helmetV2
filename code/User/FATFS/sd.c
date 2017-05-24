#include "sd.h"
#include "usart.h"

u8 SD_Type = SD_TYPE_ERR;			/* 存放SD卡的类型 */
u8 buf[SECTOR_SIZE];	/* SD数据缓冲区 */

/**
 * @brief  将SPI设置为低速模式
 * @param  
 * @retval None
 */
static void SD_SPI_SetLowSpeed(void)
{
 	SPI1_SetSpeed(SPI_BaudRatePrescaler_256);	//设置到低速模式	
}


/**
 * @brief  将SPI设置为高速模式
 * @param  
 * @retval None
 */
static void SD_SPI_SetHighSpeed(void)
{
 	SPI1_SetSpeed(SPI_BaudRatePrescaler_8);	//设置到高速模式	
}


/**
 * @brief		等待SD卡准备好
 * @param  
 * @retval 	1 已准备
					  0 等待超时
 */
static u8 SD_WaitReady(void)
{
	u32 t = 0;
	while(++t < 0XFFFFFF)	//等待 
		if(SD_SPI_ReadWriteByte(SD_DUMMY_BYTE) == SD_DUMMY_BYTE)	return 1;//总线空闲		  	
	return 0;
}


/**
 * @brief	取消片选中SD卡
 * @param  
 * @retval 	None
 */
void SD_DisSelect(void)
{
	/* 取消片选设备 */
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	//提供额外的8个时钟
 	SD_SPI_ReadWriteByte(SD_DUMMY_BYTE);	
}


/**
 * @brief	片选中SD卡，使其工作
 * @param  
 * @retval 	1 选中成功
			0 选中失败
 */
u8 SD_Select(void)
{	
	/* 片选设备 */
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	if(SD_WaitReady() == 1) return 1;
	SD_DisSelect();
	return 0;
}

	/**
 * @brief  	向SD卡发送一个命令
 * @param	  cmd 命令 
			      arg 命令参数
			      crc crc校验值
 * @retval 	SD卡返回的响应
 */
u8 SD_SendCmd(u8 cmd, u32 arg, u8 crc)
{
	u8 r1;	
	u8 retry = 0; 
	SD_DisSelect();		//取消上次片选
	SD_SPI_ReadWriteByte(SD_DUMMY_BYTE);
	if(!SD_Select())	//重新片选
		return SD_DUMMY_BYTE;	//片选失败
	//打包命令、参数以及校验和
	SD_SPI_ReadWriteByte(cmd | 0x40);	//8位命令，最高位恒为1
  SD_SPI_ReadWriteByte(arg >> 24);	//将32位的参数拆分成4字节，逐一发送
  SD_SPI_ReadWriteByte(arg >> 16);
  SD_SPI_ReadWriteByte(arg >> 8);
  SD_SPI_ReadWriteByte(arg);	  
  SD_SPI_ReadWriteByte(crc); 			//8位校验和
	
	if(cmd == CMD12)	//对于停止命令，加发空闲数据
		SD_SPI_ReadWriteByte(SD_DUMMY_BYTE);	
	
    //等待响应，或超时退出
	retry = 0X1F;
	do
	{
		r1 = SD_SPI_ReadWriteByte(0xFF);
	}while((r1 & 0X80) && retry--);	 //r1最高位若为1则不是响应，继续等待
	return r1;
}


/**
 *  @name SD_GetResponse
 *	@description 等待SD卡回应
 *	@param response 要获得的回应
 *	@return	SD_RESPONSE_NO_ERROR 成功得到回应
			SD_RESPONSE_FAILURE 等待回应超时失败或回应不匹配
 *  @notice
 */
u8 SD_GetResponse(u8 response)
{
	u16 Count = 0xFFFF;//等待次数	   						  
	while((SD_SPI_ReadWriteByte(SD_DUMMY_BYTE) != response) && Count)	Count--;	//等待得到准确的回应  	  
	if(Count == 0)	return SD_RESPONSE_FAILURE;		//得到回应失败   
	else 		return SD_RESPONSE_NO_ERROR;	//正确回应
}


/**
 *  @name 			 SD_RecvData
 *	@description 从sd卡读取一定长度的内容，保存到缓冲区buf中
 *	@param 			 buf 数据缓存区
								 len 要读取的数据长度
 *	@return 		 1 读取成功
								 0 读取失败	
 *  @notice
 */
u8 SD_RecvData(u8 *buf,u16 len)
{	
	//等待SD卡发回数据起始令牌
	if(SD_GetResponse(SD_BEGIN_END_FLAG) != SD_RESPONSE_NO_ERROR)		return 0;
	//开始接收数据
  while(len--)	
	{
		*buf++ = SD_SPI_ReadWriteByte(SD_DUMMY_BYTE);
//	  printf("%x",*buf++);
	}
		//下面是2个伪CRC（dummy CRC）
  SD_SPI_ReadWriteByte(0xFF);
	SD_SPI_ReadWriteByte(0xFF);		
  return 1;//读取成功
}



/**
 *  @name SD_SendBlock
 *	@description 向sd卡写入一个数据包的内容，共512字节
 *	@param 			 buf 数据缓存区
					 			 cmd 指令
 *	@return       1 发送成功
	           	  	0 发送失败	
 *  @notice
 */
u8 SD_SendBlock(const u8 *buf, u8 cmd)
{	
	u16 t;		  	  
	if(SD_WaitReady() == 0)return 0;//等待准备失效
	SD_SPI_ReadWriteByte(cmd);
	if(cmd!=0XFD)//不是结束指令
	{
		for(t=0;t<512;t++) SPI1_ReadWriteByte(buf[t]);//提高速度,减少函数传参时间
	  SD_SPI_ReadWriteByte(0xFF);//忽略crc
	  SD_SPI_ReadWriteByte(0xFF);
		t=SD_SPI_ReadWriteByte(0xFF);//接收响应
		if((t&0x1F)!=0x05)return 2;//响应错误									  					    
	}						 									  					    
  return 1;//写入成功
}

//获取SD卡的CID信息，包括制造商信息
//输入: u8 *cid_data(存放CID的内存，至少16Byte）	  
//返回值:0：错误
//		 1：成功													   
u8 SD_GetCID(u8 *cid_data)
{
	u8 r1;	   
  //发CMD10命令，读CID
  r1=SD_SendCmd(CMD10,0,0x01);
  if(r1==0x00)
	{
		r1=SD_RecvData(cid_data,16);//接收16个字节的数据	 
  }
	SD_DisSelect();//取消片选
	if(r1)	return 0;
	else return 1;
}	

//获取SD卡的CSD信息，包括容量和速度信息
//输入:u8 *cid_data(存放CID的内存，至少16Byte）	    
//返回值:1：NO_ERR
//		 0：错误														   
u8 SD_GetCSD(u8 *csd_data)
{
	u8 r1;	 
  r1=SD_SendCmd(CMD9,0,0x01);//发CMD9命令，读CSD
  if(r1==0)
	{
    r1=SD_RecvData(csd_data, 16);//接收16个字节的数据 
  }
	SD_DisSelect();//取消片选
	if(r1)	return 0;
	else return 1;
}  

//获取SD卡的总扇区数（扇区数）   
//返回值:0： 取容量出错 
//       其他:SD卡的容量(扇区数/512字节)
//每扇区的字节数必为512，因为如果不是512，则初始化不能通过.														  
u32 SD_GetSectorCount(void)
{
  u8 csd[16];
  u32 Capacity;  
  u8 n;
	u16 csize;  					    
	//取CSD信息，如果期间出错，返回0
  if(SD_GetCSD(csd)!=0) return 0;	    
	//如果为SDHC卡，按照下面方式计算
  if((csd[0]&0xC0)==0x40)	 //V2.00的卡
  {	
		csize = csd[9] + ((u16)csd[8] << 8) + 1;
		Capacity = (u32)csize << 10;//得到扇区数	 		   
  }else//V1.XX的卡
  {	
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
		Capacity= (u32)csize << (n - 9);//得到扇区数   
   }
   return Capacity;
}

/**
 * @brief  	初始化SD，使其进入工作模式
 * @param  
 * @retval 	0 卡错误
 */
u8 SD_Init(void)
{
	int i;
	u8 r1;		//存放命令响应
	u16 retry;	//存放超时计数
	u8 buf[4];	//临时缓冲区
	SPI1_Init();
	SD_SPI_SetLowSpeed();   /*SPI设置为低速模式，CLK时钟最大不能超过400KHz */
	for(i = 0; i < 15; ++i)		SD_SPI_ReadWriteByte(SD_DUMMY_BYTE);   /*上电延时至少74个时钟周期，这里延时80个时钟周期*/
	retry = 500;
	do{
		r1 = SD_SendCmd(CMD0,0,0x95);
	}while(r1 != 0x01 && retry-- > 0);
	SD_Type = SD_TYPE_ERR; //默认无卡状态
	if(r1 != 0x01)	return 1;
	r1 = SD_SendCmd(CMD8,0x1AA,0x87);    		//根据SD卡工作电压区分SD卡种类
  if(r1 == 0x01)       //SD V2.0
	{
		for(i = 0;i < 4;i++)
				buf[i] = SD_SPI_ReadWriteByte(SD_DUMMY_BYTE);
		if(buf[2] == 0x01 && buf[3] == 0xAA)
		{
				retry = 0xFFFE;
				do{
					SD_SendCmd(CMD55,0,0x01); //发送CMD55，告诉SD卡下条指令是特殊命令
					r1 = SD_SendCmd(CMD41,0x40000000,0x01);
				}while(r1 && retry--);
				if(retry && SD_SendCmd(CMD58,0,0x01) == 0)
				{
						for(i = 0;i < 4; i++)
							buf[i] = SD_SPI_ReadWriteByte(0xFF); //得到OCR值
						if(buf[0] & 0x40)	 SD_Type = SD_TYPE_V2HC;  //检查CCS
						else SD_Type = SD_TYPE_V2;
				}
		}
	}
	else{	//SD V1.x/ MMC	V3
		SD_SendCmd(CMD55, 0, 0X01);		//发送CMD55
		r1 = SD_SendCmd(CMD41, 0, 0X01);	//发送CMD41
		if(r1 <= 1)
		{		
			SD_Type = SD_TYPE_V1;
			retry = 0XFFFE;
			do{ //等待退出IDLE模式
					SD_SendCmd(CMD55, 0, 0X01);	//发送CMD55
					r1 = SD_SendCmd(CMD41, 0, 0X01);//发送CMD41
			}while(r1 && retry--);
		}
		else{	//MMC卡不支持CMD55+CMD41识别
			SD_Type = SD_TYPE_MMC;//MMC V3
			retry = 0XFFFE;
			do{ //等待退出IDLE模式
				r1 = SD_SendCmd(CMD1,0,0X01);//发送CMD1
			}while(r1 && retry--);  
		}
		if(retry == 0 || SD_SendCmd(CMD16, 512, 0X01) != 0)		SD_Type = SD_TYPE_ERR;//错误的卡
	}
	SD_DisSelect();			//取消片选
	SD_SPI_SetHighSpeed();	//正常传输设置为高速
	printf("SD_Type = %d\r\n",SD_Type);
	if(SD_Type)	return 0;
	if(r1)	return r1;
	return 0xaa;
}


/**
 *  @name SD_ReadDisk
 *	@description 读SD卡扇区
 *	@param		  	buf 数据缓冲区
								  sector 扇区号
								  cnt 要读取的扇区数目
 *	@return 			操作结果，1 成功；其他，失败
 *  @notice
 */
u8 SD_ReadDisk(u8 *buf, u32 sector, u8 cnt)
{
	u8 r1;
	if(SD_Type != SD_TYPE_V2HC)	sector <<= 9;//转换为字节地址
	if(cnt == 1)
	{
		r1 =25;
		r1 = SD_SendCmd(CMD17, sector, 0X01);//读命令  
		if(r1 == 0)  r1 = SD_RecvData(buf, 512);//接收512个字?
	}
	else
	{
		r1 = SD_SendCmd(CMD18, sector, 0X01);//连续读命令
		do
		{
			r1 = SD_RecvData(buf, 512);//接收512个字节	 
			buf += 512;  
		}while(--cnt && r1 == 1); 	
		SD_SendCmd(CMD12, 0, 0X01);	//发送停止命令
	}   
	SD_DisSelect();//取消片选
	
	return r1;
}


/**
 *  @name SD_WriteDisk
 *	@description 写SD卡扇区
 *	@param 	buf 数据缓冲区
			sector 扇区号
			cnt 要写入的扇区数目
 *	@return 操作结果，0 成功；其他，失败
 *  @notice
 */
u8 SD_WriteDisk(const u8 *buf, u32 sector, u8 cnt)
{
	u8 r1;
	if(SD_Type != SD_TYPE_V2HC)
		sector *= 512;//转换为字节地址
	if(cnt == 1)
	{
		r1 = SD_SendCmd(CMD24,sector,0X01);//读命令
		if(r1 == 0)//指令发送成功
		{
			r1 = SD_SendBlock(buf,0xFE);//写512个字节	   
		}
	}
	else
	{
		if(SD_Type != SD_TYPE_MMC)
		{
			SD_SendCmd(CMD55,0,0X01);	
			SD_SendCmd(CMD23,cnt,0X01);//发送指令	
		}
 		r1 = SD_SendCmd(CMD25,sector,0X01);//连续读命令
		if(r1 == 0)
		{
			do
			{
				r1 = SD_SendBlock(buf,0xFC);//接收512个字节	 
				buf += 512;  
			}while(--cnt && r1 == 1);
			r1 = SD_SendBlock(0,0xFD);//接收512个字节 
		}
	}   
	SD_DisSelect();//取消片选
  return r1;
}	




/**
 * @brief 	带超时退出的SD卡初始化函数
 * @param 	
 * @return 	1 初始化成功
			0 初始化失败
 */
u8 SD_TryInit(void)
{
	int retry = 5;
	
	/* 初始化SD卡 */
	while(--retry > 0 && SD_Init())	//检测不到SD卡
	{
		SD_DEBUG("SD Card Error!Please check!\r\n");
		delay_ms(500);
	}
	if(retry)
	{
		SD_DEBUG("Init SD Card successfully!\r\n");
		return 1;
	}
	else
	{
		SD_DEBUG("failed to init SD Card .\r\n");
		return 0;
	}
}



/**
 * @brief 	查看某个扇区的内容
 * @param 	sector 扇区号
 * @return 	None
 */
void SD_ViewSector(u8 sector)
{
	int i;

	if(!SD_ReadDisk(buf, sector, 1))
		SD_DEBUG("\r\nRead SD failed.\r\n");
	else
	{
		SD_DEBUG("\r\nView sector %d: \r\n", sector);
		for(i = 0; i < SECTOR_SIZE; ++i)
			SD_DEBUG("%x ", buf[i]);
		SD_DEBUG("\r\nSector %d END...\r\n", sector);
	}	
}

