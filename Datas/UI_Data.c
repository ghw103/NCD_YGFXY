/***************************************************************************************************
*FileName:
*Description:
*Author:xsx
*Data:
***************************************************************************************************/


/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/

#include	"UI_Data.h"

#include	"MyMem.h"

#include	<string.h>
#include	"stdio.h"

/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

static LinkStack GB_ActivityLinkStack;	


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
*FunctionName: startActivity
*Description: 启动一个新的activity
*Input: pageCreate -- activity的创建函数
*		pram -- 传入的参数
*Output: 
*Return: MyState_TypeDef -- 启动成功与否
*Author: xsx
*Date: 2016年12月20日11:16:59
***************************************************************************************************/
MyState_TypeDef startActivity(MyState_TypeDef (* pageCreate)(Activity * thizactivity, void * pram), void * pram)
{
	Activity * activity = NULL;
	
	if(NULL == pageCreate)
		return My_Fail;
	
	//为新页面申请内存
	activity = MyMalloc(sizeof(Activity));
	
	if(activity)
	{
		memset(activity, 0, sizeof(Activity));
		
		activity->pageCreate = pageCreate;
		
		//新页面入栈
		if(My_Pass == StackPush(&GB_ActivityLinkStack, activity))
		{
			//执行页面创建,创建成功则显示
			if(My_Pass == activity->pageCreate(activity, pram))
			{
				//新页面显示
				if(activity->pageStart)
					activity->pageStart();
				
				return My_Pass;
			}
			//创建失败，则出栈此页面，并销毁
			else
				StackPop(&GB_ActivityLinkStack, NULL, true);
		}
		
		//入栈失败，则销毁
		MyFree(activity);
	}
	
	return My_Fail;
}

/***************************************************************************************************
*FunctionName: backToActivity
*Description: 回退
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月20日11:53:00
***************************************************************************************************/
MyState_TypeDef backToActivity(char * pageName)
{
	Activity * activity = NULL;
	
	if(NULL == pageName)
		return My_Fail;
	
	while(My_Pass == StackTop(&GB_ActivityLinkStack, &activity))
	{
		if(0 == strcmp(activity->pageName, pageName))
		{
			if(activity->pageResume)
				activity->pageResume();
			
			return My_Pass;
		}
		else
			StackPop(&GB_ActivityLinkStack, NULL, false);
	}
	
	return My_Fail;
}

/***************************************************************************************************
*FunctionName: InitActivity
*Description: 初始化一个页面的事件函数
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月20日16:26:14
***************************************************************************************************/
void InitActivity(Activity * activity, char * activityName, void (* pageStart)(void), 
	void (* pageInput)(unsigned char *pbuf , unsigned short len), 
	void (* pageFresh)(void),
	void (* pageHide)(void),
	void (* pageResume)(void),
	void (* pageDestroy)(void))
{
	activity->pageName = activityName;
	activity->pageStart = pageStart;
	activity->pageInput = pageInput;
	activity->pageFresh = pageFresh;
	activity->pageHide = pageHide;
	activity->pageResume = pageResume;
	activity->pageDestroy = pageDestroy;
}

/***************************************************************************************************
*FunctionName: activityInputFunction
*Description: UI系统的输入接口
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月20日16:27:52
***************************************************************************************************/
void activityInputFunction(unsigned char *pbuf , unsigned short len)
{
	Activity * activity = NULL;
	
	if(My_Pass == StackTop(&GB_ActivityLinkStack, &activity))
	{
		if( (activity) && (activity->pageInput))
			activity->pageInput(pbuf, len);
	}
}

/***************************************************************************************************
*FunctionName: activityFreshFunction
*Description: UI系统的刷新接口
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月20日16:28:30
***************************************************************************************************/
void activityFreshFunction(void)
{
	Activity * activity = NULL;
	
	if(My_Pass == StackTop(&GB_ActivityLinkStack, &activity))
	{
		if( (activity) && (activity->pageFresh))
			activity->pageFresh();
	}
}

