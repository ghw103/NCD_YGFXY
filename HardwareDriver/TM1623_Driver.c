
/***************************************************************************************************
*FileName：TM1623_Driver
*Description：排队功能模块驱动
*Author：xsx
*Data：2016年3月12日14:31:46
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"TM1623_Driver.h"
#include 	"delay.h"

/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/
/*
	表示LED灯的状态，每个LED 分红色和绿色共占2位
	00  都熄灭 
	01	红灯亮，绿灯灭
	10	绿灯亮，红灯灭
	11	都亮
	3字节共24位，可表示12个双色LED灯
	0和1位表示LED1，以此往后
*/
static unsigned int GB_S_TM1623LEDState = 0;

static unsigned char GB_S_TM1623KEYState[5] = {0, 0, 0, 0, 0};
/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/
static void TM1623_SDA_IN(void);
static void TM1623_SDA_OUT(void);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName：TM1623_Init
*Description：排队功能模块GPIO初始化
*Input：None
*Output：None
*Author：xsx
*Data：2016年3月12日14:32:15
***************************************************************************************************/
void TM1623_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(TM1623_SCK_Rcc | TM1623_STB_Rcc | TM1623_SDA_Rcc, ENABLE);//使能GPIOB时钟

	//GPIOB8,B9初始化设置
	GPIO_InitStructure.GPIO_Pin = TM1623_SCK_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(TM1623_SCK_Group, &GPIO_InitStructure);//初始化
	
	GPIO_InitStructure.GPIO_Pin = TM1623_SDA_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(TM1623_SDA_Group, &GPIO_InitStructure);//初始化
	
	GPIO_InitStructure.GPIO_Pin = TM1623_STB_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(TM1623_STB_Group, &GPIO_InitStructure);//初始化
	
}
/***************************************************************************************************
*FunctionName：TM1623_SDA_IN
*Description：排队功能模块通信SDA脚输入
*Input：None
*Output：None
*Author：xsx
*Data：2016年3月12日14:33:11
***************************************************************************************************/
static void TM1623_SDA_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	//GPIOB8,B9初始化设置
	GPIO_InitStructure.GPIO_Pin = TM1623_SDA_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(TM1623_SDA_Group, &GPIO_InitStructure);//初始化
}
/***************************************************************************************************
*FunctionName：TM1623_SDA_OUT
*Description：排队功能模块通信SDA脚输出
*Input：None
*Output：None
*Author：xsx
*Data：2016年3月12日14:33:23
***************************************************************************************************/
static void TM1623_SDA_OUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	//GPIOB8,B9初始化设置
	GPIO_InitStructure.GPIO_Pin = TM1623_SDA_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(TM1623_SDA_Group, &GPIO_InitStructure);//初始化
}

/***************************************************************************************************
*FunctionName：WriteByte
*Description：排队功能模块，写一个字节
*Input：data -- 写入的数据
*Output：None
*Author：xsx
*Data：2016年3月12日14:34:15
***************************************************************************************************/
void TM1623_WriteByte(unsigned char data)
{
	unsigned char i=0;
	
	for(i=0; i<8; i++)
	{
		if(data & 0x01)
			GPIO_SetBits(TM1623_SDA_Group, TM1623_SDA_Pin);
		else
			GPIO_ResetBits(TM1623_SDA_Group, TM1623_SDA_Pin);
			
		delay_us(5);
		GPIO_SetBits(TM1623_SCK_Group, TM1623_SCK_Pin);
		delay_us(5);

		GPIO_ResetBits(TM1623_SCK_Group, TM1623_SCK_Pin);
		delay_us(5);

		data >>= 1;
	}
}

/***************************************************************************************************
*FunctionName：TM1623_Send_CMD
*Description：发送命令给排队模块
*Input：cmd -- 发送的命令
*Output：None
*Author：xsx
*Data：2016年3月12日14:38:39
***************************************************************************************************/
void TM1623_Send_CMD(unsigned char cmd)
{
	GPIO_SetBits(TM1623_STB_Group, TM1623_STB_Pin);
	delay_us(5);
	
	GPIO_ResetBits(TM1623_STB_Group, TM1623_STB_Pin);
	delay_us(5);
	
	TM1623_WriteByte(cmd);
}

/***************************************************************************************************
*FunctionName：TM1623_ReadKey
*Description：读取按键信息
*Input：None
*Output：None
*Author：xsx
*Data：2016年3月12日14:44:16
***************************************************************************************************/
void TM1623_ReadKey(void)
{
	unsigned char i=0,j;
	unsigned char data = 0;	
	
	
	TM1623_Send_CMD(0x03); 

	TM1623_Send_CMD(0x42);
	delay_us(5);
	TM1623_SDA_IN();
	
	for(j=0; j<5; j++)
	{
		data = 0;
		
		for(i=0; i<8; i++)
		{
			data = data >> 1;
			GPIO_ResetBits(TM1623_SCK_Group, TM1623_SCK_Pin);
			delay_us(5);
			GPIO_SetBits(TM1623_SCK_Group, TM1623_SCK_Pin);
			delay_us(5);
			
			if(GPIO_ReadInputDataBit(TM1623_SDA_Group, TM1623_SDA_Pin))
			{
				data = data|0x80;
			}
		}
		GB_S_TM1623KEYState[j] = data;
	}
	TM1623_SDA_OUT();
}

/***************************************************************************************************
*FunctionName：ChangeTM1623LedStatues
*Description：按照指定状态写入排队模块
*Input：ledindex -- 灯编号
*		statues -- 对应编号的状态
*Output：None
*Author：xsx
*Data：2016年3月12日16:40:34
***************************************************************************************************/
void ChangeTM1623LedStatues(unsigned char ledindex, TM1623_LED_State statues)
{
	unsigned int temp = statues;
	
	if(ledindex >   12)
		return;
	
	TM1623_Send_CMD(0x03);
	TM1623_Send_CMD(0x44);
	
	TM1623_Send_CMD(0xc0+(ledindex/2)*2);
	
	if((statues & 0x01) == 0)
		GB_S_TM1623LEDState &= ~(((unsigned int)1)<<(ledindex*2));
	else
		GB_S_TM1623LEDState |= (((unsigned int)1)<<(ledindex*2));
	
	if(((statues & 0x02)>>1) == 0)
		GB_S_TM1623LEDState &= ~(((unsigned int)1)<<(ledindex*2+1));
	else
		GB_S_TM1623LEDState |= (((unsigned int)1)<<(ledindex*2+1));
	
	temp = (unsigned char)((GB_S_TM1623LEDState>>(ledindex/2*4))&0x0f);
	temp <<= 4;

	TM1623_WriteByte(temp);
	
	TM1623_Send_CMD(0x8f);
}
/***************************************************************************************************
*FunctionName：GetLedStatues
*Description：读取一个LED的状态
*Input：None
*Output：None
*Author：xsx
*Data：2016年3月17日17:54:58
***************************************************************************************************/
TM1623_LED_State GetLedStatues(unsigned char ledindex)
{
	unsigned char temp = 0;
	
	temp = (unsigned char)((GB_S_TM1623LEDState>>(ledindex*2))&0x03);

	return (TM1623_LED_State)temp;
}


/***************************************************************************************************
*FunctionName：
*Description：
*Input：None
*Output：None
*Author：xsx
*Data：
***************************************************************************************************/
TM1623_KEY_State GetTheKeyStatues(unsigned char keyindex)
{
	/*9个按键只会使用前2个字节*/
	unsigned char tempkeydata = GB_S_TM1623KEYState[keyindex/4];
	
	if(keyindex > 3)
		keyindex -= 4;
	
	tempkeydata >>= ((keyindex/2)+ keyindex);
	
	tempkeydata = tempkeydata&0x01;
	
	return (TM1623_KEY_State)tempkeydata;
}

/***************************************************************************************************
*FunctionName:  getTM1623KeyData
*Description:  获取从1623读取的数据
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年3月27日 15:00:19
***************************************************************************************************/
unsigned char getTM1623KeyData(unsigned char index)
{
	return GB_S_TM1623KEYState[index];
}



