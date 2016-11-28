/***************************************************************************************************
*FileName:
*Description:
*Author: xsx_kair
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/

#include	"LabServer_Fun.h"
#include	"Test_Fun.h"
#include	"Test_Task.h"
#include	"DeviceDao.h"

#include	"System_Data.h"
#include	"CRC16.h"
#include	"Define.h"
#include 	"tcpip.h"
#include	"MyMem.h"
#include	<string.h>
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
struct netconn *pxNetCon;
struct netbuf *pxRxBuffer;
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static void ProcessCMD(unsigned char *buf, unsigned short len,struct netconn *pxNetCon);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/****************************************File Start*************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName: ProcessQuest
*Description: 处理客户端请求
*Input: parm -- 客户端
*Output: none
*Author: xsx
*Date: 2016年8月24日14:40:18
***************************************************************************************************/
void ProcessQuest(void * parm)
{
	err_t err;
	
	unsigned char *pcRxString;
	unsigned short usLength;
	unsigned short crc;
	
	pxNetCon = parm;
	/* We expect to immediately get data. */
	err = netconn_recv( pxNetCon , &pxRxBuffer);

	if( err == ERR_OK )
	{
		/* Where is the data? */
		netbuf_data( pxRxBuffer, ( void * ) &pcRxString, &usLength );
		
		crc = pcRxString[usLength-2];
		crc = (crc<<8) + pcRxString[usLength-1];
		
		if(crc != CalModbusCRC16Fun1(pcRxString, usLength -2))
			ProcessCMD(pcRxString ,usLength-2, pxNetCon);
		
		netbuf_delete( pxRxBuffer );
	}
	
	netconn_close( pxNetCon );
	netconn_delete( pxNetCon );
}

static void ProcessCMD(unsigned char *buf, unsigned short len, struct netconn *pxNetCon)
{
	char *pxbuf1;
	char *pxbuf2;
	unsigned short temp = 0xffff;
	unsigned short i=0;
	
	pxbuf1 = MyMalloc(4096);
	pxbuf2 = MyMalloc(10);
	if(pxbuf1 && pxbuf2)
	{
		memset(pxbuf1, 0, 4096);
		
		if(!strncmp( (char *)buf, "Read Device Info", 16 ))
		{
			//sprintf(pxbuf1, "{\"deviceid\":\"%s\",\"devicestatus\":%d}", ((DeviceInfo *)(GetGB_DeviceInfo2()))->deviceid, GetTestStatusFlorLab());
		}
		else if(!strncmp( (char *)buf, "Start Test", 10 ))
		{
			if(0 == GetTestStatusFlorLab())
			{
				StartTest(GetTestDataForLab());
				
				sprintf(pxbuf1, "OK");
			}
			else
				sprintf(pxbuf1, "Startted");
		}
		else if(!strncmp( (char *)buf, "Read Test Data", 14 ))
		{
			i = 0;
			
			sprintf(pxbuf1, "{\"data\":[");
			while(pdPASS == TakeTestPointData(&temp))
			{
				if(i == 0)
					sprintf(pxbuf2, "%d", temp);
				else
					sprintf(pxbuf2, ",%d", temp);
				strcat(pxbuf1, pxbuf2);
				i++;
			}
			sprintf(pxbuf2, "],\"status\":%d}", GetTestStatusFlorLab());
			strcat(pxbuf1, pxbuf2);
		}

		netconn_write( pxNetCon, pxbuf1, strlen(pxbuf1), NETCONN_COPY );
	}
	
	MyFree(pxbuf1);
	MyFree(pxbuf2);
}

/****************************************end of file************************************************/
