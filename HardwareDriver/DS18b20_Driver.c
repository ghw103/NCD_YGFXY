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
const static unsigned char CrcTable[256] =
{
	  0,  94, 188, 226,  97,  63, 221, 131, 194, 156, 126,  32, 163, 253,  31,  65,
	157, 195,  33, 127, 252, 162,  64,  30,  95,   1, 227, 189,  62,  96, 130, 220,
	 35, 125, 159, 193,  66,  28, 254, 160, 225, 191,  93,   3, 128, 222,  60,  98,
	190, 224,   2,  92, 223, 129,  99,  61, 124,  34, 192, 158,  29,  67, 161, 255,
	 70,  24, 250, 164,  39, 121, 155, 197, 132, 218,  56, 102, 229, 187,  89,   7,
	219, 133, 103,  57, 186, 228,   6,  88,  25,  71, 165, 251, 120,  38, 196, 154,
	101,  59, 217, 135,   4,  90, 184, 230, 167, 249,  27,  69, 198, 152, 122,  36,
	248, 166,  68,  26, 153, 199,  37, 123,  58, 100, 134, 216,  91,   5, 231, 185,
	140, 210,  48, 110, 237, 179,  81,  15,  78,  16, 242, 172,  47, 113, 147, 205,
	 17,  79, 173, 243, 112,  46, 204, 146, 211, 141, 111,  49, 178, 236,  14,  80,
	175, 241,  19,  77, 206, 144, 114,  44, 109,  51, 209, 143,  12,  82, 176, 238,
	 50, 108, 142, 208,  83,  13, 239, 177, 240, 174,  76,  18, 145, 207,  45, 115,
	202, 148, 118,  40, 171, 245,  23,  73,   8,  86, 180, 234, 105,  55, 213, 139,
	 87,   9, 235, 181,  54, 104, 138, 212, 149, 203,  41, 119, 244, 170,  72,  22,
	233, 183,  85,  11, 136, 214,  52, 106,  43, 117, 151, 201,  74,  20, 246, 168,
	116,  42, 200, 150,  21,  75, 169, 247, 182, 232,  10,  84, 215, 137, 107,  53,
};
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static void DS18B20_GPIO_Config(void);
static void DS18B20_Config(void);
static void DS18B20_Mode_IPU(void);
static void DS18B20_Mode_Out_PP(void);
static bool DS18B20_Rst(void);
static void DS18B20_Write_Byte(const unsigned char dat);
static unsigned char DS18B20_Read_Byte(void);
static void DS18B20_Read_Bytes(unsigned char *buf, unsigned char len);
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
	
	DS18B20_Config();
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
static bool DS18B20_Rst(void)
{
	uint8_t pulse_time = 0;
	
	/* 主机设置为推挽输出 */
	DS18B20_Mode_Out_PP();
	
	DS18B20_DATA_OUT(DS18B20_LOW);
	/* 主机至少产生480us的低电平复位信号 */
	delay_us(600);
	/* 主机在产生复位信号后，需将总线拉高 */
	DS18B20_DATA_OUT(DS18B20_HIGH);
	
	/*从机接收到主机的复位信号后，会在15~60us后给主机发一个存在脉冲*/
	delay_us(20);
	
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
		return false;
	else
	{
		delay_us(240);
		return true;
	}
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
static void DS18B20_Write_Byte(const unsigned char dat)
{
	unsigned char i;
	DS18B20_Mode_Out_PP();
	
	for( i=0; i<8; i++ )
	{
		DS18B20_DATA_OUT(DS18B20_LOW);
		delay_us(8);
		
		if(dat & (1<<i))
		{
			DS18B20_DATA_OUT(DS18B20_HIGH);
			delay_us(58);
		}
		else
		{
			delay_us(60);
		
			DS18B20_DATA_OUT(DS18B20_HIGH);
			delay_us(2);
		}
		
	}
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
static unsigned char DS18B20_Read_Byte(void)
{
	unsigned char i, dat = 0;	
	
	for(i=0; i<8; i++) 
	{
		DS18B20_Mode_Out_PP();		
		DS18B20_DATA_OUT(DS18B20_LOW);
		delay_us(5);
		
		/* 设置成输入，释放总线，由外部上拉电阻将总线拉高 */
		DS18B20_DATA_OUT(DS18B20_HIGH);
		DS18B20_Mode_IPU();
		delay_us(12);
		
		if( DS18B20_DATA_IN() == SET )
			dat |= (1<<i);
		delay_us(45);
	}
	
	return dat;
}

/***************************************************************************************************
*FunctionName:  DS18B20_Read_Bytes
*Description:  读取多个字节
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年3月23日 16:24:55
***************************************************************************************************/
static void DS18B20_Read_Bytes(unsigned char *buf, unsigned char len)
{
	unsigned char i = 0;
	
	for(i=0; i<len; i++)
	{
		*buf = DS18B20_Read_Byte();
		buf++;
	}
}

/***************************************************************************************************
*FunctionName:  DS18B20_CRC
*Description:  对18b20读取的数据校验
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年3月23日 16:29:39
***************************************************************************************************/
static unsigned char DS18B20_CRC(const unsigned char *buf, unsigned char len)
{
	unsigned char crc_data = 0, i = 0;
	for(i=0; i<len; i++)						//查表校验
		crc_data = CrcTable[crc_data^buf[i]];
	
	return crc_data;
}

/***************************************************************************************************
*FunctionName:  DS18B20_Config
*Description:  配置18B20
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年3月23日 16:32:05
***************************************************************************************************/
static void DS18B20_Config(void)
{
	DS18B20_Rst();	
	DS18B20_Write_Byte(0xcc);
	DS18B20_Write_Byte(0x4e);
	DS18B20_Write_Byte(0x19);
	DS18B20_Write_Byte(0x1a);
	DS18B20_Write_Byte(0x1f);		//9bit模式，0.5摄氏度
	
	DS18B20_Rst();	
	DS18B20_Write_Byte(0xcc);
	DS18B20_Write_Byte(0x48);
	
	DS18B20_Rst();	
	DS18B20_Write_Byte(0xcc);
	DS18B20_Write_Byte(0xb8);
}

void startDS18B20(void)
{
	DS18B20_Rst();
	DS18B20_Write_Byte(0XCC);				/* 跳过 ROM */
	DS18B20_Write_Byte(0X44);				/* 开始转换 */
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
float readDS18B20Temp(void)
{
	uint8_t value[9], i=0;
	
	short s_tem;
	float f_tem = 300;
	
	DS18B20_Rst();
	DS18B20_Write_Byte(0XCC);				/* 跳过 ROM */
	DS18B20_Write_Byte(0XBE);				/* 读温度值 */
	
	DS18B20_Read_Bytes(value, 9);
	
	if(0 == DS18B20_CRC(value, 9))
	{
		s_tem = value[1]<<8;
		s_tem = s_tem | value[0];
		
		if( s_tem < 0 )		/* 负温度 */
			f_tem = (~s_tem+1) * 0.0625;	
		else
			f_tem = s_tem * 0.0625;
	}
	
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

/****************************************end of file************************************************/





