#ifndef LCD_D_H_H
#define LCD_D_H_H

#define	MaxSendDataLen		100

void SelectPage(unsigned short index);
void ReadCurrentPageId(void);
void ReadLcdSoftVersion(void);
void DisText(unsigned short addr, void *data, unsigned short len);
void ClearText(unsigned short addr, unsigned short len);
void WriteRadioData(unsigned short addr, void *data, unsigned short len);
void BasicPic(unsigned short addr,unsigned short datanum, unsigned short soureid ,unsigned short xs,unsigned short ys ,unsigned short xe,unsigned short ye ,
	unsigned short txs,unsigned short tys);
void SendKeyCode(unsigned char keycode);
unsigned short GetBufLen(unsigned char *p ,unsigned short len);
void DspNum(unsigned short addr , unsigned int num, unsigned char len);
void BasicUI(unsigned short addr , unsigned short cmd, unsigned short datapacknum, void *data , unsigned short len);
void SetLEDLight(unsigned char num);
void WriteVarIcoNum(unsigned short addr, unsigned short num);

#endif


