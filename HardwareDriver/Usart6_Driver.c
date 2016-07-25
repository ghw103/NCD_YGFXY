/***************************************************************************************************
*FileName:Usart6_Driver
*Description:串口6的驱动，目前用于液晶通信
*Author:xsx
*Data:2016年4月29日11:26:51
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include 	"stm32f4xx.h"
#include 	"Usart6_Driver.h"

/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/
static xQueueHandle xRxQueue = NULL;									//接收队列
static xQueueHandle xTxQueue = NULL;									//发送队列
static xSemaphoreHandle xRXMutex = NULL;								//互斥量
static xSemaphoreHandle xTXMutex = NULL;								//互斥量
/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/
static void Usart6_Os_Init(void);
static void ConfigUsart6(void);
static portBASE_TYPE prvUsart6_ISR_NonNakedBehaviour( void );
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName：Usart6_Os_Init
*Description：创建串口6的队列互斥量
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月29日11:28:04
***************************************************************************************************/
static void Usart6_Os_Init(void)
{
	xRxQueue = xQueueCreate( xRxQueue6_Len, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	xTxQueue = xQueueCreate( xTxQueue6_Len, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	
	vSemaphoreCreateBinary(xTXMutex);
	vSemaphoreCreateBinary(xRXMutex);
}

/***************************************************************************************************
*FunctionName：ConfigUsart6
*Description：串口6的端口初始化和配置
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月29日11:28:25
***************************************************************************************************/
static void ConfigUsart6(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 开启GPIO_D的时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	/* 开启串口3的时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);

	USART_InitStructure.USART_BaudRate   = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;
	USART_InitStructure.USART_Parity     = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;

	USART_Init(USART6, &USART_InitStructure);
  
	/* 使能串口3 */
	USART_Cmd(USART6, ENABLE);
	//使能接收中断
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
	
	
	/* NVIC configuration */
	/* Configure the Priority Group to 2 bits */


	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/***************************************************************************************************
*FunctionName：Usart6_Init
*Description：串口6外部调用初始化函数
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月29日11:28:56
***************************************************************************************************/
void Usart6_Init(void)
{
	Usart6_Os_Init();
	ConfigUsart6();
}

/***************************************************************************************************
*FunctionName：USART6_IRQHandler
*Description：串口6的中断函数
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月29日11:29:13
***************************************************************************************************/
void USART6_IRQHandler(void)
{			
	prvUsart6_ISR_NonNakedBehaviour();
}

/***************************************************************************************************
*FunctionName：prvUsart6_ISR_NonNakedBehaviour
*Description：串口6的中断服务函数
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月29日11:29:32
***************************************************************************************************/
__attribute__((__noinline__))
static portBASE_TYPE prvUsart6_ISR_NonNakedBehaviour( void )
{
	signed portCHAR     cChar;
	portBASE_TYPE     xHigherPriorityTaskWoken = pdFALSE;

	portBASE_TYPE retstatus;

	if(USART_GetITStatus(USART6 , USART_IT_TXE) == SET)
	{
		/* The interrupt was caused by the THR becoming empty.  Are there any
		more characters to transmit?
		Because FreeRTOS is not supposed to run with nested interrupts, put all OS
		calls in a critical section . */
		portENTER_CRITICAL();
			retstatus = xQueueReceiveFromISR( xTxQueue, &cChar, &xHigherPriorityTaskWoken );
		portEXIT_CRITICAL();

		if (retstatus == pdTRUE)
		{
			/* A character was retrieved from the queue so can be sent to the THR now. */
			USART_SendData(USART6, cChar);
		}
		else
		{
			/* Queue empty, nothing to send so turn off the Tx interrupt. */
			USART_ITConfig(USART6, USART_IT_TXE, DISABLE);
		}
	}

	if(USART_GetITStatus(USART6, USART_IT_RXNE) == SET)
	{
		/* The interrupt was caused by the receiver getting data. */
		cChar = USART_ReceiveData(USART6);

		/* Because FreeRTOS is not supposed to run with nested interrupts, put all OS
		calls in a critical section . */
		portENTER_CRITICAL();
			xQueueSendFromISR(xRxQueue, &cChar, &xHigherPriorityTaskWoken);
		portEXIT_CRITICAL();
	}

	/* The return value will be used by portEXIT_SWITCHING_ISR() to know if it
	should perform a vTaskSwitchContext(). */
	return ( xHigherPriorityTaskWoken );
}

/***************************************************************************************************
*FunctionName：EnableUsart6TXInterrupt
*Description：开启一次发送中断（发送队列中数据）
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月29日11:18:28
***************************************************************************************************/
void EnableUsart6TXInterrupt(void)
{
	USART_ITConfig(USART6, USART_IT_TXE, ENABLE);
}

/***************************************************************************************************
*FunctionName：GetUsart6RXQueue, GetUsart6TXQueue,GetUsart6Mutex
*Description：获取串口6的发送接收队列,和队列互斥量
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月29日11:22:06
***************************************************************************************************/
xQueueHandle GetUsart6RXQueue(void)
{
	return xRxQueue;
}

xQueueHandle GetUsart6TXQueue(void)
{
	return xTxQueue;
}

xSemaphoreHandle GetUsart6RXMutex(void)
{
	return xRXMutex;
}

xSemaphoreHandle GetUsart6TXMutex(void)
{
	return xTXMutex;
}
