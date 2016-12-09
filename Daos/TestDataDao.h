/****************************************file start****************************************************/
#ifndef _TESTDATA_D_H
#define	_TESTDATA_D_H

#include	"Define.h"

#define	DataShowNumInPage		8							//一页8个数据

#pragma pack(1)
typedef struct TestDataHead_tag {
	unsigned int datanum;
	unsigned int readindex;
	unsigned short crc;
}TestDataHead;
#pragma pack()

#pragma pack(1)
typedef struct ReadTestDataPackage_Tag
{
	TestData testData[DataShowNumInPage];									//读取数据缓冲区
	TestDataHead testDataHead;												//测试数据保存文件的文件头
	unsigned int startReadIndex;											//开始读取索引,0开始
	unsigned char maxReadNum;												//最大读取数目
	unsigned char readDataNum;												//成功读取到的数据数目
}ReadTestDataPackage;
#pragma pack()

MyState_TypeDef WriteTestData(TestData * testdata);
MyState_TypeDef ReadTestData(ReadTestDataPackage * readpackage);

#endif

/****************************************end of file************************************************/
