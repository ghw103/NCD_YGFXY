#ifndef _DEFINE_H_H
#define _DEFINE_H_H

#include	"stm32f4xx.h"
#include	"ff.h"

#define		NormalCode	0											//正常程序
#define		DebugCode	1											//测试程序
#define		TestCode	2											//测试程序
	
#define		CodeType	DebugCode									//程序分类
/***************************************************************************************************/
/***************************************************************************************************/
/*****************************************软件版本**************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/*V1.0.03*/
#define	GB_SoftVersion_1	1
#define	GB_SoftVersion_2	0
#define	GB_SoftVersion_3	3
/**********************************************************************************************************/
/******************************************操作结果变量*************************************************/
/**********************************************************************************************************/
typedef enum
{ 
	My_Pass = 1,		//操作成功
	My_Fail = 0			//操作失败
}MyState_TypeDef;
/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************/
/******************************************fatfs结构体定义*************************************************/
/**********************************************************************************************************/
/*此结构体用于在进行sd操作是申请内存*/
typedef struct
{
	FIL file;
	UINT br;
	UINT bw;
	FRESULT res;
	FSIZE_t size;
}FatfsFileInfo_Def; 

typedef struct
{
	DIR dir;
	FRESULT res;
	FSIZE_t size;
}FatfsDirInfo_Def; 

#pragma pack(1)
typedef struct
{
	unsigned char year;
	unsigned char month;
	unsigned char day;
	unsigned int num;
	unsigned int index;
}TestDateInfo_Def;
#pragma pack()
#define	TestDataDateRepeatNum	10					//同一个日期在日期管理文件中出现的次数
/**********************************************************************************************************/
/**********************************************************************************************************/

#define	PaiDuiWeiNum	9							//排队位置数目
typedef enum
{
	NotStart = 0,									//没开始,卡在卡槽
	statues1 = 1,									//等待从卡槽拔出，阶段1和2共用
	statues2 = 2,									//已从卡槽拔出，等待插入排队位，阶段1和2共用
	statues3 = 3,									//已插入排队位，等待加样（弹出提示框，加样后确定），阶段1
	statues4 = 4,									//已加样，倒计时开始，阶段1
	statues5 = 5,									//时间未到，卡被异常拔出（提示插回），等待插回，阶段1和2共用
	statues6 = 6,									//异常拔出时已插回，阶段1和2共用
	statues7 = 7,									//时间小于60S，切回排队界面，阶段1和2共用
	statues8 = 8,									//时间小于20S，提示将卡放入卡槽，等待从排队拔出状态，阶段1和2共用
	statues9 = 9,									//等待插入卡槽状态，阶段1和2共用
	statues10 = 10,									//等待插入卡槽状态，阶段1和2共用
	statues11 = 11,									//等待插入卡槽状态，阶段1和2共用
	statues12 = 12,									//等待插入卡槽状态，阶段1和2共用
	statues13 = 13,									//等待插入卡槽状态，阶段1和2共用
	statuesNull = 0xff,
}MyPaiDuiStatues;


/**********************************************************************************************************/
/******************************************时间结构体定义**************************************************/
/**********************************************************************************************************/
#pragma pack(1)
typedef struct
{
	unsigned char year;
	unsigned char month;
	unsigned char day;
	unsigned char hour;
	unsigned char min;
	unsigned char sec;
}MyTime_Def;
#pragma pack()
/**********************************************************************************************************/
/**********************************************************************************************************/



/**********************************************************************************************************/
/******************************************温度结构体定义**************************************************/
/**********************************************************************************************************/
#pragma pack(1)
typedef struct
{
	float  E_Temperature;			//环境温度
	float	O_Temperature;			//被测物体温度
	float I_Temperature;			//内部温度
}MyTemp_Def;
#pragma pack()
/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************/
/******************************************二维码相关定义**************************************************/
/**********************************************************************************************************/

#define	ScanOutTime						10			//????s
#define	CodeMaxLen						300			//???????
#define	ItemNameLen						20			//????????
#define	ItemMeasureLen					10			//????????
#define	CardPiCiHaoLen					30			//????????


/*???????*/
typedef enum
{
	CardCodeScanning = 0,												//正在扫描
	CardCodeScanOK = 1,												//扫描二维码成功
	CardCodeScanFail = 2,											//扫描失败
	CardCodeTimeOut = 3,											//过期
}ScanCodeResult;

#pragma pack(1)
typedef struct CardInfo_Tag
{
	char	ItemName[ItemNameLen];						//????,??10???
	unsigned char	TestType;							//??????
	float	NormalResult;								//????
	float	LowstResult;								//????????
	float	HighestResult;								//????????
	char	ItemMeasure[ItemMeasureLen];				//????	
	unsigned short ItemLocation;						//T???
	unsigned char ItemBiaoQuNum;						//????????????,???2?
	float	ItemFenDuan;			//?????????2????????,3??2?
	float	ItemBiaoQu[2][3];			//?????????
			//2 --- ?2???
			//3 --- a , b, c  (????)
	unsigned char CardWaitTime;										//???????
	unsigned short CLineLocation;									//c???
	char	CardPiCi[CardPiCiHaoLen];								//?????
	RTC_DateTypeDef	CardBaoZhiQi;										//???
	unsigned short CRC16;												//????CRC
}CardCodeInfo;
#pragma pack()


#define	MaxLocation			7000						//最大行程
#define	StartTestLocation	700							//测试起始位置
#define	EndTestLocation		5000						//测试结束为止
#define	WaittingCardLocation	6500					//等待插卡的位置
#define	AvregeNum		10															//??????????,??????????
#define	MaxPointLen		(EndTestLocation - StartTestLocation)/AvregeNum				//?????


/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************/
/******************************************样品ID相关定义**************************************************/
/**********************************************************************************************************/
#define	MaxSampleIDLen	20											//最大支持样品id长度为20
/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************/
/******************************************界面相关定义****************************************************/
/**********************************************************************************************************/

typedef struct LineInfo_Tag
{
	unsigned short MaxData;				//实时曲线最大的数值
	unsigned short Y_Scale;				//曲线y轴最低的刻度
	unsigned short MUL_Y;				//Y轴放大倍数
}LineInfo;

typedef struct SysPage_Tag
{
	unsigned char (*CurrentPage)(void * pram);
	void (*LCDInput)(unsigned char *pbuf , unsigned short len);
	void (*PageUpDate)(void);
	MyState_TypeDef (*PageInit)(void * pram);
	MyState_TypeDef (*PageBufferMalloc)(void);
	MyState_TypeDef (*PageBufferFree)(void);
	
	unsigned char (*ParentPage)(void *  parm);
	unsigned char (*ChildPage)(void *  parm);
}SysPage;

#define	DataNumInPage		8							//一页8个数据
/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************/
/******************************************屏幕相关定义****************************************************/
/**********************************************************************************************************/
#define	LCD_Head_1		0xAA											//协议头1
#define	LCD_Head_2		0xBB											//协议头2

#define W_REGSITER 		0x80          									//写入数据,无应答          
#define R_REGSITER 	0x81          									//读取数据，有应答
#define W_ADDRESS  		0x82											//写变量寄存器，无应答
#define R_ADDRESS  	0x83											//读变量寄存器数据，有应答
#define W_LINE     		0x84          									//写曲线缓冲区，无应答

#pragma pack(1)
typedef struct BasicICO_Tag
{
	unsigned short X;
	unsigned short Y;
	unsigned short ICO_ID;
}Basic_ICO;
#pragma pack()
/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************/
/******************************************操作人相关定义****************************************************/
/**********************************************************************************************************/
#define	MaxUserNum	20													//最多保存的用户数目
#define	MaxPageShowNum	10												//一页显示用户数目

#define	MaxNameLen	11
#define	MaxSexLen	5
#define	MaxAgeLen	7
#define	MaxPhoneLen	17
#define	MaxJobLen	33
#define	MaxDescLen	65

#pragma pack(1)
typedef struct UserTag
{ 
	char user_name[MaxNameLen];
	char user_sex[MaxSexLen];
	char user_age[MaxAgeLen];
	char user_phone[MaxPhoneLen];
	char user_job[MaxJobLen];
	char user_desc[MaxDescLen];
	unsigned short crc;
}User_Type;
#pragma pack() 
/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************/
/******************************************设备信息相关定义****************************************************/
/**********************************************************************************************************/
#define	MaxDeviceIDLen	21
#define	MaxDeviceNameLen	51
#define	MaxDeviceMakerLen	51
#define	MaxDeviceMakerPhoneLen	21
#define	MaxDeviceUnitLen	51

#pragma pack(1)
typedef struct DeviceInfo_Tag
{
	char isfresh;																	//是否有更新，如果是1，则上传数据
	char deviceid[MaxDeviceIDLen];													//设备id
	char devicename[MaxDeviceNameLen];												//设备名称
	char devicemaker[MaxDeviceMakerLen];												//设备制造商
	char devicemakerphone[MaxDeviceMakerPhoneLen];											//设备制造商电话
	char deviceunit[MaxDeviceUnitLen];												//设备使用单位
	User_Type deviceuser;												//设备使用人
	unsigned short crc;
}DeviceInfo;
#pragma pack() 
/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************/
/******************************************自检功能相关定义****************************************************/
/**********************************************************************************************************/

typedef enum
{ 
	ReadServerData = 0,												//加载服务器数据
	ReadServerError = 1,												//服务器数据失败
	ReadServerSuccess = 2,												//服务器数据成功
	
	ErWeiMaChecking = 3,												//正在检查二维码
	ErWeiMaError = 4,													//二维码错误
	ErWeiMaSuccess = 5,													//二维码正常
	
	DataBasesChecking = 6,												//正在检查存储模块
	DataBasesError = 7,													//存储模块错误
	DataBasesSuccess = 8,												//存储模块正常
	
	WIFIChecking = 9,													//正在检查WIFI
	WIFIError = 10,														//WIFI错误
	WIFISuccess = 11,													//WIFI正常
	
	ADDAChecking = 12,													//正在检查采集模块
	ADDAError = 13,														//采集模块错误
	ADDASuccess = 14,													//采集模块正常
	
	MotorChecking = 15,													//正在检查采集模块
	MotorError = 16,														//采集模块错误
	MotorSuccess = 17,													//采集模块正常
	
	
	
	SelfCheckOver = 18													//自检完成
}SelfCheck_TypeDef;

typedef enum
{ 
	SelfCheck_NoResult = 0,														//自检未完成
	SelfCheck_OK = 1,													//自检正常
	SelfCheck_Error = 2,												//自检有不可忽略的错误
	SelfCheck_Alam = 3,													//自检有告警项
}SelfCheckResult_TypeDef;
/**********************************************************************************************************/
/**********************************************************************************************************/


/**********************************************************************************************************/
/******************************************网络相关定义****************************************************/
/**********************************************************************************************************/
#pragma pack(1)
typedef struct ip_Tag
{
	unsigned char ip_1;
	unsigned char ip_2;
	unsigned char ip_3;
	unsigned char ip_4;
}IP_Def;
#pragma pack()

#define	MaxWifiListNum	20												//最多显示20个热点
#define	PageWifiNum		10
#define	MaxSSIDLen		50
#define	MaxKEYLen		20

/*wifi热点*/
#pragma pack(1)
typedef struct wifi_Tag
{
	char ssid[MaxSSIDLen];														//SSID
	char auth[20];														//认证模式
	char encry[10];														//加密方式
	unsigned char indicator;											//信号强度
	char key[MaxKEYLen];														//密码
	unsigned short crc;
}WIFI_Def;
#pragma pack()

/*网卡定义*/
typedef enum
{
	NetNone = 0,														//无网络
	Line_Mode = 1,														//有线网
	Wifi_Mode = 2														//wiif
}NetCard_Type;

typedef struct mynetbuf_tag
{
	void * data;
	unsigned short datalen;
}mynetbuf;

typedef enum
{ 
	Link_Down = 0,
	Link_Up = 1
}LinkState_TypeDef;

/*ip获取方式*/
typedef enum
{
	DHCP_Mode = 1,														//自动获取动态ip
	User_Mode = 2														//用户设置ip
}NetIP_Type;


#pragma pack(1)
typedef struct NetData_Tag
{
	IP_Def myip;														//我的ip
	IP_Def serverip;													//服务器ip
	NetIP_Type ipmode;
	unsigned short crc;
}NetData;
#pragma pack()

#define	DefaultNetCard		Line_Mode									//默认网卡
#define	DefaultIPMode		DHCP_Mode									//ip获取方式，针对有线网

/*超级病例服务器IP*/
#define	NCD_ServerIP_1		123
#define	NCD_ServerIP_2		57
#define	NCD_ServerIP_3		94
#define	NCD_ServerIP_4 		39
#define	NCD_ServerPort 		8080

/*客户服务器*/
#define	User_ServerIP_1		192
#define	User_ServerIP_2		168
#define	User_ServerIP_3		1
#define	User_ServerIP_4		100
#define	User_ServerPort		9600
/**********************************************************************************************************/
/**********************************************************************************************************/

#define	MaxTestDataSaveNum	(unsigned int)1000000

#endif

