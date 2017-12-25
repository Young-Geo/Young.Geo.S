#ifndef __H_XLIST_H__
#define __H_XLIST_H__

/* include */
#include "afx.h"
#include "xchain.h"

/* macro */
#define XLIST_HASH_MAX 2713

#define XLIST_PTR 0
#define XLIST_STRING 1
#define XLIST_XLIST 2

/* define */
typedef struct _xlist
{
  unsigned char type ;
  char * key ;
  char * value ;
  struct _xlist * prev ;
  struct _xlist * next ;
  int hash ;
} xlist ;

#define xlist_class_clean( xlst , type ) { \
xlist * x_class_clean_tmp = null ; \
for ( x_class_clean_tmp = xlst ; x_class_clean_tmp && ( x_class_clean_tmp ->next ) ; x_class_clean_tmp = x_class_clean_tmp ->next ) { \
type * x_class_clean_tmp_type = (type *)x_class_clean_tmp ->value ; \
if ( !x_class_clean_tmp_type ) continue ; \
delete ( x_class_clean_tmp_type ) ; \
}	\
xlist_clean ( xlst ) ; \
}

#define xlist_class_reset( xlst , type ) { \
xlist * x_class_clean_tmp = null ; \
for ( x_class_clean_tmp = xlst ; x_class_clean_tmp && ( x_class_clean_tmp ->next ) ; x_class_clean_tmp = x_class_clean_tmp ->next ) { \
type * x_class_clean_tmp_type = (type *)x_class_clean_tmp ->value ; \
if ( !x_class_clean_tmp_type ) continue ; \
delete ( x_class_clean_tmp_type ) ; \
}	\
xlist_reset ( xlst ) ; \
}

xlist * xlist_init () ;
void xlist_push ( xlist *xlst , const char * key , int type , char * value ) ;
#define xlist_add xlist_cat
void xlist_cat ( xlist *xlst , const char * key , int type , char * value ) ;
void xlist_combin ( xlist * xlst , xlist * l ) ;
xlist * xlist_get ( xlist *x , const char *key ) ;
int xlist_set ( xlist *xlst , const char * key , char *value ) ;
char * xlist_getv ( xlist * lst , const char *key ) ;
xlist * xlist_index ( xlist * lst , int idx ) ;
#define xlist_end(xs) ((xs)?(xs->prev):NULL)
int xlist_size ( xlist *lst ) ;
#define xlist_isempty(xs) ((!xs)||((xs) ->prev == (xs)))
#define xlist_notempty(xs) ((xs)&&((xs) ->prev != (xs)))
void xlist_order ( xlist *xs ) ;
void xlist_order_big ( xlist *xs ) ;
void xlist_reverse ( xlist *xs ) ;

void xlist_dump ( xlist *xs , int level ) ;
xlist * xlist_dup ( xlist *src ) ;
void xlist_terminal ( xlist *x , xlist * pos ) ;
void xlist_clean ( xlist ** px ) ;
void xlist_reset ( xlist *x ) ;

xlist * xlist_break ( xlist * xs , xlist *index ) ;
xlist * xlist_insert ( xlist * xs , xlist * index , const char * key , int type , char * value ) ;
void xlist_delete ( xlist *xs , const char *key ) ;
xlist * xlist_pop ( xlist * xs ) ;
char * xlist_popv ( xlist * xs ) ;
char * xlist_popv_notzero ( xlist * xs ) ;
xlist * xlist_cut ( xlist * xs ) ;
xlist * xlist_remove ( xlist * xs , const char * key ) ;
void	xlist_kill_remove(xlist * xs , const char * key);
#define xlist_first(xs) (xs)
#define xlist_firstv(xs) ((xs)->value)
#define xlist_last(xs)  (xs->prev->prev)
#define xlist_lastv(xs)  (((xs)->prev->prev)->value)

xlist * xlist_str2lst (char *str , char * gap) ;
void xlist_lst2str (xlist * xs , char ** data , int * len ) ;

char ** xstrsplit ( char *string, char *delimiter, int max_tokens ) ;
void xstrsplit_free (char ** xarray) ;

#define XPOINTER_TO_SIZE(p)	((size_t) (p))
#define XSIZE_TO_POINTER(s)	((void *)(size_t) (s))
#define XPOINTER_TO_INT(p)	((int) (p))
#define XINT_TO_POINTER(s)	((void)(int) (s))

#endif