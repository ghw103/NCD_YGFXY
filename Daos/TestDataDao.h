/****************************************file start****************************************************/
#ifndef _TESTDATA_D_H
#define	_TESTDATA_D_H

#include	"Define.h"
#include	"SystemSet_Data.h"

#define	DataShowNumInPage		8							//一页8个数据

typedef enum
{
	DESC = 0,												//正序
	ASC = 1													//逆序
}OrderByEnum;

//读取请求信息
#pragma pack(1)
typedef struct PageRequest_tag {
	unsigned int startElementIndex;											//起始读取索引，0为第一个
	unsigned int pageSize;													//每页的数目
	OrderByEnum orderType;													//排序方式
	unsigned short crc;
}PageRequest;
#pragma pack()

//读取到的数据
#pragma pack(1)
typedef struct Page_tag {
	unsigned short ElementsSize;											//读取到的数据数目
	TestData testData[DataShowNumInPage];									//读取到的数据
	unsigned short crc;
}Page;
#pragma pack()


MyState_TypeDef WriteTestData(TestData * testdata, unsigned int writeIndex);
MyState_TypeDef ReadTestData(PageRequest * pageRequest, Page * page, SystemSetData * systemSetData);

#endif

/****************************************end of file************************************************/
