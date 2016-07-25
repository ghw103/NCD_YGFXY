/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"CodeScan_Task.h"
#include	"CodeScanFunction.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"
/******************************************************************************************/
/*****************************************局部变量声明*************************************/

#define vCodeScanTask_PRIORITY			( ( unsigned portBASE_TYPE ) 2U )
const char * CodeScanTaskName = "vCodeScanTask";

static xQueueHandle xStartScanQueue = NULL ;			//互斥量，如果接收到数据，则开始扫描二维码，接收的数据为二维码存放地址
static xQueueHandle xScanResultQueue = NULL;											//发送扫描结果
/******************************************************************************************/
/*****************************************局部函数声明*************************************/

static void vCodeScanTask( void *pvParameters );

/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

/************************************************************************
** 函数名:
** 功  能:
** 输  入:无
** 输  出:无
** 返  回：无
** 备  注：无
** 时  间:  
** 作  者：xsx                                                 
************************************************************************/
MyState_TypeDef StartCodeScanTask(void)
{
	if(xStartScanQueue == NULL)
		xStartScanQueue = xQueueCreate(1, sizeof(void *));
	
	if(xStartScanQueue == NULL)
		return My_Fail;
	
	if(xScanResultQueue == NULL)
		xScanResultQueue = xQueueCreate(1, sizeof(ScanCodeResult));
	
	if(xScanResultQueue == NULL)
		return My_Fail;
	
	xTaskCreate( vCodeScanTask, CodeScanTaskName, configMINIMAL_STACK_SIZE+100, NULL, vCodeScanTask_PRIORITY, NULL );
	
	return My_Pass;
}

/************************************************************************
** 函数名:
** 功  能:
** 输  入:无
** 输  出:无
** 返  回：无
** 备  注：无
** 时  间:  
** 作  者：xsx                                                 
************************************************************************/
static void vCodeScanTask( void *pvParameters )
{
	void * parm;
	ScanCodeResult scanresult;
	
	while(1)
	{
		if(pdPASS == xQueueReceive( xStartScanQueue, &parm, portMAX_DELAY))
		{
			vTaskDelay(1000 * portTICK_RATE_MS);
			scanresult = ScanCodeFun(parm);										//读取二维码
			vTaskDelay(1000 * portTICK_RATE_MS);
			
			/*发送测试结果*/
			xQueueSend( xScanResultQueue, &scanresult, 10000/portTICK_RATE_MS );
				
			parm = NULL;
		}
		
		vTaskDelay(1000 * portTICK_RATE_MS);
	}
}

/***************************************************************************************************
*FunctionName：StartScanQRCode
*Description：开始读取二维码，是扫码任务进入运行状态
*Input：parm -- 读取二维码的地址，传入的应该是存放二维码数据的地址，所以队列发送的时候发送的是存放二维码地址的指针的地址
*Output：None
*Author：xsx
*Data：2016年7月11日14:56:57
***************************************************************************************************/
MyState_TypeDef StartScanQRCode(void * parm)
{
	if(pdPASS == xQueueSend( xStartScanQueue, &parm, 10*portTICK_RATE_MS))
		return My_Pass;
	else
		return My_Fail;	
}

MyState_TypeDef TakeScanQRCodeResult(ScanCodeResult *scanresult)
{
	if(pdPASS == xQueueReceive( xScanResultQueue, scanresult,  10/portTICK_RATE_MS))
		return My_Pass;
	else
		return My_Fail;
}

