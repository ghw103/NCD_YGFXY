/***************************************************************************************************
*FileName:IAP_Fun
*Description:IAP功能
*Author:xsx
*Data:2017年2月16日16:27:25
***************************************************************************************************/


/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"IAP_Fun.h"

#include	"Md5FileDao.h"
#include	"AppFileDao.h"

#include	"MyMem.h"
#include	"Md5.h"
#include	"Define.h"
#include	"MyTools.h"
#include	"Delay.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"

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
*FunctionName: checkMd5
*Description: 计算并对比MD5
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年2月20日14:02:35
***************************************************************************************************/
MyState_TypeDef checkMd5(void)
{
	char originMd5[40];				//原始MD5
	char currentMd5[40];				//当前MD5
	
	//读取文件中的md5
	memset(originMd5, 0, 40);
	if(My_Fail == ReadMd5File(originMd5))
		return My_Fail;
	
	//计算更新固件的md5值
	memset(currentMd5, 0, 40);
	md5sum(currentMd5);
	
	//对比MD5
	if(true == CheckStrIsSame(originMd5, currentMd5, 32))
		return My_Pass;
	else
		return My_Fail;
}

/***************************************************************************************************
*FunctionName: checkNewFirmwareIsSuccessDownload
*Description: 检查是否成功下载新固件
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年2月21日09:02:28
***************************************************************************************************/
MyState_TypeDef checkNewFirmwareIsSuccessDownload(void)
{
	//检查是否有新程序
	if(My_Pass == checkNewAppFileIsExist())
	{
		//如果有新固件，对比MD5
		if(My_Pass == checkMd5())
			return My_Pass;	
	}
	
	return My_Fail;
}


