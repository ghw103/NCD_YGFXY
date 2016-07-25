/***************************************************************************************************
*FileName:TcpServerInfo
*Description:TCP服务器信息，ip，端口等
*Author:xsx
*Data:2016年4月21日17:20:59
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"TcpServerInfo.h"

#include 	"api.h"
/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/
static ServerInfo_TypeDef GB_ServerInfo1;						//内部服务器

static ServerInfo_TypeDef GB_ServerInfo2;						//超级病例

/*只有开机才更新一次*/
static struct ip_addr GB_Server_ipaddr1;						//当前正在使用的服务器ip
static unsigned short GB_Server_port1;							//当前正在使用的服务器端口
static struct ip_addr GB_server_ipaddr2;						//当前正在使用的服务器ip
static unsigned short GB_Server_port2;							//当前正在使用的服务器端口
/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName：SetDefaultServerInfo
*Description：设置默认服务器信息
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月21日17:40:55
***************************************************************************************************/
void SetDefaultServerInfo(void)
{
	SetServerInfo(InsideServerInfo, Default_ipaddr1, Default_ipaddr2, Default_ipaddr3, Default_ipaddr4, Default_ipport);
	SetServerInfo(OutsideServerInfo, Default_ipaddr1, Default_ipaddr2, Default_ipaddr3, Default_ipaddr4, Default_ipport);
}

/***************************************************************************************************
*FunctionName：SetServerInfo
*Description：设置服务器信息
*Input：servertype -- 服务器类别,  (myipaddr1，myipaddr4)服务器ip,   myport -- 服务器端口号
*Output：None
*Author：xsx
*Data：2016年4月21日17:37:03
***************************************************************************************************/
void SetServerInfo(ServerInfoType servertype, unsigned short myipaddr1, unsigned short myipaddr2, unsigned short myipaddr3, 
	unsigned short myipaddr4, unsigned short myport)
{
	ServerInfo_TypeDef * tempserverinfo;
	
	if(servertype == InsideServerInfo)
		tempserverinfo = &GB_ServerInfo1;
	else
		tempserverinfo = &GB_ServerInfo2;
	
	tempserverinfo->ipaddr1 = myipaddr1;
	tempserverinfo->ipaddr2 = myipaddr2;
	tempserverinfo->ipaddr3 = myipaddr3;
	tempserverinfo->ipaddr4 = myipaddr4;
	tempserverinfo->port = myport;
}

/***************************************************************************************************
*FunctionName：GetServerInfo
*Description：获取服务器信息
*Input：servertype -- 服务器类别
*Output：返回所需要的服务器信息地址
*Author：xsx
*Data：2016年4月21日17:38:50
***************************************************************************************************/
ServerInfo_TypeDef * GetServerInfo(ServerInfoType servertype)
{
	ServerInfo_TypeDef * tempserverinfo;
	
	if(servertype == InsideServerInfo)
		tempserverinfo = &GB_ServerInfo1;
	else
		tempserverinfo = &GB_ServerInfo2;
	
	return tempserverinfo;
}