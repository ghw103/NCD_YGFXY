/***************************************************************************************************
*FileName:
*Description:
*Author: xsx_kair
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"Timer4_Driver.h"
#include	"stm32f4xx_tim.h"
#include	"CardLimit_Driver.h"
#include	"DRV8825_Driver.h"
#include	"Define.h"
#include	"Motor_Fun.h"
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
extern unsigned char motorstautes;
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/****************************************File Start*************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************
*FunctionName:Timer_Init
*Description:???3???,?????1S??
*Input:None
*Output:None
*Author:xsx
*Data:2016?5?14?15:48:03
***************************************************************************************************/
void Timer4_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  	//TIM14????    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); 	//??PORTF??	
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_TIM4); //GPIOF9??????14
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //????
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//??100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //??????
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //??
	GPIO_Init(GPIOD,&GPIO_InitStructure);              //???PF9
	  GPIO_ResetBits(GPIOD,GPIO_Pin_15);
	
	TIM_TimeBaseStructure.TIM_Prescaler=84-1;  //?????
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Down; //??????
	TIM_TimeBaseStructure.TIM_Period= 200;   //??????
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);//??????14
	
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //?????3????
	
	//???TIM14 Channel1 PWM??	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //???????:TIM????????2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //??????
	TIM_OCInitStructure.TIM_Pulse = 100-1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //????:TIM???????
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);  //??T??????????TIM1 4OC1

	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);  //??TIM14?CCR1????????
 
	TIM_ARRPreloadConfig(TIM4,ENABLE);//ARPE?? 
	
	
    NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; //???3??
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x04; //?????1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01; //????3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	StopTimer4();
}

void StartTimer4(void)
{
	TIM_Cmd(TIM4, ENABLE); 						//?????3
	TIM_SetCounter(TIM4, 200);
}

void StopTimer4(void)
{
	TIM_Cmd(TIM4, DISABLE); 						//?????3
}

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //????
	{
		SetDRVSleepStatues(OnLine);
		
		if(GetGB_MotorDir() == Forward)
			SetGB_MotorLocation(GetGB_MotorLocation() + 1);
		else
			SetGB_MotorLocation(GetGB_MotorLocation() - 1);
		
		if(BackLimited)
			SetGB_MotorLocation(0);
		
		if(PreLimited)
		{
			SetGB_MotorLocation(60000);
		}
		
		if(GetGB_MotorTargetLocation() > GetGB_MotorLocation())
			SetDRVDir(Forward);
		else
			SetDRVDir(Reverse);
	
		if(GetGB_MotorTargetLocation() == GetGB_MotorLocation())
		{
			StopTimer4();
			motorstautes = 0;
		}
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  //???????
}
/****************************************end of file************************************************/

