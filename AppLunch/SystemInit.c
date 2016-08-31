/***************************************************************************************************
*FileName:SystemInit
*Description: 系统所有模块初始化
*Author : xsx
*Data: 2016年4月21日14:18:28
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"SystemInit.h"

#include	"Led_Driver.h"
#include 	"Usart3_Driver.h"
#include 	"Usart2_Driver.h"
#include 	"Usart1_Driver.h"
#include	"SPI1_Driver.h"
#include	"Ads8325_Driver.h"
#include	"DRV8825_Driver.h"
#include	"CodeScanner_Driver.h"
#include	"usbd_cdc_vcp.h"
#include	"user_fatfs.h"
#include 	"MLX90614_Driver.h"
#include	"RTC_Driver.h"
#include 	"Usart6_Driver.h"
#include 	"Iwdg_Driver.h"
#include	"CardLimit_Driver.h"
#include 	"Usart4_Driver.h"
#include	"Wifi_Driver.h"
#include	"Timer4_Driver.h"
#include	"Timer3_Driver.h"
#include	"PT8211_Driver.h"
#include	"TM1623_Driver.h"
#include	"MAX4051_Driver.h"
#include	"TLV5617_Driver.h"
#include	"LEDCheck_Driver.h"

#include	"WifiFunction.h"
#include	"OutModel_Fun.h"
#include	"Net_Data.h"
#include	"System_Data.h"
#include	"Motor_Fun.h"
#include	"SDFunction.h"

#include	"Delay.h"
/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/
extern void SystemInit(void);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName：MySystemInit
*Description：系统初始化
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月21日14:20:44
***************************************************************************************************/
void MySystemBSPInit(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	SystemInit();						//手动系统初始化
	delay_ms(50);
	
	Led_GPIOInit();						//系统指示灯初始化
	delay_ms(50);
	
	Timer4_Init();						//定时器初始化
	Timer3_Init();						//定时器初始化
	delay_ms(50);
	
	Usart3_Init();						//串口3初始化
	delay_ms(50);
	
	Usart2_Init();						//串口2初始化
	delay_ms(50);
	
	Usart4_Init();						//串口4初始化
	delay_ms(50);
	
	Usart6_Init();						//串口6初始化
	delay_ms(50);
	
	Usart1_Init();						//串口2初始化
	delay_ms(50);

	ADS_IO_Init();						//ADC IO初始化
	delay_ms(50);
	
	DA_IO_Init();						//DA io初始化
	delay_ms(50);
	
	Limit_Init();						//限位开关初始化
	delay_ms(50);
	
	DRV_Init();							//电机 IO初始化
	delay_ms(50);
	
	CodeScanner_GPIOInit();				//二维码初始化
	delay_ms(50);
	
	My_USBInit();						//USB端口初始化
	delay_ms(50);
	
	SMBus_Init();						//温度检测初始化
	delay_ms(50);
	
	My_RTC_Init();						//RTC初始化
	delay_ms(50);
	
	Wifi_GPIOInit();					//WIFI端口初始化
	delay_ms(50);
	
	MAX4051_Init();						//采集放大倍数端口初始化
	delay_ms(50);
	
	PT8211_GPIOInit();					//音频端口初始化
	ConfigI2S(16000);
	delay_ms(50);

	TM1623_Init();						//排队模块初始化
	delay_ms(50);
	
	LEDCheck_Init();					//检测发光模块的发光二极管状态初始化
	delay_ms(10);
	
	FatfsInit();						//文件系统初始化
	
	IWDG_Init(3, 2000);					//看门狗初始化,超时时间2S
	delay_ms(50);
}

/***************************************************************************************************
*FunctionName：MySystemModuleInit
*Description：system module init
*Input：None
*Output：None
*Author：xsx
*Data：2016年6月1日14:03:27
***************************************************************************************************/
void MySystemDataInit(void)
{	
	OutModel_Init();					//排队模块数据初始化
	
	CreateNetQueue();					//网络数据初始化
	
	InitMutex();						//wifi数据初始化
	
	InitMotorData();
	
	ReadNetData(GetGB_NetData());		//读取网络设置
	
	ReadDeviceInfo(GetGB_DeviceInfo());	//读取设备信息
}
