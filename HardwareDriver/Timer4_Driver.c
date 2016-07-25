/***************************************************************************************************
*FileName：Timer_Driver
*Description：定时器驱动，使用通用定时器3，1S时基，用于检测卡反应时间倒计时
*Author：xsx
*Data：2015年8月26日14:30:46
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/

#include	"Timer4_Driver.h"
#include	"stm32f4xx_tim.h"
#include	"CardLimit_Driver.h"
#include	"DRV8825_Driver.h"
#include	"Motor_Fun.h"
/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/
extern unsigned char motorstautes;
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
*FunctionName：Timer_Init
*Description：定时器3初始化，这里设置成1S中断
*Input：None
*Output：None
*Author：xsx
*Data：2016年5月14日15:48:03
***************************************************************************************************/
void Timer4_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  	//TIM14时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); 	//使能PORTF时钟	
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_TIM4); //GPIOF9复用为定时器14
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure);              //初始化PF9
	  GPIO_ResetBits(GPIOD,GPIO_Pin_15);
	
	TIM_TimeBaseStructure.TIM_Prescaler=84-1;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Down; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period= 200;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);//初始化定时器14
	
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //允许定时器3更新中断
	
	//初始化TIM14 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 100-1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性低
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 4OC1

	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM14在CCR1上的预装载寄存器
 
	TIM_ARRPreloadConfig(TIM4,ENABLE);//ARPE使能 
	
	
    NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x04; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	StopTimer4();
}

void StartTimer4(void)
{
	TIM_Cmd(TIM4, ENABLE); 						//使能定时器3
	TIM_SetCounter(TIM4, 200);
}

void StopTimer4(void)
{
	TIM_Cmd(TIM4, DISABLE); 						//使能定时器3
}

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //溢出中断
	{
		SetDRVSleepStatues(OnLine);
		
		if(GetGB_MotorDir() == Forward)
			SetGB_MotorLocation(GetGB_MotorLocation() + 1);
		else
			SetGB_MotorLocation(GetGB_MotorLocation() - 1);
		
		if(BackLimited)
			SetGB_MotorLocation(0);
		
		if(PreLimited)
			SetGB_MotorLocation(MaxLocation);
		
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
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  //清除中断标志位
}

