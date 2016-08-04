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

unsigned char m[12]={31,28,31,30,31,30,31,31,30,31,30,31};
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
	unsigned short count = 0;
	mynetbuf netbuf;
	while(1)
	{
		netbuf.data = MyMalloc(4096);
		if(netbuf.data)
		{
			netbuf.datalen =0 ;
			while(pdPASS == USB_GetChar(((unsigned char *)netbuf.data)+netbuf.datalen, 10 / portTICK_RATE_MS))
				netbuf.datalen++;
			
			if(netbuf.datalen > 0)
			{
				SendDataBySocketA(&netbuf);
				USB_PutStr(netbuf.data, netbuf.datalen);
				MyFree(netbuf.data);
			}
			else
				MyFree(netbuf.data);
		}
		vTaskDelay(100);
	}

}

