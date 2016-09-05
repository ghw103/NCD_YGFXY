#ifndef __TESTF_H__
#define __TESTF_H__

#include	"Define.h"
#include 	"FreeRTOS.h"
#include	"MyTest_Data.h"

#define	DataMaxWinSize			20

typedef struct PointData_tag {
	unsigned short data;
	unsigned short index;
}PointData;

#pragma pack(1)
typedef struct PeakData_tag {
	unsigned short StartLocation;											//起峰位置
	unsigned short StartValue;												//起峰值
	unsigned short PeakLocation;											//波峰位置
	unsigned short PeakValue;												//波峰值
	unsigned short EndLocation;												//峰落位置
	unsigned short EndValue;												//峰落值
	unsigned short PeakWidth;												//峰宽度
	unsigned short UpWidth;													//峰上升宽度
	unsigned short DownWidth;												//峰下降宽度
	float PeakScale;														//起峰与落峰比值
	unsigned char step;														//找峰步骤
}PeakData;
#pragma pack()

#pragma pack(1)
typedef struct TempCalData_tag{
	unsigned short tempvalue;
	double tempvalue2;
	short testline2[MaxPointLen];
	double lastdata;														//记录上次滤波数据
	
	PeakData peakdata[10];													//定于10个峰
	unsigned char peaknum;
	
	unsigned short maxdata;
	ResultState resultstatues;											//测试结果状态
}TempCalData;
#pragma pack()

MyState_TypeDef InitTestFunData(void);
ResultState TestFunction(void * parm);

MyState_TypeDef TakeTestPointData(void * data);
MyState_TypeDef TakeTestResultData(void * data);

#endif

