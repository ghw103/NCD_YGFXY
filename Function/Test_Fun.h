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
typedef struct TempCalData_tag{
	double tempvalue1;
	double tempvalue2;
	double average;														//平均值
	double stdev;														//标准差
	float CV1;															//变异系数1
	float CV2;															//变异系数2
	unsigned short tempvalue3;
	unsigned short temptestline[MaxPointLen + FilterNum];
	double lastdata;														//记录上次滤波数据
	
	unsigned short maxdata;
	ItemData * itemData;												//测试数据指针
	ResultState resultstatues;											//测试结果状态
}TempCalData;
#pragma pack()

MyState_TypeDef InitTestFunData(void);
ResultState TestFunction(void * parm);

MyState_TypeDef TakeTestPointData(void * data);
MyState_TypeDef TakeTestResultData(void * data);

#endif

