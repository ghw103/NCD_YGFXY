/***************************************************************************************************
*FileName:DeviceInfo_Data
*Description:设备信息
*Author:xsx
*Data:2016年4月29日15:39:04
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"DeviceInfo_Data.h"
#include	"malloc.h"
#include	"user_fatfs.h"

#include	"TcpClientFunction.h"
#include	"QueueUnits.h"

#include	"cJSON.h"

#include 	"FreeRTOS.h"
#include 	"task.h"

#include	"string.h"
#include 	"stdlib.h"
/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/
const char deviceinfoitem[11][10] = 
{
	"设备id\0",
	"设备名称\0",
	"制造商\0",
	"售后电话\0",
	"使用单位\0",
	"责任人\0",
	"年龄\0",
	"性别\0",
	"联系方式\0",
	"职位\0",
	"备注\0",
};
const unsigned char itemlen[11] = {20,50,50,50,50,10,10,10,20,20,40};
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
*FunctionName：GetDeviceInfoStr
*Description：获取默认的设备信息数据，以字符串的形式
*Input：None
*Output：None
*Author：xsx
*Data：2016年5月6日09:51:28
***************************************************************************************************/
MyState_TypeDef GetDeviceInfoStr(FIL * file, const char * filepath,  char * filestr)
{
	FatfsFileInfo_Def * myfile1 = NULL;
	MyState_TypeDef statues = My_Fail;
	
	FIL * targetfile = NULL;
	
	myfile1 = mymalloc(sizeof(FatfsFileInfo_Def));
	if(myfile1 == NULL)
		return statues;
		
	mymemset(myfile1, 0, sizeof(FatfsFileInfo_Def));
		
	if(file != NULL)
	{
		targetfile = file;
	}
	else if(filepath != NULL)
	{
		
		/*读取设备信息*/
		myfile1->res = f_open(&myfile1->file, filepath, FA_READ);
		
		if(FR_OK == myfile1->res)
		{
			targetfile = &myfile1->file;
		}

	}
	
	if(targetfile != NULL)
	{
		myfile1->size = f_size(targetfile);
		myfile1->res = f_read(targetfile, filestr, myfile1->size, &myfile1->br);
		
		if(FR_OK == myfile1->res)
			statues = My_Pass;
	}
	
		
	if(file != NULL)
		;
	else if(filepath != NULL)
		f_close(targetfile);
	
	myfree(myfile1);
	
	return statues;
}

/***************************************************************************************************
*FunctionName：TanslateFilestrToDeviceStruct
*Description：将从文件中读到的数据解析成设备信息结构体
*Input：None
*Output：None
*Author：xsx
*Data：2016年5月6日10:14:39
***************************************************************************************************/
MyState_TypeDef TanslateFilestrToDeviceStruct(DeviceInfo * temp, char *filestr)
{
	char *out;
	cJSON *json;
	unsigned char i=0;
	unsigned char nCount = 0;
	MyState_TypeDef statues = My_Fail;
	
	unsigned char templen = 0;
	char * item = NULL;
	
	json = cJSON_Parse(filestr);
	if (json)
	{
		cJSON *pArrayItem = NULL;
		
		nCount = cJSON_GetArraySize ( json );
		
		for(i=0; i<nCount; i++)
		{

			switch(i)
			{
				case 0: item = temp->deviceid; break;
				case 1: item = temp->devicename; break;
				case 2: item = temp->devicemaker; break;
				case 3: item = temp->devicemakerphone; break;
				case 4: item = temp->deviceunit; break;
				case 5: item = temp->deviceuser.user_name; break;
				case 6: item = temp->deviceuser.user_age; break;
				case 7: item = temp->deviceuser.user_sex; break;
				case 8: item = temp->deviceuser.user_phone; break;
				case 9: item = temp->deviceuser.user_job; break;
				case 10: item = temp->deviceuser.user_desc; break;
				default : break;
			}
			
			pArrayItem = cJSON_GetObjectItem(json, deviceinfoitem[i]);
            out = cJSON_Print( pArrayItem );
			
			templen = strlen(out);
			if(templen > 2)
			{
				mymemcpy(item, out+1, templen-2);
			}
			
			myfree( out ); 
		}
		
		statues = My_Pass;
		
		cJSON_Delete(json);
	}
	
	return statues;
}

/***************************************************************************************************
*FunctionName：TanslateFilestrToDeviceStruct
*Description：从文件解析成设备信息结构体
*Input：None
*Output：None
*Author：xsx
*Data：2016年5月6日10:14:39
***************************************************************************************************/
MyState_TypeDef TanslateFileToDeviceStruct(DeviceInfo * temp, FIL * file, const char * filepath)
{
	char *out;
	cJSON *json;
	unsigned char i=0;
	unsigned char nCount = 0;
	MyState_TypeDef statues = My_Fail;
	
	unsigned char templen = 0;
	char * item = NULL;
	
	char *buf = NULL;

	buf = mymalloc(1024);
		
	if(buf == NULL)
		return statues;

	mymemset(buf, 0, 1024);
	
	if(My_Pass != GetDeviceInfoStr(file, filepath, buf))
		goto END;
	
	json = cJSON_Parse(buf);
	if (json)
	{
		cJSON *pArrayItem = NULL;
		
		nCount = cJSON_GetArraySize ( json );
		
		for(i=0; i<nCount; i++)
		{

			switch(i)
			{
				case 0: item = temp->deviceid; break;
				case 1: item = temp->devicename; break;
				case 2: item = temp->devicemaker; break;
				case 3: item = temp->devicemakerphone; break;
				case 4: item = temp->deviceunit; break;
				case 5: item = temp->deviceuser.user_name; break;
				case 6: item = temp->deviceuser.user_age; break;
				case 7: item = temp->deviceuser.user_sex; break;
				case 8: item = temp->deviceuser.user_phone; break;
				case 9: item = temp->deviceuser.user_job; break;
				case 10: item = temp->deviceuser.user_desc; break;
				default : break;
			}
			
			pArrayItem = cJSON_GetObjectItem(json, deviceinfoitem[i]);
            out = cJSON_Print( pArrayItem );
			
			templen = strlen(out);
			if(templen > 2)
			{
				mymemcpy(item, out+1, templen-2);
			}
			
			myfree( out ); 
		}
		
		statues = My_Pass;
		
		cJSON_Delete(json);
	}
	
	END:
	myfree(buf);
	return statues;
}

/***************************************************************************************************
*FunctionName：TanslateDeviceStructToFilestr
*Description：将设备信息结构体转化成cjson字符串
*Input：None
*Output：None
*Author：xsx
*Data：2016年5月6日10:14:39
***************************************************************************************************/
MyState_TypeDef TanslateDeviceStructToFilestr(DeviceInfo * temp, char *filestr)
{
	cJSON *json, *fld;
	unsigned char i=0;
	char * item = NULL;
	char *out = NULL;
	MyState_TypeDef statues = My_Fail;
	
	json = cJSON_CreateObject();
	if(json)
	{
		for(i=0; i<11; i++)
		{
			switch(i)
			{
				case 0: item = temp->deviceid; break;
				case 1: item = temp->devicename; break;
				case 2: item = temp->devicemaker; break;
				case 3: item = temp->devicemakerphone; break;
				case 4: item = temp->deviceunit; break;
				case 5: item = temp->deviceuser.user_name; break;
				case 6: item = temp->deviceuser.user_age; break;
				case 7: item = temp->deviceuser.user_sex; break;
				case 8: item = temp->deviceuser.user_phone; break;
				case 9: item = temp->deviceuser.user_job; break;
				case 10: item = temp->deviceuser.user_desc; break;
				default : break;
			}
			
			fld = cJSON_CreateString(item);
			
			if(fld)
			{
				cJSON_AddItemToObject(json, deviceinfoitem[i], fld);
			}
			else
			{
				cJSON_Delete(json);
				goto END;
			}

		}
		
		out = cJSON_Print(json);
		mymemcpy(filestr, out, strlen(out));
		
		cJSON_Delete(json);
		
		statues = My_Pass;
	}
	
	END:
		
		return statues;
}


/***************************************************************************************************
*FunctionName：GetDeviceInfoData
*Description：获取设备信息,由于设备信息量比较大，所以只在需要时读取
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月29日15:46:20
***************************************************************************************************/
MyState_TypeDef GetDeviceInfoData(DeviceInfo * temp)
{
	FatfsFileInfo_Def * myfile1 = NULL;
	MyState_TypeDef statues = My_Fail;
	
	myfile1 = mymalloc(sizeof(FatfsFileInfo_Def));
	if(myfile1 == NULL)
		return statues;
	
	mymemset(myfile1, 0, sizeof(FatfsFileInfo_Def));
	
	/*读取设备信息*/
	myfile1->res = f_open(&myfile1->file, "0:/MyDeviceInfo.txt", FA_READ);
	
	/*文件不存在，则使用默认设备信息新建*/
	if(myfile1->res == FR_NO_FILE)
	{	
		char *buf = NULL;

		buf = mymalloc(1024);
		
		if(buf == NULL)
			goto END;

		mymemset(buf, 0, 1024);
		
		if(FR_OK == f_open(&myfile1->file, "0:/MyDeviceInfo.txt", FA_CREATE_ALWAYS | FA_WRITE | FA_READ))
		{
			if(My_Pass == GetDeviceInfoStr(NULL, "0:/DefaultFile/MyDeviceInfo.txt", buf))
			{
				if(FR_OK == f_write(&myfile1->file, buf, strlen(buf), &myfile1->bw))
				{
					if(My_Pass == TanslateFilestrToDeviceStruct(temp, buf))
						statues = My_Pass;
				}
			}
				
			f_close(&myfile1->file);
		}
		
		myfree(buf);

	}
	else if(myfile1->res == FR_OK)
	{
		if(My_Pass == TanslateFileToDeviceStruct(temp, &myfile1->file, NULL))
			statues = My_Pass;

		
		f_close(&myfile1->file);
	}
	
	END:
		myfree(myfile1);
		
	return statues;
}

MyState_TypeDef UpDataDeviceInfo(DeviceInfo * temp)
{
	FatfsFileInfo_Def * myfile1 = NULL;
	MyState_TypeDef statues = My_Fail;
	
	myfile1 = mymalloc(sizeof(FatfsFileInfo_Def));
	if(myfile1 == NULL)
		return statues;
	
	mymemset(myfile1, 0, sizeof(FatfsFileInfo_Def));
	
	/*读取设备信息*/
	myfile1->res = f_open(&myfile1->file, "0:/MyDeviceInfo.txt", FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
	
	if(FR_OK == myfile1->res)
	{
		char *buf = NULL;
		
		buf = mymalloc(1024);
		
		if(buf == NULL)
			goto END;
		
		mymemset(buf, 0, 1024);
		TanslateDeviceStructToFilestr(temp, buf);
		
		f_write(&myfile1->file, buf, strlen(buf), &myfile1->bw);
		
		myfree(buf);
		
		f_close(&myfile1->file);
		
		statues = My_Pass;
	}
	
	END:
		myfree(myfile1);
		return statues;
}
