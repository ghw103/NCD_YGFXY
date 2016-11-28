/****************************************file start****************************************************/
#ifndef DEVICEDAO_H_H
#define DEVICEDAO_H_H

#include	"Define.h"


/*读写设备信息*/
MyState_TypeDef SaveDeviceInfo(DeviceInfo * deviceinfo);
MyState_TypeDef ReadDeviceInfo(DeviceInfo * deviceinfo);

#endif

/****************************************end of file************************************************/
