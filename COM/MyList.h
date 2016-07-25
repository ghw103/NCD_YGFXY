#ifndef __MYLIST_H__
#define __MYLIST_H__

#include	"Define.h"
#include	"Test_Data.h"

/*链表节点*/
typedef struct Node{
    void *data;
    struct Node *next;
	struct Node *pre;
}Node;

/*链表结构*/
typedef struct {
    struct Node *head;					//头节点
    struct Node *tail;					//尾节点
	struct Node *current;				//当前结点
} List;

List *CreateList(void);
MyState_TypeDef DestroyList(List *list, MyState_TypeDef (*fun)(void *data));
MyState_TypeDef InsertNodeToList(List *list, void * data, unsigned char mode);
MyState_TypeDef ListIsContainNode(List *list, Node * n);
MyState_TypeDef DeleteNodeFromList(List *list, Node * n, MyState_TypeDef (*fun)(void *data));
Node *GetNodeFromList(List *list, void *key, MyState_TypeDef (*fun)(void *data, void *key));
unsigned char ListSize(List *list);
MyState_TypeDef SetCurrentNode(List *list, Node *n);
Node *GetCurrentNode(List *list);
#endif

