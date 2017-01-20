/***************************************************************************************************
*FileName:	DS18b20_Driver
*Description:	18b20驱动程序
*Author: xsx_kair
*Data:	2017年1月18日11:04:09
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/

#include	"DS18b20_Driver.h"
#include	"Delay.h"

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static void DS18B20_GPIO_Config(void);
static void DS18B20_Mode_IPU(void);
static void DS18B20_Mode_Out_PP(void);
static void DS18B20_Rst(void);
static uint8_t DS18B20_Presence(void);
static unsigned char CRC8(unsigned char * buf);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/****************************************File Start*************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName: DS18B20_Init
*Description: 初始化18b20
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年1月18日11:13:23
***************************************************************************************************/
void DS18B20_Init(void)
{
	
	DS18B20_GPIO_Config();
}

/***************************************************************************************************
*FunctionName: DS18B20_GPIO_Config
*Description: IO初始化
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年1月18日11:04:39
***************************************************************************************************/
static void DS18B20_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(DS18B20_CLK, ENABLE); 
														   
  	GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;	
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	GPIO_Init(DS18B20_PORT, &GPIO_InitStructure); 
}

/***************************************************************************************************
*FunctionName: DS18B20_Mode_IPU
*Description: 配置端口为上啦输入
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年1月18日11:08:19
***************************************************************************************************/
static void DS18B20_Mode_IPU(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN ; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	
	GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);	 
}

/***************************************************************************************************
*FunctionName: DS18B20_Mode_Out_PP
*Description: 配置为推挽输出
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年1月18日11:08:49
***************************************************************************************************/
static void DS18B20_Mode_Out_PP(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);	 	 
}


/***************************************************************************************************
*FunctionName: DS18B20_Rst
*Description: 复位18b20
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年1月18日11:10:32
***************************************************************************************************/
static void DS18B20_Rst(void)
{
	/* 主机设置为推挽输出 */
	DS18B20_Mode_Out_PP();
	
	DS18B20_DATA_OUT(DS18B20_LOW);
	
	/* 主机至少产生480us的低电平复位信号 */
	delay_us(750);

	/* 主机在产生复位信号后，需将总线拉高 */
	DS18B20_DATA_OUT(DS18B20_HIGH);
	
	/*从机接收到主机的复位信号后，会在15~60us后给主机发一个存在脉冲*/
	delay_us(15);
}

/***************************************************************************************************
*FunctionName: DS18B20_Presence
*Description: 检测从机给主机返回的存在脉冲
*Input: 
*Output: 
*Return: 
 * 0：成功
 * 1：失败
*Author: xsx
*Date: 2017年1月18日11:11:08
***************************************************************************************************/
static uint8_t DS18B20_Presence(void)
{
	uint8_t pulse_time = 0;
	
	/* 主机设置为上拉输入 */
	DS18B20_Mode_IPU();
	
	/* 等待存在脉冲的到来，存在脉冲为一个60~240us的低电平信号 
	 * 如果存在脉冲没有来则做超时处理，从机接收到主机的复位信号后，会在15~60us后给主机发一个存在脉冲
	 */
	while( DS18B20_DATA_IN() && pulse_time<200 )
	{
		pulse_time++;
		delay_us(1);
	}	
	/* 经过100us后，存在脉冲都还没有到来*/
	if( pulse_time >=200 )
		return 1;
	else
		pulse_time = 0;
	
	/* 存在脉冲到来，且存在的时间不能超过240us */
	while( !DS18B20_DATA_IN() && pulse_time<240 )
	{
		pulse_time++;
		delay_us(1);
	}	
	if( pulse_time >=240 )
		return 1;
	else
		return 0;
}

/***************************************************************************************************
*FunctionName: DS18B20_Read_Bit
*Description: 读取1bit
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年1月18日11:11:37
***************************************************************************************************/
static uint8_t DS18B20_Read_Bit(void)
{
	uint8_t dat;
	
	/* 读0和读1的时间至少要大于60us */	
	DS18B20_Mode_Out_PP();
	/* 读时间的起始：必须由主机产生 >1us <15us 的低电平信号 */
	DS18B20_DATA_OUT(DS18B20_LOW);
	delay_us(5);
	
	/* 设置成输入，释放总线，由外部上拉电阻将总线拉高 */
	DS18B20_DATA_OUT(DS18B20_HIGH);
	DS18B20_Mode_IPU();
	delay_us(12);
	
	if( DS18B20_DATA_IN() == SET )
		dat = 1;
	else
		dat = 0;
	
	/* 这个延时参数请参考时序图 */
	delay_us(45);
	
	return dat;
}

/***************************************************************************************************
*FunctionName: DS18B20_Read_Byte
*Description: 从DS18B20读一个字节，低位先行
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年1月18日11:12:04
***************************************************************************************************/
uint8_t DS18B20_Read_Byte(void)
{
	uint8_t i, j, dat = 0;	
	
	for(i=0; i<8; i++) 
	{
		j = DS18B20_Read_Bit();		
		dat = (dat) | (j<<i);
	}
	
	return dat;
}

/***************************************************************************************************
*FunctionName: DS18B20_Write_Byte
*Description: 写一个字节到DS18B20，低位先行
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年1月18日11:12:39
***************************************************************************************************/
void DS18B20_Write_Byte(uint8_t dat)
{
	uint8_t i, testb;
	DS18B20_Mode_Out_PP();
	
	for( i=0; i<8; i++ )
	{
		testb = dat&0x01;
		dat = dat>>1;		
		/* 写0和写1的时间至少要大于60us */
		if (testb)
		{			
			DS18B20_DATA_OUT(DS18B20_LOW);
			/* 1us < 这个延时 < 15us */
			delay_us(8);
			
			DS18B20_DATA_OUT(DS18B20_HIGH);
			delay_us(58);
		}		
		else
		{			
			DS18B20_DATA_OUT(DS18B20_LOW);
			/* 60us < Tx 0 < 120us */
			delay_us(70);
			
			DS18B20_DATA_OUT(DS18B20_HIGH);			
			/* 1us < Trec(恢复时间) < 无穷大*/
			delay_us(2);
		}
	}
}


/*
 * 存储的温度是16 位的带符号扩展的二进制补码形式
 * 当工作在12位分辨率时，其中5个符号位，7个整数位，4个小数位
 *
 *         |---------整数----------|-----小数 分辨率 1/(2^4)=0.0625----|
 * 低字节  | 2^3 | 2^2 | 2^1 | 2^0 | 2^(-1) | 2^(-2) | 2^(-3) | 2^(-4) |
 *
 *
 *         |-----符号位：0->正  1->负-------|-----------整数-----------|
 * 高字节  |  s  |  s  |  s  |  s  |    s   |   2^6  |   2^5  |   2^4  |
 *
 * 
 * 温度 = 符号位 + 整数 + 小数*0.0625
 */
float DS18B20_Get_Temp(void)
{
	uint8_t value[9], i=0;
	
	short s_tem;
	float f_tem;
	
	DS18B20_Rst();	   
	DS18B20_Presence();	 
	DS18B20_Write_Byte(0XCC);				/* 跳过 ROM */
	DS18B20_Write_Byte(0X44);				/* 开始转换 */
	
	DS18B20_Rst();
	DS18B20_Presence();
	DS18B20_Write_Byte(0XCC);				/* 跳过 ROM */
	DS18B20_Write_Byte(0XBE);				/* 读温度值 */
	
	for(i=0; i<9; i++)
		value[i] = DS18B20_Read_Byte();
	
	if(value[8] == CRC8(value))
	{
		
		s_tem = value[1]<<8;
		s_tem = s_tem | value[0];
		
		if( s_tem < 0 )		/* 负温度 */
			f_tem = (~s_tem+1) * 0.0625;	
		else
			f_tem = s_tem * 0.0625;
	}
	else
		f_tem = -200;							//如果crc错误，将温度设置成-200，表明读取失败，18b20的最低温度为-55
	
	return f_tem; 	
}

 /**
  * @brief  在匹配 ROM 情况下获取 DS18B20 温度值 
  * @param  ds18b20_id：用于存放 DS18B20 序列号的数组的首地址
  * @retval 无
  */
void DS18B20_ReadId ( uint8_t * ds18b20_id )
{
	uint8_t uc;
	
	
	DS18B20_Write_Byte(0x33);       //读取序列号
	
	for ( uc = 0; uc < 8; uc ++ )
	  ds18b20_id [ uc ] = DS18B20_Read_Byte();
	
}

static unsigned char CRC8(unsigned char * buf)
{     
	unsigned char i,x; 
	unsigned char crcbuff;  

	unsigned char crc=0;  

	for(x = 0; x <8; x++)  
	{  
		crcbuff = buf[x];  

		for(i = 0; i < 8; i++)   
		{   
			if(((crc ^ crcbuff)&0x01)==0)   
				crc >>= 1;   
			else 
			{   
				crc ^= 0x18;   //CRC=X8+X5+X4+1  

				crc >>= 1;   

				crc |= 0x80;   

			}           
		
			crcbuff >>= 1;         

		}
	}  

	return crc; 
} 
/****************************************end of file************************************************/





