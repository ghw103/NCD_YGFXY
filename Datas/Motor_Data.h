#ifndef _MOTOR_S_D_H__
#define _MOTOR_S_D_H__


/*电机工作模式*/
typedef enum
{ 
	Absolute_L = 1,							//绝对定位
	Relative_L = 2,							//相对定位
}MotorWorkMode_Def;

/*发送给电机的数据*/
typedef struct
{ 
	MotorWorkMode_Def mode;
	short value;
}MotorWorkValue_Def;

typedef enum
{ 
	NotLimitted = 0,						//没限位
	Limitted = 1							//限位
}LimitState_Def;





typedef enum
{ 
	OnLine = 0,										//电机在线
	OffLine	= 1										//电机离线
}DRVSleep_Type;

typedef enum
{ 
	Forward = 1,									//电机正向
	Reverse	= 0										//电机反向
}DRVDir_Type;

typedef enum
{ 
	LowPower = 1,									//电机低功耗
	NonamalPower = 0								//电机正常功耗
}DRVPower_Type;

typedef enum
{ 
	FastMode = 1,									//电机低功耗
	LowMode = 0										//电机正常功耗
}DRVDecay_Type;


LimitState_Def GetMotorPreLimitState(void);
void SetMotorPreLimitState(LimitState_Def value);

LimitState_Def GetMotorBackLimitState(void);
void SetMotorBackLimitState(LimitState_Def value);

unsigned short GetGB_MotorLocation(void);
void SetGB_MotorLocation(unsigned short value);

unsigned short GetGB_MotorTargetLocation(void);
void SetGB_MotorTargetLocation(unsigned short value);

unsigned short GetGB_MotorDir(void);
void SetGB_MotorDir(DRVDir_Type dir);

#endif

