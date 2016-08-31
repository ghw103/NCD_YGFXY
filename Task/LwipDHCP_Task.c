/***************************************************************************************************
*FileName：LwipDHCP_Task
*Description：LWIP动态获取IP任务
*Author：xsx
*Data：2016年3月9日14:38:36
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/

#include	"LwipDHCP_Task.h"
#include	"LwIPConfig.h"

#include	"MyMem.h"

#include 	"FreeRTOS.h"
#include 	"task.h"

/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

#define LwipDHCPTask_PRIORITY			2			//看门狗任务优先级
const char * LwipDHCPTaskName = "vLwipDhcpTask";		//看门狗任务名

static DHCP_State_TypeDef GB_DHCPState = DHCP_START;
/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/

static void vLwipDHCPTask( void *pvParameters );	//看门狗任务

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName：StartvSysLedTask
*Description：建立系统指示灯任务
*Input：None
*Output：None
*Author：xsx
*Data：2015年8月26日16:58:13
***************************************************************************************************/
void StartvLwipDHCPTask(void * pram)
{
	SetGB_DHCPState(DHCP_START);
	
	xTaskCreate( vLwipDHCPTask, LwipDHCPTaskName, configMINIMAL_STACK_SIZE, pram, LwipDHCPTask_PRIORITY, NULL );
}

/***************************************************************************************************
*FunctionName：GetGB_DHCPState
*Description：获取最新的DHCP状态
*Input：None
*Output：DHCP状态
*Author：xsx
*Data：2016年3月9日14:49:52
***************************************************************************************************/
DHCP_State_TypeDef GetGB_DHCPState(void)
{
	return GB_DHCPState;
}

/***************************************************************************************************
*FunctionName：SetGB_DHCPState
*Description：更新DHCP状态
*Input：NewState -- 最新的DHCP状态
*Output：None
*Author：xsx
*Data：2016年3月9日14:50:32
***************************************************************************************************/
void SetGB_DHCPState(DHCP_State_TypeDef NewState)
{
	GB_DHCPState = NewState;
}

/***************************************************************************************************
*FunctionName：vSysLedTask
*Description：系统指示灯闪烁表面程序正常运行
*Input：None
*Output：None
*Author：xsx
*Data：2015年8月26日16:58:46
***************************************************************************************************/
static void vLwipDHCPTask( void *pvParameters )
{
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
	unsigned int IPaddress;
	struct netif * tempnetif = (struct netif *)pvParameters;
  
	while(1)
	{
		switch (GetGB_DHCPState())
		{
			case DHCP_START:
				{
					IPaddress = 0;
					
					dhcp_start(tempnetif);
					
					SetGB_DHCPState(DHCP_WAIT_ADDRESS);
				}
				break;

			case DHCP_WAIT_ADDRESS:
				{
					/* Read the new IP address */
					IPaddress = tempnetif->ip_addr.addr;
//					printf("正在获取IP.....\r\n");
					if (IPaddress!=0) 
					{
						SetGB_DHCPState(DHCP_ADDRESS_ASSIGNED);					

						/* Stop DHCP */
						dhcp_stop(tempnetif);
					}
					else
					{
						/* DHCP timeout */
						if (tempnetif->dhcp->tries > MAX_DHCP_TRIES)
						{
							SetGB_DHCPState(DHCP_TIMEOUT);
//							printf("IP获取失败.....\r\n");
							/* Stop DHCP */
							dhcp_stop(tempnetif);

							/* Static address used */
							IP4_ADDR(&ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
							IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
							IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
							netif_set_addr(tempnetif, &ipaddr , &netmask, &gw);
					  
//							printf("使用静态IP %d.%d.%d.%d\r\n", IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3);
						}
					}
				}
				break;

			default: break;
		}
    
		/* wait 250 ms */
		vTaskDelay(250 / portTICK_RATE_MS);
	}   
}


