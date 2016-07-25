#ifndef _TCP_S_D_H__
#define _TCP_C_D_H__


//默认服务器ip
#define	Default_ipaddr1		192
#define	Default_ipaddr2		168
#define	Default_ipaddr3		1
#define	Default_ipaddr4		100
#define	Default_ipport		8080


typedef enum
{ 
	InsideServerInfo = 1,			//内部服务器
	OutsideServerInfo = 2			//超级病例服务器
}ServerInfoType;


typedef struct
{
	unsigned char ipaddr1;
	unsigned char ipaddr2;
	unsigned char ipaddr3;
	unsigned char ipaddr4;
	unsigned short port;
}ServerInfo_TypeDef; 

void SetDefaultServerInfo(void);
void SetServerInfo(ServerInfoType servertype, unsigned short myipaddr1, unsigned short myipaddr2, unsigned short myipaddr3, 
	unsigned short myipaddr4, unsigned short myport);
ServerInfo_TypeDef * GetServerInfo(ServerInfoType servertype);

#endif