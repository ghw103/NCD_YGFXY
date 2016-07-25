/***************************************************************************************************
*FileName: MyList
*Description: 链表操作
*Author:xsx
*Data:2016年4月21日14:22:46
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"MyList.h"
#include	"malloc.h"

/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

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
*FunctionName：CreateList
*Description：新建链表
*Input：None
*Output：返回新建的链表，如果为null表示新建失败
*Author：xsx
*Data：2016年5月16日20:29:30
***************************************************************************************************/
List *CreateList(void)
{
	List * myList = NULL;
	
	myList = (List *)mymalloc(sizeof(List));
	if(myList)
	{
		mymemset(myList, 0, sizeof(List));
	}
	
	return myList;
}

/***************************************************************************************************
*FunctionName：DestroyList
*Description：删除链表，释放空间
*Input：list -- 要删除的链表
		MyState_TypeDef (*fun)(void *data) -- 如果不为null，则执行用来删除节点内的数据
*Output：返回操作结果
*Author：xsx
*Data：2016年5月16日20:31:44
***************************************************************************************************/
MyState_TypeDef DestroyList(List *list, MyState_TypeDef (*fun)(void *data))
{
	struct Node *node = NULL;
	struct Node *next = NULL;
	
	if(list)
	{
		/*获取第一个节点*/
		node = list->head;
		
		/*如果节点存在*/
		while(node)
		{
			/*是否需要删除节点内的数据*/
			if(fun != NULL)
			{
				if(fun(node->data) != My_Fail)
					return My_Fail;
			}
			
			/*获取下一个节点*/
			next = node->next;
			
			/*删除当前结点*/
			myfree(node);
			
			node = next;
		}
	
		myfree(list);
	
		return My_Pass;
	}
	else
		return My_Fail;
}

/***************************************************************************************************
*FunctionName：InsertNodeToList
*Description：在链表的结尾插入节点
*Input：None
*Output：None
*Author：xsx
*Data：2016年5月16日20:41:07
***************************************************************************************************/
MyState_TypeDef InsertNodeToList(List *list, void * data, unsigned char mode)
{
	struct Node *node = NULL;
	
	if(list&&data)
	{
		node = mymalloc(sizeof(Node));
		if(node)
		{
			if(list->head == NULL)
			{
				list->head = node;
				list->tail = node;
				node->next = NULL;
				node->pre = NULL;
				node->data = data;
			}
			else
			{
				node->pre = list->tail;
				node->next = NULL;
				list->tail->next = node;
				list->tail = node;
				node->data = data;
			}
			
			if(mode)
				list->current = node;
			
			return My_Pass;
		}
		else
			return My_Fail;
		
	}
	else
		return My_Fail;
}

/***************************************************************************************************
*FunctionName：ListIsContainNode
*Description：判断链表中是否包含指定节点
*Input：None
*Output：None
*Author：xsx
*Data：2016年5月16日20:47:22
***************************************************************************************************/
MyState_TypeDef ListIsContainNode(List *list, Node * n)
{
	struct Node *node = NULL;
	if(list&&n)
	{
		node = list->head;
		
		/*如果节点存在*/
		while(node)
		{
			if(node == n)
				return My_Pass;
			
			node = node->next;
		}
		return My_Fail;
	}
	else
		return My_Fail;
}

/***************************************************************************************************
*FunctionName：DeleteNodeFromList
*Description：从链表中删除节点
*Input：list -- 链表
		n -- 要删除的节点
		fun -- 是否要对删除的节点做处理，null表示不处理
*Output：None
*Author：xsx
*Data：2016年5月16日20:42:57
***************************************************************************************************/
MyState_TypeDef DeleteNodeFromList(List *list, Node * n, MyState_TypeDef (*fun)(void *data))
{
	if(list&&n)
	{
		if(My_Pass == ListIsContainNode(list, n))
		{
			if(n->pre)
				n->pre->next = n->next;
			else
				list->head = n->next;
			
			if(n->next)
				n->next->pre = n->pre;
			else
				list->tail = n->pre;
			
			if(list->current == n)
				list->current = NULL;
			
			if(fun != NULL)
			{
				if(My_Fail == fun(n->data))
					return My_Fail;
			}

			myfree(n);
			
			return My_Pass;
		}
		else
			return My_Fail;
	}
	else
		return My_Fail;
}

/***************************************************************************************************
*FunctionName：GetNodeFromList
*Description：按照条件获取节点
*Input：fun -- 获取节点的条件函数
*Output：None
*Author：xsx
*Data：2016年5月16日20:56:09
***************************************************************************************************/
Node *GetNodeFromList(List *list, void *key, MyState_TypeDef (*fun)(void *data, void *key))
{
	struct Node *node = NULL;

	if(list)
	{
		node = list->head;
		while(node)
		{
			if(fun == NULL)
				return NULL;
			else
			{
				/*比较成功后返回节点*/
				if(My_Pass == fun(node->data, key))
				{
					return node;
				}
				else
					node = node->next;
			}
		}
		
		return NULL;
	}
	else
		return NULL;
}

/***************************************************************************************************
*FunctionName：ListSize
*Description：获取链表的大小
*Input：None
*Output：None
*Author：xsx
*Data：2016年5月16日20:59:01
***************************************************************************************************/
unsigned char ListSize(List *list)
{
	struct Node *node = NULL;
	unsigned char count = 0;
	
	if(list)
	{
		node = list->head;
		while(node)
		{
			count++;
			node = node->next;
		}
		return count;
	}
	else
		return 0;
}

/***************************************************************************************************
*FunctionName：SetCurrentNode
*Description：设置当前操作的节点
*Input：None
*Output：None
*Author：xsx
*Data：2016年5月16日21:05:10
***************************************************************************************************/
MyState_TypeDef SetCurrentNode(List *list, Node *n)
{
	if(list&&n)
	{
		if(ListIsContainNode(list, n) == My_Pass)
		{
			list->current = n;
			return My_Pass;
		}
		else
			return My_Fail;
	}
	else
		return My_Fail;
}

/***************************************************************************************************
*FunctionName：GetCurrentNode
*Description：获取当前操作的节点
*Input：None
*Output：None
*Author：xsx
*Data：2016年5月16日21:05:26
***************************************************************************************************/
Node *GetCurrentNode(List *list)
{
	if(list)
	{
		return list->current;
	}
	else
		return NULL;
}
