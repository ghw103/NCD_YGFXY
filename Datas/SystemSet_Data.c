/***************************************************************************************************
*FileName:	SystemSet_Data
*Description:	系统设置参数
*Author: xsx_kair
*Data:	2016年12月16日16:20:43
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"SystemSet_Data.h"

#include	"CRC16.h"

#include	<string.h>
#include	"stdio.h"
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static SystemSetData GB_SystemSetData;								//系统参数
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/****************************************File Start*************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName: setDefaultSetData
*Description: 初始化为默认配置
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月19日09:17:30
***************************************************************************************************/
void setDefaultSetData(SystemSetData * systemSetData)
{
	if(systemSetData)
	{
		systemSetData->isAutoPrint = false;
		systemSetData->isMute = false;
		systemSetData->ledLightIntensity = 100;
		systemSetData->ledSleepTime = 60;
		
		sprintf(systemSetData->deviceInfo.deviceid, "ncd-device");
		systemSetData->deviceInfo.crc = CalModbusCRC16Fun1(&(systemSetData->deviceInfo), sizeof(DeviceInfo) - 2);
		
		systemSetData->netSet.ipmode = DHCP_Mode;
		systemSetData->netSet.crc = CalModbusCRC16Fun1(&(systemSetData->netSet), sizeof(NetSet) - 2);
		
		systemSetData->crc = CalModbusCRC16Fun1(systemSetData, sizeof(SystemSetData) - 2);
	}
}

/***************************************************************************************************
*FunctionName: getSystemSetData, setSystemSetData
*Description: 读写系统参数信息，写入的数据源只能是SD卡中保存的数据
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月16日16:50:13
***************************************************************************************************/
void getSystemSetData(SystemSetData * systemSetData)
{
	if(systemSetData)
	{
		//如果crc错误。恢复默认值
		if(GB_SystemSetData.crc != CalModbusCRC16Fun1(&GB_SystemSetData, sizeof(SystemSetData) - 2))
			setDefaultSetData(&GB_SystemSetData);
		
		memcpy(systemSetData, &GB_SystemSetData, sizeof(SystemSetData));
	}
}

void setSystemSetData(SystemSetData * systemSetData)
{
	if(systemSetData)
	{
		//对读取的数据进行校验
		if(systemSetData->ledLightIntensity > 100)
			systemSetData->ledLightIntensity = 100;
		else if(systemSetData->ledLightIntensity < 10)
			systemSetData->ledLightIntensity = 10;
		
		//0xffff表示不休眠
		if(systemSetData->ledSleepTime != 0xffff)
		{
			if(systemSetData->ledSleepTime > 600)
				systemSetData->ledSleepTime = 100;
			else if(systemSetData->ledSleepTime < 10)
				systemSetData->ledSleepTime = 10;
		}
		
		systemSetData->crc = CalModbusCRC16Fun1(systemSetData, sizeof(SystemSetData) - 2);
		
		memcpy(&GB_SystemSetData, systemSetData, sizeof(SystemSetData));
	}
}

/***************************************************************************************************
*FunctionName: getdeviceInfo
*Description: 获取设备信息
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月16日16:38:22
***************************************************************************************************/
void getDeviceInfo(DeviceInfo * deviceinfo)
{
	if(deviceinfo)
	{
		//如果crc错误。恢复默认值
		if(GB_SystemSetData.crc != CalModbusCRC16Fun1(&GB_SystemSetData, sizeof(SystemSetData) - 2))
			setDefaultSetData(&GB_SystemSetData);
		
		memcpy(deviceinfo, &(GB_SystemSetData.deviceInfo), sizeof(DeviceInfo));

	}
}

/***************************************************************************************************
*FunctionName: deviceInfoIsNew
*Description: 设备信息是否有更新
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月16日16:59:16
***************************************************************************************************/
bool deviceInfoIsNew(void)
{
	if(GB_SystemSetData.crc == CalModbusCRC16Fun1(&GB_SystemSetData, sizeof(SystemSetData) - 2))
		return GB_SystemSetData.deviceInfo.isnew;
	else
		return false;
}

/***************************************************************************************************
*FunctionName: getNetSet
*Description: 获取网络设置
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月16日16:39:18
***************************************************************************************************/
void getNetSet(NetSet * netSet)
{
	if(netSet)
	{
		//如果crc错误。恢复默认值
		if(GB_SystemSetData.crc != CalModbusCRC16Fun1(&GB_SystemSetData, sizeof(SystemSetData) - 2))
			setDefaultSetData(&GB_SystemSetData);
		
		memcpy(netSet, &(GB_SystemSetData.netSet), sizeof(NetSet));
	}
}

/***************************************************************************************************
*FunctionName: isAutoPrint
*Description: 是否自动打印
*Input: 
*Output: 
*Return: 如果crc错误，则恢复默认值：不自动打印
*Author: xsx
*Date: 2016年12月16日16:43:04
***************************************************************************************************/
bool isAutoPrint(void)
{
	if(GB_SystemSetData.crc == CalModbusCRC16Fun1(&GB_SystemSetData, sizeof(SystemSetData) - 2))
		return GB_SystemSetData.isAutoPrint;
	else
		return false;
}

/***************************************************************************************************
*FunctionName: isMute
*Description: 是否静音，如果crc错误，则恢复默认值：不静音
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月16日16:43:47
***************************************************************************************************/
bool isMute(void)
{
	if(GB_SystemSetData.crc == CalModbusCRC16Fun1(&GB_SystemSetData, sizeof(SystemSetData) - 2))
		return GB_SystemSetData.isMute;
	else
		return false;
}

/***************************************************************************************************
*FunctionName: getLedSleepTime
*Description: 读取屏幕休眠时间，如果crc错误，则恢复默认1分钟，0代表不休眠
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月16日16:45:46
***************************************************************************************************/
unsigned short getLedSleepTime(void)
{
	if(GB_SystemSetData.crc == CalModbusCRC16Fun1(&GB_SystemSetData, sizeof(SystemSetData) - 2))
		return GB_SystemSetData.ledSleepTime;
	else
		return 60;
}

/***************************************************************************************************
*FunctionName: getLedLightIntensity
*Description: 读取led亮度，如果crc错误，则恢复默认：100%
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月16日16:47:11
***************************************************************************************************/
unsigned char getLedLightIntensity(void)
{
	if(GB_SystemSetData.crc == CalModbusCRC16Fun1(&GB_SystemSetData, sizeof(SystemSetData) - 2))
		return GB_SystemSetData.ledLightIntensity;
	else
		return 100;
}
/****************************************end of file************************************************/
