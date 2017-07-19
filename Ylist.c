#include "Ylist.h"


static void _list_init(Ylist_t *Ylist)
{
	Ylist->next = Ylist;
	Ylist->prev = Ylist;
}


Ylist* Ylist_init()
{
	Ylist_t *Ylist = Ymalloc(sizeof(Ylist_t));
	if (!Ylist)
		return NULL;
	_list_init(Ylist);
	return Ylist;
}

static  void _list_add(Ylist_t *new_node,
			      Ylist_t *prev,
			      Ylist_t *next)
{
	next->prev = new_node;
	new_node->next = next;
	new_node->prev = prev;
	prev->next = new_node;
}

int 	Ylist_add_head(Ylist_t *Ylist, Node_t *node)
{
	Ylist_t *Ynode = NULL;
	
	if (!node)
		return -1;
	
	Ynode = Ymalloc(sizeof(Ylist_t));
	if (!Ynode)
		return -2;
	
	Ynode->node = node;
	_list_add(Ynode, Ylist, Ylist->next);
	return 0;
}

int 	Ylist_add_tail(Ylist_t *Ylist, Node_t *node)
{
	Ylist_t *Ynode = NULL;
	
	if (!node)
		return -1;
	
	Ynode = Ymalloc(sizeof(Ylist_t));
	if (!Ynode)
		return -2;
	
	Ynode->node = node;
	_list_add(Ynode, Ylist->prev, Ylist);
	return 0;
}



static void _list_del(Ylist_t *entry)
{
	Ylist *prev = NULL, *next = NULL;
	prev = entry->prev;
	next = entry->next;
	next->prev = prev;
	prev->next = next;
}

Node_t* Ylist_del_head(Ylist_t *Ylist, int _if_free)
{
	Ylist_t *entry = NULL;
	Node_t *node = NULL;
	if (!Ylist || !Ylist->next)
		return NULL;
	
	entry = Ylist->next;
	if (entry == Ylist)
		return NULL;
	
	node = entry->node;
	Yassert(node);
	
	_list_del(entry);
	if (_if_free)
	{
		Yfree(node);
		node = NULL;
	}
	Yfree(entry);
	
	return node;
}
Node_t* Ylist_del_tail(Ylist_t *Ylist, int _if_free)
{	
	Ylist_t *entry = NULL;
	Node_t *node = NULL;
	if (!Ylist || !Ylist->prev)
		return NULL;
	
	entry = Ylist->prev;
	if (entry == Ylist)
		return NULL;
	
	node = entry->node;
	Yassert(node);
	
	_list_del(entry);
	if (_if_free)
	{
		Yfree(node);
		node = NULL;
	}
	Yfree(entry);
	
	return node;
}


static void _list_replace(Ylist_t *old_node,
				Ylist_t *new_node)
{
	new_node->next = old_node->next;
	new_node->next->prev = new_node;
	new_node->prev = old_node->prev;
	new_node->prev->next = new_node;
}

static void _list_move_head(Ylist_t *Ylist, Ylist_t *d_node)
{
	if (d_node->next && d_node->prev)
		_list_del(d_node);
	_list_add(d_node, Ylist, Ylist->next);
}

static void _list_move_tail(Ylist_t *Ylist, Ylist_t *d_node)
{
	if (d_node->next && d_node->prev)
		_list_del(d_node);
	_list_add(d_node, Ylist->prev, Ylist);
}


static int _list_empty(Ylist_t *Ylist)
{
       return (Ylist->next == Ylist) && (Ylist->prev == Ylist);
}

int Ylist_isempty(Ylist_t *Ylist)
{
	return _list_empty(Ylist);
}

int Ylist_islast(Ylist_t *Yhead, Ylist_t *Ylast)
{
	return Ylast->next == Yhead;
}


#if 0
static void _list_splice(Ylist_t *Ylist, Ylist_t *Ysp_list)
{
       struct Ylist_t *first = Ysp_list->next;
       struct Ylist_t *last  = Ysp_list->prev;
       struct Ylist_t *at    = Ylist->next;
 
       first->prev = Ylist;
       Ylist->next = first;
 
       last->next = at;
       at->prev = last;
}
#endif
static void _list_splice(const Ylist_t *Ysp_list,
				 Ylist_t *prev,
				 Ylist_t *next)
{
	Ylist_t *first = Ylist->next;
	Ylist_t *last = Ylist->prev;

	first->prev = prev;
	prev->next = first;

	last->next = next;
	next->prev = last;
}

static void _list_rotate_left(Ylist_t *Ylist)
{
	Ylist_t *first;

	if (!Ylist_isempty(Ylist)) {
		first = Ylist->next;
		_list_move_tail(Ylist, first);
	}
}

int Ylist_rotate_left(Ylist_t *Ylist)
{
	_list_rotate_left(Ylist);
}

int Ylist_push(Ylist_t *Ylist, Node_t *node)
{
	return Ylist_add_head(Ylist, node);
}
Ylist_t * Ylist_pop(Ylist_t *Ylist)
{
	Ylist_t *entry = NULL;
	if (!Ylist || !Ylist->next)
		return NULL;
	
	entry = Ylist->next;
	if (entry == Ylist)
		return NULL;
	
	_list_del(entry);
	return entry;
}
Node_t * Ylist_popv(Ylist_t *Ylist)
{
	return Ylist_del_head(Ylist, 0);
}
int Ylist_clear(Ylist_t *Ylist)
{
	if (!Ylist)
	{
		return -1;
	}
	for ( ; ; )
	{
		if (Ylist->next == Ylist && Ylist->prev == Ylist)
			break;
		Ylist_del_head(Ylist, 1);
	}
	return 0;
}

int Ylist_destory(Ylist_t *Ylist)
{
	if (!Ylist)
	{
		return -1;	
	}
	Ylist_clear(Ylist);
	Yfree(Ylist);
	return 0;
}

int	Ylist_length(Ylist_t *Ylist)
{
	unsigned int size = 0;
	Ylist_t *head = NULL, *Ynode;
	if (!Ylist)
	{
		return -1;
	}
	for (head = Ylist, Ynode = Ylist->next; Ynode != head; Ynode = Ynode->next && Ynode->next)
	{
		++size;
	}
	return size;
}















