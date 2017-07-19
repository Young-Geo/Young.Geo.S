#include "Ylist.h"


typedef struct
{
	Y_node head;
	Y_node *cursor;
	int length;
} _Ylist;

Ylist* Ylist_Create()
{
	_Ylist *ylist = (_Ylist *)malloc(sizeof(_Ylist));
	if (!ylist) {
		fprintf(stderr, "Ylist_Create error\n");
		return NULL;
	}
	memset(ylist, 0, sizeof(_Ylist));
	return ylist;
}

void Ylist_Clear(Ylist *ylist, int ex)
{
	_Ylist*_ylist = NULL;
	Y_node *cur_node = NULL, *next_node = NULL;
	int i = 0;

	if (!ylist)
		return;
	_ylist = (_Ylist*)ylist;
	if (_ylist->length <= 0)
		return;
	cur_node = _ylist->head.next;
	for (i = 0; i < _ylist->length && cur_node; ++i)
	{
		next_node = cur_node->next;
		if (ex && cur_node->value)
			free(cur_node->value);
		free(cur_node);
		cur_node = next_node;
	}
	memset(_ylist, 0, sizeof(_Ylist));
}
void Ylist_Destory(Ylist *ylist)
{
	if (!ylist)
		return;
	Ylist_Clear(ylist, 1);
	free(ylist);
}

int	 Ylist_Length(Ylist *ylist)
{
	_Ylist*_ylist;
	if (!ylist)
		return -1;
	_ylist = (_Ylist*)ylist;
	if (_ylist->length < 0)
		return -1;
	return _ylist->length;
}

Y_node* Ylist_Insert(Ylist *ylist, VALUE *value, int pos)
{
	_Ylist *_ylist = NULL;
	Y_node *node = NULL, *pre_node = NULL, *last_node = NULL;
	int i = 0;

	if (!ylist || !value) {
		fprintf(stderr, "Ylist_Insert NULL error\n");
		return NULL;
	}
	_ylist = (_Ylist*)ylist;
	if (pos < 0 || pos > _ylist->length)
		pos = _ylist->length;
	node = (Y_node *)malloc(sizeof(Y_node));
	if (!node) {
		fprintf(stderr, "Ylist_Insert malloc error\n");
		return NULL;
	}
	if (_ylist->length == 0) {
		node->pre = node;
		_ylist->cursor = node;
	}
	node->value = value;
	pre_node = &_ylist->head;
	for (i = 0; i < pos; ++i)
	{
		pre_node = pre_node->next;
	}

	node->next = pre_node->next;
	pre_node->next = node;
	if (node->next) {
		node->pre = node->next->pre;
		node->next->pre = node;
	}

	++_ylist->length;

	if (pre_node == &_ylist->head) {
		last_node = Ylist_Getnode(ylist, _ylist->length-1);
		if (last_node)
			last_node->next = node;
	}
	return node;
}

VALUE* Ylist_Delete(Ylist *ylist, int pos, int if_delete_value)
{
	_Ylist *_ylist = NULL;
	Y_node *node = NULL, *pre_node = NULL, *last_node = NULL;
	VALUE *value = NULL;
	int i = 0;

	if (!ylist) {
		fprintf(stderr, "Ylist_Delete NULL error\n");
		return NULL;
	}
	_ylist = (_Ylist *)ylist;
	if (_ylist->length <= 0)
		return NULL;
	if (pos >= _ylist->length || pos < 0) {
		fprintf(stderr, "Ylist_Delete pos error\n");
		return NULL;
	}
	pre_node = &_ylist->head;
	for (i = 0; i < pos; ++i)
	{
		pre_node = pre_node->next;
	}
	node = pre_node->next;
	pre_node->next = node->next;
	if (node && node->next)
		node->next->pre = node->pre;
	--_ylist->length;

	if (pre_node->next && pre_node == &_ylist->head) {
		last_node = Ylist_Getnode(ylist, _ylist->length - 1);
		if (last_node)
			last_node->next = pre_node->next;
	}
	if (_ylist->cursor == node)
		_ylist->cursor = node->next;
	if (node)
		value = node->value;
	else
		return NULL;
	free(node);
	if (if_delete_value && value) {
		free(value);
		value = NULL;
	}
	return value;
}


Y_node* Ylist_Getnode(Ylist* ylist, int pos)
{
	_Ylist *_ylist = NULL;
	Y_node *pre_node = NULL;
	int i = 0;

	if (!ylist) {
		fprintf(stderr, "Ylist_Getnode ylist NULL error\n");
		return NULL;
	}
	_ylist = (_Ylist *)ylist;
	if (_ylist->length <= 0)
		return NULL;
	if (pos < 0)
		pos = 0;
	if (pos >= _ylist->length)
		pos = _ylist->length - 1;
	pre_node = &_ylist->head;
	for (i = 0; i < pos; ++i)
	{
		if (pre_node->next)
			pre_node = pre_node->next;
	}
	return pre_node->next;
}

VALUE*Ylist_GetnodebyCursor_next(Ylist *ylist)
{
	Y_node *node = NULL;
	_Ylist *_ylist = NULL;
	if (!ylist) {
		fprintf(stderr, "Ylist_GetnodebyCursor_next NULL error \n");
		return NULL;
	}
	_ylist = (_Ylist*)ylist;
	node = _ylist->cursor;
	if (node && node->next)
		_ylist->cursor = node->next;
	return node->value;
}
VALUE*Ylist_GetnodebyCursor_prev(Ylist *ylist)
{
	Y_node *node = NULL;
	_Ylist *_ylist = NULL;
	if (!ylist) {
		fprintf(stderr, "Ylist_GetnodebyCursor_prev NULL error \n");
		return NULL;
	}
	_ylist = (_Ylist*)ylist;
	node = _ylist->cursor;
	if (node && node->pre)
		_ylist->cursor = node->pre;
	return node->value;
}

VALUE*		Ylist_GetnodebyCursor(Ylist *ylist)
{
	Y_node *node = NULL;
	_Ylist *_ylist = NULL;
	if (!ylist) {
		fprintf(stderr, "Ylist_GetNodeByCursor NULL error \n");
		return NULL;
	}
	_ylist = (_Ylist *)ylist;
	return _ylist->cursor->value;
}

VALUE*		Ylist_ResetCursor(Ylist *ylist)
{
	Y_node *node = NULL;
	_Ylist *_ylist = NULL;
	if (!ylist) {
		fprintf(stderr, "Ylist_Reset NULL error \n");
		return NULL;
	}
	_ylist = (_Ylist *)ylist;
	_ylist->cursor = _ylist->head.next;
	return _ylist->cursor->value;
}

VALUE*		Ylist_Push(Ylist *ylist, VALUE *value)
{
	Y_node *node = NULL;
	if (!ylist || !value) {
		fprintf(stderr, "Ylist_Push NULL error \n");
		return NULL;
	}
	node = Ylist_Insert(ylist, value, 0);
	if (!node)
		return NULL;
	return node->value;
}

VALUE*		Ylist_Pop(Ylist *ylist)
{
	if (!ylist) {
		fprintf(stderr, "Ylist_Pop NULL error \n");
		return NULL;
	}
	return Ylist_Delete(ylist, 0, 0);
}

VALUE*		Ylist_Top(Ylist *ylist)
{
	Y_node *node = NULL;
	if (!ylist) {
		fprintf(stderr, "Ylist_Top NULL error \n");
		return NULL;
	}
	node = Ylist_Getnode(ylist, 0);
	if (!node)
		return NULL;
	return node->value;
}

VALUE*		Ylist_Queue_enqueue(Ylist *ylist, VALUE *value)
{
	int len = 0;
	Y_node *node = NULL;
	if (!ylist || !value) {
		fprintf(stderr, "Ylist_Queue_enqueue NULL error \n");
		return NULL;
	}
	len = Ylist_Length(ylist);
	if (len < 0)
		return NULL;
	node = Ylist_Insert(ylist, value, len);
	if (!node)
		return NULL;
	return value;
}
VALUE*		Ylist_Queue_dequeue(Ylist *ylist)
{
	int len = 0;
	VALUE *value = NULL;
	if (!ylist) {
		fprintf(stderr, "Ylist_Queue_dequeue NULL error \n");
		return NULL;
	}
	value = Ylist_Delete(ylist, 0, 0);
	return value;
}


