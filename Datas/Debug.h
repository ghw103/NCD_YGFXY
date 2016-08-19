/****************************************file start****************************************************/
#if (DebugCode == CodeType)

#ifndef	DEBUG_H
#define	DEBUG_H

void SetGB_DebugCount(unsigned short count);
unsigned short GetGB_DebugCount(void);
void SetGB_DebugQRErrorCount(unsigned short count);
unsigned short GetGB_DebugQRErrorCount(void);
void SetGB_DebugResult(float result);
float GetGB_DebugResult(unsigned char index);
void ClearDebugData(void);

#endif

#endif
/****************************************end of file************************************************/
