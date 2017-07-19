#include "Ybasis.h"

typedef void Node_t;

typedef struct _Ylist_t{
        struct _Ylist_t *next, *prev;
		Node_t *node;
} Ylist_t;

Ylist* Ylist_init();

int 	Ylist_add_head(Ylist_t *, Node_t *);
int 	Ylist_add_tail(Ylist_t *, Node_t *);

Node_t* Ylist_del_head(Ylist_t *, int);
Node_t* Ylist_del_tail(Ylist_t *, int);

int Ylist_clear(Ylist_t *);
int Ylist_destory(Ylist_t *);

int Ylist_isempty(Ylist_t *);
int Ylist_islast(Ylist_t *Yhead, Ylist_t *Ylast);
int Ylist_rotate_left(Ylist_t *Ylist);

int Ylist_push(Ylist_t *, Node_t *);
Ylist_t * Ylist_pop(Ylist_t *);
Node_t * Ylist_popv(Ylist_t *);
int	Ylist_length(Ylist_t*);
