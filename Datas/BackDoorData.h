/****************************************file start****************************************************/
#ifndef	BACKDOOR_D_H
#define	BACKDOOR_D_H

#pragma pack(1)
typedef struct BackDoorData_tag {
	float data1;
	float data2[6];
	float data3[4];
	float data4[2];
	float data5[2];
}BackDoorData;
#pragma pack()

//每个值测试次数
#define	data1_t	20											//最低检测线
#define	data2_t	3											//线性
#define	data3_t	3											//准确度
#define	data4_t	10											//重复性
#define	data5_t	9											//批间差

void SetS_TestItemName(void * name);
void SetS_ItemIndex(unsigned char index);
unsigned char GetS_ItemIndex(void);
void SetS_CategoryIndex(unsigned char index);
unsigned char GetS_CategoryIndex(void);
void SetS_TestIndex(unsigned char index);
unsigned char GetS_TestIndex(void);
double GetCurrentData(void);

#endif

/****************************************end of file************************************************/
