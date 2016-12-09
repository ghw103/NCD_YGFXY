/***************************************************************************************************
*FileName:
*Description:
*Author: xsx_kair
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"SystemStart_Task.h"

#include 	"netconf.h"
#include	"ReadInputData_Task.h"
#include	"CodeScan_Task.h"
#include	"USBCMD_Task.h"
#include	"SystemUI_Task.h"
#include	"Iwdg_Task.h"
#include	"Universal_Task.h"
#include	"Test_Task.h"
#include	"OutModel_Task.h"
#include	"PlaySong_Task.h"
#include	"NormalUpLoad_Task.h"
#include	"user_fatfs.h"
#include	"WelcomePage.h"
#include	"ReadBarCode_Task.h"

#include	"SelfTest_Fun.h"

#include	"UI_Data.h"
#include	"SelfCheck_Data.h"

#include	"Define.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
#define	SystemStartTask_PRIORITY			2
const char * SystemStartTaskName = "vSystemStartTask";
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static void vSystemStartTask( void *pvParameters );
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/****************************************File Start*************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************
*FunctionName: StartSystemStartTask
*Description: 创建系统初始化任务
*Input: 
*Output: 
*Author: xsx
*Date: 2016年9月21日11:58:17
***************************************************************************************************/
void StartSystemStartTask(void)
{
	xTaskCreate( vSystemStartTask, SystemStartTaskName, configMINIMAL_STACK_SIZE, NULL, SystemStartTask_PRIORITY, NULL );
}

/***************************************************************************************************
*FunctionName: vSystemStartTask
*Description: 系统初始化以及自检，启动
*Input: 
*Output: 
*Author: xsx
*Date: 2016年9月21日11:58:46
***************************************************************************************************/
static void vSystemStartTask( void *pvParameters )
{
	/*开启看门狗任务*/
	StartvIwdgTask();
	
	/*通用任务*/
	StartvUniversalTask();
	
	/*播放音频*/
	StartvPlaySongTask();
	
	/*显示欢迎界面*/
	PageAdvanceTo(DspWelcomePage, NULL);
	
	/*系统界面控制任务*/
	StartvSystemUITask();
	
	/*读取输入数据任务*/
	StartvReadInDataTask();
	
	/*等待启动自检程序*/
	while(SelfCheck_None == GetGB_SelfCheckStatus())
		vTaskDelay(500 / portTICK_RATE_MS);
	
	/*开始自检*/
	if(SelfChecking == GetGB_SelfCheckStatus())
	{
		SelfTest_Function();
		vTaskDelay(1000 * portTICK_RATE_MS);
	}
	
	/*自检错误*/
	while(SelfCheck_Error == GetGB_SelfCheckStatus())
		vTaskDelay(portMAX_DELAY);
	
	/*创建读取条码枪任务*/
	StartBarCodeTask();
	
	/*开启测试任务*/
	StartvTestTask();
	
	/*开启网络任务*/
	StartEthernet();

	/*上传任务*/
	StartvNormalUpLoadTask();
	
	/*开启读二维码任务*/
	StartCodeScanTask();
	
	/*USB测试任务*/
	StartUSBCMDTask();
	
	vTaskDelete(NULL);
}

/****************************************end of file************************************************/
