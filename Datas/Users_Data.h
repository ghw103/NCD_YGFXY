#ifndef _USERS_S_D_H__
#define _USERS_S_D_H__


#include	"Define.h"
#include 	<stdlib.h>
#include	"cJSON.h"

unsigned char GetUsersNum(void);
MyState_TypeDef GetUserListJSON(void);
MyState_TypeDef SaveUserListJSON(void);
void DeleteUserListJSON(void);

MyState_TypeDef AddAUserJSON(User_Type *user);
MyState_TypeDef DeleteAUserJSON(User_Type *user);
MyState_TypeDef GetAUserByIndex(unsigned char index, User_Type * user);

MyState_TypeDef ParseJSONAUserJSON(User_Type * user, cJSON * cjson);
cJSON * CreateAUserJSON(User_Type * user);
#endif

