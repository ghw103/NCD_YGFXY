/***************************************************************************************************
*FileName:TLV5617_Driver
*Description:发光模块驱动，控制发光亮度和基线高度
*Author:xsx
*Data:2016年4月22日16:33:12
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"TLV5617_Driver.h"
#include	"SPI1_Driver.h"

#include	"Delay.h"
/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/
static unsigned short GB_LedValue = 0;						//发光二极管亮度
static unsigned short GB_CLineValue = 0;					//基线高度
/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/
static void DA_Write(unsigned short data);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

void DA_IO_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(DA_SCK_Rcc | DA_SDA_Rcc | DA_CS_Rcc, ENABLE);

	GPIO_InitStructure.GPIO_Pin = DA_SCK_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(DA_SCK_Group, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = DA_SDA_Pin;
    GPIO_Init(DA_SDA_Group, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = DA_CS_Pin;
    GPIO_Init(DA_CS_Group, &GPIO_InitStructure);
	
}


static void DA_Write(unsigned short data)
{
	unsigned char i;
	
	DA_SCK_H();
	delay_us(5);
	
	for(i=0; i<16; i++)
    {
		if(data & ((unsigned short)0x8000))
			DA_SDA_H();
        else
			DA_SDA_L();
		delay_us(5);
	
		data <<= 1;
		
		DA_SCK_L();
		delay_us(5);
		
		DA_SCK_H();
		delay_us(5);
    }

}
/***************************************************************************************************
*FunctionName：GetGB_LedValue
*Description：读取当前发光二极管亮度
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月22日18:19:48
***************************************************************************************************/
unsigned short GetGB_LedValue(void)
{
	return GB_LedValue;
}

/***************************************************************************************************
*FunctionName：SetGB_LedValue
*Description：设置发光二极管亮度 0 - 1023,实际测试中最大320
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月22日18:19:29
***************************************************************************************************/
void SetGB_LedValue(unsigned short value)
{
	unsigned short temp = value;
	
	
	DA_CS_H();
	delay_us(10);
	
	DA_CS_L();
	delay_us(50);
	
	GB_LedValue = value;
	
	//低2位为0
	temp <<= 2;					
	
	/*更新DAC B值*/
	temp &= ~((unsigned short)1<<(15));
	temp &= ~((unsigned short)1<<(12));
	
	/*快速模式*/
	temp &= ~((unsigned short)1<<(14));
	
	/*正常模式*/
	temp &= ~((unsigned short)1<<(13));
	
	DA_Write(temp);
	
	delay_us(50);
	DA_CS_H();
}

/***************************************************************************************************
*FunctionName：GetGB_CLineValue
*Description：获取当前基线高度值 0 - 1023
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月22日18:19:12
***************************************************************************************************/
unsigned short GetGB_CLineValue(void)
{
	return GB_CLineValue;
}

/***************************************************************************************************
*FunctionName：SetGB_CLineValue
*Description：设置基线高度 0-1023， 最高1023可使基线输出5V
*Input：value -- 共12位有效位
*Output：None
*Author：xsx
*Data：2016年4月22日18:18:55
***************************************************************************************************/
void SetGB_CLineValue(unsigned short value)
{
	unsigned short temp = value;
	
	DA_CS_H();
	delay_us(10);
	
	DA_CS_L();
	delay_us(50);
	
	GB_CLineValue = value;
	
	//低2位为0
	temp <<= 2;					
	
	/*更新DAC B值*/
	temp |= ((unsigned short)1<<(15));
	temp &= ~((unsigned short)1<<(12));
	
	/*快速模式*/
	temp &= ~((unsigned short)1<<(14));
	
	/*正常模式*/
	temp &= ~((unsigned short)1<<(13));

	DA_Write(temp);
	
	delay_us(50);
	DA_CS_H();
}
