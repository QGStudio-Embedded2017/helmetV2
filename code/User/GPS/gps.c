#include "gps.h"
#include"stm32f10x.h"
#include"uart4.h"
	
u8 GPS_BUF[RX_BUFFER_SIZE];				
vu16 GPSMSG_FLAG = 0;							
vu16 GPSMSG_INDEX = 0;
vu8 GPS_DEBUG = 0; 

/*
@name:NMEA_Comma_Pos()
@param:buf--u8*:指定的字符串指针
	   cx--u8：第cx个逗号
@return:返回一个偏移值
@description:从buf字符串中找到第cx个’,'所在的地址，返回相应的偏移值
*/
u8 NMEA_Comma_Pos(u8 *buf,u8 cx)
{	 		    
	u8 *p=buf;
	while(cx)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
}

/*
@name: NMEA_Pow
@param: m--基数；
		n--幂
@return：m的n次幂
@description:计算m的n次幂，并且返回
*/
u32 NMEA_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

int NMEA_Str2num(u8 *buf,u8*dx)
{
	u8 *p=buf;
	u32 ires=0,fres=0;
	u8 ilen=0,flen=0,i;
	u8 mask=0;
	int res;
	while(1)
	{
		if(*p=='-'){mask|=0X02;p++;}
		if(*p==','||(*p=='*'))break;
		if(*p=='.'){mask|=0X01;p++;}
		else if(*p>'9'||(*p<'0'))	
		{	
			ilen=0;
			flen=0;
			break;
		}	
		if(mask&0X01)flen++;
		else ilen++;
		p++;
	}
	if(mask&0X02)buf++;
	for(i=0;i<ilen;i++)
		ires+=NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');
	if(flen>5)flen=5;	
	*dx=flen;	 		
	for(i=0;i<flen;i++)	  
		fres+=NMEA_Pow(10,flen-1-i)*(buf[ilen + 1 + i] - '0');
	res = ires * NMEA_Pow(10,flen) + fres;
	if(mask & 0X02)	res = -res;		   
	return res;
}	  

//GPRMC
void NMEA_GPRMC_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx;     
	u32 temp;	   
	float rs;  
	p1 = (u8*)strstr((const char *)buf,"GPRMC");
	posx = NMEA_Comma_Pos(p1,1);								
	if(posx != 0XFF)
	{
		temp = NMEA_Str2num(p1+posx,&dx)/NMEA_Pow(10,dx);	 
		gpsx->utc.hour = temp / 10000;
		gpsx->utc.min = (temp / 100) % 100;
		gpsx->utc.sec = temp % 100;	 	 
	}	
	posx = NMEA_Comma_Pos(p1,3);								
	if(posx != 0XFF)
	{
		temp = NMEA_Str2num(p1 + posx,&dx);		 	 
		gpsx->latitude = temp / NMEA_Pow(10,dx+2);	
		rs = temp % NMEA_Pow(10,dx+2);			
		gpsx->latitude = gpsx->latitude*NMEA_Pow(10,5) + (rs * NMEA_Pow(10,5-dx)) / 60;
	}
	posx = NMEA_Comma_Pos(p1,4);							
	if(posx!=0XFF) gpsx->nshemi =* (p1+posx);					 
 	posx=NMEA_Comma_Pos(p1,5);								
	if(posx != 0XFF)
	{												  
		temp = NMEA_Str2num(p1+posx,&dx);		 	 
		gpsx->longitude = temp / NMEA_Pow(10,dx+2);
		rs = temp%NMEA_Pow(10,dx+2);					 
		gpsx->longitude = gpsx->longitude * NMEA_Pow(10,5) + (rs * NMEA_Pow(10,5-dx)) / 60;
	}
	posx = NMEA_Comma_Pos(p1,6);						
	if(posx != 0XFF) gpsx->ewhemi =* (p1+posx);		 
	posx = NMEA_Comma_Pos(p1,9);							
	if(posx != 0XFF)
	{
		temp = NMEA_Str2num(p1+posx,&dx);		 			
		gpsx->utc.date = temp / 10000;
		gpsx->utc.month = (temp / 100) % 100;
		gpsx->utc.year = 2000 + temp % 100;	 	 
	} 

}

//GPGSV
void NMEA_GPGSV_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p,*p1,dx;
	u8 len,i,j,slx=0;
	u8 posx;   	 
	p = buf;
	p1 = (u8*)strstr((const char *)p,"$GPGSV");
	len = p1[7] - '0';								
	posx = NMEA_Comma_Pos(p1,3); 			
	if(posx != 0XFF)	gpsx->svnum = NMEA_Str2num(p1 + posx,&dx);
	for(i = 0;i < len;i++)
	{	 
		p1 = (u8*)strstr((const char *)p,"$GPGSV");  
		for(j = 0;j < 4;j++)
		{	  
			posx = NMEA_Comma_Pos(p1,4 + j * 4);
			if(posx != 0XFF)gpsx->slmsg[slx].num = NMEA_Str2num(p1 + posx,&dx);
			else break; 
			posx = NMEA_Comma_Pos(p1,5 + j * 4);
			if(posx != 0XFF)	gpsx->slmsg[slx].eledeg = NMEA_Str2num(p1 + posx,&dx);
			else break;
			posx = NMEA_Comma_Pos(p1,6 + j * 4);
			if(posx != 0XFF)	gpsx->slmsg[slx].azideg = NMEA_Str2num(p1 + posx,&dx);
			else break; 
			posx = NMEA_Comma_Pos(p1,7 + j * 4);
			if(posx != 0XFF)	gpsx->slmsg[slx].sn = NMEA_Str2num(p1 + posx,&dx);	
			else break;
			slx++;	   
		}   
 		p = p1 + 1;
	}   
}

//GPGGA
void NMEA_GPGGA_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx;    
	p1 = (u8*)strstr((const char *)buf,"$GPGGA");
	posx = NMEA_Comma_Pos(p1,6);						
	if(posx != 0XFF)	gpsx->gpssta = NMEA_Str2num(p1 + posx,&dx);	
	posx = NMEA_Comma_Pos(p1,7);							
	if(posx != 0XFF)	gpsx->posslnum = NMEA_Str2num(p1 + posx,&dx); 
	posx = NMEA_Comma_Pos(p1,9);							
	if(posx != 0XFF)	gpsx->altitude = NMEA_Str2num(p1 + posx,&dx);  
}

//GPGSA
void NMEA_GPGSA_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx; 
	u8 i;   
	p1 = (u8*)strstr((const char *)buf,"$GPGSA");
	posx = NMEA_Comma_Pos(p1,2);						
	if(posx != 0XFF)	gpsx->fixmode = NMEA_Str2num(p1 + posx,&dx);	
	for(i = 0;i < 12;i++)							
	{
		posx = NMEA_Comma_Pos(p1,3 + i);					 
		if(posx != 0XFF)	gpsx->possl[i] = NMEA_Str2num(p1 + posx,&dx);
		else break; 
	}				  
	posx = NMEA_Comma_Pos(p1,15);							
	if(posx != 0XFF)	gpsx->pdop = NMEA_Str2num(p1 + posx,&dx);  
	posx = NMEA_Comma_Pos(p1,16);								
	if(posx != 0XFF)	gpsx->hdop = NMEA_Str2num(p1 + posx,&dx);  
	posx = NMEA_Comma_Pos(p1,17);								
	if(posx != 0XFF)	gpsx->vdop=NMEA_Str2num(p1+posx,&dx);  
}

//GPVTG
void NMEA_GPVTG_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx;    
	p1 = (u8*)strstr((const char *)buf,"$GPVTG");							 
	posx = NMEA_Comma_Pos(p1,7);							
	if(posx != 0XFF)
	{
		gpsx->speed = NMEA_Str2num(p1 + posx,&dx);
		if(dx < 3)	gpsx->speed *= NMEA_Pow(10,3 - dx);	 	 		
	}
}  

void Send_NMEA_GPRMC(nmea_msg *gpsx)
{
	printf("year:%d ", gpsx->utc.year);
	printf("month:%d ", gpsx->utc.month);
	printf("date:%d ", gpsx->utc.date);
	printf("hour:%d ", (gpsx->utc.hour+8)%24);
	printf("min:%d ", gpsx->utc.min);
	printf("sec:%d ", gpsx->utc.sec);
	
	printf("%c latit:", gpsx->nshemi);
	printf("%f ", (float)gpsx->latitude/100000);

	printf("%c longi:", gpsx->ewhemi);
	printf("%f\r\n", (float)gpsx->longitude/100000);
}

void Send_NMEA_GPGSV(nmea_msg *gpsx)
{
	int i, j = gpsx->svnum;
	printf("satellites in view is %d \r\n", gpsx->svnum);
	printf("satellite number  elevation in degrees    azimuth in degrees to true   SNR in dB\r\n");
	for (i = 0; i < j; i++)
	{
		printf("%d\t\t\t",gpsx->slmsg[i].num);
		printf("%d\t\t\t",gpsx->slmsg[i].eledeg);
		printf("%d\t\t\t",gpsx->slmsg[i].azideg);
		printf("%d",gpsx->slmsg[i].sn);
		printf("\r\n");
	}
}

void Send_NMEA_GPGGA(nmea_msg *gpsx)
{
	printf("GPS quality indicator: %d    Number of satellites in view: %d    high:%.3f\r\n", \
		gpsx->gpssta, gpsx->posslnum, (float)gpsx->altitude/10);
}

void Send_NMEA_GPGSA(nmea_msg *gpsx)
{
	int i;
	printf("mode: %d\tPDOP: %.3f\tHDOP:%.3f\tVDOP:%.3f", \
		gpsx->fixmode, (float)gpsx->pdop/100, (float)gpsx->hdop/100, (float)gpsx->vdop/100);
	printf("\r\nNO:");
	for (i = 0 ; i < gpsx->posslnum; i++)
	{
		printf("%d ",gpsx->possl[i]);
	}
	printf("\r\n");
}

void Send_NMEA_GPVTG(nmea_msg *gpsx)
{
	printf("speed: %f\r\n\r\n", (float)gpsx->speed/1000);
}

/*
@name:GPSMSG_Analysis()
@param:gpsx--nmea_msg*:储存GPS模块地理信息的一个结构体
@return:none
@description:分析GPS模块发送出来的数据，并将相关信息解析到gpsx结构体里面
*/
void GPSMSG_Analysis(nmea_msg *gpsx, u8 *buf)
{
		NMEA_GPRMC_Analysis(gpsx,buf);	
		NMEA_GPGSV_Analysis(gpsx,buf);	
		NMEA_GPGGA_Analysis(gpsx,buf);	
		NMEA_GPGSA_Analysis(gpsx,buf);	
		NMEA_GPVTG_Analysis(gpsx,buf);	
}

//void Send_NMEA_MSG(nmea_msg *gpsx)
//{
//		Send_NMEA_GPRMC(gpsx);
//		Send_NMEA_GPGSV(gpsx);
//		Send_NMEA_GPGGA(gpsx);
//		Send_NMEA_GPGSA(gpsx);
//		Send_NMEA_GPVTG(gpsx);
//}


/*
@name: Send_NMEA_MSG(nmea_msg *gpsx);
@param:gpsx--指向结构体nmea_msg的一个指针，该结构体用来储存行李箱的位置信息
@return:none
@description:打印行李箱的位置信息，包括时间，经纬度，速度和海拔高度
*/
void Send_NMEA_MSG(nmea_msg *gpsx)
{
	printf("time: %d/%d/%d ",gpsx->utc.year,gpsx->utc.month,gpsx->utc.date);   //年月日
	printf("%d:%d:%d\n",(gpsx->utc.hour+8)%24,gpsx->utc.min,gpsx->utc.sec);    //时分秒
	printf("locate: %f%c %f%c\n",(float)gpsx->longitude/100000,gpsx->ewhemi,(float)gpsx->latitude/100000,gpsx->nshemi);//经纬度
//  printf("speed: %f\r\n", (float)gpsx->speed/1000);  //速度
	printf("height: %.3f\n\n",(float)gpsx->altitude/10); //海拔高度
}
