/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"MyEncryptTool.h"
#include	"MyMem.h"
#include	"user_fatfs.h"


#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	"string.h"
/******************************************************************************************/
/*****************************************局部变量声明*************************************/

/******************************************************************************************/
/*****************************************局部函数声明*************************************/

/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

/***************************************************************************************************
*FunctionName：MyDencrypt
*Description：二维码数据密文转明文
*Input：source -- 密文数据
*		target -- 转换后的明文
*		len -- 密文和明文的长度（密文和明文长度一样）
*Output：None
*Author：xsx
*Data：2016年4月25日15:32:39
***************************************************************************************************/
unsigned char MyDencrypt(char *source, char *target, unsigned short len)
{
	PointBuffer * TempPointBuffer = NULL;
	MyState_TypeDef statues = My_Fail;

	unsigned short i=0, j=0;
	
	TempPointBuffer = MyMalloc(sizeof(PointBuffer));
	if(TempPointBuffer)
	{
		memset(TempPointBuffer, 0, sizeof(PointBuffer));
		
		TempPointBuffer->myfile.res = f_open(&(TempPointBuffer->myfile.file), "0:/MyWord.ncd", FA_READ);
		if(FR_OK == TempPointBuffer->myfile.res)
		{
			for(TempPointBuffer->i=0; TempPointBuffer->i<94; TempPointBuffer->i++)
			{
				TempPointBuffer->myfile.res = f_read(&(TempPointBuffer->myfile.file), (TempPointBuffer->word[TempPointBuffer->i]), 94, &(TempPointBuffer->myfile.br));
				
				if((FR_OK == TempPointBuffer->myfile.res) && (TempPointBuffer->myfile.br == 94))
				{
					for(TempPointBuffer->j=0; TempPointBuffer->j<94; TempPointBuffer->j++)
						TempPointBuffer->word[TempPointBuffer->i][TempPointBuffer->j] -= 0x7f;
				}
				else
					break;
			}
			
			f_close(&(TempPointBuffer->myfile.file));
			
			if(TempPointBuffer->i == 94)
				statues = My_Pass;
		}
		
		if(statues == My_Pass)
		{
			TempPointBuffer->myfile.res = f_open(&(TempPointBuffer->myfile.file), "0:/MyKey.ncd", FA_READ);
			if(FR_OK == TempPointBuffer->myfile.res)
			{
				TempPointBuffer->myfile.res = f_read(&(TempPointBuffer->myfile.file), (TempPointBuffer->key), 54, &(TempPointBuffer->myfile.br));
				
				if((FR_OK == TempPointBuffer->myfile.res) && (TempPointBuffer->myfile.br == 54))
				{
					for(TempPointBuffer->j=0; TempPointBuffer->j<54; TempPointBuffer->j++)
						TempPointBuffer->key[TempPointBuffer->j] -= 0x7f;
					
					statues = My_Pass;
				}
				else
					statues = My_Fail;
				
				f_close(&(TempPointBuffer->myfile.file));
			}
			else
				statues = My_Fail;
		}
		
		if(statues == My_Fail)
			goto END;
		
		for(i=0; i<len ;i++)
		{
			TempPointBuffer->tempIndex = (i + len) % 54;
			TempPointBuffer->tempV1 = TempPointBuffer->key[TempPointBuffer->tempIndex] - 33;
			TempPointBuffer->tempV2 = source[i];
			
			for(j=0; j<94; j++)
			{
				if(TempPointBuffer->word[TempPointBuffer->tempV1][j] == TempPointBuffer->tempV2)
				{
					*target++ = j+33;
					break;	
				}
			}
		}
		
		statues = My_Pass;
	}
	
	END:
		MyFree(TempPointBuffer);
		
		if(statues == My_Fail)
			memset(target, 0, len);
		
		return statues;

}

