/***************************************************************************************************
*FileName：LCD_Driver
*Description：液晶屏的一些功能函数
*Author：xsx
*Data：2016年4月29日10:13:01
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/

#include	"LCD_Driver.h"
#include 	"Usart6_Driver.h"

#include	"CRC16.h"
#include	"Define.h"
#include	"MyMem.h"

#include	"stdio.h"
#include	"string.h"

/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/
static char *txdat = NULL;
static unsigned char tempbuf[100];
/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/
static void WriteLCDRegister(unsigned char reg, void *data, unsigned char len);
static void ReadLCDRegister(unsigned char reg, unsigned char len);
static void WriteLCDData(unsigned short addr, void *data, unsigned char len);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************
*FunctionName: WriteRegister
*Description: 写寄存器
*Input: reg -- 寄存器地址
*		data -- 写入数据
*		datalen -- 写入数据的长度
*Output: 无
*Author: xsx
*Date: 2016年8月5日15:18:01
***************************************************************************************************/
static void WriteLCDRegister(unsigned char reg, void *data, unsigned char len)
{			
	char *q = NULL;
	unsigned char *p = (unsigned char *)data;
	unsigned char i=0;
	
	txdat = MyMalloc(len + 10);
	if(txdat == NULL)
		return;
	
	memset(txdat, 0, len + 10);
	q = txdat;
	
	*q++ = LCD_Head_1;
	*q++ = LCD_Head_2;
	*q++ = len+4;
	
	*q++ = W_REGSITER;
	
	*q++ = reg;

	for(i=0; i<len; i++)
		*q++ = *p++;
	
	CalModbusCRC16Fun2(txdat+3, len + 2, q);
	
	USART6_SentData(txdat, txdat[2]+3);

	MyFree(txdat);
}

/***************************************************************************************************
*FunctionName: ReadLCDRegister
*Description: 读取屏幕寄存器值
*Input: reg -- 寄存器地址
*		len -- 读取长度
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月12日11:01:48
***************************************************************************************************/
static void ReadLCDRegister(unsigned char reg, unsigned char len)
{			
	char *q = NULL;
	
	txdat = MyMalloc(16);
	if(txdat == NULL)
		return;
	
	memset(txdat, 0, 16);
	q = txdat;
	
	*q++ = LCD_Head_1;
	*q++ = LCD_Head_2;
	*q++ = 1 + 4;
	
	*q++ = R_REGSITER;
	
	*q++ = reg;

	*q++ = len;
	
	CalModbusCRC16Fun2(txdat+3, 1 + 2, q);
	
	USART6_SentData(txdat, txdat[2]+3);
	
	MyFree(txdat);
}

static void WriteLCDData(unsigned short addr, void *data, unsigned char len)
{
	char *q = NULL;
	unsigned char *p = (unsigned char *)data;
	unsigned char i=0;
	
	txdat = MyMalloc(len + 10);
	if(txdat == NULL)
		return;
	
	memset(txdat, 0, len + 10);
	q = txdat;
	
	*q++ = LCD_Head_1;
	*q++ = LCD_Head_2;
	*q++ = len+5;

	*q++ = W_ADDRESS;
	
	*q++ = addr>>8;
	*q++ = addr;

	for(i=0; i<len; i++)
		*q++ = *p++;
	
	CalModbusCRC16Fun2(txdat+3, len + 3, q);
	
	USART6_SentData(txdat, txdat[2]+3);

	MyFree(txdat);
}

/***************************************************************************************************
*FunctionName：SelectPage
*Description：选择显示的图片id
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月30日17:02:39
***************************************************************************************************/
void SelectPage(unsigned short index)
{
	tempbuf[0] = index >> 8;
	tempbuf[1] = index;
	WriteLCDRegister(0x03, tempbuf, 2);
}

/***************************************************************************************************
*FunctionName: ReadCurrentPageId
*Description: 读取当前页面id
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月12日11:02:58
***************************************************************************************************/
void ReadCurrentPageId(void)
{
	ReadLCDRegister(0x03, 2);
}

/***************************************************************************************************
*FunctionName: ReadLcdSoftVersion
*Description: 读取屏幕的软件版本
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月19日14:40:12
***************************************************************************************************/
void ReadLcdSoftVersion(void)
{
	ReadLCDRegister(0x00, 1);
}

/***************************************************************************************************
*FunctionName：SendKeyCode
*Description：键控功能
*Input：None
*Output：None
*Author：xsx
*Data：2016年5月1日16:46:15
***************************************************************************************************/
void SendKeyCode(unsigned char keycode)
{
	WriteLCDRegister(0x4f, &keycode, 1);
}

/************************************************************************
** 函数名:SetLEDLight
** 功  能:num -- 屏幕亮度，1 -- 1%， 100--100%
** 输  入:无
** 输  出:无
** 返  回：无
** 备  注：无
** 时  间:  
** 作  者：xsx                                                 
************************************************************************/
void SetLEDLight(unsigned char num)
{
	WriteLCDRegister(0x01, &num, 1);
}
/***************************************************************************************************
*FunctionName：DisText
*Description：显示文本
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月30日17:02:53
***************************************************************************************************/
void DisText(unsigned short addr, void *data, unsigned short len)
{				
	WriteLCDData(addr, data, len);
}

void ClearText(unsigned short addr, unsigned short len)
{	
	memset(tempbuf, 0xff, len);
	DisText(addr, tempbuf, len);
}

void WriteRadioData(unsigned short addr, void *data, unsigned short len)
{
	WriteLCDData(addr, data, len);
}

void BasicUI(unsigned short addr , unsigned short cmd, unsigned short datapacknum, void *data , unsigned short len)
{
	unsigned char *p = (unsigned char *)data;
	unsigned char i=0;

	//基本图形指令
	tempbuf[0] = cmd>>8;		tempbuf[1] = cmd;
	//基本图形数据包个数
	tempbuf[2] = datapacknum>>8;		tempbuf[3] = datapacknum;
	
	//由于keil是低字节在前，而发生给屏的数据需要高字节在前，所以在这里将高低互换
	for(i=0; i<len/2; i++)
	{
		tempbuf[4+i*2] = p[i*2+1];
		tempbuf[5+i*2] = p[i*2];
	}
	
	WriteLCDData(addr, tempbuf, len+4);
}

/***************************************************************************************************
*FunctionName：BasicPic
*Description：基本图形
*Input：addr -- 控件地址   ， datanum -- 数据包个数  ， soureid -- 图片源页面id，  
*Output：None
*Author：xsx
*Data：
***************************************************************************************************/
void BasicPic(unsigned short addr,unsigned short datanum, unsigned short soureid ,unsigned short xs,unsigned short ys ,unsigned short xe,unsigned short ye ,
	unsigned short txs,unsigned short tys)
{
	//cmd 0x0006
	tempbuf[0] = 0;		tempbuf[1] = 6;
		
	//数据包个数1
	tempbuf[2] = datanum>>8;		tempbuf[3] = datanum;
		
	//原页面id
	tempbuf[4] = soureid>>8;	tempbuf[5] = soureid;
	//原页面x
	tempbuf[6] = xs>>8;	tempbuf[7] = xs;
	//原页面x
	tempbuf[8] = ys>>8;	tempbuf[9] = ys;
	//原页面x
	tempbuf[10] = xe>>8;	tempbuf[11] = xe;
	//原页面x
	tempbuf[12] = ye>>8;	tempbuf[13] = ye;
	//原页面x
	tempbuf[14] = txs>>8;	tempbuf[15] = txs;
	//原页面x
	tempbuf[16] = tys>>8;	tempbuf[17] = tys;

	WriteLCDData(addr, tempbuf, 18);
}



/************************************************************************
** 函数名:GetBufLen
** 功  能:迪文屏返回的字符串以0xff结尾
** 输  入:无
** 输  出:无
** 返  回：无
** 备  注：无
** 时  间:  
** 作  者：xsx                                                 
************************************************************************/
unsigned short GetBufLen(unsigned char *p ,unsigned short len)
{
	unsigned short i ;
	for(i=0; i<len; i++)
			{
				if(p[i] == 0xff)
				{
					return i;
				}
			}
		return 0;
}

void DspNum(unsigned short addr , unsigned int num, unsigned char len)
{
	unsigned char i=0;
	for(i=0; i<len; i++)
	{
		tempbuf[i] = (num >> ((len-i-1)*8));
	}

	WriteLCDData(addr, tempbuf, len);
}

void WriteVarIcoNum(unsigned short addr, unsigned short num)
{
	tempbuf[0] = (unsigned char)(num >> 8);
	tempbuf[1] = (unsigned char)num ;

	WriteLCDData(addr, tempbuf, 2);
}
	


