/***************************************************************************************************
*FileName:UserS_Data
*Description:操作人数据
*Author:xsx
*Data:2016年5月5日19:51:19
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"Users_Data.h"
#include	"malloc.h"
#include	"user_fatfs.h"
#include	"SDFunction.h"

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

static cJSON * GB_UserCJSON = NULL;
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
*FunctionName：GetUserNum
*Description：获取已有操作人数目
*Input：None
*Output：None
*Author：xsx
*Data：2016年5月10日20:13:08
***************************************************************************************************/
unsigned char GetUsersNum(void)
{
	if(GB_UserCJSON == NULL)
		return 0;

	else
		return cJSON_GetArraySize(GB_UserCJSON);
}

/***************************************************************************************************
*FunctionName：GetUserListJSON
*Description：获取所有操作人信息
*Input：None
*Output：None
*Author：xsx
*Data：2016年5月11日14:10:45
***************************************************************************************************/
MyState_TypeDef GetUserListJSON(void)
{
	/*判断是否已存在*/
	if(GB_UserCJSON == NULL)
	{
		/*如果从文件中获取成功*/
		GB_UserCJSON = ReadUserFileToJSON();
		
		/*如果读取失败则新建空JSON*/
		if(NULL == GB_UserCJSON)
		{
			GB_UserCJSON = cJSON_CreateObject();
			if(GB_UserCJSON == NULL)
				return My_Fail;
		}
	}
	
	return My_Pass;
}

/***************************************************************************************************
*FunctionName：SaveUserListJSON
*Description：保存所有用户信息
*Input：None
*Output：None
*Author：xsx
*Data：2016年5月11日14:14:04
***************************************************************************************************/
MyState_TypeDef SaveUserListJSON(void)
{
	/*判断是否已存在*/
	if(GB_UserCJSON == NULL)
		return My_Fail;
	
	SaveUserJSONToFile(GB_UserCJSON);
	return My_Pass;
}

/***************************************************************************************************
*FunctionName：DeleteUserListJSON
*Description：从内存中删除所有用户信息
*Input：None
*Output：None
*Author：xsx
*Data：2016年5月11日14:14:20
***************************************************************************************************/
void DeleteUserListJSON(void)
{
	cJSON_Delete(GB_UserCJSON);
	GB_UserCJSON = NULL;
}

/***************************************************************************************************
*FunctionName：AddAUserJSON
*Description：添加一个用户
*Input：None
*Output：None
*Author：xsx
*Data：2016年5月11日14:14:45
***************************************************************************************************/
MyState_TypeDef AddAUserJSON(User_Type *user)
{
	cJSON * tempjson = NULL;
	cJSON * userjson = NULL;
	if(user)
	{
		if(GB_UserCJSON == NULL)
			return My_Fail;
		
		userjson = CreateAUserJSON(user);
		if(userjson == NULL)
			return My_Fail;
		
		/*先查找看操作人是否已存在*/
		tempjson = cJSON_GetObjectItem(GB_UserCJSON, user->user_name);
		
		/*如果存在，就替换掉*/
		if(tempjson)
			cJSON_ReplaceItemInObject(GB_UserCJSON, user->user_name, userjson);
		
		/*如果不存在，就新建*/
		else
		{
			/*是否已到上限*/
			if(cJSON_GetArraySize(GB_UserCJSON) > MaxUserNum)
			{
				cJSON_Delete(userjson);
				return My_Fail;
			}
			
			cJSON_AddItemToObject(GB_UserCJSON, user->user_name, userjson);
		}
		
		return My_Pass;
	}
	else
		return My_Fail;
}

/***************************************************************************************************
*FunctionName：DeleteAUserJSON
*Description：删除一个操作人
*Input：None
*Output：None
*Author：xsx
*Data：2016年5月11日14:22:00
***************************************************************************************************/
MyState_TypeDef DeleteAUserJSON(User_Type *user)
{
	cJSON * tempjson = NULL;
	
	if(user)
	{	
		if(GB_UserCJSON == NULL)
			return My_Fail;
		
		/*先查找看操作人是否已存在*/
		tempjson = cJSON_GetObjectItem(GB_UserCJSON, user->user_name);
		
		/*如果存在，就替换掉*/
		if(tempjson)
			cJSON_DeleteItemFromObject(GB_UserCJSON, user->user_name);
		
		return My_Pass;
	}
	else
		return My_Fail;
}

MyState_TypeDef GetAUserByIndex(unsigned char index, User_Type * user)
{
	cJSON *json = NULL;
	
	if(GB_UserCJSON == NULL)
		return My_Fail;
	
	
	if(user == NULL)
		return My_Fail;
	
	json = cJSON_GetArrayItem(GB_UserCJSON, index);
	if(json == NULL)
		return My_Fail;
	else
	{
		return ParseJSONAUserJSON(user, json);
	}
}

MyState_TypeDef ParseJSONAUserJSON(User_Type * user, cJSON * cjson)
{
	char *out;
	
	cJSON *pArrayItem = NULL;
	
	if(cjson)
	{
		/*读取名字*/
		pArrayItem = cJSON_GetArrayItem(cjson, 0);
		if(pArrayItem)
		{
			out = cJSON_Print( pArrayItem );

			if(strlen(out) > 2)
				mymemcpy(user->user_name, out+1, strlen(out)-2);
		
			myfree( out );
		}
		else
			return My_Fail;
		
		/*读取年龄*/
		pArrayItem = cJSON_GetArrayItem(cjson, 1);
		
		if(pArrayItem)
		{
			out = cJSON_Print( pArrayItem );

			if(strlen(out) > 2)
				mymemcpy(user->user_age, out+1, strlen(out)-2);
		
			myfree( out );
		}
		else
			return My_Fail;
		
		/*读取性别*/
		pArrayItem = cJSON_GetArrayItem(cjson, 2);
		
		if(pArrayItem)
		{
			out = cJSON_Print( pArrayItem );

			if(strlen(out) > 2)
				mymemcpy(user->user_sex, out+1, strlen(out)-2);
		
			myfree( out );
		}
		else
			return My_Fail;
		
		/*读取联系方式*/
		pArrayItem = cJSON_GetArrayItem(cjson, 3);
		
		if(pArrayItem)
		{
			out = cJSON_Print( pArrayItem );

			if(strlen(out) > 2)
				mymemcpy(user->user_phone, out+1, strlen(out)-2);
		
			myfree( out );
		}
		else
			return My_Fail;
		
		/*读取职务*/
		pArrayItem = cJSON_GetArrayItem(cjson, 4);
		
		if(pArrayItem)
		{
			out = cJSON_Print( pArrayItem );

			if(strlen(out) > 2)
				mymemcpy(user->user_job, out+1, strlen(out)-2);
		
			myfree( out );
		}
		else
			return My_Fail;
		
		/*读取备注*/
		pArrayItem = cJSON_GetArrayItem(cjson, 5);
		
		if(pArrayItem)
		{
			out = cJSON_Print( pArrayItem );

			if(strlen(out) > 2)
				mymemcpy(user->user_desc, out+1, strlen(out)-2);
		
			myfree( out );
			
			return My_Pass;
		}
		else
			return My_Fail;
	}
	
	else
		return My_Fail;

}

cJSON * CreateAUserJSON(User_Type * user)
{
	char *out;
	cJSON * cjson = NULL;
	
	cJSON *pArrayItem = NULL;
	MyState_TypeDef statues = My_Fail;

	cjson = cJSON_CreateObject();
	if(cjson)
	{
		/*读取名字*/
		pArrayItem = cJSON_CreateString(user->user_name);
		if(pArrayItem)
		{
			cJSON_AddItemToObject(cjson, "名字", pArrayItem);
		}
		else
			goto END;
		
		
		/*读取年龄*/
		pArrayItem = cJSON_CreateString(user->user_age);
		if(pArrayItem)
		{
			cJSON_AddItemToObject(cjson, "年龄", pArrayItem);
		}
		else
			goto END;
		
		/*读取性别*/
		pArrayItem = cJSON_CreateString(user->user_sex);
		if(pArrayItem)
		{
			cJSON_AddItemToObject(cjson, "性别", pArrayItem);
		}
		else
			goto END;
		
		/*读取联系方式*/
		pArrayItem = cJSON_CreateString(user->user_phone);
		if(pArrayItem)
		{
			cJSON_AddItemToObject(cjson, "联系方式", pArrayItem);
		}
		else
			goto END;
		
		/*读取职务*/
		pArrayItem = cJSON_CreateString(user->user_job);
		if(pArrayItem)
		{
			cJSON_AddItemToObject(cjson, "职务", pArrayItem);
		}
		else
			goto END;
		
		/*读取备注*/
		pArrayItem = cJSON_CreateString(user->user_desc);
		if(pArrayItem)
		{
			cJSON_AddItemToObject(cjson, "备注", pArrayItem);
		}
		else
			goto END;

		statues = My_Pass;
	}
	
	END:
		if(statues == My_Fail)
		{
			cJSON_Delete(cjson);
			cjson = NULL;
		}
		
		return cjson;
}
