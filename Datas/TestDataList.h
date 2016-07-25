#ifndef __MYLIST_T_H__
#define __MYLIST_T_H__

#include	"Define.h"
#include	"ErWeiMa_Data.h"
#include	"Timer_Data.h"
#include	"Motor_Data.h"

#include	"MyList.h"

#define	AvregeNum		10															//??????????,??????????
#define	MaxPointLen		(EndTestLocation - StartTestLocation)/AvregeNum				//?????
#define	MaxCardLocation	10


typedef struct TestData_tag {
	User_Type user; 
	char sampleid[MaxSampleIDLen];
	TestCardCodeInfo temperweima;
	unsigned short TestDataLine[MaxPointLen];
	Timer timer;
	unsigned char testlocation;
} TestData;

MyState_TypeDef InitMyTestDataList(void);
List *GetMyTestDataList(void);
TestData *GetCurrentNodeData(void);

MyState_TypeDef Compare(void *data, void *key);
TestData * CreateTestData(unsigned char index);
unsigned char GetUsableLocation(void);

MyState_TypeDef SetTester(User_Type *user);
MyState_TypeDef SetSampleID(void *sampleid);
MyState_TypeDef SetErWeiMaData(TestCardCodeInfo * data);
Timer *GetCurrentTimer(void);
#endif

