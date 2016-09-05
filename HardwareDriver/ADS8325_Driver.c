/***************************************************************************************************
*FileName：Ads8325_Driver
*Description：ADC芯片ADS8325驱动
*Author：xsx
*Data：2016年3月18日14:11:02
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/

#include	"Ads8325_Driver.h"

#include	"Delay.h"

/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName：ADS_IO_Init
*Description：AD芯片端口初始化
*Input：None
*Output：None
*Author：xsx
*Data：2016年3月18日14:37:44
***************************************************************************************************/
void ADS_IO_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;

	/* Enable SMBUS_PORT clocks */
	RCC_AHB1PeriphClockCmd(ADS_Rcc, ENABLE);

    /*配置SMBUS_SCK、SMBUS_SDA为集电极开漏输出*/
	GPIO_InitStructure.GPIO_Pin = ADS_SCK_Pin | ADS_CS_Pin |ADS_SDA_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(ADS_GpioGroup, &GPIO_InitStructure);
	
    ADS_CS_H();
}

/***************************************************************************************************
*FunctionName：TM1623_SDA_IN
*Description：排队功能模块通信SDA脚输入
*Input：None
*Output：None
*Author：xsx
*Data：2016年3月12日14:33:11
***************************************************************************************************/
static void ADS_SDA_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = ADS_SDA_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(ADS_GpioGroup, &GPIO_InitStructure);//初始化
}
/***************************************************************************************************
*FunctionName：TM1623_SDA_OUT
*Description：排队功能模块通信SDA脚输出
*Input：None
*Output：None
*Author：xsx
*Data：2016年3月12日14:33:23
***************************************************************************************************/
static void ADS_SDA_OUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = ADS_SDA_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(ADS_GpioGroup, &GPIO_InitStructure);//初始化
}

/***************************************************************************************************
*FunctionName：ADS8325
*Description：采集光强，mv
*Input：None
*Output：光强的电压值mv
*Author：xsx
*Data：2016年6月27日16:50:52
***************************************************************************************************/
double ADS8325(void)
{
	unsigned short AData=0;
	unsigned char i=16;
	double temp = 0;
	
	ADS_SDA_OUT();
	ADS_CS_H();
	ADS_SCK_H();
	ADS_SDA_H();
	
	ADS_CS_L();
	
	ADS_SDA_IN();
	delay_us(1);
	while(ADS_SDA_PIN())
	{
		ADS_SCK_L();
		delay_us(1);
		
		ADS_SCK_H();
		delay_us(1);
		
		AData++;
		if(AData > 200)
			return 0;
	}
	
	for(i=0; i<16; i++)
	{
		ADS_SCK_L();

		ADS_SCK_H();
		delay_us(1);
		
		AData=AData<<1;
		if(ADS_SDA_PIN())
			AData=AData|0x01;
		
		delay_us(1);
	}
	
	delay_us(10);
	ADS_CS_H();
	
	temp = AData;

	temp /= 65535;
	temp *= 4096;
	
	return temp;
} 


