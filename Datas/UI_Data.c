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
#include	"MyTools.h"

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
MyState_TypeDef startActivity(MyState_TypeDef (* pageCreate)(Activity * thizactivity, Intent * pram), Intent * pram)
{
	Activity * activity = NULL;
	
	if(NULL == pageCreate)
		return My_Fail;
	
	//为新页面申请内存
	activity = MyMalloc(sizeof(Activity));
	
	if(activity)
	{
		memset(activity, 0, sizeof(Activity));
		
		activity->pageCreate = (MyState_TypeDef (*)(void * thizactivity, Intent * pram))pageCreate;
		
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
				StackPop(&GB_ActivityLinkStack, true);
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
			StackPop(&GB_ActivityLinkStack, false);
	}
	
	return My_Fail;
}

/***************************************************************************************************
*FunctionName: backToFatherActivity
*Description: 返回父页面
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日10:52:37
***************************************************************************************************/
MyState_TypeDef backToFatherActivity(void)
{
	Activity * activity = NULL;
	
	//出栈当前页面
	StackPop(&GB_ActivityLinkStack, false);
	
	if(My_Pass == StackTop(&GB_ActivityLinkStack, &activity))
	{
		if(activity->pageResume)
			activity->pageResume();
			
		return My_Pass;
	}
	
	return My_Fail;
}

/***************************************************************************************************
*FunctionName: getFatherActivityName
*Description: 获取父页面名字
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日11:03:51
***************************************************************************************************/
char * getFatherActivityName(void)
{
	if((GB_ActivityLinkStack.top) && (GB_ActivityLinkStack.top->lastStackNode) && (GB_ActivityLinkStack.top->lastStackNode->activity))
		return GB_ActivityLinkStack.top->lastStackNode->activity->pageName;
	
	return NULL;
}

/***************************************************************************************************
*FunctionName: getCurrentActivityName
*Description: 获取当前页面名字
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日11:03:51
***************************************************************************************************/
char * getCurrentActivityName(void)
{
	if((GB_ActivityLinkStack.top) && (GB_ActivityLinkStack.top->activity))
		return GB_ActivityLinkStack.top->activity->pageName;
	
	return NULL;
}

/***************************************************************************************************
*FunctionName:  checkFatherActivityIs
*Description:  检查父页面是不是pageName
*Input:  pageName -- 父页面名字
*Output:  
*Return:  
*Author:  xsx
*Date: 09:27:19
***************************************************************************************************/
bool checkFatherActivityIs(char * pageName)
{
	char * fatherActivityName = NULL;
	
	if((GB_ActivityLinkStack.top) && (GB_ActivityLinkStack.top->lastStackNode) && (GB_ActivityLinkStack.top->lastStackNode->activity))
		fatherActivityName = GB_ActivityLinkStack.top->lastStackNode->activity->pageName;
	
	if(fatherActivityName && pageName)
	{
		if(CheckStrIsSame(fatherActivityName, pageName, strlen(pageName)))
			return true;
	}
	
	return false;
}

/***************************************************************************************************
*FunctionName: destroyTopActivity
*Description: 销毁栈顶页面
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:42:32
***************************************************************************************************/
void destroyTopActivity(void)
{
	StackPop(&GB_ActivityLinkStack, false);
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

