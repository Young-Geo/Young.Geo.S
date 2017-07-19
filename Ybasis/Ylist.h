#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#if 0
#ifdef  __cplusplus
extern "C" {
#endif
#endif

#ifndef __YLIST__H_
#define __YLIST__H_
typedef void VALUE;
typedef void Ylist;
typedef struct _Y_LIST_NODE Y_node;
struct _Y_LIST_NODE
{
	Y_node *pre;
	Y_node *next;
	VALUE *value;
};

Ylist*		Ylist_Create();
void		Ylist_Clear(Ylist *);
void		Ylist_Destory(Ylist *);
int			Ylist_Length(Ylist*);
Y_node*		Ylist_Getnode(Ylist*, int pos);
Y_node*		Ylist_Insert(Ylist*, VALUE *value, int pos);
VALUE*		Ylist_Delete(Ylist*, int pos, int if_delete_value);

VALUE*		Ylist_GetnodebyCursor_next(Ylist*);
VALUE*		Ylist_GetnodebyCursor_prev(Ylist*);
VALUE*		Ylist_GetnodebyCursor(Ylist*);
VALUE*		Ylist_ResetCursor(Ylist*);

VALUE*		Ylist_Push(Ylist*, VALUE*);
VALUE*		Ylist_Pop(Ylist*);
VALUE*		Ylist_Top(Ylist*);

VALUE*		Ylist_Queue_enqueue(Ylist*, VALUE*);
VALUE*		Ylist_Queue_dequeue(Ylist*);
#endif

#if 0
#ifdef  __cplusplus
}
#endif
#endif