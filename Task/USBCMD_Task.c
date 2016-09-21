/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"USBCMD_Task.h"
#include 	"usbd_cdc_vcp.h"
#include	"CodeScanner_Driver.h"
#include	"MyEncryptTool.h"
#include	"MyMem.h"
#include	"user_fatfs.h"
#include	"Ads8325_Driver.h"
#include	"DRV8825_Driver.h"
#include 	"MLX90614_Driver.h"
#include	"RTC_Driver.h"
#include	"TLV5617_Driver.h"
#include 	"Usart4_Driver.h"
#include	"WifiFunction.h"
#include	"CardLimit_Driver.h"
#include	"PT8211_Driver.h"
#include	"Define.h"
#include	"OutModel_Fun.h"
#include	"MyTest_Data.h"
#include	"SDFunction.h"
#include	"CodeScan_Task.h"
#include	"MAX4051_Driver.h"
#include	"WifiFunction.h"
#include	"ServerFun.h"
#include	"PlaySong_Task.h"
#include	"Timer4_Driver.h"
#include	"Motor_Fun.h"
#include	"QueueUnits.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#include	"ff.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"
#include 	"time.h"
/******************************************************************************************/
/*****************************************局部变量声明*************************************/
#define vUSBCMDTask_PRIORITY			( ( unsigned portBASE_TYPE ) 2U )
const char * USBCMDTaskName = "vUSBCOMTask";

/******************************************************************************************/
/*****************************************局部函数声明*************************************/

static void vUSBCMDTask( void *pvParameters );
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
void StartUSBCMDTask(void)
{
	xTaskCreate( vUSBCMDTask, USBCMDTaskName, configMINIMAL_STACK_SIZE, NULL, vUSBCMDTask_PRIORITY, NULL );
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
static void vUSBCMDTask( void *pvParameters )
{
	static unsigned char buf[1000];
	unsigned char count = 0;
	
	while(1)
	{
/*		count =0 ;
		while(pdPASS == USB_GetChar(buf+count, 10 / portTICK_RATE_MS))
			count++;
			
		if(count > 0)
		{
			SendDataToQueue(GetUsart4TXQueue(), GetUsart4TXMutex(), buf, count, 1, 50 / portTICK_RATE_MS, EnableUsart4TXInterrupt);
		}
		
		count =0 ;
		while(pdPASS == ReceiveDataFromQueue(GetUsart4RXQueue(), GetUsart4RXMutex(), buf+count, 1, 1, 10 / portTICK_RATE_MS))
			count++;
			
		if(count > 0)
		{
			USB_PutStr(buf, count);
		}*/
		vTaskDelay(1000 / portTICK_RATE_MS);
	}

}

