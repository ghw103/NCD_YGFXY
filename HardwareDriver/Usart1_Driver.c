/***************************************************************************************************
*FileName:Usart1_Driver
*Description:串口1驱动
*Author:xsx
*Data:2016年4月29日13:37:14
***************************************************************************************************/


/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include 	"stm32f4xx.h"
#include 	"Usart1_Driver.h"





/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/
static xQueueHandle xRxQueue;									//接收队列
static xQueueHandle xTxQueue;									//发送队列
static xSemaphoreHandle xRXMutex;								//互斥量
static xSemaphoreHandle xTXMutex;								//互斥量




/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/
static void Usart1_Os_Init(void);
static void ConfigUsart1(void);
static portBASE_TYPE prvUsart1_ISR_NonNakedBehaviour( void );






/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName：Usart1_Os_Init
*Description：串口1队列互斥量创建
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月29日13:37:35
***************************************************************************************************/
static void Usart1_Os_Init(void)
{
	xRxQueue = xQueueCreate( xRxQueue1_Len, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	xTxQueue = xQueueCreate( xTxQueue1_Len, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	
	vSemaphoreCreateBinary(xRXMutex);
	vSemaphoreCreateBinary(xTXMutex);
}

/***************************************************************************************************
*FunctionName：ConfigUsart1
*Description：串口1的端口初始化
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月29日11:58:48
***************************************************************************************************/
static void ConfigUsart1(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 开启GPIO_D的时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	/* 开启串口3的时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//串口1的端口配置
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	//串口1的工作模式配置
	USART_InitStructure.USART_BaudRate   = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;
	USART_InitStructure.USART_Parity     = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;

	USART_Init(USART1, &USART_InitStructure);

	/* 使能串口3 */
	USART_Cmd(USART1, ENABLE);
	//使能接收中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	/* NVIC configuration */
	/* Configure the Priority Group to 2 bits */


	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/***************************************************************************************************
*FunctionName：Usart1_Init
*Description：串口1初始化，外部调用
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月29日11:59:09
***************************************************************************************************/
void Usart1_Init(void)
{
	Usart1_Os_Init();
	ConfigUsart1();
}

/***************************************************************************************************
*FunctionName：USART1_IRQHandler
*Description：串口1中断函数
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月29日11:59:24
***************************************************************************************************/
void USART1_IRQHandler(void)
{			
	/* This ISR can cause a context switch, so the first statement must be a
	call to the portENTER_SWITCHING_ISR() macro.  This must be BEFORE any
	variable declarations. */
//	portENTER_SWITCHING_ISR();

	prvUsart1_ISR_NonNakedBehaviour();

	/* Exit the ISR.  If a task was woken by either a character being received
	or transmitted then a context switch will occur. */
//	portEXIT_SWITCHING_ISR();	
}

/***************************************************************************************************
*FunctionName：prvUsart1_ISR_NonNakedBehaviour
*Description：串口1中断服务函数
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月29日11:59:44
***************************************************************************************************/
__attribute__((__noinline__))
static portBASE_TYPE prvUsart1_ISR_NonNakedBehaviour( void )
{
		/* Now we can declare the local variables. */
	signed portCHAR     cChar;
	portBASE_TYPE     xHigherPriorityTaskWoken = pdFALSE;

	portBASE_TYPE retstatus;

	if(USART_GetITStatus(USART1 , USART_IT_TXE) == SET)
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
			USART_SendData(USART1, cChar);
		}
		else
		{
			/* Queue empty, nothing to send so turn off the Tx interrupt. */
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
		}
	}

	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		/* The interrupt was caused by the receiver getting data. */
		cChar = USART_ReceiveData(USART1);
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
*FunctionName：EnableUsart1TXInterrupt
*Description：开启一次发送中断（发送队列中数据）
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月29日11:18:28
***************************************************************************************************/
void EnableUsart1TXInterrupt(void)
{
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

/***************************************************************************************************
*FunctionName：GetUsart1RXQueue, GetUsart1TXQueue,GetUsart1Mutex
*Description：获取串口1的发送接收队列,和队列互斥量
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月29日11:22:06
***************************************************************************************************/
xQueueHandle GetUsart1RXQueue(void)
{
	return xRxQueue;
}

xQueueHandle GetUsart1TXQueue(void)
{
	return xTxQueue;
}

xSemaphoreHandle GetUsart1RXMutex(void)
{
	return xRXMutex;
}

xSemaphoreHandle GetUsart1TXMutex(void)
{
	return xTXMutex;
}

