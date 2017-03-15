/****************************************file start****************************************************/
#ifndef LOGSDAO_D_H
#define LOGSDAO_D_H

#include	"Define.h"

typedef struct Logs_Tag
{
	FatfsFileInfo_Def myfile;
	char * dataBuf;
	MyTime_Def time;
	char writeBuf[1024];
}Logs;

MyState_TypeDef log_Write(void * data, char errCode);

#endif

/****************************************end of file************************************************/
