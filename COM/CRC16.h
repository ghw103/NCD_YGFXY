#ifndef __CRC16_H__
#define __CRC16_H__

unsigned short CalModbusCRC16Fun1(void *puchMsg, unsigned short usDataLen);
void CalModbusCRC16Fun2(void *puchMsg, unsigned short usDataLen, void *crc);
#endif

